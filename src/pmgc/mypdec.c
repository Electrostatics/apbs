/**
 *  @ingroup PMGC
 *  @author  Tucker Beck [fortran ->c translation], Michael Holst [original]
 *  @brief
 *  @version $Id:
 *
 *  @attention
 *  @verbatim
 *
 * APBS -- Adaptive Poisson-Boltzmann Solver
 *
 * Nathan A. Baker (nathan.baker@pnl.gov)
 * Pacific Northwest National Laboratory
 *
 * Additional contributing authors listed in the code documentation.
 *
 * Copyright (c) 2010-2020 Battelle Memorial Institute.
 * Developed at the Pacific Northwest National Laboratory, operated by Battelle Memorial Institute, Pacific Northwest Division for the U.S. Department Energy.
 * Portions Copyright (c) 2002-2010, Washington University in St. Louis.
 * Portions Copyright (c) 2002-2020, Nathan A. Baker.  
 * Portions Copyright (c) 1999-2002, The Regents of the University of California.
 * Portions Copyright (c) 1995, Michael Holst.
 * All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * -  Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * - Neither the name of Washington University in St. Louis nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @endverbatim
 */

#include "mypdec.h"

double v1;
double v2;
double v3;
double conc1;
double conc2;
double conc3;
double vol;
double relSize;
int nion;
double charge[MAXIONS];
double sconc[MAXIONS];

VPUBLIC void Vmypdefinitlpbe(int *tnion, double *tcharge, double *tsconc) {

    int i;

    nion = *tnion;
    if (nion > MAXIONS) {
        Vnm_print(2, "Vmypde: Warning: Ignoring extra ion species\n");
        nion = MAXIONS;
    }

    for (i=1; i<=nion; i++) {
        VAT(charge, i) = VAT(tcharge, i);
        VAT(sconc,  i) = VAT(tsconc,  i);
    }
}



VPUBLIC void Vmypdefinitnpbe(int *tnion, double *tcharge, double *tsconc) {

    int i;

    nion = *tnion;
    if (nion > MAXIONS) {
        Vnm_print(2, "Vmypde: Warning: Ignoring extra ion species\n");
        nion = MAXIONS;
    }

    for (i=1; i<=nion; i++) {
        VAT(charge, i) = VAT(tcharge, i);
        VAT( sconc, i) = VAT( tsconc, i);
    }
}



VPUBLIC void Vmypdefinitsmpbe(int *tnion, double *tcharge, double *tsconc,
        double *smvolume, double *smsize) {

    int i;

    WARN_UNTESTED;

    VABORT_MSG0("Not tested");

    if (*tnion > 3) {
            Vnm_print(2, "SMPBE: modified theory handles only three ion species.\n");
            Vnm_print(2, "       Ignoring the rest of the ions!\n");
            Vnm_print(2, "       (mypde.f::mypdefinit)\n");
    }

    v1 = VAT(tcharge, 1);
    v2 = VAT(tcharge, 2);
    v3 = VAT(tcharge, 3);
    conc1 = VAT(tsconc, 1);
    conc2 = VAT(tsconc, 2);
    conc3 = VAT(tsconc, 3);

    vol = *smvolume;
    relSize = *smsize;
}



VPUBLIC void Vc_vec(double *coef, double *uin, double *uout,
        int *nx, int *ny, int *nz, int *ipkey) {

    if (*ipkey == -2) {
        Vc_vecsmpbe(coef, uin, uout, nx, ny, nz, ipkey);
    } else {
        Vc_vecpmg(coef, uin, uout, nx, ny, nz, ipkey);
    }
}



