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

#include "gsd.h"
#ifdef _OPENMP
#include <omp.h>
#endif

/* 27-point stencil does ~4x the work per point of 7-point, so lower gate pays. */
#ifndef PMG_OMP_MIN_N27
#define PMG_OMP_MIN_N27 8192
#endif

#if defined(_OPENMP) && (_OPENMP >= 201307)
#define GSRB27_JBLOCK 4       /* Rows per plane wavefront publication step */
#define GSRB27_MAX_NZ 1025    /* Max nz for plane progress array (else serial) */
#ifndef GSRB27_SPIN_LIMIT
#define GSRB27_SPIN_LIMIT 1000
#endif
#if defined(__unix__) || defined(__APPLE__)
#include <sched.h>
#define GSRB27_YIELD() ((void)sched_yield())
#else
#define GSRB27_YIELD() ((void)0)
#endif
#endif

VPUBLIC void Vgsrb(int *nx, int *ny, int *nz,
        int *ipc, double *rpc,
        double *ac, double *cc, double *fc,
        double *x, double *w1, double *w2, double *r,
        int *itmax, int *iters,
        double *errtol, double *omega,
        int *iresid, int *iadjoint) {

    int numdia; /// @todo: doc

    MAT2(ac, *nx * *ny * *nz, 1);

    // Do in one step ***
    numdia = VAT(ipc, 11);
    if (numdia == 7) {
        Vgsrb7x(nx, ny, nz,
                ipc, rpc,
                RAT2(ac, 1,1), cc, fc,
                RAT2(ac, 1,2), RAT2(ac, 1,3), RAT2(ac, 1,4),
                x, w1, w2, r,
                itmax, iters, errtol, omega, iresid, iadjoint);
    } else if (numdia == 27) {
        Vgsrb27x(nx, ny, nz,
                 ipc, rpc,
                 RAT2(ac, 1, 1), cc, fc,
                 RAT2(ac, 1, 2), RAT2(ac, 1, 3), RAT2(ac, 1, 4),
                 RAT2(ac, 1, 5), RAT2(ac, 1, 6),
                 RAT2(ac, 1, 7), RAT2(ac, 1, 8), RAT2(ac, 1, 9), RAT2(ac, 1,10),
                 RAT2(ac, 1,11), RAT2(ac, 1,12), RAT2(ac, 1,13), RAT2(ac, 1,14),
                 x, w1, w2, r,
                 itmax, iters, errtol, omega, iresid, iadjoint);
    } else {
        Vnm_print(2, "GSRB: invalid stencil type given...\n");
    }
}



VPUBLIC void Vgsrb7x(int *nx,int *ny,int *nz,
        int *ipc, double *rpc,
        double *oC, double *cc, double *fc,
        double *oE, double *oN, double *uC,
        double *x, double *w1, double *w2, double *r,
        int *itmax, int *iters,
        double *errtol, double *omega,
        int *iresid, int *iadjoint) {

    int i, j, k, ioff;

    MAT3(cc, *nx, *ny, *nz);
    MAT3(fc, *nx, *ny, *nz);
    MAT3( x, *nx, *ny, *nz);
    MAT3(w1, *nx, *ny, *nz);
    MAT3(w2, *nx, *ny, *nz);
    MAT3( r, *nx, *ny, *nz);

    MAT3(oE, *nx, *ny, *nz);
    MAT3(oN, *nx, *ny, *nz);
    MAT3(uC, *nx, *ny, *nz);
    MAT3(oC, *nx, *ny, *nz);

    for (*iters=1; *iters<=*itmax; (*iters)++) {

        // Do the red points ***
        #pragma omp parallel for private(i, j, k, ioff)
        for (k=2; k<=*nz-1; k++) {
            for (j=2; j<=*ny-1; j++) {
                ioff = (1 - *iadjoint) * (    (j + k + 2) % 2)
                     + (    *iadjoint) * (1 - (j + k + 2) % 2);
                for (i=2+ioff; i<=*nx-1; i+=2) {
                    VAT3(x, i, j, k) = (
                            VAT3(fc,   i,  j,  k)
                         +  VAT3(oN,   i,   j,   k) * VAT3(x,   i, j+1,   k)
                         +  VAT3(oN,   i, j-1,   k) * VAT3(x,   i, j-1,   k)
                         +  VAT3(oE,   i,   j,   k) * VAT3(x, i+1,   j,   k)
                         +  VAT3(oE, i-1,   j,   k) * VAT3(x, i-1,   j,   k)
                         + VAT3( uC,   i,   j, k-1) * VAT3(x,   i,   j, k-1)
                         + VAT3( uC,   i,   j,   k) * VAT3(x,   i,   j, k+1)
                         ) / (VAT3(oC, i, j, k) + VAT3(cc, i, j, k));
                }
            }
        }

        // Do the black points
        #pragma omp parallel for private(i, j, k, ioff)
        for (k=2; k<=*nz-1; k++) {
            for (j=2; j<=*ny-1; j++) {
                ioff =   (    *iadjoint) * (    (j + k + 2) % 2 )
                       + (1 - *iadjoint) * (1 - (j + k + 2) % 2 );
                for (i=2+ioff;i<=*nx-1; i+=2) {
                    VAT3(x, i, j, k) = (
                            VAT3(fc,   i,   j,   k)
                         +  VAT3(oN,   i,   j,   k) * VAT3(x,   i,j+1,  k)
                         +  VAT3(oN,   i, j-1,   k) * VAT3(x,   i,j-1,  k)
                         +  VAT3(oE,   i,   j,   k) * VAT3(x, i+1,  j,  k)
                         +  VAT3(oE, i-1,   j,   k) * VAT3(x, i-1,  j,  k)
                         + VAT3( uC,   i,   j, k-1) * VAT3(x,   i,  j,k-1)
                         + VAT3( uC,   i,   j,   k) * VAT3(x,   i,  j,k+1)
                         ) / (VAT3(oC, i, j, k) + VAT3(cc, i, j, k));
                }
            }
        }
    }

    if (*iresid == 1)
        Vmresid7_1s(nx, ny, nz, ipc, rpc, oC, cc, fc, oE, oN, uC, x, r);
}



