/**
 * @file    vgrid_dx_test.c
 * @brief   Unit tests for buffered OpenDX formatting in Vgrid_writeDX
 */

#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "apbscfg.h"
#include "generic/vhal.h"
#include "maloc/maloc.h"
#include "mg/vgrid.h"

static uint64_t rng_state = 0x20261107ULL;

static uint64_t xorshift64(void) {
    uint64_t x = rng_state;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    rng_state = x;
    return x;
}

static double rand_u01(void) {
    return (double)(xorshift64() >> 11) * (1.0 / 9007199254740992.0);
}

static char* extract_dx_payload(const char *path, size_t *out_len) {
    FILE *fp;
    long sz;
    char *buf;
    size_t nread;
    char *marker;
    char *start;

    fp = fopen(path, "rb");
    if (fp == NULL) return NULL;
    if (fseek(fp, 0, SEEK_END) != 0) { fclose(fp); return NULL; }
    sz = ftell(fp);
    if (sz <= 0) { fclose(fp); return NULL; }
    if (fseek(fp, 0, SEEK_SET) != 0) { fclose(fp); return NULL; }

    buf = (char *)malloc((size_t)sz + 1);
    if (buf == NULL) { fclose(fp); return NULL; }
    nread = fread(buf, 1, (size_t)sz, fp);
    fclose(fp);
    buf[nread] = '\0';

    marker = strstr(buf, "data follows\n");
    if (marker == NULL) { free(buf); return NULL; }
    start = marker + strlen("data follows\n");
    marker = strstr(start, "attribute \"dep\"");
    if (marker == NULL) { free(buf); return NULL; }

    *out_len = (size_t)(marker - start);
    memmove(buf, start, *out_len);
    buf[*out_len] = '\0';
    return buf;
}

static char* format_expected_dx(const double *data, const double *pvec,
                                int nx, int ny, int nz, size_t *out_len) {
    size_t cap;
    char *buf;
    size_t pos;
    size_t icol;
    int i, j, k;
    size_t u;
    int n;

    cap = (size_t)nx * (size_t)ny * (size_t)nz * 24U + 64U;
    buf = (char *)malloc(cap);
    if (buf == NULL) return NULL;
    pos = 0;
    icol = 0;

    for (i = 0; i < nx; i++) {
        for (j = 0; j < ny; j++) {
            for (k = 0; k < nz; k++) {
                u = (size_t)k * (size_t)nx * (size_t)ny + (size_t)j * (size_t)nx + (size_t)i;
                if (pvec != NULL && !(pvec[u] > 0.0)) continue;
                n = snprintf(buf + pos, cap - pos, "%12.6e ", data[u]);
                VASSERT(n > 0 && pos + (size_t)n + 2U < cap);
                pos += (size_t)n;
                icol++;
                if (icol == 3U) {
                    icol = 0;
                    buf[pos++] = '\n';
                }
            }
        }
    }
    if (icol != 0U) {
        buf[pos++] = '\n';
    }
    buf[pos] = '\0';
    *out_len = pos;
    return buf;
}