VPRIVATE void eval_boltzmann_vec(double *coef, double *uin, double *uout,
        double *duout, int *nx, int *ny, int *nz, int *ipkey, int mode) {
    /* mode: 0 = c(u) only, 1 = dc(u) only, 2 = both c(u) and dc(u) */
    double zcf_c[MAXIONS + 1], zcf_dc[MAXIONS + 1], zu[MAXIONS + 1];
    double c, u, acc_c, acc_dc, am_zero, am_zero_dc, am_neg, am_pos, arg, e;
    int ichopped_c = 0, ichopped_dc = 0;
    int ichopped_neg, ichopped_pos, iion, n, i;

    n = (*nx) * (*ny) * (*nz);
    if (*ipkey != 0) {
        if ((*ipkey) > 1 && (*ipkey) % 2 == 1 && (*ipkey) <= MAXPOLY) {
            VABORT_MSG0("eval_boltzmann_vec: Polynomial approximation unavailable\n");
        }
        VABORT_MSG0("eval_boltzmann_vec: Linear approximation unavailable\n");
    }
    if (nion <= 0) {
        if (mode != 1) { for (i = 1; i <= n; i++) VAT(uout, i) = 0.0; }
        if (mode != 0) { for (i = 1; i <= n; i++) VAT(duout, i) = 0.0; }
        return;
    }
    for (iion = 1; iion <= nion; iion++) {
        zcf_c[iion] = -1.0 * VAT(sconc, iion) * VAT(charge, iion);
        zcf_dc[iion] = VAT(sconc, iion) * VAT(charge, iion) * VAT(charge, iion);
        zu[iion] = -1.0 * VAT(charge, iion);
    }

    /* Interleave chunks so interior solute runs (coef == 0) do not starve threads. */
#ifdef _OPENMP
    #pragma omp parallel for default(shared) \
        private(i, iion, c, u, acc_c, acc_dc, am_zero, am_zero_dc, am_neg, am_pos, arg, e, ichopped_neg, ichopped_pos) \
        reduction(+ : ichopped_c, ichopped_dc) \
        schedule(static, 2048) if ((size_t)n >= PMG_OMP_MIN_N_EXP)
#endif
    for (i = 1; i <= n; i++) {
        c = VAT(coef, i);
        if (c == 0.0) {
            if (mode != 1) VAT(uout, i) = 0.0;
            if (mode != 0) VAT(duout, i) = 0.0;
            continue;
        }
        u = VAT(uin, i);
        acc_c = 0.0;
        acc_dc = 0.0;
        for (iion = 1; iion <= nion; iion++) {
            am_neg = VMAX2(VMIN2(zu[iion] * u, 0.0), SINH_MIN);
            am_pos = VMIN2(VMAX2(zu[iion] * u, 0.0), SINH_MAX);
            ichopped_neg = (int)(am_neg / SINH_MIN);
            ichopped_pos = (int)(am_pos / SINH_MAX);
            if (mode == 0) {
                am_zero = VMIN2(ZSMALL, VABS(zcf_c[iion] * c)) * ZLARGE;
                arg = am_zero * (am_neg + am_pos);
                acc_c = acc_c + zcf_c[iion] * c * exp(arg);
                ichopped_c += (int)floor(am_zero + 0.5) * (ichopped_neg + ichopped_pos);
            } else if (mode == 1) {
                am_zero_dc = VMIN2(ZSMALL, VABS(zcf_dc[iion] * c)) * ZLARGE;
                arg = am_zero_dc * (am_neg + am_pos);
                acc_dc += zcf_dc[iion] * c * exp(arg);
                ichopped_dc += (int)floor(am_zero_dc + 0.5) * (ichopped_neg + ichopped_pos);
            } else {
                am_zero = VMIN2(ZSMALL, VABS(zcf_c[iion] * c)) * ZLARGE;
                am_zero_dc = VMIN2(ZSMALL, VABS(zcf_dc[iion] * c)) * ZLARGE;
                arg = am_zero * (am_neg + am_pos);
                e = exp(arg);
                acc_c = acc_c + zcf_c[iion] * c * e;
                acc_dc += zcf_dc[iion] * c * ((am_zero_dc == am_zero) ? e : exp(am_zero_dc * (am_neg + am_pos)));
                ichopped_c += (int)floor(am_zero + 0.5) * (ichopped_neg + ichopped_pos);
                ichopped_dc += (int)floor(am_zero_dc + 0.5) * (ichopped_neg + ichopped_pos);
            }
        }
        if (mode != 1) VAT(uout, i) = acc_c;
        if (mode != 0) VAT(duout, i) = acc_dc;
    }
    if (ichopped_c > 0) Vnm_print(2, "Vc_vecpmg: trapped exp overflows: %d\n", ichopped_c);
    if (ichopped_dc > 0) Vnm_print(2, "Vdc_vec: trapped exp overflows: %d\n", ichopped_dc);
}

