/**
 * @file    vpmg_fill_test.c
 * @brief   Unit tests for unit-stride Vpmg dielectric smoothing and sphere marking
 */

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "apbscfg.h"
#include "generic/mgparm.h"
#include "generic/pbeparm.h"
#include "generic/vacc.h"
#include "generic/valist.h"
#include "generic/vatom.h"
#include "generic/vhal.h"
#include "generic/vpbe.h"
#include "maloc/maloc.h"
#include "mg/vpmg.h"
#include "mg/vpmgp.h"

#define IJK(i,j,k) (((k)*(nx)*(ny))+((j)*(nx))+(i))

typedef struct sAtomSpec {
    double x;
    double y;
    double z;
    double radius;
    double charge;
} AtomSpec;

static Valist* build_alist(const AtomSpec *specs, int natoms) {
    Valist *alist;
    Vatom *atom;
    int i, d;
    double c;

    alist = Valist_ctor();
    VASSERT(alist != VNULL);
    alist->number = natoms;
    alist->atoms = (Vatom *)Vmem_malloc(alist->vmem, (size_t)natoms,
                                        sizeof(Vatom));
    VASSERT(alist->atoms != VNULL);

    for (d = 0; d < 3; d++) {
        alist->mincrd[d] = VLARGE;
        alist->maxcrd[d] = -VLARGE;
    }
    alist->maxrad = 0.0;
    alist->charge = 0.0;

    for (i = 0; i < natoms; i++) {
        atom = &(alist->atoms[i]);
        atom->position[0] = specs[i].x;
        atom->position[1] = specs[i].y;
        atom->position[2] = specs[i].z;
        Vatom_setRadius(atom, specs[i].radius);
        Vatom_setCharge(atom, specs[i].charge);
        Vatom_setPartID(atom, -1);
        Vatom_setEpsilon(atom, 0.0);
        Vatom_setAtomID(atom, i);
        atom->resName[0] = '\0';
        atom->atomName[0] = '\0';

        for (d = 0; d < 3; d++) {
            c = atom->position[d];
            if (c < alist->mincrd[d]) alist->mincrd[d] = c;
            if (c > alist->maxcrd[d]) alist->maxcrd[d] = c;
        }
        if (specs[i].radius > alist->maxrad) {
            alist->maxrad = specs[i].radius;
        }
        alist->charge += specs[i].charge;
    }

    for (d = 0; d < 3; d++) {
        alist->center[d] = 0.5 * (alist->mincrd[d] + alist->maxcrd[d]);
    }
    return alist;
}

/* Full-grid brute-force sphere marker (no bounding-box clipping) */
static void mark_sphere_oracle(double rtot, const double *tpos,
                               int nx, int ny, int nz,
                               double hx, double hy, double hz,
                               double xmin, double ymin, double zmin,
                               double *array, double markVal) {
    int i, j, k;
    double fi, fj, fk;
    double posx, posy, posz, rtot2, dx2, dy2, dz2;

    posx = tpos[0] - xmin;
    posy = tpos[1] - ymin;
    posz = tpos[2] - zmin;
    rtot2 = VSQR(rtot);

    for (k = 0, fk = 0.0; k < nz; k++, fk += 1.0) {
        dz2 = VSQR(posz - hz * fk);
        for (j = 0, fj = 0.0; j < ny; j++, fj += 1.0) {
            dy2 = VSQR(posy - hy * fj);
            for (i = 0, fi = 0.0; i < nx; i++, fi += 1.0) {
                dx2 = VSQR(posx - hx * fi);
                if ((dz2 + dy2 + dx2) <= rtot2) {
                    array[IJK(i,j,k)] = markVal;
                }
            }
        }
    }
}

