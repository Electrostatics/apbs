/**
 *  @file    pbamparm.c
 *  @ingroup PBAMparm
 *  @author  Andrew Stevens
 *  @brief   Class PBAMparm methods
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

#include "pbamparm.h"

VEMBED(rcsid="$Id$")

#if !defined(VINLINE_MGPARM)

#endif /* if !defined(VINLINE_MGPARM) */


VPUBLIC PBAMparm* PBAMparm_ctor(PBAMparm_CalcType type) {

    /* Set up the structure */
    PBAMparm *thee = VNULL;
    thee = (PBAMparm*)Vmem_malloc(VNULL, 1, sizeof(PBAMparm));
    VASSERT( thee != VNULL);
    VASSERT( PBAMparm_ctor2(thee, type) == VRC_SUCCESS );

    return thee;
}

VPUBLIC Vrc_Codes PBAMparm_ctor2(PBAMparm *thee, PBAMparm_CalcType type) {

    int i;

    if (thee == VNULL) return VRC_FAILURE;

    thee->parsed = 0; 
    thee->type = type;
    thee->salt = 0;
    

    return VRC_SUCCESS;
}

VPUBLIC void PBAMparm_dtor(PBAMparm **thee) {
    if ((*thee) != VNULL) {
        PBAMparm_dtor2(*thee);
        Vmem_free(VNULL, 1, sizeof(PBAMparm), (void **)thee);
        (*thee) = VNULL;
    }
}

VPUBLIC void PBAMparm_dtor2(PBAMparm *thee) { ; }

VPUBLIC Vrc_Codes PBAMparm_check(PBAMparm *thee) {

    Vrc_Codes rc;

    rc = VRC_SUCCESS;

    Vnm_print(0, "PBAMparm_check:  checking PBAMparm object of type %d.\n",
              thee->type);

    /* Check to see if we were even filled... */
    if (!thee->parsed) {
        Vnm_print(2, "PBAMparm_check:  not filled!\n");
        return VRC_FAILURE;
    }


    /* Check type settings */
    //if ((thee->type != GFCT_MANUAL)&& (thee->type != GFCT_AUTO)&& (thee->type != GFCT_NONE)) {
    if(thee->type != PBAMCT_AUTO) {
         Vnm_print(2,"PBAMparm_check: type not set");
         rc = VRC_FAILURE;
    }

    return rc;
}

VPUBLIC void PBAMparm_copy(PBAMparm *thee, PBAMparm *parm) {
    VASSERT(thee != VNULL);
    VASSERT(parm != VNULL);

    thee->type = parm->type;
    thee->parsed = parm->parsed;

    thee->salt = parm->salt;
    thee->setsalt = parm->setsalt;
    for (int i=0; i<VMAX_ARGLEN; i++) thee->runtype[i] = parm->runtype[i];
    thee->setruntype = parm->setruntype;

    for (int i=0; i<VMAX_ARGLEN; i++) thee->runname[i] = parm->runname[i];
    thee->setrunname = parm->setrunname;
}


VPRIVATE Vrc_Codes PBAMparm_parseSalt(PBAMparm *thee, Vio *sock){
    const char* name = "salt";
    char tok[VMAX_BUFSIZE];
	double tf;
    if(Vio_scanf(sock, "%s", tok) == 0) {
        Vnm_print(2, "parsePBAM:  ran out of tokens on %s!\n", name);
        return VRC_WARNING;
    }
    
    if (sscanf(tok, "%lf", &tf) == 0){
        Vnm_print(2, "NOsh:  Read non-float (%s) while parsing %s keyword!\n", tok, name);
        return VRC_WARNING;
    }else{
        thee->salt = tf;
    }
    thee->setsalt = 1;
    return VRC_SUCCESS;
}

VPRIVATE Vrc_Codes PBAMparm_parseRunType(PBAMparm *thee, Vio *sock){
    const char* name = "runtype";
    char tok[VMAX_BUFSIZE];

    if(Vio_scanf(sock, "%s", tok) == 0) {
      Vnm_print(2, "parsePBAM:  ran out of tokens on %s!\n", name);
      return VRC_WARNING;
    } else {
      strncpy(thee->runtype, tok, VMAX_ARGLEN);
      thee->setruntype=1;
    }

    return VRC_SUCCESS;
}

VPRIVATE Vrc_Codes PBAMparm_parseRunName(PBAMparm *thee, Vio *sock){
    const char* name = "runname";
    char tok[VMAX_BUFSIZE];

    if(Vio_scanf(sock, "%s", tok) == 0) {
      Vnm_print(2, "parsePBAM:  ran out of tokens on %s!\n", name);
      return VRC_WARNING;
    } else {
      strncpy(thee->runname, tok, VMAX_ARGLEN);
      thee->setrunname=1;
    }

    return VRC_SUCCESS;
}

VPRIVATE Vrc_Codes PBAMparm_parseRandorient(PBAMparm *thee, Vio *sock){
    const char* name = "randorient";
    thee->setrandorient=1;
    return VRC_SUCCESS;
}

VPRIVATE Vrc_Codes PBAMparm_parseSalt(PBAMparm *thee, Vio *sock){
    const char* name = "salt";
    char tok[VMAX_BUFSIZE];
    double tf;
    if(Vio_scanf(sock, "%s", tok) == 0) {
        Vnm_print(2, "parsePBAM:  ran out of tokens on %s!\n", name);
        return VRC_WARNING;
    }
    
    if (sscanf(tok, "%lf", &tf) == 0){
        Vnm_print(2, "NOsh:  Read non-float (%s) while parsing %s keyword!\n", tok, name);
        return VRC_WARNING;
    }else{
        thee->salt = tf;
    }
    thee->setsalt = 1;
    return VRC_SUCCESS;
}


VPUBLIC Vrc_Codes PBAMparm_parseToken(PBAMparm *thee, char tok[VMAX_BUFSIZE],
  Vio *sock) {

    if (thee == VNULL) {
        Vnm_print(2, "parsePBAM:  got NULL thee!\n");
        return VRC_WARNING;
    }
    if (sock == VNULL) {
        Vnm_print(2, "parsePBAM:  got NULL socket!\n");
        return VRC_WARNING;
    }

    Vnm_print(0, "PBAMparm_parseToken:  trying %s...\n", tok);

    if (Vstring_strcasecmp(tok, "salt") == 0) {
        return PBAMparm_parseSalt(thee, sock);
    }else if (Vstring_strcasecmp(tok, "runtype") == 0) {
        return PBAMparm_parseRunType(thee, sock);
    }else if (Vstring_strcasecmp(tok, "runname") == 0) {
        return PBAMparm_parseRunName(thee, sock);
    }else if (Vstring_strcasecmp(tok, "randorient") == 0) {
        return PBAMparm_parseRandorient(thee, sock);
    }else if (Vstring_strcasecmp(tok, "pbc") == 0) {
        return PBAMparm_parsePBCS(thee, sock);
    }else if (Vstring_strcasecmp(tok, "3dmap") == 0) {
        return PBAMparm_parse3Dmap(thee, sock);
    }else if (Vstring_strcasecmp(tok, "grid2d") == 0) {
        return PBAMparm_parseGrid2D(thee, sock);
    }else {
        Vnm_print(2, "parsePBAM:  Unrecognized keyword (%s)!\n", tok);
        return VRC_WARNING;
    }
    return VRC_FAILURE;
}
