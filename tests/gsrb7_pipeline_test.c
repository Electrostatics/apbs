/**
 * @file    gsrb7_pipeline_test.c
 * @brief   Unit and integration tests for pipelined Vgsrb7x and Vmvcs residual reuse
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
#include "generic/pbeparm.h"
#include "generic/valist.h"
#include "generic/vatom.h"
#include "generic/vhal.h"
#include "generic/vpbe.h"
#include "maloc/maloc.h"
#include "mg/vpmg.h"
#include "mg/vpmgp.h"
#include "pmgc/gsd.h"
#include "pmgc/matvecd.h"

/* Exact serial reference matching upstream Vgsrb7x and Vmresid7_1s term order. */
static void ref_gsrb7x(int nx, int ny, int nz,
        const double *oC, const double *cc, const double *fc,
        const double *oE, const double *oN, const double *uC,
        double *x, double *r,
        int itmax, int *iters, int iresid, int iadjoint) {

    int i, j, k, ioff;
    size_t sx, sxy, idx;

    sx = (size_t)nx;
    sxy = sx * (size_t)ny;

    for (*iters = 1; *iters <= itmax; (*iters)++) {
        /* Red (or black when iadjoint == 1) pass */
        for (k = 2; k <= nz - 1; k++) {
            for (j = 2; j <= ny - 1; j++) {
                ioff = (1 - iadjoint) * ((j + k + 2) % 2)
                     + (    iadjoint) * (1 - (j + k + 2) % 2);
                for (i = 2 + ioff; i <= nx - 1; i += 2) {
                    idx = (size_t)(k - 1) * sxy + (size_t)(j - 1) * sx + (size_t)(i - 1);
                    x[idx] = (fc[idx]
                            + oN[idx]       * x[idx + sx]
                            + oN[idx - sx]  * x[idx - sx]
                            + oE[idx]       * x[idx + 1]
                            + oE[idx - 1]   * x[idx - 1]
                            + uC[idx - sxy] * x[idx - sxy]
                            + uC[idx]       * x[idx + sxy]) / (oC[idx] + cc[idx]);
                }
            }
        }
        /* Black (or red when iadjoint == 1) pass */
        for (k = 2; k <= nz - 1; k++) {
            for (j = 2; j <= ny - 1; j++) {
                ioff = (    iadjoint) * ((j + k + 2) % 2)
                     + (1 - iadjoint) * (1 - (j + k + 2) % 2);
                for (i = 2 + ioff; i <= nx - 1; i += 2) {
                    idx = (size_t)(k - 1) * sxy + (size_t)(j - 1) * sx + (size_t)(i - 1);
                    x[idx] = (fc[idx]
                            + oN[idx]       * x[idx + sx]
                            + oN[idx - sx]  * x[idx - sx]
                            + oE[idx]       * x[idx + 1]
                            + oE[idx - 1]   * x[idx - 1]
                            + uC[idx - sxy] * x[idx - sxy]
                            + uC[idx]       * x[idx + sxy]) / (oC[idx] + cc[idx]);
                }
            }
        }
    }

    if (iresid == 1) {
        for (k = 2; k <= nz - 1; k++) {
            for (j = 2; j <= ny - 1; j++) {
                for (i = 2; i <= nx - 1; i++) {
                    idx = (size_t)(k - 1) * sxy + (size_t)(j - 1) * sx + (size_t)(i - 1);
                    r[idx] = fc[idx]
                           + oN[idx]       * x[idx + sx]
                           + oN[idx - sx]  * x[idx - sx]
                           + oE[idx]       * x[idx + 1]
                           + oE[idx - 1]   * x[idx - 1]
                           + uC[idx - sxy] * x[idx - sxy]
                           + uC[idx]       * x[idx + sxy]
                           - (oC[idx] + cc[idx]) * x[idx];
                }
            }
        }
    }
}

static double det_val(size_t idx, unsigned int salt) {
    unsigned int h;
    h = (unsigned int)((idx * 2654435761U) ^ (salt * 2246822519U));
    h ^= (h >> 16);
    h *= 2246822507U;
    h ^= (h >> 13);
    return ((double)(h & 0xFFFFU) / 65535.0) - 0.5;
}