VPUBLIC void Vc_vecpmg(double *coef, double *uin, double *uout,
        int *nx, int *ny, int *nz, int *ipkey) {
    eval_boltzmann_vec(coef, uin, uout, VNULL, nx, ny, nz, ipkey, 0);
}



VPUBLIC void Vc_vecsmpbe(double *coef, double *uin, double *uout,
        int *nx, int *ny, int *nz, int *ipkey) {

    int ideg;
    double zcf2, zu2;
    double am_zero, am_neg, am_pos;
    double argument, poly, fact;

    int ichopped, ichopped_neg, ichopped_pos;
    int iion;
    int n, i, ii, ipara, ivect;

    int nproc = 1;

    // Added by DG SMPBE variables and common blocks
    double fracOccA, fracOccB, fracOccC, phi, ionStr;
    double z1, z2, z3, ca, cb, cc, a, k;
    double a1_neg, a1_pos, a2_neg, a2_pos;
    double a3_neg, a3_pos, a1, a2, a3;
    double f, g, gpark, alpha;

    WARN_UNTESTED;

    Vnm_print(2, "Vc_vecsmpbe: v1      = %f\n", v1);
    Vnm_print(2, "Vc_vecsmpbe: v2      = %f\n", v2);
    Vnm_print(2, "Vc_vecsmpbe: v3      = %f\n", v3);
    Vnm_print(2, "Vc_vecsmpbe: conc1   = %f\n", conc1);
    Vnm_print(2, "Vc_vecsmpbe: conc2   = %f\n", conc2);
    Vnm_print(2, "Vc_vecsmpbe: conc3   = %f\n", conc3);
    Vnm_print(2, "Vc_vecsmpbe: vol     = %f\n", vol);
    Vnm_print(2, "Vc_vecsmpbe: relSize = %f\n", relSize);

    Vnm_print(2, "Vc_vecsmpbe: nion    = %d\n", nion);

    Vnm_print(2, "Vc_vecsmpbe: charge  = [");
    for (i=1; i<=nion; i++)
        Vnm_print(2, "%f ", VAT(charge, i));
    Vnm_print(2, "]\n");

    Vnm_print(2, "Vc_vecsmpbe: sconc   = [");
    for (i=1; i<=nion; i++)
        Vnm_print(2, "%f ", VAT(sconc, i));
    Vnm_print(2, "]\n");



    // Find parallel loops (ipara), remainder (ivect)
    n = *nx * *ny * *nz;
    ipara = n / nproc;
    ivect = n % nproc;

    for (i=1; i<=n; i++)
        VAT(uout, i) = 0;

    // Initialize the chopped counter
    ichopped = 0;

    z1 = v1;
    z2 = v2;
    z3 = v3;
    ca = conc1;
    cb = conc2;
    cc = conc3;
    a  = vol;
    k  = relSize;

    if (k - 1 < ZSMALL)
        Vnm_print(2, "Vc_vecsmpbe: k=1, using special routine\n");

    // Derived quantities
    fracOccA = Na * ca * VPOW(a, 3.0);
    fracOccB = Na * cb * VPOW(a, 3.0);
    fracOccC = Na * cc * VPOW(a, 3.0);

    phi    = (fracOccA / k) + fracOccB + fracOccC;
    alpha  = (fracOccA / k) / (1 - phi);
    ionStr = 0.5 * (ca * VPOW(z1, 2.0) + cb * VPOW(z2, 2.0) + cc * VPOW(z3, 2));

    for (i=1; i<=n; i++) {

        am_zero = VMIN2(ZSMALL, VABS(VAT(coef, i))) * ZLARGE;

        // Compute the arguments for exp(-z*u) term
        a1_neg = VMAX2(VMIN2(-1.0 * z1 * VAT(uin, i), 0.0), SINH_MIN);
        a1_pos = VMIN2(VMAX2(-1.0 * z1 * VAT(uin, i), 0.0), SINH_MAX);

        // Compute the arguments for exp(-u) term
        a2_neg = VMAX2(VMIN2(-1.0 * z2 * VAT(uin, i), 0.0), SINH_MIN);
        a2_pos = VMIN2(VMAX2(-1.0 * z2 * VAT(uin, i), 0.0), SINH_MAX);

        // Compute the arguments for exp(u) term
        a3_neg = VMAX2(VMIN2(-1.0 * z3 * VAT(uin, i), 0.0), SINH_MIN);
        a3_pos = VMIN2(VMAX2(-1.0 * z3 * VAT(uin, i), 0.0), SINH_MAX);

        a1 = am_zero * (a1_neg + a1_pos);
        a2 = am_zero * (a2_neg + a2_pos);
        a3 = am_zero * (a3_neg + a3_pos);

        gpark = (1 + alpha * exp(a1)) / (1 + alpha);

        if (k - 1 <  ZSMALL) {
            f = z1 * ca * exp(a1) + z2 * cb * exp(a2) + z3 * cc * exp(a3);
            g = 1 - phi + fracOccA * exp(a1)
                        + fracOccB * exp(a2)
                        + fracOccC * exp(a3);
        } else {
            f = z1 * ca * exp(a1) * VPOW(gpark, k-1)
              + z2 * cb * exp(a2)
              + z3 * cc * exp(a3);
            g = (1 - phi + fracOccA / k) * VPOW(gpark, k)
              + fracOccB * exp(a2)
              + fracOccC * exp(a3);
        }

        VAT(uout, i) = -1.0 * VAT(coef, i) * (0.5 / ionStr) * (f / g);

        // Count chopped values
        ichopped_neg = (int)((a1_neg + a2_neg+a3_neg) / SINH_MIN);
        ichopped_pos = (int)((a1_pos + a2_pos+a3_pos) / SINH_MAX);
        ichopped += (int)floor(am_zero+0.5) * (ichopped_neg + ichopped_pos);
    }

    // Info
    if (ichopped > 0)
        Vnm_print(2, "Vc_vecsmpbe: trapped exp overflows: %d\n", ichopped);

}

