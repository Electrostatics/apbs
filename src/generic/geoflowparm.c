/**
 *  @file    geoflowparm.c
 *  @ingroup GEOFLOWparm
 *  @author  Andrew Stevens
 *  @brief   Class GEOFLOWparm methods
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
 * Copyright (c) 2010-2012 Battelle Memorial Institute. Developed at the
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

#include "geoflowparm.h"

VEMBED(rcsid="$Id$")

#if !defined(VINLINE_MGPARM)

#endif /* if !defined(VINLINE_MGPARM) */


VPUBLIC GEOFLOWparm* GEOFLOWparm_ctor(GEOFLOWparm_CalcType type) {

    /* Set up the structure */
    GEOFLOWparm *thee = VNULL;
    thee = (GEOFLOWparm*)Vmem_malloc(VNULL, 1, sizeof(GEOFLOWparm));
    VASSERT( thee != VNULL);
    VASSERT( GEOFLOWparm_ctor2(thee, type) == VRC_SUCCESS );

    return thee;
}

VPUBLIC Vrc_Codes GEOFLOWparm_ctor2(GEOFLOWparm *thee, GEOFLOWparm_CalcType type) {

    int i;

    if (thee == VNULL) return VRC_FAILURE;

    thee->parsed = 0;
    thee->type = type;
    thee->vdw = 0;
//    thee->dcel = 0.25;
//    thee->pres = 0.008;
//    thee->gama = 0.0001;

    return VRC_SUCCESS;
}

VPUBLIC void GEOFLOWparm_dtor(GEOFLOWparm **thee) {
    if ((*thee) != VNULL) {
        GEOFLOWparm_dtor2(*thee);
        Vmem_free(VNULL, 1, sizeof(GEOFLOWparm), (void **)thee);
        (*thee) = VNULL;
    }
}

VPUBLIC void GEOFLOWparm_dtor2(GEOFLOWparm *thee) { ; }

VPUBLIC Vrc_Codes GEOFLOWparm_check(GEOFLOWparm *thee) {

    Vrc_Codes rc;

    rc = VRC_SUCCESS;

    Vnm_print(0, "GEOFLOWparm_check:  checking GEOFLOWparm object of type %d.\n",
              thee->type);

    /* Check to see if we were even filled... */
    if (!thee->parsed) {
        Vnm_print(2, "GEOFLOWparm_check:  not filled!\n");
        return VRC_FAILURE;
    }


    /* Check type settings */
    if ((thee->type != GFCT_MANUAL)&& (thee->type != GFCT_AUTO)&& (thee->type != GFCT_NONE)) {
         Vnm_print(2,"GEOFLOWparm_check: type not set");
         rc = VRC_FAILURE;
    }

    return rc;
}

VPUBLIC void GEOFLOWparm_copy(GEOFLOWparm *thee, GEOFLOWparm *parm) {
    VASSERT(thee != VNULL);
    VASSERT(parm != VNULL);

    thee->type = parm->type;
    thee->parsed = parm->parsed;

    thee->vdw = parm->vdw;
//    thee->dcel = parm->dcel;
//    thee->pres = parm->pres;
//    thee->gama = parm->gama;
}

Vrc_Codes FUBAR(const char* name){
    Vnm_print(2, "parseGEOFLOW:  ran out of tokens on %s!\n", name);
    return VRC_WARNING;
}

Vrc_Codes parseNonNeg(double* tf, double def, int* set, char* name, Vio* sock){
    char tok[VMAX_BUFSIZE];
    if(Vio_scanf(sock, "%s", tok) == 0) {
        *tf = def;
        return FUBAR(name);
    }

    if (sscanf(tok, "%lf", tf) == 0){
        Vnm_print(2, "NOsh:  Read non-float (%s) while parsing %s keyword!\n", tok, name);
        *tf = def;
        return VRC_WARNING;
    }else if(*tf < 0.0){
        Vnm_print(2, "parseGEOFLOW:  %s must be greater than 0!\n", name);
        *tf = def;
        return VRC_WARNING;
    }

    *set = 1;
    return VRC_SUCCESS;
}

VPRIVATE Vrc_Codes GEOFLOWparm_parseVDW(GEOFLOWparm *thee, Vio *sock){
    const char* name = "vdw";
    char tok[VMAX_BUFSIZE];
    if(Vio_scanf(sock, "%s", tok) == 0) {
        return FUBAR(name);
    }

    int tf;
    if (sscanf(tok, "%u", &tf) == 0){
        Vnm_print(2, "NOsh:  Read non-unsigned int (%s) while parsing %s keyword!\n", tok, name);
        return VRC_WARNING;
    }else if(tf != 0 && tf != 1){
        Vnm_print(2, "parseGEOFLOW:  %s must be 0 or 1!\n", name);
        return VRC_WARNING;
    }else{
        thee->vdw = tf;
    }
    thee->setvdw = 1;
    return VRC_SUCCESS;
}

VPUBLIC Vrc_Codes GEOFLOWparm_parseToken(GEOFLOWparm *thee, char tok[VMAX_BUFSIZE],
  Vio *sock) {

    if (thee == VNULL) {
        Vnm_print(2, "parseGEOFLOW:  got NULL thee!\n");
        return VRC_WARNING;
    }
    if (sock == VNULL) {
        Vnm_print(2, "parseGEOFLOW:  got NULL socket!\n");
        return VRC_WARNING;
    }

    Vnm_print(0, "GEOFLOWparm_parseToken:  trying %s...\n", tok);


//    if (Vstring_strcasecmp(tok, "press") == 0) {
//        return parseNonNeg(&(thee->pres), 0.008, &(thee->setpres), "pres", sock);
//    } else if (Vstring_strcasecmp(tok, "gamma") == 0) {
//        return parseNonNeg(&(thee->gama), 0.0001, &(thee->setgama), "gama", sock);
//    } else if (Vstring_strcasecmp(tok, "grid") == 0) {
//        return parseNonNeg(&(thee->dcel), 0.25, &(thee->setdcel), "dcel", sock);
//    } else
    if (Vstring_strcasecmp(tok, "vdwdisp") == 0) {
        return GEOFLOWparm_parseVDW(thee, sock);
    } else {
        Vnm_print(2, "parseGEOFLOW:  Unrecognized keyword (%s)!\n", tok);
        return VRC_WARNING;
    }

    return VRC_FAILURE;
}