static int check_gsrb7_case(int nx, int ny, int nz,
        int itmax, int iadjoint, int iresid, int nthreads) {

    size_t n, idx;
    int i, j, k;
    int ipc[16];
    double rpc[4];
    double *oC, *cc, *fc, *oE, *oN, *uC;
    double *x_ref, *x_got, *r_ref, *r_got, *w1, *w2;
    double errtol, omega;
    int iters_ref, iters_got;
    int nx_a, ny_a, nz_a, itmax_a, iresid_a, iadj_a;
    int ok;

#ifdef _OPENMP
    omp_set_num_threads(nthreads);
#else
    (void)nthreads;
#endif

    n = (size_t)nx * (size_t)ny * (size_t)nz;
    oC    = (double *)malloc(n * sizeof(double));
    cc    = (double *)malloc(n * sizeof(double));
    fc    = (double *)malloc(n * sizeof(double));
    oE    = (double *)malloc(n * sizeof(double));
    oN    = (double *)malloc(n * sizeof(double));
    uC    = (double *)malloc(n * sizeof(double));
    x_ref = (double *)malloc(n * sizeof(double));
    x_got = (double *)malloc(n * sizeof(double));
    r_ref = (double *)malloc(n * sizeof(double));
    r_got = (double *)malloc(n * sizeof(double));
    w1    = (double *)malloc(n * sizeof(double));
    w2    = (double *)malloc(n * sizeof(double));
    if (!oC || !cc || !fc || !oE || !oN || !uC ||
        !x_ref || !x_got || !r_ref || !r_got || !w1 || !w2) {
        fprintf(stderr, "FAIL: malloc failed for %dx%dx%d\n", nx, ny, nz);
        return 0;
    }

    memset(ipc, 0, sizeof(ipc));
    memset(rpc, 0, sizeof(rpc));
    ipc[10] = 7;

    for (k = 0; k < nz; k++) {
        for (j = 0; j < ny; j++) {
            for (i = 0; i < nx; i++) {
                int bnd;
                idx = ((size_t)k * (size_t)ny + (size_t)j) * (size_t)nx + (size_t)i;
                bnd = (i == 0 || i == nx - 1 || j == 0 || j == ny - 1 || k == 0 || k == nz - 1);
                oE[idx] = 0.25 + 0.15 * det_val(idx, 1U);
                oN[idx] = 0.25 + 0.15 * det_val(idx, 2U);
                uC[idx] = 0.25 + 0.15 * det_val(idx, 3U);
                oC[idx] = 4.5 + 0.5 * det_val(idx, 4U);
                cc[idx] = 0.2 + 0.1 * det_val(idx, 5U);
                fc[idx] = det_val(idx, 6U);
                x_ref[idx] = bnd ? (100.0 + (double)(idx % 37U)) : det_val(idx, 7U);
                x_got[idx] = x_ref[idx];
                r_ref[idx] = -777.25 + (double)(idx % 19U);
                r_got[idx] = r_ref[idx];
                w1[idx] = 0.0;
                w2[idx] = 0.0;
            }
        }
    }

    iters_ref = -1;
    iters_got = -1;
    errtol = 0.0;
    omega = 1.0;
    nx_a = nx;
    ny_a = ny;
    nz_a = nz;
    itmax_a = itmax;
    iresid_a = iresid;
    iadj_a = iadjoint;

    ref_gsrb7x(nx, ny, nz, oC, cc, fc, oE, oN, uC, x_ref, r_ref,
               itmax, &iters_ref, iresid, iadjoint);
    Vgsrb7x(&nx_a, &ny_a, &nz_a, ipc, rpc, oC, cc, fc, oE, oN, uC,
            x_got, w1, w2, r_got, &itmax_a, &iters_got, &errtol, &omega,
            &iresid_a, &iadj_a);

    ok = 1;
    if (iters_got != iters_ref) {
        fprintf(stderr, "FAIL: iters mismatch (%d vs %d) on %dx%dx%d itmax=%d iadj=%d iresid=%d nt=%d\n",
                iters_got, iters_ref, nx, ny, nz, itmax, iadjoint, iresid, nthreads);
        ok = 0;
    }
    if (ok && memcmp(x_got, x_ref, n * sizeof(double)) != 0) {
        fprintf(stderr, "FAIL: x bit mismatch on %dx%dx%d itmax=%d iadj=%d iresid=%d nt=%d\n",
                nx, ny, nz, itmax, iadjoint, iresid, nthreads);
        ok = 0;
    }
    if (ok && memcmp(r_got, r_ref, n * sizeof(double)) != 0) {
        fprintf(stderr, "FAIL: r bit mismatch on %dx%dx%d itmax=%d iadj=%d iresid=%d nt=%d\n",
                nx, ny, nz, itmax, iadjoint, iresid, nthreads);
        ok = 0;
    }

    free(oC); free(cc); free(fc); free(oE); free(oN); free(uC);
    free(x_ref); free(x_got); free(r_ref); free(r_got); free(w1); free(w2);
    return ok;
}

