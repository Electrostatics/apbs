/** @defgroup PBSAMparm PBSAMparm class
 *  @brief    Parameter which holds useful parameters for Poisson-boltzmann 
 *            analytical method calculations
 */

/**
 *  @file     pbsamparm.h
 *  @ingroup  PBSAMparm
 *  @brief    Contains declarations for class PBSAMparm
 *  @version  $Id$
 *  @author   Lisa Felberg
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
 * Copyright (c) 2010-2020 Battelle Memorial Institute. Developed at the
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


#ifndef _PBSAMPARM_H_
#define _PBSAMPARM_H_

/* Generic header files */
#include "maloc/maloc.h"

#include "generic/vhal.h"
#include "generic/vstring.h"

 /** @brief   Number of things that can be written out in a single calculation
 *  @ingroup PBSAMparm
 */
#define CHR_MAXLEN 1000
#define PBSAMPARM_MAXWRITE 15
#define PBSAMPARM_MAXMOL 150

/**
 * @brief  Calculation type
 * @ingroup PBSAMparm
 */
enum ePBSAMparm_CalcType {
    //other methods disabled for now only auto currently implemented.
	//PBSAMCT_MANUAL=0,  /**< PBSAM-manual */
    PBSAMCT_AUTO=1,  /**< PBSAM-auto */
    //PBSAMCT_NONE=2 /**< not defined */
};

/**
 * @brief  Declare PBSAMparm_CalcType type
 * @ingroup  PBSAMparm
 */
typedef enum ePBSAMparm_CalcType PBSAMparm_CalcType;

/**
 *  @ingroup PBSAMparm
 *  @author  Lisa Felberg
 *  @brief   Parameter structure for PBSAM-specific variables from input files
 *  @note    If you add/delete/change something in this class, the member
 *           functions -- especially PBSAMparm_copy -- must be modified
 *           accordingly
 */
typedef struct sPBSAMparm {

    PBSAMparm_CalcType type;  /**< What type of PBSAM calculation? */
    int parsed;  /**< Has this structure been filled? (0 = no, 1 = yes) */

    /* The only parms in addition to PBAM would be MSMS
       IMAT and Selfpol */
    int settolsp;
    double tolsp;

    int setmsms;
    double probe_radius;
    double density;

    int setsurf;
    int surfct;
    char surffil[PBSAMPARM_MAXMOL][CHR_MAXLEN];

    int setimat;
    int imatct;
    char imatfil[PBSAMPARM_MAXMOL][CHR_MAXLEN];

    int setexp;
    int expct;
    char expfil[PBSAMPARM_MAXMOL][CHR_MAXLEN];

} PBSAMparm;

/** @brief   Construct PBSAMparm object
 *  @ingroup PBSAMparm
 *  @author  Lisa Felberg
 *  @param   type Type of PBSAM calculation
 *  @returns Newly allocated and initialized PBSAMparm object
 */
VEXTERNC PBSAMparm* PBSAMparm_ctor(PBSAMparm_CalcType type);

/** @brief   FORTRAN stub to construct PBSAMparm object ?????????!!!!!!!
 *  @ingroup PBSAMparm
 *  @author  Lisa Felberg
 *  @param   thee Space for PBSAMparm object
 *  @param   type Type of MG calculation
 *  @returns Success enumeration
 */
VEXTERNC Vrc_Codes PBSAMparm_ctor2(PBSAMparm *thee, PBSAMparm_CalcType type);

/** @brief   Object destructor
 *  @ingroup PBSAMparm
 *  @author  Lisa Felberg
 *  @param   thee  Pointer to memory location of PBSAMparm object
 */
VEXTERNC void PBSAMparm_dtor(PBSAMparm **thee);

/** @brief   FORTRAN stub for object destructor   ?????????!!!!!!!!!!!!
 *  @ingroup PBSAMparm
 *  @author  Lisa Felberg
 *  @param   thee  Pointer to PBSAMparm object
 */
VEXTERNC void PBSAMparm_dtor2(PBSAMparm *thee);

/** @brief   Consistency check for parameter values stored in object
 *  @ingroup PBSAMparm
 *  @author  Lisa Felberg
 *  @param   thee   PBSAMparm object
 *  @returns Success enumeration
 */
VEXTERNC Vrc_Codes PBSAMparm_check(PBSAMparm *thee);

/** @brief   Parse an MG keyword from an input file
 *  @ingroup PBSAMparm
 *  @author  Lisa Felberg
 *  @param   thee   PBSAMparm object
 *  @param   tok    Token to parse
 *  @param   sock   Stream for more tokens
 *  @returns Success enumeration (1 if matched and assigned; -1 if matched, but there's some sort
 *            of error (i.e., too few args); 0 if not matched)
 */
VEXTERNC Vrc_Codes PBSAMparm_parseToken(PBSAMparm *thee, char tok[VMAX_BUFSIZE],
                    Vio *sock);
/**
 * @brief copy PBSAMparm object int thee.
 * @ingroup PBSAMparm
 * @author
 * @param thee PBSAMparm object to be copied into
 * @param parm PBSAMparm object.
 */
VEXTERNC void PBSAMparm_copy(PBSAMparm *thee, PBSAMparm *parm);

/**
 * @brief Find sphere tolerance for coarse-graining
 * @ingroup PBSAMparm
 * @author
 * @param thee PBSAMparm object to be copied into
 * @param sock The stream from which parameter is taken
 */
VPRIVATE Vrc_Codes PBSAMparm_parseTolsp(PBSAMparm *thee, Vio *sock);

/**
 * @brief Find vertex files for each molecule and save them
 * @ingroup PBSAMparm
 * @author
 * @param thee PBSAMparm object to be copied into
 * @param sock The stream from which parameter is taken
 */
VPRIVATE Vrc_Codes PBSAMparm_parseSurf(PBSAMparm *thee, Vio *sock);

/**
 * @brief Find IMAT files for each molecule and save them
 * @ingroup PBSAMparm
 * @author
 * @param thee PBSAMparm object to be copied into
 * @param sock The stream from which parameter is taken
 */
VPRIVATE Vrc_Codes PBSAMparm_parseImat(PBSAMparm *thee, Vio *sock);

/**
 * @brief Find expansion files for each molecule and save them
 * @ingroup PBSAMparm
 * @author
 * @param thee PBSAMparm object to be copied into
 * @param sock The stream from which parameter is taken
 */
VPRIVATE Vrc_Codes PBSAMparm_parseExp(PBSAMparm *thee, Vio *sock);

/**
 * @brief Find msms flag for if MSMS is to be run
 * @ingroup PBSAMparm
 * @author
 * @param thee PBSAMparm object to be copied into
 * @param sock The stream from which parameter is taken
 */
VPRIVATE Vrc_Codes PBSAMparm_parseMSMS(PBSAMparm *thee, Vio *sock);

#endif

