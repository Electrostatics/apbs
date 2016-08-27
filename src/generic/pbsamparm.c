/**
 *  @file    pbsamparm.c
 *  @ingroup PBSAMparm
 *  @author  Andrew Stevens
 *  @brief   Class PBSAMparm methods
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

#include "pbsamparm.h"

VEMBED(rcsid="$Id$")

#if !defined(VINLINE_MGPARM)

#endif /* if !defined(VINLINE_MGPARM) */


VPUBLIC PBSAMparm* PBSAMparm_ctor(PBSAMparm_CalcType type) {

    /* Set up the structure */
    PBSAMparm *thee = VNULL;
    thee = (PBSAMparm*)Vmem_malloc(VNULL, 1, sizeof(PBSAMparm));
    VASSERT( thee != VNULL);
    VASSERT( PBSAMparm_ctor2(thee, type) == VRC_SUCCESS );

    return thee;
}

VPUBLIC Vrc_Codes PBSAMparm_ctor2(PBSAMparm *thee, PBSAMparm_CalcType type) {

    int i;

    if (thee == VNULL) return VRC_FAILURE;

    thee->tolsp = 2.5;
    thee->setmsms = 0;
    thee->probe_radius = 1.5;
    thee->density = 3.0;

    thee->setsurf = 0;
    thee->surfct = 0;

    thee->setimat = 0;
    thee->imatct = 0;

    thee->setexp = 0;
    thee->expct = 0;

    return VRC_SUCCESS;
}

VPUBLIC void PBSAMparm_dtor(PBSAMparm **thee) {
    if ((*thee) != VNULL) {
        PBSAMparm_dtor2(*thee);
        Vmem_free(VNULL, 1, sizeof(PBSAMparm), (void **)thee);
        (*thee) = VNULL;
    }
}

VPUBLIC void PBSAMparm_dtor2(PBSAMparm *thee) { ; }

VPUBLIC Vrc_Codes PBSAMparm_check(PBSAMparm *thee) {

    Vrc_Codes rc;

    rc = VRC_SUCCESS;

    Vnm_print(0, "PBSAMparm_check:  checking PBSAMparm object of type %d.\n",
              thee->type);

    /* Check to see if we were even filled... */
    if (!thee->parsed) {
        Vnm_print(2, "PBSAMparm_check:  not filled!\n");
        return VRC_FAILURE;
    }


    /* Check type settings */
    if(thee->type != PBSAMCT_AUTO) {
         Vnm_print(2,"PBSAMparm_check: type not set");
         rc = VRC_FAILURE;
    }

    return rc;
}

VPUBLIC void PBSAMparm_copy(PBSAMparm *thee, PBSAMparm *parm) {
    int i, j;
    VASSERT(thee != VNULL);
    VASSERT(parm != VNULL);


    thee->settolsp = parm->settolsp;
    thee->tolsp = parm->tolsp;

    thee->setmsms = parm->setmsms;
    thee->probe_radius = parm->probe_radius;
    thee->density = parm->density;
    thee->setsurf = parm->setsurf;
    thee->surfct  = parm->surfct;
    thee->setimat = parm->setimat;
    thee->imatct  = parm->imatct;
    thee->setexp  = parm->setexp;
    thee->expct   = parm->expct;

    for (i=0; i<PBSAMPARM_MAXWRITE; i++)
    {
        for (j=0; j<CHR_MAXLEN; j++)
        { 
            thee->surffil[i][j] = parm->surffil[i][j];
            thee->imatfil[i][j] = parm->imatfil[i][j];
            thee->expfil[i][j] = parm->expfil[i][j];
        }
    }
}

//Parsing vertex file
VPRIVATE Vrc_Codes PBSAMparm_parseSurf(PBSAMparm *thee, Vio *sock){
    const char* name = "surf";
    char tok[VMAX_BUFSIZE];

    if(Vio_scanf(sock, "%s", tok) == 0) {
        Vnm_print(2, "parsePBSAM:  ran out of tokens on %s!\n", name);
        return VRC_WARNING;
    } else {
       strncpy(thee->surffil[thee->surfct], tok, CHR_MAXLEN);
      thee->surfct += 1;
    }
    return VRC_SUCCESS;
}


//Parsing imat prefix file
VPRIVATE Vrc_Codes PBSAMparm_parseMSMS(PBSAMparm *thee, Vio *sock){
    thee->setmsms = 1;
    return VRC_SUCCESS;
}
//Parsing imat prefix file
VPRIVATE Vrc_Codes PBSAMparm_parseImat(PBSAMparm *thee, Vio *sock){
    const char* name = "imat";
    char tok[VMAX_BUFSIZE];

    if(Vio_scanf(sock, "%s", tok) == 0) {
        Vnm_print(2, "parsePBSAM:  ran out of tokens on %s!\n", name);
        return VRC_WARNING;
    } else {
       strncpy(thee->imatfil[thee->imatct], tok, CHR_MAXLEN);
      thee->imatct += 1;
    }
    return VRC_SUCCESS;
}

//Parsing imat prefix file
VPRIVATE Vrc_Codes PBSAMparm_parseExp(PBSAMparm *thee, Vio *sock){
    const char* name = "exp";
    char tok[VMAX_BUFSIZE];

    if(Vio_scanf(sock, "%s", tok) == 0) {
        Vnm_print(2, "parsePBSAM:  ran out of tokens on %s!\n", name);
        return VRC_WARNING;
    } else {
       strncpy(thee->expfil[thee->expct], tok, CHR_MAXLEN);
      thee->expct += 1;
    }
    return VRC_SUCCESS;
}

VPRIVATE Vrc_Codes PBSAMparm_parseTolsp(PBSAMparm *thee, Vio *sock){
    const char* name = "tolsp";
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
        thee->tolsp = tf; 
    }   
    thee->settolsp = 1;
    return VRC_SUCCESS;
}


VPUBLIC Vrc_Codes PBSAMparm_parseToken(PBSAMparm *thee, char tok[VMAX_BUFSIZE],
  Vio *sock) {

    if (thee == VNULL) {
        Vnm_print(2, "parsePBSAM:  got NULL thee!\n");
        return VRC_WARNING;
    }
    if (sock == VNULL) {
        Vnm_print(2, "parsePBSAM:  got NULL socket!\n");
        return VRC_WARNING;
    }

    Vnm_print(0, "PBSAMparm_parseToken:  trying %s...\n", tok);

    // Molecule terms
    if (Vstring_strcasecmp(tok, "surf") == 0) {
        return PBSAMparm_parseSurf(thee, sock);
    }else if (Vstring_strcasecmp(tok, "msms") == 0) {
        return PBSAMparm_parseMSMS(thee, sock);
    }else if (Vstring_strcasecmp(tok, "imat") == 0) {
        return PBSAMparm_parseImat(thee, sock);
    }else if (Vstring_strcasecmp(tok, "exp") == 0) {
        return PBSAMparm_parseExp(thee, sock);
    }else if (Vstring_strcasecmp(tok, "tolsp") == 0) {
        return PBSAMparm_parseTolsp(thee, sock);
    }

    else {
        Vnm_print(2, "parsePBSAM:  Unrecognized keyword (%s)!\n", tok);
        return VRC_WARNING;
    }
    return VRC_FAILURE;
}