static int test_full_260k_values(void) {
    const int nx = 65;
    const int ny = 65;
    const int nz = 62; /* 261,950 values (not divisible by 3; ~56 buffer flushes) */
    const size_t ntot = (size_t)nx * (size_t)ny * (size_t)nz;
    double *data;
    size_t idx;
    int exp10, m_idx;
    double mults[8];
    Vgrid *grid;
    char title[32];
    const char *dx_file = "vgrid_dx_test_260k.dx";
    size_t got_len, exp_len;
    char *got_payload;
    char *exp_payload;

    data = (double *)calloc(ntot, sizeof(double));
    VASSERT(data != NULL);

    mults[0] = 1.0;
    mults[1] = nextafter(1.0, 0.0);
    mults[2] = nextafter(1.0, 2.0);
    mults[3] = 9.999994999999999;
    mults[4] = 9.999995;
    mults[5] = 9.999995000000001;
    mults[6] = 1.234565;
    mults[7] = 1.234575;

    idx = 0;
    data[idx++] = 0.0;
    data[idx++] = -0.0;
    data[idx++] = 1.0;
    data[idx++] = -1.0;
    data[idx++] = DBL_MIN;
    data[idx++] = -DBL_MIN;
    data[idx++] = DBL_MAX;
    data[idx++] = -DBL_MAX;
    data[idx++] = ldexp(1.0, -1074);
    data[idx++] = -ldexp(1.0, -1074);
    data[idx++] = NAN;
    data[idx++] = INFINITY;
    data[idx++] = -INFINITY;

    for (exp10 = -105; exp10 <= 105; exp10++) {
        double base = pow(10.0, (double)exp10);
        for (m_idx = 0; m_idx < 8; m_idx++) {
            if (idx + 2 <= ntot) {
                data[idx++] = base * mults[m_idx];
                data[idx++] = -base * mults[m_idx];
            }
        }
    }

    while (idx < ntot) {
        double u = rand_u01();
        if (u < 0.70) {
            data[idx++] = 100.0 * (rand_u01() - 0.5);
        } else if (u < 0.90) {
            int e2 = (int)(xorshift64() % 2091ULL) - 1070;
            data[idx++] = ldexp(2.0 * rand_u01() - 1.0, e2);
        } else {
            uint64_t mant = 1000000ULL + (xorshift64() % 9000000ULL);
            int e10 = (int)(xorshift64() % 199ULL) - 99;
            double v = ((double)mant + 0.5) * pow(10.0, (double)(e10 - 6));
            data[idx++] = (xorshift64() & 1ULL) ? -v : v;
        }
    }

    strcpy(title, "vgrid_dx_test");
    grid = Vgrid_ctor(nx, ny, nz, 0.5, 0.5, 0.5, -10.0, -10.0, -10.0, data);
    Vgrid_writeDX(grid, "FILE", "ASC", VNULL, dx_file, title, VNULL);
    Vgrid_dtor(&grid);

    got_payload = extract_dx_payload(dx_file, &got_len);
    exp_payload = format_expected_dx(data, NULL, nx, ny, nz, &exp_len);
    remove(dx_file);
    free(data);

    if (got_payload == NULL || exp_payload == NULL) {
        fprintf(stderr, "FAIL: missing DX payload buffer\n");
        free(got_payload);
        free(exp_payload);
        return 0;
    }
    if (got_len != exp_len || memcmp(got_payload, exp_payload, exp_len) != 0) {
        size_t p;
        for (p = 0; p < got_len && p < exp_len; p++) {
            if (got_payload[p] != exp_payload[p]) break;
        }
        fprintf(stderr, "FAIL: DX payload mismatch at byte %lu (got_len=%lu exp_len=%lu)\n",
                (unsigned long)p, (unsigned long)got_len, (unsigned long)exp_len);
        free(got_payload);
        free(exp_payload);
        return 0;
    }

    free(got_payload);
    free(exp_payload);
    return 1;
}

static int test_pvec_nan_and_mask(void) {
    const int nx = 4, ny = 4, nz = 4;
    const size_t ntot = 64U;
    double data[64];
    double pvec[64];
    size_t i;
    Vgrid *grid;
    char title[32];
    const char *dx_file = "vgrid_dx_test_mask.dx";
    size_t got_len, exp_len;
    char *got_payload;
    char *exp_payload;

    for (i = 0; i < ntot; i++) {
        data[i] = ((i % 2 == 0) ? 1.0 : -1.0) * ((double)(i + 1) * 1.234565e-3);
        if (i % 5 == 0) pvec[i] = NAN;
        else if (i % 4 == 0) pvec[i] = 0.0;
        else if (i % 3 == 0) pvec[i] = -1.0;
        else pvec[i] = 1.0;
    }
    data[7] = -0.0;
    pvec[7] = 0.5;
    data[9] = 9.9999995;
    pvec[9] = 1.0;

    strcpy(title, "vgrid_mask_test");
    grid = Vgrid_ctor(nx, ny, nz, 0.5, 0.5, 0.5, 0.0, 0.0, 0.0, data);
    Vgrid_writeDX(grid, "FILE", "ASC", VNULL, dx_file, title, pvec);
    Vgrid_dtor(&grid);

    got_payload = extract_dx_payload(dx_file, &got_len);
    exp_payload = format_expected_dx(data, pvec, nx, ny, nz, &exp_len);
    remove(dx_file);

    if (got_payload == NULL || exp_payload == NULL ||
        got_len != exp_len || memcmp(got_payload, exp_payload, exp_len) != 0) {
        fprintf(stderr, "FAIL: masked pvec DX payload mismatch\n");
        free(got_payload);
        free(exp_payload);
        return 0;
    }
    free(got_payload);
    free(exp_payload);
    return 1;
}

int main(void) {
    Vio_start();
    Vnm_redirect(1);
    if (!test_full_260k_values()) { Vio_stop(); return 1; }
    if (!test_pvec_nan_and_mask()) { Vio_stop(); return 1; }
    Vio_stop();
    remove("io.mc");
    printf("vgrid_dx_test: ALL PASSED\n");
    return 0;
}
