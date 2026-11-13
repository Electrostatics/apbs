/**
 * @file    openmp_parity_test.c
 * @brief   Thread-count parity tests across OpenMP setup, solver, energy, and forces
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _OPENMP
#include <omp.h>
#endif

#include "apbscfg.h"
#include "generic/mgparm.h"
#include "generic/nosh.h"
#include "generic/pbeparm.h"
#include "generic/vacc.h"
#include "generic/valist.h"
#include "generic/vatom.h"
#include "generic/vhal.h"
#include "generic/vpbe.h"
#include "maloc/maloc.h"
#include "mg/vpmg.h"
#include "mg/vpmgp.h"
#include "routines.h"

#ifdef _OPENMP
typedef struct sRunResult {
    double sasa;
    double sav;
    double wca_sum;
    double lpbe_qm;
    double npbe_qm;
    double *epsx_smol;
    double *charge_spl0;
    double *charge_spl2;
    double *u_lpbe;
    double *u_npbe;
    AtomForce comps_force[40];
    AtomForce total_force;
} RunResult;

static Valist* make_asymmetric_molecule(void) {
    const int natoms = 40;
    Valist *alist;
    Vatom *atom;
    int i, d;

    alist = Valist_ctor();
    alist->number = natoms;
    alist->atoms = (Vatom *)Vmem_malloc(alist->vmem, (size_t)natoms, sizeof(Vatom));
    for (d = 0; d < 3; d++) {
        alist->mincrd[d] = VLARGE;
        alist->maxcrd[d] = -VLARGE;
    }
    alist->maxrad = 0.0;
    alist->charge = 0.0;

    for (i = 0; i < natoms; i++) {
        double px = -4.2 + 8.4 * ((double)((i * 17 + 3) % 41) / 40.0);
        double py = -4.1 + 8.2 * ((double)((i * 29 + 7) % 43) / 42.0);
        double pz = -4.0 + 8.0 * ((double)((i * 31 + 11) % 37) / 36.0);
        double rad = 1.45 + 0.55 * ((double)(i % 7) / 6.0);
        double chg = ((i % 2 == 0) ? 1.0 : -1.0) * (0.20 + 0.45 * ((double)(i % 5) / 4.0));

        atom = &(alist->atoms[i]);
        atom->position[0] = px;
        atom->position[1] = py;
        atom->position[2] = pz;
        Vatom_setRadius(atom, rad);
        Vatom_setCharge(atom, chg);
        Vatom_setEpsilon(atom, 0.55 + 0.03 * (double)(i % 9));
        Vatom_setPartID(atom, -1);
        Vatom_setAtomID(atom, i);
        for (d = 0; d < 3; d++) {
            if (atom->position[d] < alist->mincrd[d]) alist->mincrd[d] = atom->position[d];
            if (atom->position[d] > alist->maxcrd[d]) alist->maxcrd[d] = atom->position[d];
        }
        if (rad > alist->maxrad) alist->maxrad = rad;
        alist->charge += chg;
    }
    for (d = 0; d < 3; d++) {
        alist->center[d] = 0.5 * (alist->mincrd[d] + alist->maxcrd[d]);
    }
    return alist;
}

static void run_pipeline_at_threads(int nthreads, size_t ngrid, RunResult *out) {
    /* 65^3 = 274,625 (> PMG_OMP_MIN_N = 32,768, 5 blocks in Vpmg_qmEnergyNONLIN);
     * coarse level 1 = 33^3 = 35,937 (> PMG_OMP_MIN_N27 = 8,192), so PMG_OMP_BIG,
     * multi-block qmEnergyNONLIN, and Vgsrb27x wavefront all run! */
    Valist *alist_arr[NOSH_MAXMOL];
    double ionc[2] = {0.15, 0.15};
    double ionr[2] = {2.0, 2.0};
    double ionq[2] = {1.0, -1.0};
    double grid_sp[3] = {0.5, 0.5, 0.5};
    int dime[3] = {65, 65, 65};
    APOLparm *apolparm;
    Vpbe *pbe_lpbe, *pbe_npbe, *pbe_spl;
    MGparm *mgparm;
    PBEparm *pbeparm;
    Vpmgp *pmgp_lpbe, *pmgp_npbe, *pmgp_spl;
    Vpmg *pmg_lpbe, *pmg_npbe, *pmg_spl;
    NOsh *nosh;
    Vmem *mem;
    int nforce;
    AtomForce *af;
    int i;
    double wca_atom;

#ifdef _OPENMP
    omp_set_num_threads(nthreads);
#else
    (void)nthreads;
