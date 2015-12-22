/** @defgroup GEOFLOWparm GEOFLOWparm class
 *  @brief    Parameter which holds useful parameters for GEOFLOWeric multigrid
 *            calculations
 */

/**
 *  @file     geoflowparm.h
 *  @ingroup  GEOFLOWparm
 *  @brief    Contains declarations for class GEOFLOWparm
 *  @version  $Id$
 *  @author   Andrew Stevens
 *
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
 * Copyright (c) 2010-2014 Battelle Memorial Institute. Developed at the
 * Pacific Northwest National Laboratory, operated by Battelle Memorial
 * Institute, Pacific Northwest Division for the U.S. Department of Energy.
 *
 * Portions Copyright (c) 2002-2010, Washington University in St. Louis.
 * Portions Copyright (c) 2002-2010, Nathan A. Baker.
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


#ifndef _GEOFLOWPARM_H_
#define _GEOFLOWPARM_H_

/* Generic header files */
#include "maloc/maloc.h"

#include "generic/vhal.h"
#include "generic/vstring.h"

/**
 * @brief  Calculation type
 * @ingroup GEOFLOWparm
 */
enum eGEOFLOWparm_CalcType {
    GFCT_MANUAL=0,  /**< GEOFLOW-manual */
    GFCT_AUTO=1,  /**< GEOFLOW-auto */
    GFCT_NONE=2 /**< not defined */
};

/**
 * @brief  Declare GEOFLOWparm_CalcType type
 * @ingroup  GEOFLOWparm
 */
typedef enum eGEOFLOWparm_CalcType GEOFLOWparm_CalcType;

/**
 *  @ingroup GEOFLOWparm
 *  @author  Andrew Stevens, Kyle Monson
 *  @brief   Parameter structure for GEOFLOW-specific variables from input files
 *  @note    If you add/delete/change something in this class, the member
 *           functions -- especially GEOFLOWparm_copy -- must be modified
 *           accordingly
 */
typedef struct sGEOFLOWparm {

    GEOFLOWparm_CalcType type;  /**< What type of GEOFLOW calculation? */
    int parsed;  /**< Has this structure been filled? (0 = no, 1 = yes) */

    /* *** GENERIC PARAMETERS *** */
//    double dcel;
//    double pres;
//    double gama;
    int vdw;
    
//    int setdcel;
//    int setpres;
//   int setgama;
    int setvdw;
    double etol; /**< user defined error tolerance */

} GEOFLOWparm;

/** @brief   Construct GEOFLOWparm object
 *  @ingroup GEOFLOWparm
 *  @author  Andrew Stevens, Kyle Monson
 *  @param   type Type of GEOFLOW calculation
 *  @returns Newly allocated and initialized GEOFLOWparm object
 */
VEXTERNC GEOFLOWparm*  GEOFLOWparm_ctor(GEOFLOWparm_CalcType type);

/** @brief   FORTRAN stub to construct GEOFLOWparm object ?????????!!!!!!!
 *  @ingroup GEOFLOWparm
 *  @author  Andrew Stevens, Kyle Monson
 *  @param   thee Space for GEOFLOWparm object
 *  @param   type Type of MG calculation
 *  @returns Success enumeration
 */
VEXTERNC Vrc_Codes      GEOFLOWparm_ctor2(GEOFLOWparm *thee, GEOFLOWparm_CalcType type);

/** @brief   Object destructor
 *  @ingroup GEOFLOWparm
 *  @author  Andrew Stevens, Kyle Monson
 *  @param   thee  Pointer to memory location of GEOFLOWparm object
 */
VEXTERNC void     GEOFLOWparm_dtor(GEOFLOWparm **thee);

/** @brief   FORTRAN stub for object destructor   ?????????!!!!!!!!!!!!
 *  @ingroup GEOFLOWparm
 *  @author  Andrew Stevens, Kyle Monson
 *  @param   thee  Pointer to GEOFLOWparm object
 */
VEXTERNC void     GEOFLOWparm_dtor2(GEOFLOWparm *thee);

/** @brief   Consistency check for parameter values stored in object
 *  @ingroup GEOFLOWparm
 *  @author  Andrew Stevens, Kyle Monson
 *  @param   thee   GEOFLOWparm object
 *  @returns Success enumeration
 */
VEXTERNC Vrc_Codes      GEOFLOWparm_check(GEOFLOWparm *thee);

/** @brief   Parse an MG keyword from an input file
 *  @ingroup GEOFLOWparm
 *  @author  Andrew Stevens, Kyle Monson
 *  @param   thee   GEOFLOWparm object
 *  @param   tok    Token to parse
 *  @param   sock   Stream for more tokens
 *  @returns Success enumeration (1 if matched and assigned; -1 if matched, but there's some sort
 *            of error (i.e., too few args); 0 if not matched)
 */
VEXTERNC Vrc_Codes      GEOFLOWparm_parseToken(GEOFLOWparm *thee, char tok[VMAX_BUFSIZE],
                    Vio *sock);
/**
 * @brief copy GEOFLOWparm object int thee.
 * @ingroup GEOFLOWparm
 * @author
 * @param thee GEOFLOWparm object to be copied into
 * @param parm GEOFLOWparm object.
 */
VEXTERNC void GEOFLOWparm_copy(GEOFLOWparm *thee, GEOFLOWparm *parm);

VPRIVATE Vrc_Codes GEOFLOWparm_parseVDW(GEOFLOWparm *thee, Vio *sock);

VPRIVATE Vrc_Codes GEOFLOWparm_parseETOL(GEOFLOWparm *thee, Vio *sock);



#endif

