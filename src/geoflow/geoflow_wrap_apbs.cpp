/**
 *  @file    geoflow_wrap_apbs.c
 *  @ingroup Frontend
 *  @author  Elizabeth Jurrus
 *  @brief   APBS-related Geoflow wrappers
 *
 *  @version $Id$
 *  @attention
 *  @verbatim
 *
 * APBS -- Adaptive Poisson-Boltzmann Solver
 *
 *  Nathan A. Baker (nathan.baker@pnnl.gov)
 *  Pacific Northwest National Laboratory
 *
 *  Additional contributing authors listed in the code documentation.
 *
 * Copyright (c) 2010-2020 Battelle Memorial Institute. Developed at the
 * Pacific Northwest National Laboratory, operated by Battelle Memorial
 * Institute, Pacific Northwest Division for the U.S. Department of Energy.
 *
 * Portions Copyright (c) 2002-2010, Washington University in St. Louis.
 * Portions Copyright (c) 2002-2020, Nathan A. Baker.
 * Portions Copyright (c) 1999-2002, The Regents of the University of
 * California.
 * Portions Copyright (c) 1995, Michael Holst.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of the developer nor the names of its contributors may be
 * used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @endverbatim
 */

#include "geoflow_wrap_apbs.h"

#include "GeometricFlow.h"

struct GeometricFlowOutput runGeometricFlowWrapAPBS
    ( struct GeometricFlowInput geoflowParams,
    Valist* molecules )   // or Valist* molecules[]
{
    //cout << "boo from GeometricFlowWrap!" << endl; 

    //
    //  create the GeometricFlow object
    //
    geoflow::GeometricFlow GF( geoflowParams );

    //
    //
    //cout << "converting atom list" << endl;
    geoflow::AtomList atomList;
    Vatom *atom;
    unsigned int natoms = Valist_getNumberAtoms(molecules);
    //cout << "natoms: " << natoms << endl;
    for (unsigned int i=0; i < natoms; i++) 
    {		
        atom = Valist_getAtom(molecules, i);
        //cout << "i: " << i << endl;
        geoflow::Atom myAtom( 
                GF.getFFModel(),
            Vatom_getPosition(atom)[0],
            Vatom_getPosition(atom)[1],		
            Vatom_getPosition(atom)[2],		
            Vatom_getRadius(atom) * GF.getRadExp(),
            Vatom_getCharge(atom) );
        atomList.add( myAtom );
    }
    //cout << "done with atom list" << endl;
    //atomList.print();

    //
    //  run Geoflow!
    //
    struct GeometricFlowOutput GFO = GF.run( atomList );
    
    return GFO;
}