#endif

    memset(alist_arr, 0, sizeof(alist_arr));
    alist_arr[0] = make_asymmetric_molecule();

    pbe_lpbe = Vpbe_ctor(alist_arr[0], 2, ionc, ionr, ionq,
                         298.15, 2.0, 78.54, 1.4, 0, 10.0, 0.0, 0.0, 2.0, 0.0);
    out->sasa = Vacc_SASA(pbe_lpbe->acc, 1.4);

    apolparm = APOLparm_ctor();
    apolparm->srad = 1.4;
    apolparm->bconc = 0.033428;
    apolparm->sdens = 10.0;
    apolparm->dpos = 0.2;
    apolparm->grid[0] = 0.4;
    apolparm->grid[1] = 0.4;
    apolparm->grid[2] = 0.4;
    apolparm->setwat = 1;
    apolparm->watsigma = 3.165558;
    apolparm->watepsilon = 0.6501696;
    out->sav = Vacc_totalSAV(pbe_lpbe->acc, pbe_lpbe->clist, apolparm, 1.4);
    out->wca_sum = 0.0;
    for (i = 0; i < 40; i++) {
        wca_atom = 0.0;
        Vacc_wcaEnergyAtom(pbe_lpbe->acc, apolparm, alist_arr[0],
                           pbe_lpbe->clist, i, &wca_atom);
        out->wca_sum += wca_atom;
    }
    APOLparm_dtor(&apolparm);

    /* 1. LPBE srfm smol (65^3, nlev = 4 -> coarse level 1 is 33^3, level 2 is 17^3) */
    mgparm = MGparm_ctor(MCT_MANUAL);
    mgparm->dime[0] = dime[0]; mgparm->dime[1] = dime[1]; mgparm->dime[2] = dime[2];
    mgparm->grid[0] = grid_sp[0]; mgparm->grid[1] = grid_sp[1]; mgparm->grid[2] = grid_sp[2];
    mgparm->nlev = 4;
    mgparm->setnlev = 1;
    mgparm->chgm = VCM_BSPL2;
    mgparm->nonlintype = NONLIN_LPBE;
    mgparm->method = VSOL_MG;
    mgparm->setdime = 1; mgparm->setgrid = 1;

    pbeparm = PBEparm_ctor();
    pbeparm->molid = 1;
    pbeparm->pbetype = PBE_LPBE;
    pbeparm->bcfl = BCFL_SDH;
    pbeparm->srfm = VSM_MOLSMOOTH;
    pbeparm->srad = 1.4;
    pbeparm->swin = 0.3;
    pbeparm->temp = 298.15;
    pbeparm->pdie = 2.0;
    pbeparm->sdie = 78.54;

    pmgp_lpbe = Vpmgp_ctor(mgparm);
    pmgp_lpbe->bcfl = BCFL_SDH;
    pmg_lpbe = Vpmg_ctor(pmgp_lpbe, pbe_lpbe, 0, VNULL, mgparm, PCE_TOTAL);
    VASSERT(Vpmg_fillco(pmg_lpbe, VSM_MOLSMOOTH, 0.3, VCM_TRIL,
                        0, VNULL, 0, VNULL, 0, VNULL, 0, VNULL, 0, VNULL, 0, VNULL));
    memcpy(out->epsx_smol, pmg_lpbe->epsx, ngrid * sizeof(double));
    memcpy(out->charge_spl0, pmg_lpbe->charge, ngrid * sizeof(double));
    VASSERT(Vpmg_solve(pmg_lpbe));
    memcpy(out->u_lpbe, pmg_lpbe->u, ngrid * sizeof(double));
    out->lpbe_qm = Vpmg_qmEnergy(pmg_lpbe, 0);

    Vpmg_dtor(&pmg_lpbe);
    Vpmgp_dtor(&pmgp_lpbe);
    Vpbe_dtor(&pbe_lpbe);

    /* 2. NPBE srfm spl2 (65^3, nlev = 4) + polar forces (PCF_COMPS and PCF_TOTAL) */
    mgparm->nonlintype = NONLIN_NPBE;
    mgparm->method = VSOL_Newton;
    pbeparm->pbetype = PBE_NPBE;
    pbeparm->srfm = VSM_SPLINE;

    pbe_npbe = Vpbe_ctor(alist_arr[0], 2, ionc, ionr, ionq,
                         298.15, 2.0, 78.54, 1.4, 0, 10.0, 0.0, 0.0, 2.0, 0.0);
    pmgp_npbe = Vpmgp_ctor(mgparm);
    pmgp_npbe->bcfl = BCFL_SDH;
    pmg_npbe = Vpmg_ctor(pmgp_npbe, pbe_npbe, 0, VNULL, mgparm, PCE_TOTAL);
    VASSERT(Vpmg_fillco(pmg_npbe, VSM_SPLINE, 0.3, VCM_BSPL2,
                        0, VNULL, 0, VNULL, 0, VNULL, 0, VNULL, 0, VNULL, 0, VNULL));
    memcpy(out->charge_spl2, pmg_npbe->charge, ngrid * sizeof(double));
    VASSERT(Vpmg_solve(pmg_npbe));
    memcpy(out->u_npbe, pmg_npbe->u, ngrid * sizeof(double));
    out->npbe_qm = Vpmg_qmEnergy(pmg_npbe, 0);

    pbe_spl = pbe_npbe;
    pmgp_spl = pmgp_npbe;
    pmg_spl = pmg_npbe;
    nosh = NOsh_ctor(0, 1);
    mem = Vmem_ctor("FORCE_TEST");

    pbeparm->calcforce = PCF_COMPS;
    nforce = 0;
    af = VNULL;
    VASSERT(forceMG(mem, nosh, pbeparm, mgparm, pmg_spl, &nforce, &af, alist_arr) == 1);
    VASSERT(nforce == 40 && af != VNULL);
    memcpy(out->comps_force, af, 40U * sizeof(AtomForce));
    Vmem_free(mem, (size_t)nforce, sizeof(AtomForce), (void **)&af);

    pbeparm->calcforce = PCF_TOTAL;
    nforce = 0;
    af = VNULL;
    VASSERT(forceMG(mem, nosh, pbeparm, mgparm, pmg_spl, &nforce, &af, alist_arr) == 1);
    VASSERT(nforce == 1 && af != VNULL);
    out->total_force = af[0];
    Vmem_free(mem, 1U, sizeof(AtomForce), (void **)&af);

    Vmem_dtor(&mem);
    NOsh_dtor(&nosh);
    Vpmg_dtor(&pmg_spl);
    Vpmgp_dtor(&pmgp_spl);
    Vpbe_dtor(&pbe_spl);
    PBEparm_dtor(&pbeparm);
    MGparm_dtor(&mgparm);
    Valist_dtor(&(alist_arr[0]));
}

