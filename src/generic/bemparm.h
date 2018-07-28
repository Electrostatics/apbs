/** @defgroup BEMparm BEMparm class
 *  @brief    Parameter which holds useful parameters for generic multigrid
 *            calculations
 */

/**
 *  @file     beparm.h
 *  @ingroup  BEMparm
 *  @brief    Contains declarations for class BEMparm
 *  @version  $Id$
 *  @author   Nathan A. Baker, Weihua Geng, and Andrew J. Stevens
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


#ifndef _BEMPARM_H_
#define _BEMPARM_H_

/* Generic header files */
#include "maloc/maloc.h"

#include "generic/vhal.h"
#include "generic/vstring.h"

/**
 * @brief  Calculation type
 * @ingroup BEMparm
 */
enum eBEMparm_CalcType {
    BCT_MANUAL=0,  /**< bem-manual */
    BCT_NONE=1 /**< not defined */
};

/**
 * @brief  Declare BEMparm_CalcType type
 * @ingroup  BEMparm
 */
typedef enum eBEMparm_CalcType BEMparm_CalcType;

/**
 *  @ingroup BEMparm
 *  @author  Nathan Baker and Todd Dolinsky and Weihua Geng
 *  @brief   Parameter structure for BEM-specific variables from input files
 *  @note    If you add/delete/change something in this class, the member
 *           functions -- especially BEMparm_copy -- must be modified
 *           accordingly
 */
typedef struct sBEMparm {

    BEMparm_CalcType type;  /**< What type of BEM calculation? */
    int parsed;  /**< Has this structure been filled? (0 = no, 1 = yes) */

    /* *** GENERIC PARAMETERS *** */
    Vchrg_Src  chgs; /**< Charge source (Charge, Multipole, Induced Dipole,
                      * NL Induced.  Not currently implemented but should be relatively easy to add in the future (cf Pengyu Ren) */
    int tree_order;  /**< User-defined order for the treecode expansion */
    int settree_order;  /**< Flag, @see tree_order */
    int tree_n0; /**< Number of particles per leaf of the tree */
    int settree_n0; /**< Flag, @see tree_npart */
    double mac;  /**< Multipole acceptance criterion (should be between 0 and 1) */
    int setmac; /**< Flag, @see mac */
    int nonlintype; /**< Linearity Type Method to be used */
    int setnonlintype; /**< Flag, @see nonlintype */

    int mesh; /**< 0 for msms, 1 for NanoShaper SES, 2 for NanoShaper Skin */
    int setmesh; /**< Flag, @see mesh */

    int outdata; /**< 0 does not output vtk, 1 outputs vtk */
    int setoutdata; /**<Flag, @see outdata */

} BEMparm;

/** @brief   Construct BEMparm object
 *  @ingroup BEMparm
 *  @author  Nathan Baker
 *  @param   type Type of BEM calculation
 *  @returns Newly allocated and initialized BEMparm object
 */
VEXTERNC BEMparm*  BEMparm_ctor(BEMparm_CalcType type);

/** @brief   FORTRAN stub to construct BEMparm object
 *  @ingroup BEMparm
 *  @author  Nathan Baker and Todd Dolinsky
 *  @param   thee Space for BEMparm object
 *  @param   type Type of MG calculation
 *  @returns Success enumeration
 */
VEXTERNC Vrc_Codes      BEMparm_ctor2(BEMparm *thee, BEMparm_CalcType type);

/** @brief   Object destructor
 *  @ingroup BEMparm
 *  @author  Nathan Baker
 *  @param   thee  Pointer to memory location of BEMparm object
 */
VEXTERNC void     BEMparm_dtor(BEMparm **thee);

/** @brief   FORTRAN stub for object destructor
 *  @ingroup BEMparm
 *  @author  Nathan Baker
 *  @param   thee  Pointer to BEMparm object
 */
VEXTERNC void     BEMparm_dtor2(BEMparm *thee);

/** @brief   Consistency check for parameter values stored in object
 *  @ingroup BEMparm
 *  @author  Nathan Baker
 *  @param   thee   BEMparm object
 *  @returns Success enumeration
 */
VEXTERNC Vrc_Codes      BEMparm_check(BEMparm *thee);

/**
 * @brief Copy object info into thee
 * @author Nathan Baker
 * @param thee destination object
 * @param parm source object
 */
VEXTERNC void BEMparm_copy(BEMparm *thee, BEMparm *parm);

/** @brief   Parse an MG keyword from an input file
 *  @ingroup BEMparm
 *  @author  Nathan Baker and Todd Dolinsky
 *  @param   thee   BEMparm object
 *  @param   tok    Token to parse
 *  @param   sock   Stream for more tokens
 *  @returns Success enumeration (1 if matched and assigned; -1 if matched, but there's some sort
 *            of error (i.e., too few args); 0 if not matched)
 */
VEXTERNC Vrc_Codes      BEMparm_parseToken(BEMparm *thee, char tok[VMAX_BUFSIZE],
                    Vio *sock);

#endif