VPUBLIC void Vdc_vec(double *coef, double *uin, double *uout,
        int *nx, int *ny, int *nz, int *ipkey) {

    int i;
    int n = *nx * *ny * *nz;

    if(*ipkey == -2) {
        Vdc_vecsmpbe(coef, uin, uout, nx, ny, nz, ipkey);
    } else {
        Vdc_vecpmg(coef, uin, uout, nx, ny, nz, ipkey);
    }
}

VPUBLIC void Vdc_vecpmg(double *coef, double *uin, double *uout,
        int *nx, int *ny, int *nz, int *ipkey) {
    eval_boltzmann_vec(coef, uin, VNULL, uout, nx, ny, nz, ipkey, 1);
}



VPUBLIC void Vdc_vecsmpbe(double *coef, double *uin, double *uout,
        int *nx, int *ny, int *nz, int *ipkey) {

    int ideg, iion;
    double zcf2, zu2;
    double am_zero, am_neg, am_pos;
    double argument, poly, fact;
    int ichopped, ichopped_neg, ichopped_pos;

    int n, i, ii;
    int ipara, ivect;

    int nproc = 1;

    // Added by DG SMPBE variables and common blocks
    double fracOccA, fracOccB, fracOccC, phi, ionStr;
    double z1, z2, z3, ca, cb, cc, a, k;
    double a1_neg, a1_pos, a2_neg, a2_pos;
    double a3_neg, a3_pos, a1, a2, a3;
    double f, g, fprime, gprime, gpark, alpha;

    WARN_UNTESTED;

    // Find parallel loops (ipara), remainder (ivect)
    n = *nx * *ny * *nz;
    ipara = n / nproc;
    ivect = n % nproc;

    for (i=1; i<=n; i++)
      VAT(uout, i) = 0.0;

    // Initialize the chopped counter
    ichopped = 0;

    z1 = v1;
    z2 = v2;
    z3 = v3;
    ca = conc1;
    cb = conc2;
    cc = conc3;
    a  = vol;
    k  = relSize;

    if (k - 1 < ZSMALL)
        Vnm_print(2, "Vdc_vecsmpbe: k=1, using special routine\n");

    // Derived quantities
    fracOccA = Na * ca * VPOW(a, 3.0);
    fracOccB = Na * cb * VPOW(a, 3.0);
    fracOccC = Na * cc * VPOW(a, 3.0);
    phi = fracOccA / k + fracOccB + fracOccC;
    alpha = (fracOccA / k) /(1 - phi);
    ionStr = 0.5*(ca * VPOW(z1, 2) + cb * VPOW(z2, 2) + cc * VPOW(z3, 2));

    for (i=1; i<=n; i++) {

        am_zero = VMIN2(ZSMALL, VABS(VAT(coef, i))) * ZLARGE;

        // Compute the arguments for exp(-z*u) term
        a1_neg = VMAX2(VMIN2(-1.0 * z1 * VAT(uin, i), 0.0), SINH_MIN);
        a1_pos = VMIN2(VMAX2(-1.0 * z1 * VAT(uin, i), 0.0), SINH_MAX);

        // Compute the arguments for exp(-u) term
        a2_neg = VMAX2(VMIN2(-1.0 * z2 * VAT(uin, i), 0.0), SINH_MIN);
        a2_pos = VMIN2(VMAX2(-1.0 * z2 * VAT(uin, i), 0.0), SINH_MAX);

        // Compute the arguments for exp(u) term
        a3_neg = VMAX2(VMIN2(-1.0 * z3 * VAT(uin, i), 0.0), SINH_MIN);
        a3_pos = VMIN2(VMAX2(-1.0 * z3 * VAT(uin, i), 0.0), SINH_MAX);

        a1 = am_zero * (a1_neg + a1_pos);
        a2 = am_zero * (a2_neg + a2_pos);
        a3 = am_zero * (a3_neg + a3_pos);

        gpark = (1 + alpha * exp(a1)) / (1 + alpha);

        if (k - 1 < ZSMALL) {
            f = z1 * ca * exp(a1) + z2 * cb * exp(a2) + z3 * cc * exp(a3);
            g = 1 - phi + fracOccA * exp(a1)
                        + fracOccB * exp(a2)
                        + fracOccC * exp(a3);

            fprime =
                   - VPOW(z1, 2) * ca * exp(a1)
                   - VPOW(z2, 2) * cb * exp(a2)
                   - VPOW(z3, 2) * cc * exp(a3);

            gprime =
                   - z1 * fracOccA * exp(a1)
                   - z2 * fracOccB * exp(a2)
                   - z3 * fracOccC * exp(a3);
        } else {
            f = z1 * ca * exp(a1) * VPOW(gpark, k - 1)
              + z2 * cb * exp(a2)
              + z3 * cc * exp(a3);
            g = (1 - phi + fracOccA / k) * VPOW(gpark, k)
              + fracOccB * exp(a2)
              + fracOccC * exp(a3);

            fprime =
                   - VPOW(z1, 2) * ca * exp(a1) * VPOW(gpark, k - 2)
                   * (gpark + (k - 1) * (alpha / (1 + alpha)) * exp(a1))
                   - VPOW(z2, 2) * cb * exp(a2)
                   - VPOW(z3, 2) * cc * exp(a3);

            gprime =
                   - k * z1 * (alpha / (1 + alpha)) * exp(a1)
                   * (1 - phi + fracOccA / k) * VPOW(gpark, k - 1)
                   - z2 * fracOccB * exp(a2)
                   - z3 * fracOccC * exp(a3);

        }

        VAT(uout, i) = -1.0 * VAT(coef, i) * (0.5 / ionStr)
                     * (fprime * g - gprime * f) / VPOW(g, 2.0);

        // Count chopped values
        ichopped_neg = (int)((a1_neg + a2_neg + a3_neg) / SINH_MIN);
        ichopped_pos = (int)((a1_pos + a2_pos + a3_pos) / SINH_MAX);
        ichopped += (int)floor(am_zero+0.5) * (ichopped_neg + ichopped_pos);
    }

    // Info
    if (ichopped > 0)
        Vnm_print(2, "Vdc_vecsmpbe: trapped exp overflows: %d\n", ichopped);
}

VPUBLIC void Vc_dc_vecpmg(double *coef, double *uin, double *uout,
        double *duout, int *nx, int *ny, int *nz, int *ipkey) {
    eval_boltzmann_vec(coef, uin, uout, duout, nx, ny, nz, ipkey, 2);
}