/* Reference oracle for fillcoCoefMol (VSM_MOL and VSM_MOLSMOOTH) */
static void fill_mol_oracle(Vpbe *pbe, const Vpmgp *pmgp, Vsurf_Meth surfMeth,
                            double *epsx, double *epsy, double *epsz,
                            double *kappa) {
    Valist *alist;
    Vacc *acc;
    Vatom *atom;
    VaccSurf *asurf;
    double xmin, xmax, ymin, ymax, zmin, zmax;
    double hx, hy, hz, srad, irad, ionstr, ionmask, epsw, epsp, arad, area;
    double *apos;
    double pos[3];
    double *w1, *w2, *w3;
    double frac;
    int nx, ny, nz, ntot, i, j, k, iatom, ipt, numpts;

    alist = pbe->alist;
    acc = pbe->acc;
    srad = Vpbe_getSolventRadius(pbe);
    irad = Vpbe_getMaxIonRadius(pbe);
    ionstr = Vpbe_getBulkIonicStrength(pbe);
    epsw = Vpbe_getSolventDiel(pbe);
    epsp = Vpbe_getSoluteDiel(pbe);

    nx = pmgp->nx;
    ny = pmgp->ny;
    nz = pmgp->nz;
    ntot = nx * ny * nz;
    hx = pmgp->hx;
    hy = pmgp->hy;
    hz = pmgp->hzed;

    xmin = pmgp->xcent - 0.5 * pmgp->xlen;
    ymin = pmgp->ycent - 0.5 * pmgp->ylen;
    zmin = pmgp->zcent - 0.5 * pmgp->zlen;
    xmax = pmgp->xcent + 0.5 * pmgp->xlen;
    ymax = pmgp->ycent + 0.5 * pmgp->ylen;
    zmax = pmgp->zcent + 0.5 * pmgp->zlen;

    for (i = 0; i < ntot; i++) {
        epsx[i] = epsw;
        epsy[i] = epsw;
        epsz[i] = epsw;
    }

    for (iatom = 0; iatom < Valist_getNumberAtoms(alist); iatom++) {
        atom = Valist_getAtom(alist, iatom);
        apos = Vatom_getPosition(atom);
        arad = Vatom_getRadius(atom);
        if ((apos[0] <= xmin) || (apos[0] >= xmax)
            || (apos[1] <= ymin) || (apos[1] >= ymax)
            || (apos[2] <= zmin) || (apos[2] >= zmax)) {
            continue;
        }
        if (arad > VSMALL) {
            mark_sphere_oracle(arad + srad, apos, nx, ny, nz, hx, hy, hz,
                               xmin + 0.5 * hx, ymin, zmin, epsx, epsp);
            mark_sphere_oracle(arad + srad, apos, nx, ny, nz, hx, hy, hz,
                               xmin, ymin + 0.5 * hy, zmin, epsy, epsp);
            mark_sphere_oracle(arad + srad, apos, nx, ny, nz, hx, hy, hz,
                               xmin, ymin, zmin + 0.5 * hz, epsz, epsp);
        }
    }

    (void)Vacc_SASA(acc, srad);
    if (srad > VSMALL) {
        for (iatom = 0; iatom < Valist_getNumberAtoms(alist); iatom++) {
            atom = Valist_getAtom(alist, iatom);
            area = Vacc_atomSASA(acc, srad, atom);
            if (area > 0.0) {
                asurf = Vacc_atomSASPoints(acc, srad, atom);
                for (ipt = 0; ipt < asurf->npts; ipt++) {
                    pos[0] = asurf->xpts[ipt];
                    pos[1] = asurf->ypts[ipt];
                    pos[2] = asurf->zpts[ipt];
                    mark_sphere_oracle(srad, pos, nx, ny, nz, hx, hy, hz,
                                       xmin + 0.5 * hx, ymin, zmin, epsx, epsw);
                    mark_sphere_oracle(srad, pos, nx, ny, nz, hx, hy, hz,
                                       xmin, ymin + 0.5 * hy, zmin, epsy, epsw);
                    mark_sphere_oracle(srad, pos, nx, ny, nz, hx, hy, hz,
                                       xmin, ymin, zmin + 0.5 * hz, epsz, epsw);
                }
            }
        }
    }

    if (surfMeth == VSM_MOLSMOOTH) {
        w1 = (double *)malloc((size_t)ntot * sizeof(double));
        w2 = (double *)malloc((size_t)ntot * sizeof(double));
        w3 = (double *)malloc((size_t)ntot * sizeof(double));
        VASSERT(w1 != NULL && w2 != NULL && w3 != NULL);
        for (i = 0; i < ntot; i++) {
            w1[i] = epsx[i];
            w2[i] = epsy[i];
            w3[i] = epsz[i];
        }
        for (i = 0; i < nx; i++) {
            for (j = 0; j < ny; j++) {
                for (k = 0; k < nz; k++) {
                    frac = 1.0 / w1[IJK(i,j,k)];
                    frac += 1.0 / w2[IJK(i,j,k)];
                    frac += 1.0 / w3[IJK(i,j,k)];
                    numpts = 3;
                    if (j > 0) {
                        frac += 1.0 / w2[IJK(i,j-1,k)];
                        numpts += 1;
                    }
                    if (k > 0) {
                        frac += 1.0 / w3[IJK(i,j,k-1)];
                        numpts += 1;
                    }
                    if (i < (nx - 1)) {
                        frac += 1.0 / w2[IJK(i+1,j,k)];
                        frac += 1.0 / w3[IJK(i+1,j,k)];
                        numpts += 2;
                        if (j > 0) {
                            frac += 1.0 / w2[IJK(i+1,j-1,k)];
                            numpts += 1;
                        }
                        if (k > 0) {
                            frac += 1.0 / w3[IJK(i+1,j,k-1)];
                            numpts += 1;
                        }
                    }
                    epsx[IJK(i,j,k)] = numpts / frac;

                    frac = 1.0 / w2[IJK(i,j,k)];
                    frac += 1.0 / w1[IJK(i,j,k)];
                    frac += 1.0 / w3[IJK(i,j,k)];
                    numpts = 3;
                    if (i > 0) {
                        frac += 1.0 / w1[IJK(i-1,j,k)];
                        numpts += 1;
                    }
                    if (k > 0) {
                        frac += 1.0 / w3[IJK(i,j,k-1)];
                        numpts += 1;
                    }
                    if (j < (ny - 1)) {
                        frac += 1.0 / w1[IJK(i,j+1,k)];
                        frac += 1.0 / w3[IJK(i,j+1,k)];
                        numpts += 2;
                        if (i > 0) {
                            frac += 1.0 / w1[IJK(i-1,j+1,k)];
                            numpts += 1;
                        }
                        if (k > 0) {
                            frac += 1.0 / w3[IJK(i,j+1,k-1)];
                            numpts += 1;
                        }
                    }
                    epsy[IJK(i,j,k)] = numpts / frac;

                    frac = 1.0 / w3[IJK(i,j,k)];
                    frac += 1.0 / w1[IJK(i,j,k)];
                    frac += 1.0 / w2[IJK(i,j,k)];
                    numpts = 3;
                    if (i > 0) {
                        frac += 1.0 / w1[IJK(i-1,j,k)];
                        numpts += 1;
                    }
                    if (j > 0) {
                        frac += 1.0 / w2[IJK(i,j-1,k)];
                        numpts += 1;
                    }
                    if (k < (nz - 1)) {
                        frac += 1.0 / w1[IJK(i,j,k+1)];
                        frac += 1.0 / w2[IJK(i,j,k+1)];
                        numpts += 2;
                        if (i > 0) {
                            frac += 1.0 / w1[IJK(i-1,j,k+1)];
                            numpts += 1;
                        }
                        if (j > 0) {
                            frac += 1.0 / w2[IJK(i,j-1,k+1)];
                            numpts += 1;
                        }
                    }
                    epsz[IJK(i,j,k)] = numpts / frac;
                }
            }
        }
        free(w1);
        free(w2);
        free(w3);
    }

    ionmask = (ionstr > VPMGSMALL) ? 1.0 : 0.0;
    for (i = 0; i < ntot; i++) {
        kappa[i] = ionmask;
    }
    if (ionstr >= VPMGSMALL) {
        for (iatom = 0; iatom < Valist_getNumberAtoms(alist); iatom++) {
            atom = Valist_getAtom(alist, iatom);
            apos = Vatom_getPosition(atom);
            arad = Vatom_getRadius(atom);
            if (arad <= VSMALL) continue;
            if ((apos[0] < (xmin - irad - arad))
                || (apos[0] > (xmax + irad + arad))
                || (apos[1] < (ymin - irad - arad))
                || (apos[1] > (ymax + irad + arad))
                || (apos[2] < (zmin - irad - arad))
                || (apos[2] > (zmax + irad + arad))) {
                continue;
            }
            mark_sphere_oracle(irad + arad, apos, nx, ny, nz, hx, hy, hz,
                               xmin, ymin, zmin, kappa, 0.0);
        }
    }
}