VPRIVATE void gsrb27_row(int j, int k, int parity, int nx, int ny,
        const double *oC, const double *cc, const double *fc,
        const double *oE, const double *oN, const double *uC,
        const double *oNE, const double *oNW,
        const double *uE, const double *uW, const double *uN, const double *uS,
        const double *uNE, const double *uNW, const double *uSE, const double *uSW,
        double *x) {

    double tmpO, tmpU, tmpD;
    size_t sx, sxy, base_jk, idx, idxd;
    int i, ioff;

    sx = (size_t)nx;
    sxy = sx * (size_t)ny;
    base_jk = (size_t)(k - 1) * sxy + (size_t)(j - 1) * sx;
    ioff = parity ^ ((j + k) & 1);

    for (i = 2 + ioff; i <= nx - 1; i += 2) {
        idx = base_jk + (size_t)(i - 1);
        idxd = idx - sxy;
        tmpO = oN[idx] * x[idx + sx] + oN[idx - sx] * x[idx - sx]
             + oE[idx] * x[idx + 1]  + oE[idx - 1]  * x[idx - 1]
             + oNE[idx] * x[idx + sx + 1] + oNW[idx] * x[idx + sx - 1]
             + oNW[idx - sx + 1] * x[idx - sx + 1] + oNE[idx - sx - 1] * x[idx - sx - 1];
        tmpU = uC[idx]  * x[idx + sxy]
             + uN[idx]  * x[idx + sxy + sx]     + uS[idx]  * x[idx + sxy - sx]
             + uE[idx]  * x[idx + sxy + 1]      + uW[idx]  * x[idx + sxy - 1]
             + uNE[idx] * x[idx + sxy + sx + 1] + uNW[idx] * x[idx + sxy + sx - 1]
             + uSE[idx] * x[idx + sxy - sx + 1] + uSW[idx] * x[idx + sxy - sx - 1];
        tmpD = uC[idxd] * x[idxd]
             + uS[idxd + sx]      * x[idxd + sx]      + uN[idxd - sx]      * x[idxd - sx]
             + uW[idxd + 1]       * x[idxd + 1]       + uE[idxd - 1]       * x[idxd - 1]
             + uSW[idxd + sx + 1] * x[idxd + sx + 1] + uSE[idxd + sx - 1] * x[idxd + sx - 1]
             + uNW[idxd - sx + 1] * x[idxd - sx + 1] + uNE[idxd - sx - 1] * x[idxd - sx - 1];
        x[idx] = (fc[idx] + (tmpO + tmpU + tmpD)) / (oC[idx] + cc[idx]);
    }
}