#endif /* _OPENMP */

int main(void) {
#ifndef _OPENMP
    printf("openmp_parity_test: SKIP (OpenMP disabled)\n");
    return 77;
#else
    const size_t ngrid = 65U * 65U * 65U;
    RunResult r1, r4;
    int nt = omp_get_num_procs();

    if (nt > 4) nt = 4;
    if (nt < 2) {
        printf("openmp_parity_test: SKIP (need >= 2 processors, got %d)\n", nt);
        return 77;
    }

    Vnm_redirect(1);
    memset(&r1, 0, sizeof(r1));
    memset(&r4, 0, sizeof(r4));
    r1.epsx_smol = (double *)calloc(ngrid, sizeof(double));
    r1.charge_spl0 = (double *)calloc(ngrid, sizeof(double));
    r1.charge_spl2 = (double *)calloc(ngrid, sizeof(double));
    r1.u_lpbe = (double *)calloc(ngrid, sizeof(double));
    r1.u_npbe = (double *)calloc(ngrid, sizeof(double));
    r4.epsx_smol = (double *)calloc(ngrid, sizeof(double));
    r4.charge_spl0 = (double *)calloc(ngrid, sizeof(double));
    r4.charge_spl2 = (double *)calloc(ngrid, sizeof(double));
    r4.u_lpbe = (double *)calloc(ngrid, sizeof(double));
    r4.u_npbe = (double *)calloc(ngrid, sizeof(double));

    run_pipeline_at_threads(1, ngrid, &r1);
    run_pipeline_at_threads(nt, ngrid, &r4);

    if (memcmp(&r1.sasa, &r4.sasa, sizeof(double)) != 0 ||
        memcmp(&r1.sav, &r4.sav, sizeof(double)) != 0 ||
        memcmp(&r1.wca_sum, &r4.wca_sum, sizeof(double)) != 0 ||
        memcmp(&r1.lpbe_qm, &r4.lpbe_qm, sizeof(double)) != 0 ||
        memcmp(&r1.npbe_qm, &r4.npbe_qm, sizeof(double)) != 0) {
        fprintf(stderr, "FAIL: scalar SASA/SAV/WCA/qmEnergy parity mismatch (1t vs 4t)\n");
        return 1;
    }
    if (memcmp(r1.epsx_smol, r4.epsx_smol, ngrid * sizeof(double)) != 0 ||
        memcmp(r1.charge_spl0, r4.charge_spl0, ngrid * sizeof(double)) != 0 ||
        memcmp(r1.charge_spl2, r4.charge_spl2, ngrid * sizeof(double)) != 0 ||
        memcmp(r1.u_lpbe, r4.u_lpbe, ngrid * sizeof(double)) != 0 ||
        memcmp(r1.u_npbe, r4.u_npbe, ngrid * sizeof(double)) != 0) {
        fprintf(stderr, "FAIL: grid array parity mismatch (1t vs 4t)\n");
        return 1;
    }
    if (memcmp(r1.comps_force, r4.comps_force, sizeof(r1.comps_force)) != 0) {
        fprintf(stderr, "FAIL: PCF_COMPS force parity mismatch (1t vs 4t)\n");
        return 1;
    }
    if (memcmp(&r1.total_force, &r4.total_force, sizeof(AtomForce)) != 0) {
        fprintf(stderr, "FAIL: PCF_TOTAL force parity mismatch (1t vs 4t)\n");
        return 1;
    }

    free(r1.epsx_smol); free(r1.charge_spl0); free(r1.charge_spl2); free(r1.u_lpbe); free(r1.u_npbe);
    free(r4.epsx_smol); free(r4.charge_spl0); free(r4.charge_spl2); free(r4.u_lpbe); free(r4.u_npbe);
    remove("io.mc");
    printf("openmp_parity_test: ALL PASSED (1 vs %d threads)\n", nt);
    return 0;
#endif
}