static int run_fillco_case(const char *label, const AtomSpec *specs, int natoms,
                           int nx, int ny, int nz,
                           double hx, double hy, double hz,
                           double xcent, double ycent, double zcent,
                           Vbcfl bcfl, Vsurf_Meth surfMeth) {
    double ionc[2] = {0.15, 0.15};
    double ionr[2] = {2.0, 2.0};
    double ionq[2] = {1.0, -1.0};
    Valist *alist;
    Vpbe *pbe;
    MGparm *mgparm;
    Vpmgp *pmgp;
    Vpmg *pmg;
    size_t ntot;
    double *ref_epsx, *ref_epsy, *ref_epsz, *ref_kappa;
    int ok;

    alist = build_alist(specs, natoms);
    pbe = Vpbe_ctor(alist, 2, ionc, ionr, ionq, 298.15, 2.0, 78.54,
                    1.4, 0, 10.0, 0.0, 0.0, 2.0, 0.0);

    mgparm = MGparm_ctor(MCT_MANUAL);
    mgparm->dime[0] = nx;
    mgparm->dime[1] = ny;
    mgparm->dime[2] = nz;
    mgparm->grid[0] = hx;
    mgparm->grid[1] = hy;
    mgparm->grid[2] = hz;
    mgparm->nlev = 4;
    mgparm->nonlintype = NONLIN_LPBE;
    mgparm->method = VSOL_MG;
    mgparm->chgs = VCM_CHARGE;
    mgparm->useAqua = 0;

    pmgp = Vpmgp_ctor(mgparm);
    pmgp->xcent = xcent;
    pmgp->ycent = ycent;
    pmgp->zcent = zcent;
    pmgp->bcfl = BCFL_ZERO;

    pmg = Vpmg_ctor(pmgp, pbe, 0, VNULL, mgparm, PCE_NO);
    pmgp->bcfl = bcfl;

    if (!Vpmg_fillco(pmg, surfMeth, 0.3, VCM_TRIL,
                     0, VNULL, 0, VNULL, 0, VNULL,
                     0, VNULL, 0, VNULL, 0, VNULL)) {
        fprintf(stderr, "FAIL [%s]: Vpmg_fillco returned 0\n", label);
        return 0;
    }

    ntot = (size_t)nx * (size_t)ny * (size_t)nz;
    ref_epsx = (double *)malloc(ntot * sizeof(double));
    ref_epsy = (double *)malloc(ntot * sizeof(double));
    ref_epsz = (double *)malloc(ntot * sizeof(double));
    ref_kappa = (double *)malloc(ntot * sizeof(double));
    VASSERT(ref_epsx != NULL && ref_epsy != NULL
            && ref_epsz != NULL && ref_kappa != NULL);

    fill_mol_oracle(pbe, pmgp, surfMeth,
                    ref_epsx, ref_epsy, ref_epsz, ref_kappa);

    ok = 1;
    if (memcmp(pmg->epsx, ref_epsx, ntot * sizeof(double)) != 0) {
        fprintf(stderr, "FAIL [%s]: epsx mismatch vs brute-force oracle\n", label);
        ok = 0;
    }
    if (memcmp(pmg->epsy, ref_epsy, ntot * sizeof(double)) != 0) {
        fprintf(stderr, "FAIL [%s]: epsy mismatch vs brute-force oracle\n", label);
        ok = 0;
    }
    if (memcmp(pmg->epsz, ref_epsz, ntot * sizeof(double)) != 0) {
        fprintf(stderr, "FAIL [%s]: epsz mismatch vs brute-force oracle\n", label);
        ok = 0;
    }
    if (memcmp(pmg->kappa, ref_kappa, ntot * sizeof(double)) != 0) {
        fprintf(stderr, "FAIL [%s]: kappa mismatch vs brute-force oracle\n", label);
        ok = 0;
    }

    free(ref_epsx);
    free(ref_epsy);
    free(ref_epsz);
    free(ref_kappa);
    Vpmg_dtor(&pmg);
    Vpmgp_dtor(&pmgp);
    MGparm_dtor(&mgparm);
    Vpbe_dtor(&pbe);
    Valist_dtor(&alist);
    return ok;
}

