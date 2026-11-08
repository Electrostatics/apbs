/**
 * @file    pmg_trilin_test.c
 * @brief   Unit tests for fixed-weight trilinear Vrestrc and VinterpPMG fast paths
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "apbscfg.h"
#include "generic/vhal.h"
#include "maloc/maloc.h"
#include "pmgc/buildPd.h"
#include "pmgc/matvecd.h"

static unsigned long rng_hi = 0x9e3779b9UL;
static unsigned long rng_lo = 0x7f4a7c15UL;

static double next_unit_val(void) {
    rng_hi = (rng_hi * 1664525UL + 1013904223UL) & 0xffffffffUL;
    rng_lo = (rng_lo * 22695477UL + 1UL) & 0xffffffffUL;
    return ((double)(rng_hi >> 5) * 67108864.0 + (double)(rng_lo >> 6))
           * (2.0 / 9007199254740992.0) - 1.0;
}

static int test_trilinear_restrc_interp(void) {
    int shapes[3][3] = {
        {17, 17, 17},
        {33, 33, 33},
        {65, 49, 33}
    };
    int s;

    for (s = 0; s < 3; s++) {
        int nxf, nyf, nzf, nxc, nyc, nzc;
        size_t nf, nc, idx;
        double *pc;
        double *xf_in1, *xf_in2;
        double *xc_fast, *xc_ref;
        double *xc_in1, *xc_in2;
        double *xf_fast, *xf_ref;
        double *ac_dummy;
        int ipc_dummy[100];
        double rpc_dummy[100];
        double v;

        nxf = shapes[s][0];
        nyf = shapes[s][1];
        nzf = shapes[s][2];
        nxc = (nxf - 1) / 2 + 1;
        nyc = (nyf - 1) / 2 + 1;
        nzc = (nzf - 1) / 2 + 1;
        nf = (size_t)nxf * (size_t)nyf * (size_t)nzf;
        nc = (size_t)nxc * (size_t)nyc * (size_t)nzc;

        pc = (double *)calloc(27 * nc, sizeof(double));
        xf_in1 = (double *)calloc(nf, sizeof(double));
        xf_in2 = (double *)calloc(nf, sizeof(double));
        xc_fast = (double *)calloc(nc, sizeof(double));
        xc_ref = (double *)calloc(nc, sizeof(double));
        xc_in1 = (double *)calloc(nc, sizeof(double));
        xc_in2 = (double *)calloc(nc, sizeof(double));
        xf_fast = (double *)calloc(nf, sizeof(double));
        xf_ref = (double *)calloc(nf, sizeof(double));
        ac_dummy = (double *)calloc(14 * nf, sizeof(double));
        VASSERT(pc != NULL && xf_in1 != NULL && xf_in2 != NULL
                && xc_fast != NULL && xc_ref != NULL
                && xc_in1 != NULL && xc_in2 != NULL
                && xf_fast != NULL && xf_ref != NULL && ac_dummy != NULL);

        memset(ipc_dummy, 0, sizeof(ipc_dummy));
        memset(rpc_dummy, 0, sizeof(rpc_dummy));

        for (idx = 0; idx < nf; idx++) {
            v = next_unit_val();
            xf_in1[idx] = v;
            xf_in2[idx] = v;
            /* Non-zero destination seed to verify boundary slice handling */
            v = next_unit_val();
            xf_fast[idx] = v;
            xf_ref[idx] = v;
        }
        for (idx = 0; idx < nc; idx++) {
            v = next_unit_val();
            xc_in1[idx] = v;
            xc_in2[idx] = v;
            v = next_unit_val();
            xc_fast[idx] = v;
            xc_ref[idx] = v;
        }

        VbuildP_trilin(&nxf, &nyf, &nzf, &nxc, &nyc, &nzc,
                       pc, VNULL, VNULL, VNULL);
        if (pc[0] != -1.0) {
            fprintf(stderr, "FAIL: VbuildP_trilin did not set pc[0] = -1.0\n");
            return 0;
        }

        /* Fast path (pc[0] == -1.0) vs general Vrestrc2 / VinterpPMG2 (pc[0] = 0.0) */
        Vrestrc(&nxf, &nyf, &nzf, &nxc, &nyc, &nzc, xf_in1, xc_fast, pc);
        VinterpPMG(&nxc, &nyc, &nzc, &nxf, &nyf, &nzf, xc_in1, xf_fast, pc);

        pc[0] = 0.0;
        Vrestrc(&nxf, &nyf, &nzf, &nxc, &nyc, &nzc, xf_in2, xc_ref, pc);
        VinterpPMG(&nxc, &nyc, &nzc, &nxf, &nyf, &nzf, xc_in2, xf_ref, pc);

        if (memcmp(xc_fast, xc_ref, nc * sizeof(double)) != 0) {
            fprintf(stderr,
                    "FAIL: Vrestrc trilinear != Vrestrc2 on %dx%dx%d\n",
                    nxf, nyf, nzf);
            return 0;
        }
        if (memcmp(xf_fast, xf_ref, nf * sizeof(double)) != 0) {
            fprintf(stderr,
                    "FAIL: VinterpPMG trilinear != VinterpPMG2 on %dx%dx%d\n",
                    nxf, nyf, nzf);
            return 0;
        }

        /* Operator-based builders must clear pc[0] = 0.0 */
        for (idx = 0; idx < 14 * nf; idx++) {
            ac_dummy[idx] = 1.0;
        }
        pc[0] = -1.0;
        VbuildP_op7(&nxf, &nyf, &nzf, &nxc, &nyc, &nzc,
                    ipc_dummy, rpc_dummy, ac_dummy, pc);
        if (pc[0] != 0.0) {
            fprintf(stderr, "FAIL: VbuildP_op7 did not clear pc[0] to 0.0\n");
            return 0;
        }
        pc[0] = -1.0;
        VbuildP_op27(&nxf, &nyf, &nzf, &nxc, &nyc, &nzc,
                     ipc_dummy, rpc_dummy, ac_dummy, pc);
        if (pc[0] != 0.0) {
            fprintf(stderr, "FAIL: VbuildP_op27 did not clear pc[0] to 0.0\n");
            return 0;
        }

        free(pc);
        free(xf_in1);
        free(xf_in2);
        free(xc_fast);
        free(xc_ref);
        free(xc_in1);
        free(xc_in2);
        free(xf_fast);
        free(xf_ref);
        free(ac_dummy);
    }
    return 1;
}

int main(void) {
    if (!test_trilinear_restrc_interp()) {
        return 1;
    }
    printf("pmg_trilin_test: all tests passed.\n");
    return 0;
}