static Valist* make_small_molecule(void) {
    const int natoms = 12;
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
        double px = -2.4 + 4.8 * ((double)((i * 7 + 2) % 13) / 12.0);
        double py = -2.3 + 4.6 * ((double)((i * 11 + 3) % 13) / 12.0);
        double pz = -2.2 + 4.4 * ((double)((i * 5 + 1) % 11) / 10.0);
        double rad = 1.45 + 0.40 * ((double)(i % 5) / 4.0);
        double chg = ((i % 2 == 0) ? 1.0 : -1.0) * (0.25 + 0.35 * ((double)(i % 4) / 3.0));

        atom = &(alist->atoms[i]);
        atom->position[0] = px;
        atom->position[1] = py;
        atom->position[2] = pz;
        Vatom_setRadius(atom, rad);
        Vatom_setCharge(atom, chg);
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

static int run_lpbe_solve(int dim, int nlev, int istop, int nthreads,
        double *u_out, double *energy_out) {

    double ionc[2] = {0.15, 0.15};
    double ionr[2] = {2.0, 2.0};
    double ionq[2] = {1.0, -1.0};
    Valist *alist;
    Vpbe *pbe;
    MGparm *mgparm;
    Vpmgp *pmgp;
    Vpmg *pmg;
    size_t ngrid;
    int rc;

#ifdef _OPENMP
    omp_set_num_threads(nthreads);
#else
    (void)nthreads;
#endif

    ngrid = (size_t)dim * (size_t)dim * (size_t)dim;
    alist = make_small_molecule();
    pbe = Vpbe_ctor(alist, 2, ionc, ionr, ionq,
                    298.15, 2.0, 78.54, 1.4, 0, 10.0, 0.0, 0.0, 2.0, 0.0);

    mgparm = MGparm_ctor(MCT_MANUAL);
    mgparm->dime[0] = dim; mgparm->dime[1] = dim; mgparm->dime[2] = dim;
    mgparm->grid[0] = 0.5; mgparm->grid[1] = 0.5; mgparm->grid[2] = 0.5;
    mgparm->nlev = nlev;
    mgparm->setnlev = 1;
    mgparm->chgm = VCM_BSPL2;
    mgparm->nonlintype = NONLIN_LPBE;
    mgparm->method = VSOL_MG;
    mgparm->setdime = 1;
    mgparm->setgrid = 1;

    pmgp = Vpmgp_ctor(mgparm);
    pmgp->bcfl = BCFL_SDH;
    pmgp->istop = istop;
    pmgp->mgsmoo = 1;
    pmgp->errtol = 1.0e-6;
    pmg = Vpmg_ctor(pmgp, pbe, 0, VNULL, mgparm, PCE_TOTAL);

    rc = Vpmg_fillco(pmg, VSM_MOLSMOOTH, 0.3, VCM_BSPL2,
                     0, VNULL, 0, VNULL, 0, VNULL, 0, VNULL, 0, VNULL, 0, VNULL);
    if (!rc) {
        fprintf(stderr, "FAIL: Vpmg_fillco failed\n");
        return 0;
    }
    rc = Vpmg_solve(pmg);
    if (!rc) {
        fprintf(stderr, "FAIL: Vpmg_solve failed (dim=%d nlev=%d istop=%d)\n", dim, nlev, istop);
        return 0;
    }

    *energy_out = Vpmg_qmEnergy(pmg, 0);
    if (u_out != VNULL) {
        memcpy(u_out, pmg->u, ngrid * sizeof(double));
    }

    Vpmg_dtor(&pmg);
    Vpmgp_dtor(&pmgp);
    MGparm_dtor(&mgparm);
    Vpbe_dtor(&pbe);
    Valist_dtor(&alist);
    return 1;
}

int main(void) {
    static const int small_nz[] = {2, 3, 4, 5, 6, 7, 8, 9, 13};
    static const int big_grids[][3] = {
        {129, 97, 3},   /* nz_int = 1: empty slabs on threads >= 1 */
        {129, 65, 4},   /* nz_int = 2: k1 == k0 + 1 on 1t, empty slabs on 4t */
        {129, 65, 5},   /* nz_int = 3: k1 == k0 + 2 on 1t, 1-plane slabs on 4t */
        {65,  65, 9},   /* nz_int = 7: slabs of 2,2,2,1 on 4t */
        {65,  65, 11},  /* nz_int = 9: slabs of 3,2,2,2 on 4t */
        {33,  33, 33}   /* nz_int = 31: typical coarse/fine cubic grid */
    };
    static const int big_modes[][3] = {
        {2, 0, 1},
        {2, 1, 1},
        {1, 1, 1},
        {2, 0, 0},
        {0, 0, 1}
    };
    int idx_z, idx_g, idx_m, itmax, iadj, iresid;
    int nt_max;
    double *u1, *u2;
    double e0, e1_t1, e1_tn, e2, e_1lev;
    size_t n33;

    Vnm_redirect(1);

#ifdef _OPENMP
    nt_max = omp_get_num_procs();
    if (nt_max > 4) nt_max = 4;
    if (nt_max < 1) nt_max = 1;
#else
    nt_max = 1;
#endif

    /* 1. Exhaustive small-grid slab edge cases (serial path below PMG_OMP_MIN_N) */
    for (idx_z = 0; idx_z < (int)(sizeof(small_nz) / sizeof(small_nz[0])); idx_z++) {
        for (itmax = 0; itmax <= 3; itmax++) {
            for (iadj = 0; iadj <= 1; iadj++) {
                for (iresid = 0; iresid <= 1; iresid++) {
                    if (!check_gsrb7_case(9, 7, small_nz[idx_z], itmax, iadj, iresid, 1)) {
                        return 1;
                    }
                }
            }
        }
    }

    /* 2. Grids above PMG_OMP_MIN_N (32768) at 1 thread and multi-thread */
    for (idx_g = 0; idx_g < (int)(sizeof(big_grids) / sizeof(big_grids[0])); idx_g++) {
        for (idx_m = 0; idx_m < (int)(sizeof(big_modes) / sizeof(big_modes[0])); idx_m++) {
            if (!check_gsrb7_case(big_grids[idx_g][0], big_grids[idx_g][1], big_grids[idx_g][2],
                                  big_modes[idx_m][0], big_modes[idx_m][1], big_modes[idx_m][2], 1)) {
                return 1;
            }
            if (nt_max >= 2) {
                if (!check_gsrb7_case(big_grids[idx_g][0], big_grids[idx_g][1], big_grids[idx_g][2],
                                      big_modes[idx_m][0], big_modes[idx_m][1], big_modes[idx_m][2], nt_max)) {
                    return 1;
                }
            }
        }
    }

    /* 3. End-to-end Vmvcs residual-reuse checks (istop = 0, 1, 2 and nlev = 1 fallback) */
    n33 = 33U * 33U * 33U;
    u1 = (double *)calloc(n33, sizeof(double));
    u2 = (double *)calloc(n33, sizeof(double));
    if (!u1 || !u2) {
        fprintf(stderr, "FAIL: calloc failed\n");
        return 1;
    }

    if (!run_lpbe_solve(33, 4, 0, 1, VNULL, &e0) || !(e0 == e0)) {
        fprintf(stderr, "FAIL: LPBE istop=0 solve failed\n");
        return 1;
    }
    if (!run_lpbe_solve(33, 4, 1, 1, u1, &e1_t1) || !(e1_t1 == e1_t1)) {
        fprintf(stderr, "FAIL: LPBE istop=1 (1t) solve failed\n");
        return 1;
    }
    if (nt_max >= 2) {
        if (!run_lpbe_solve(33, 4, 1, nt_max, u2, &e1_tn)) {
            fprintf(stderr, "FAIL: LPBE istop=1 (%dt) solve failed\n", nt_max);
            return 1;
        }
        if (memcmp(&e1_t1, &e1_tn, sizeof(double)) != 0 ||
            memcmp(u1, u2, n33 * sizeof(double)) != 0) {
            fprintf(stderr, "FAIL: LPBE istop=1 thread parity mismatch (1t vs %dt)\n", nt_max);
            return 1;
        }
    }
    if (!run_lpbe_solve(33, 4, 2, 1, VNULL, &e2) || !(e2 == e2)) {
        fprintf(stderr, "FAIL: LPBE istop=2 fallback solve failed\n");
        return 1;
    }
    if (!run_lpbe_solve(9, 1, 1, 1, VNULL, &e_1lev) || !(e_1lev == e_1lev)) {
        fprintf(stderr, "FAIL: LPBE nlev=1 fallback solve failed\n");
        return 1;
    }

    free(u1);
    free(u2);
    remove("io.mc");
    printf("gsrb7_pipeline_test: ALL PASSED (max_threads=%d)\n", nt_max);
    return 0;
}