int main(void) {
    /*
     * Case 1: 17x17x17 grid (hx=hy=hz=0.75, xmin=ymin=zmin=-6.0) with spheres
     * placed exactly tangent and +-1 ULP from shifted and unshifted grid planes.
     * For srad=1.4, arad=1.6 => rtot=3.0 = 4.0*h: atom at x = -2.625 has
     * shifted (xmin+0.5*hx = -5.625) plane i=0 at exact distance 3.0.
     */
    AtomSpec tangent_specs[5];
    AtomSpec aniso_specs[6] = {
        { 1.30, -2.10,  0.70, 1.85,  0.60},
        { 3.05, -0.80,  1.55, 1.50, -0.40},
        {-0.45, -3.40, -0.95, 1.70,  0.25},
        { 2.15, -1.45,  2.40, 1.35, -0.45},
        { 0.10, -0.50,  0.10, 2.10,  0.30},
        { 2.80, -3.10, -1.20, 1.60, -0.30}
    };
    /*
     * Focused 17x17x17 subgrid (hx=0.35 => [-2.8, +2.8]) with off-subgrid atoms
     * along +-x, +-y, and +-z whose SAS probe spheres exercise early k/j/i
     * out-of-slab returns as well as partial boundary grazing.
     */
    AtomSpec focus_specs[8] = {
        {  0.00,   0.00,   0.00, 1.80,  0.50},
        {  1.20,  -0.70,   0.90, 1.50, -0.50},
        { -9.50,   0.00,   0.00, 1.70,  0.20},
        {  9.50,   0.00,   0.00, 1.70, -0.20},
        {  0.00,  -9.50,   0.00, 1.70,  0.20},
        {  0.00,   9.50,   0.00, 1.70, -0.20},
        {  0.00,   0.00,  -9.50, 1.70,  0.20},
        {  0.00,   0.00,   4.15, 1.60, -0.20}
    };

    tangent_specs[0].x = -2.625;
    tangent_specs[0].y = 0.0;
    tangent_specs[0].z = 0.0;
    tangent_specs[0].radius = 1.6;
    tangent_specs[0].charge = 0.5;

    tangent_specs[1].x = -2.625 * (1.0 + DBL_EPSILON);
    tangent_specs[1].y = 1.5;
    tangent_specs[1].z = -0.75;
    tangent_specs[1].radius = 1.6;
    tangent_specs[1].charge = -0.5;

    tangent_specs[2].x = -2.625 * (1.0 - DBL_EPSILON);
    tangent_specs[2].y = -1.5;
    tangent_specs[2].z = 0.75;
    tangent_specs[2].radius = 1.6;
    tangent_specs[2].charge = 0.25;

    tangent_specs[3].x = 0.75;
    tangent_specs[3].y = 2.625;
    tangent_specs[3].z = -2.625 * (1.0 + 2.0 * DBL_EPSILON);
    tangent_specs[3].radius = 1.6;
    tangent_specs[3].charge = -0.25;

    tangent_specs[4].x = -0.5;
    tangent_specs[4].y = -0.5;
    tangent_specs[4].z = 1.875;
    tangent_specs[4].radius = 1.75;
    tangent_specs[4].charge = 0.0;

    if (!run_fillco_case("cubic_tangent_mol", tangent_specs, 5,
                         17, 17, 17, 0.75, 0.75, 0.75,
                         0.0, 0.0, 0.0, BCFL_ZERO, VSM_MOL)) {
        return 1;
    }
    if (!run_fillco_case("cubic_tangent_molsmooth", tangent_specs, 5,
                         17, 17, 17, 0.75, 0.75, 0.75,
                         0.0, 0.0, 0.0, BCFL_ZERO, VSM_MOLSMOOTH)) {
        return 1;
    }
    if (!run_fillco_case("aniso_33x25x17_mol", aniso_specs, 6,
                         33, 25, 17, 0.45, 0.65, 0.85,
                         1.3, -2.1, 0.7, BCFL_ZERO, VSM_MOL)) {
        return 1;
    }
    if (!run_fillco_case("aniso_33x25x17_molsmooth", aniso_specs, 6,
                         33, 25, 17, 0.45, 0.65, 0.85,
                         1.3, -2.1, 0.7, BCFL_ZERO, VSM_MOLSMOOTH)) {
        return 1;
    }
    if (!run_fillco_case("focused_offgrid_molsmooth", focus_specs, 8,
                         17, 17, 17, 0.35, 0.35, 0.35,
                         0.0, 0.0, 0.0, BCFL_FOCUS, VSM_MOLSMOOTH)) {
        return 1;
    }

    printf("vpmg_fill_test: all tests passed.\n");
    return 0;
}
