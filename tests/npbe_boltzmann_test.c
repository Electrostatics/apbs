/**
 * @file    npbe_boltzmann_test.c
 * @brief   Unit tests for fused NPBE Boltzmann evaluation and Vnmresid_dc
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "apbscfg.h"
#include "generic/vhal.h"
#include "generic/vmatrix.h"
#include "maloc/maloc.h"
#include "pmgc/matvecd.h"
#include "pmgc/mypdec.h"

static void ref_boltzmann(const double *coef, const double *uin,
                          double *c_ref, double *dc_ref,
                          int n, int nion,
                          const double *q, const double *conc) {
    const double zsmall = 1.0e-20;
    const double zlarge = 1.0e20;
    const double sinh_min = -85.0;
    const double sinh_max = 85.0;
    int i, iion;

    for (i = 0; i < n; i++) {
        c_ref[i] = 0.0;
        dc_ref[i] = 0.0;
    }
    for (iion = 0; iion < nion; iion++) {
        double zcf_c = -1.0 * conc[iion] * q[iion];
        double zcf_dc = conc[iion] * q[iion] * q[iion];
        double zu = -1.0 * q[iion];
        for (i = 0; i < n; i++) {
            double am_zero_c = VMIN2(zsmall, VABS(zcf_c * coef[i])) * zlarge;
            double am_zero_dc = VMIN2(zsmall, VABS(zcf_dc * coef[i])) * zlarge;
            double am_neg = VMAX2(VMIN2(zu * uin[i], 0.0), sinh_min);
            double am_pos = VMIN2(VMAX2(zu * uin[i], 0.0), sinh_max);
            c_ref[i] = c_ref[i] + zcf_c * coef[i] * exp(am_zero_c * (am_neg + am_pos));
            dc_ref[i] += zcf_dc * coef[i] * exp(am_zero_dc * (am_neg + am_pos));
        }
    }
}

static int test_vc_vdc_vc_dc_parity(void) {
    int nx = 17, ny = 17, nz = 17;
    int n = nx * ny * nz;
    int nion = 3;
    int ipkey = 0;
    double q[3] = {1.0, -2.0, 2.0};
    double conc[3] = {0.15, 0.05, 0.025};
    double *coef, *uin, *c_out, *dc_out, *cdc_c, *cdc_dc, *c_ref, *dc_ref;
    int i;

    coef = (double *)calloc((size_t)n, sizeof(double));
    uin = (double *)calloc((size_t)n, sizeof(double));
    c_out = (double *)calloc((size_t)n, sizeof(double));
    dc_out = (double *)calloc((size_t)n, sizeof(double));
    cdc_c = (double *)calloc((size_t)n, sizeof(double));
    cdc_dc = (double *)calloc((size_t)n, sizeof(double));
    c_ref = (double *)calloc((size_t)n, sizeof(double));
    dc_ref = (double *)calloc((size_t)n, sizeof(double));
    VASSERT(coef && uin && c_out && dc_out && cdc_c && cdc_dc && c_ref && dc_ref);

    for (i = 0; i < n; i++) {
        /* 40% interior solute nodes with coef == 0.0 */
        coef[i] = (i % 5 < 2) ? 0.0 : (0.25 + 0.01 * (double)(i % 19));
        if (i == 18) coef[i] = 3.0e-20;    /* Fractional am_zero_c != am_zero_dc for |q| = 2 */
        if (i == 10) uin[i] = 500.0;       /* Ion-excluded extreme potential (coef == 0) */
        else if (i == 12) uin[i] = 500.0;  /* Clamped at SINH_MAX / SINH_MIN (coef > 0) */
        else if (i == 13) uin[i] = -500.0; /* Clamped at SINH_MIN / SINH_MAX (coef > 0) */
        else if (i == 14) uin[i] = 0.0;
        else uin[i] = -3.5 + 7.0 * ((double)(i % 97) / 96.0);
    }

    Vmypdefinitnpbe(&nion, q, conc);
    ref_boltzmann(coef, uin, c_ref, dc_ref, n, nion, q, conc);

    Vc_vecpmg(coef, uin, c_out, &nx, &ny, &nz, &ipkey);
    Vdc_vecpmg(coef, uin, dc_out, &nx, &ny, &nz, &ipkey);
    Vc_dc_vecpmg(coef, uin, cdc_c, cdc_dc, &nx, &ny, &nz, &ipkey);

    if (memcmp(c_out, c_ref, (size_t)n * sizeof(double)) != 0 ||
        memcmp(dc_out, dc_ref, (size_t)n * sizeof(double)) != 0 ||
        memcmp(cdc_c, c_out, (size_t)n * sizeof(double)) != 0 ||
        memcmp(cdc_dc, dc_out, (size_t)n * sizeof(double)) != 0) {
        fprintf(stderr, "FAIL: Boltzmann vector evaluation mismatch\n");
        return 0;
    }

    free(coef); free(uin); free(c_out); free(dc_out);
    free(cdc_c); free(cdc_dc); free(c_ref); free(dc_ref);
    return 1;
}

