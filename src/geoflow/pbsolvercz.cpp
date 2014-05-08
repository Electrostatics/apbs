///  @file    pbsolvercz.cpp
///  @author  Andrew Stevens, Kyle Monson, Zhan Chen, Guowei Wei
///  @brief sets up and solves PB equation, uses biCGStab from Eigen
///  @ingroup Geoflow
///  @version $Id$
///  @attention
///  @verbatim
///
/// APBS -- Adaptive Poisson-Boltzmann Solver
///
///  Nathan A. Baker (nathan.baker@pnnl.gov)
///  Pacific Northwest National Laboratory
///
///  Additional contributing authors listed in the code documentation.
///
/// Copyright (c) 2010-2012 Battelle Memorial Institute. Developed at the
/// Pacific Northwest National Laboratory, operated by Battelle Memorial
/// Institute, Pacific Northwest Division for the U.S. Department of Energy.
///
/// Portions Copyright (c) 2002-2010, Washington University in St. Louis.
/// Portions Copyright (c) 2002-2010, Nathan A. Baker.
/// Portions Copyright (c) 1999-2002, The Regents of the University of
/// California.
/// Portions Copyright (c) 1995, Michael Holst.
/// All rights reserved.
///
/// Redistribution and use in source and binary forms, with or without
/// modification, are permitted provided that the following conditions are met:
///
/// Redistributions of source code must retain the above copyright notice, this
/// list of conditions and the following disclaimer.
///
/// Redistributions in binary form must reproduce the above copyright notice,
/// this list of conditions and the following disclaimer in the documentation
/// and/or other materials provided with the distribution.
///
/// Neither the name of the developer nor the names of its contributors may be
/// used to endorse or promote products derived from this software without
/// specific prior written permission.
///
/// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
/// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
/// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
/// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
/// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
/// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
/// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
/// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
/// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
/// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
/// THE POSSIBILITY OF SUCH DAMAGE.
///
/// @endverbatim

#include <cassert>
#include <valarray>
#include <vector>

#include <Eigen/Sparse>

#include "Mat.h"
#include "modules.h"

void pbsolver(Mat<>& eps, Mat<>& phi, Mat<>& bgf, double tol, int iter)
{
	size_t nx = eps.nx(), ny = eps.ny(), nz = eps.nz();
	double dx = comdata.deltax, dy = comdata.deltay, dz = comdata.deltaz;

	Mat<> eps1(nx,ny,nz), eps2(nx,ny,nz), eps3(nx,ny,nz);
	for(size_t i = 1; i < nx; ++i) {
		for(size_t j = 1; j < ny; ++j) {
			for(size_t k = 1; k < nz; ++k) {
				eps1(i,j,k) = (eps(i+1,j,k) + eps(i,j,k))/2.0;
				eps2(i,j,k) = (eps(i,j+1,k) + eps(i,j,k))/2.0;
				eps3(i,j,k) = (eps(i,j,k+1) + eps(i,j,k))/2.0;
			}
		}
	}

	std::vector <Eigen::Triplet<double>> tripletList;
	tripletList.reserve(nx*ny*nz);
	Eigen::VectorXd phi_flat(nx*ny*nz);

	size_t n = nx*ny*nz;
	for (size_t i = 1; i <= nx; ++i) {
		for (size_t j = 1; j <= ny; ++j) {
			for(size_t k = 1; k <= nz; ++k) {
				size_t ijk = (i-1)*nz*ny + (j-1)*nz + k-1;
				if (i==1 || i==nx || j==1 || j==ny || k==1 || k==nz) {
					tripletList.push_back(Eigen::Triplet<double>(ijk, ijk, 1.0));
				} else {
					double f = -(  (eps1(i,j,k) + eps1(i-1,j,k))/dx/dx
								 + (eps2(i,j,k) + eps2(i,j-1,k))/dy/dy
								 + (eps3(i,j,k) + eps3(i,j,k-1))/dz/dz );
					tripletList.push_back(Eigen::Triplet<double>(ijk, ijk, f));

					double weit[6];
					weit[0] = eps1(i-1,j,k)/dx/dx;
					weit[1] = eps2(i,j-1,k)/dy/dy;
					weit[2] = eps3(i,j,k-1)/dz/dz;
					weit[3] = eps3(i,j,k)/dz/dz;
					weit[4] = eps2(i,j,k)/dy/dy;
					weit[5] = eps1(i,j,k)/dx/dx;

					assert(ijk > nz*ny);
					size_t jj = ijk - nz*ny;
					tripletList.push_back(Eigen::Triplet<double>(ijk, jj,
							weit[0]));

					assert(ijk > nz);
					jj = ijk - nz;
					tripletList.push_back(Eigen::Triplet<double>(ijk, jj,
							weit[1]));

					assert(ijk > 1);
					jj = ijk - 1;
					tripletList.push_back(Eigen::Triplet<double>(ijk, jj,
							weit[2]));

					assert(ijk + 1 < n);
					jj = ijk + 1;
					tripletList.push_back(Eigen::Triplet<double>(ijk, jj,
							weit[3]));

					assert(ijk + nz < n);
					jj = ijk + nz;
					tripletList.push_back(Eigen::Triplet<double>(ijk, jj,
							weit[4]));

					assert(ijk + nz*ny < n);
					jj = ijk + nz*ny;
					tripletList.push_back(Eigen::Triplet<double>(ijk, jj,
							weit[5]));
				}

				phi_flat(ijk) = phi(i,j,k);
			}
		}
	}

	Eigen::SparseMatrix<double> A(n, n);
	A.setFromTriplets(tripletList.begin(), tripletList.end());
	A.makeCompressed();

	Eigen::BiCGSTAB<Eigen::SparseMatrix<double>, Eigen::IdentityPreconditioner> solver(A);
	solver.setMaxIterations(iter);
	solver.setTolerance(tol);

	phi_flat = solver.solveWithGuess(bgf.baseInterface(), phi_flat);

	for(size_t i = 1; i <= nx; ++i) {
		for(size_t j = 1; j <= ny; ++j) {
			for(size_t k = 1; k <= nz; ++k) {
				size_t ijk = (i-1)*nz*ny + (j-1)*nz + k-1;
				phi(i,j,k) = phi_flat(ijk);
			}
		}
	}
}

