///  @file    cpbconcz2.h
///  @author  Andrew Stevens,Kyle Monson
///  @brief some definitons for APBS
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
/// Pacific Northwest National Laboratory,operated by Battelle Memorial
/// Institute,Pacific Northwest Division for the U.S. Department of Energy.
///
/// Portions Copyright (c) 2002-2010,Washington University in St. Louis.
/// Portions Copyright (c) 2002-2010,Nathan A. Baker.
/// Portions Copyright (c) 1999-2002,The Regents of the University of
/// California.
/// Portions Copyright (c) 1995,Michael Holst.
/// All rights reserved.
///
/// Redistribution and use in source and binary forms,with or without
/// modification,are permitted provided that the following conditions are met:
///
/// Redistributions of source code must retain the above copyright notice,this
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
/// AND ANY EXPRESS OR IMPLIED WARRANTIES,INCLUDING,BUT NOT LIMITED TO,THE
/// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
/// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
/// LIABLE FOR ANY DIRECT,INDIRECT,INCIDENTAL,SPECIAL,EXEMPLARY,OR
/// CONSEQUENTIAL DAMAGES (INCLUDING,BUT NOT LIMITED TO,PROCUREMENT OF
/// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,DATA,OR PROFITS; OR BUSINESS
/// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,WHETHER IN
/// CONTRACT,STRICT LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
/// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,EVEN IF ADVISED OF
/// THE POSSIBILITY OF SUCH DAMAGE.
///
/// @endverbatim

#pragma once

// From the original f90 code; need to keep this
// constant since the f90 routines that we call from here
// depend on it.
#define MAXATOMS 15000

// Four because: 0-2 => pos, 3 => radius
#define XYZRWIDTH 4

typedef struct _GeoflowOutput
{
	double area,
		volume,
		attint,
		sumpot,
		totalSolvation,
		nonpolarSolvation,
		elecSolvation;
} GeoflowOutput;

typedef struct _GeoflowInput
{
	double* dcel;
	int ffmodel;
	double extvalue;
	double* pqr;
	int maxstep;
	double crevalue;
	int iadi;
	double tottf;
	double* ljepsilon;
	double alpha;
	int igfin;
	double epsilons;
	double epsilonp;
	int idacsl;
	double tol;
	int iterf;
	double tpb;
	int itert;
	double pres;
	double gama;
	double tauval;
	double prob;
	int vdwdispersion;
	double sigmas;
	double density;
	double epsilonw;
} GeoflowInput;

#ifdef __cplusplus
extern "C"
#endif
GeoflowOutput geoflowSolvation(double xyzr[MAXATOMS][XYZRWIDTH], size_t natm,
		GeoflowInput gfin);

