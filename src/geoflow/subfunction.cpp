///  @file    subfunction.cpp
///  @author  Andrew Stevens, Kyle Monson, Zhan Chen, Guowei Wei
///  @brief sets up the source function
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

#include "Mat.h"
#include "modules.h"

double qbboundary(size_t natm, double x,double y,double z,
		double xyzr[MAXATOMS][XYZRWIDTH], double* pqr, double epsilonsp)
{
	double vbdn = 0;
	for (size_t a = 0; a < natm; ++a) {
		double x_q = x - xyzr[a][1];
		double y_q = y - xyzr[a][2];
		double z_q = z - xyzr[a][3];
		double q_q = pqr[a];
		double rr = sqrt(dot(x_q, y_q, z_q));
		vbdn += q_q/(epsilonsp*rr);
	}
	return vbdn;
}


double qbinterior(double x,double y,double z, Mat<>& charget, Mat<>& corlocqt)
{
	double fp = 0;
	for (size_t a = 1; a <= charget.nx(); ++a) {
		for (size_t ii = 1; ii <= charget.ny(); ++ii) {
			double xc = x - corlocqt(a,ii,1);
			double yc = y - corlocqt(a,ii,2);
			double zc = z - corlocqt(a,ii,3);
			if ( dot(xc,yc,zc) <= 1e-13) {
				fp -= 4.0*comdata.pi*charget(a,ii)/
						(comdata.deltax*comdata.deltay*comdata.deltaz);
			}
		}
	}

	return fp;
}

double qb(size_t i,size_t j,size_t k, double xyzr[MAXATOMS][XYZRWIDTH],
		double* pqr, Mat<>& charget, Mat<>& corlocqt, double epsilonsp)
{
	double x = xvalue(i);
	double y = yvalue(j);
	double z = zvalue(k);
	if(i < 2 || i > comdata.nx-1 ||
			j < 2 || j > comdata.ny-1 ||
			k < 2 || k > comdata.nz-1) {
		return qbboundary(charget.nx(), x,y,z, xyzr, pqr, epsilonsp);
	} else {
		return qbinterior(x,y,z, charget, corlocqt);
	}
}

void seteqb(Mat<>& bg, double xyzr[MAXATOMS][XYZRWIDTH], double* pqr,
		Mat<>& charget, Mat<>& corlocqt, double epsilonsp)
{
	for (size_t i = 1; i <= comdata.nx; ++i) {
		for (size_t j = 1; j <= comdata.ny; ++j) {
			for (size_t k = 1; k <= comdata.nz; ++k) {
				double fp = qb(i,j,k,xyzr,pqr,charget,corlocqt,epsilonsp);
				int ijk = (i-1)*comdata.ny*comdata.nz + (j-1)*comdata.nz + k - 1;
				bg[ijk] = fp;
			}
		}
	}
}