static int test_vnmresid_dc_parity(void) {
    int nx = 17, ny = 17, nz = 17;
    int n = nx * ny * nz;
    int nion = 2;
    int ipkey = 0;
    double q[2] = {1.0, -1.0};
    double conc[2] = {0.15, 0.15};
    int ipc[100];
    double rpc[100];
    double *ac, *cc, *fc, *x, *r_ref, *w1_ref, *dc_ref, *r_got, *w1_got, *dc_got;
    int i, rc;

    memset(ipc, 0, sizeof(ipc));
    memset(rpc, 0, sizeof(rpc));
    VAT(ipc, 10) = 0; /* NPBE ipkey */
    VAT(ipc, 11) = 7; /* 7-point stencil */

    ac = (double *)calloc(14U * (size_t)n, sizeof(double));
    cc = (double *)calloc((size_t)n, sizeof(double));
    fc = (double *)calloc((size_t)n, sizeof(double));
    x = (double *)calloc((size_t)n, sizeof(double));
    r_ref = (double *)calloc((size_t)n, sizeof(double));
    w1_ref = (double *)calloc((size_t)n, sizeof(double));
    dc_ref = (double *)calloc((size_t)n, sizeof(double));
    r_got = (double *)calloc((size_t)n, sizeof(double));
    w1_got = (double *)calloc((size_t)n, sizeof(double));
    dc_got = (double *)calloc((size_t)n, sizeof(double));

    for (i = 0; i < 4 * n; i++) ac[i] = 1.25 + 0.01 * (double)(i % 13);
    for (i = 0; i < n; i++) {
        cc[i] = (i % 3 == 0) ? 0.0 : 0.75;
        fc[i] = -0.2 + 0.005 * (double)(i % 31);
        x[i] = (i == 14) ? 500.0 : (-1.2 + 0.03 * (double)(i % 79));
    }

    Vmypdefinitnpbe(&nion, q, conc);
    Vnmresid(&nx, &ny, &nz, ipc, rpc, ac, cc, fc, x, r_ref, w1_ref);
    Vdc_vec(cc, x, dc_ref, &nx, &ny, &nz, &ipkey);

    rc = Vnmresid_dc(&nx, &ny, &nz, ipc, rpc, ac, cc, fc, x, r_got, w1_got, dc_got);
    if (rc != 1 ||
        memcmp(r_got, r_ref, (size_t)n * sizeof(double)) != 0 ||
        memcmp(w1_got, w1_ref, (size_t)n * sizeof(double)) != 0 ||
        memcmp(dc_got, dc_ref, (size_t)n * sizeof(double)) != 0) {
        fprintf(stderr, "FAIL: Vnmresid_dc != Vnmresid + Vdc_vec\n");
        return 0;
    }

    /* Fallback when dc == VNULL returns 0 and still computes exact residual */
    memset(r_got, 0, (size_t)n * sizeof(double));
    rc = Vnmresid_dc(&nx, &ny, &nz, ipc, rpc, ac, cc, fc, x, r_got, w1_got, VNULL);
    if (rc != 0 || memcmp(r_got, r_ref, (size_t)n * sizeof(double)) != 0) {
        fprintf(stderr, "FAIL: Vnmresid_dc fallback with NULL dc\n");
        return 0;
    }

    /* Fallback when ipkey != 0 (e.g. SMPBE) or 27-point stencil returns 0 */
    VAT(ipc, 11) = 27;
    rc = Vnmresid_dc(&nx, &ny, &nz, ipc, rpc, ac, cc, fc, x, r_got, w1_got, dc_got);
    if (rc != 0) {
        fprintf(stderr, "FAIL: Vnmresid_dc 27-point fallback\n");
        return 0;
    }

    free(ac); free(cc); free(fc); free(x);
    free(r_ref); free(w1_ref); free(dc_ref);
    free(r_got); free(w1_got); free(dc_got);
    return 1;
}

int main(void) {
    Vnm_redirect(1);
    if (!test_vc_vdc_vc_dc_parity()) return 1;
    if (!test_vnmresid_dc_parity()) return 1;
    remove("io.mc");
    printf("npbe_boltzmann_test: ALL PASSED\n");
    return 0;
}
