///  @file    main.cpp
///  @author  Andrew Stevens
///  @brief this is a wrapper to set the geoflow parameters, see fort.12
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

#include <iostream>
void pbconcz2(
		//
		// These parameters correspond directly to those read in via the datafiles
		//  (fort.12 and 17set.txt) in the original Fortran code.
		//
		// int nmol,
		double pres_i,
		double gama_i,
		int npiter,
		int ngiter,
		double tauval,
		double prob,
		//
		// 1 for ZAP-9/AM1-BCCv1; 2 for OPLS/AA
		int ffmodel,
		//
		// Angstrom (radius of water molecule based on LJ parameter sigma)
		double sigmas,
		//
		// epsilon parameter of 0 (kcal/mol) of water molecule
		double epsilonw,
		//
		// 1(on) or 0(off)- previously called REPULSIVE
		int vdwdispersion,
		//
		// (distance atom surface and box boundary)
		double extvalue,
		//
		// flag to indicate the usage of preconditioner iprec =1 (yes); 0 (no)
		// int iprec,
		// int istep,
		//
		// 0 for explicit scheme; 1 for ADI scheme
		int iadi,
		//
		// weight of previous solution to change the next solution in geometry flow
		double alpha,
		//
		// start guess for PB 1; inherit '0'
		// int ipbin,
		double tol,
		//
		// total time
		double tottf,
		double dcel,
		int maxstep,
		double epsilons,
		double epsilonp,
		int radexp,
		double crevalue,
		//
		// 0 for solvation force calculation; 1 or accuracy test
		// int idacsl,
		//
		// (use 0.03346)
		double density );

int main()
{
	int ffmodel = 1;
	pbconcz2(
		// 17,		// nmol
		0.008,		// pres_i
		0.0001,		// gama_i
		1,			// npiter
		1,			// ngiter
		1.40,		// tauval
		0.0,		// prob
		ffmodel,	// ffmodel
		1.5828,		// sigmas
		0.1554,		// epsilonw
		1,			// vdwdispersion
		1.90,		// extvalue
		// 0,		// iprec
		// 10,		// istep
		0,			// iadi
		0.50,		// ALPHA
		// 1,		// IPBIN
		1e-4,		// TOL
		3.5,		// TOTTF
		0.25,		// DCEL
		20,			// MAXSTEP
		80.00,		// EPSILONS
		1.5,		// EPSILONP
		1,			// RADEXP
		0.01,		// CREVALUE
		// 0,		// idacsl
		0.03346		//density (use 0.03346)
		);
}