VPUBLIC void Vgsrb27x(int *nx,int *ny,int *nz,
        int *ipc, double *rpc,
        double  *oC, double  *cc, double  *fc,
        double  *oE, double  *oN, double  *uC, double *oNE, double *oNW,
        double  *uE, double  *uW, double  *uN, double  *uS,
        double *uNE, double *uNW, double *uSE, double *uSW,
        double *x, double *w1, double *w2, double *r,
        int *itmax, int *iters,
        double *errtol, double *omega,
        int *iresid, int *iadjoint) {

    int pass_idx, npasses, cur_p, j, k, nx_v, ny_v, nz_v, p1, p2;
#if defined(_OPENMP) && (_OPENMP >= 201307)
    int done_plane[GSRB27_MAX_NZ], *dp = done_plane;
    int tid, nthreads, tag_base, j0, j_end, req_val, done_prev, spins;
#endif

    nx_v = *nx;
    ny_v = *ny;
    nz_v = *nz;
    p1 = (*iadjoint != 0) ? 1 : 0;
    p2 = 1 - p1;

    if (*itmax <= 0) {
        *iters = 1;
        if (*iresid == 1)
            Vmresid27_1s(nx, ny, nz, ipc, rpc, oC, cc, fc, oE, oN, uC,
                         oNE, oNW, uE, uW, uN, uS, uNE, uNW, uSE, uSW, x, r);
        return;
    }

    *iters = *itmax + 1;
    npasses = 2 * (*itmax);

    if (nx_v > 2 && ny_v > 2 && nz_v > 2) {
#if defined(_OPENMP) && (_OPENMP >= 201307)
        /*
         * Exact serial-order plane pipeline: thread tid owns k = 2 + tid + m*T
         * and starts rows j0..j_end once plane k - 1 finishes min(j_end + 1,
         * ny_v - 1). Monotonic tag_base = pass_idx * ny_v guarantees progress;
         * set OMP_NUM_THREADS <= allocated cores under container CPU quotas.
         */
        if ((size_t)nx_v * (size_t)ny_v * (size_t)nz_v >= PMG_OMP_MIN_N27
                && nz_v <= GSRB27_MAX_NZ
                && !omp_in_parallel()
                && omp_get_max_threads() > 1
                && omp_get_max_threads() <= omp_get_num_procs()) {
            #pragma omp parallel private(tid, nthreads, pass_idx, cur_p, tag_base, k, j0, j, j_end, req_val, done_prev, spins)
            {
                tid = omp_get_thread_num();
                nthreads = omp_get_num_threads();

                #pragma omp for schedule(static)
                for (k = 2; k <= nz_v - 1; k++)
                    dp[k] = 0;

                for (pass_idx = 0; pass_idx < npasses; pass_idx++) {
                    cur_p = ((pass_idx & 1) == 0) ? p1 : p2;
                    tag_base = pass_idx * ny_v;

                    for (k = 2 + tid; k <= nz_v - 1; k += nthreads) {
                        for (j0 = 2; j0 <= ny_v - 1; j0 += GSRB27_JBLOCK) {
                            j_end = (j0 + (GSRB27_JBLOCK - 1) < ny_v - 1) ? (j0 + (GSRB27_JBLOCK - 1)) : (ny_v - 1);
                            if (k > 2) {
                                req_val = tag_base + ((j_end + 1 < ny_v - 1) ? (j_end + 1) : (ny_v - 1));
                                spins = 0;
                                for (;;) {
                                    #pragma omp atomic read seq_cst
                                    done_prev = dp[k - 1];
                                    if (done_prev >= req_val) break;
                                    if (spins < GSRB27_SPIN_LIMIT) spins++;
                                    else GSRB27_YIELD();
                                }
                            }
                            for (j = j0; j <= j_end; j++)
                                gsrb27_row(j, k, cur_p, nx_v, ny_v, oC, cc, fc, oE, oN, uC,
                                           oNE, oNW, uE, uW, uN, uS, uNE, uNW, uSE, uSW, x);
                            #pragma omp atomic write seq_cst
                            dp[k] = tag_base + j_end;
                        }
                    }
                    if (pass_idx + 1 < npasses) {
                        #pragma omp barrier
                    }
                }
            }
        } else
#endif
        {
            for (pass_idx = 0; pass_idx < npasses; pass_idx++) {
                cur_p = ((pass_idx & 1) == 0) ? p1 : p2;
                for (k = 2; k <= nz_v - 1; k++)
                    for (j = 2; j <= ny_v - 1; j++)
                        gsrb27_row(j, k, cur_p, nx_v, ny_v, oC, cc, fc, oE, oN, uC,
                                   oNE, oNW, uE, uW, uN, uS, uNE, uNW, uSE, uSW, x);
            }
        }
    }

    // If specified, return the new residual as well
    if (*iresid == 1)
        Vmresid27_1s(nx, ny, nz,
                     ipc, rpc,
                      oC,  cc,  fc,
                      oE,  oN,  uC,
                     oNE, oNW,
                     uE,   uW,  uN,  uS,
                     uNE, uNW, uSE, uSW,
                       x,   r);
}
