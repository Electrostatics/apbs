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
    
    thee->setsalt = 0;
    thee->setruntype = 0;
    thee->setrunname = 0;
    thee->setunits = 0;

    thee->setrandorient = 0;

    thee->setpbcs = 0;
    thee->pbcboxlen = 1e15;

    // Electrostatics
    thee->gridpt = 15;
    printf("Found a pts flag in ctor: %d\n", thee->gridpt);
    thee->setgridpt = 0;
    thee->set3dmap = 0;
    thee->setgrid2Dname = 0;
    thee->grid2Dct = 0;
    thee->setdxname = 0;

    //Dynamics
    thee->ntraj = 1;
    thee->setntraj = 0;

    thee->settermcombine = 0;
    thee->diffct = 0;

    thee->termct = 0;
    thee->setterm = 0;

    thee->setxyz = 0;
    for (i = 0; i<PBAMPARM_MAXMOL; i++) thee->xyzct[i] = 0;

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
    if(thee->type != PBAMCT_AUTO) {
         Vnm_print(2,"PBAMparm_check: type not set");
         rc = VRC_FAILURE;
    }

    return rc;
}

VPUBLIC void PBAMparm_copy(PBAMparm *thee, PBAMparm *parm) {
    int i, j, k;
    VASSERT(thee != VNULL);
    VASSERT(parm != VNULL);

    thee->type = parm->type;
    thee->parsed = parm->parsed;

    thee->salt = parm->salt;
    thee->setsalt = parm->setsalt;
    for (i=0; i<CHR_MAXLEN; i++) thee->runtype[i] = parm->runtype[i];
    thee->setruntype = parm->setruntype;

    for (i=0; i<CHR_MAXLEN; i++) thee->runname[i] = parm->runname[i];
    thee->setrunname = parm->setrunname;

    thee->setrandorient = parm->setrandorient;

    thee->setpbcs = parm->setpbcs;
    thee->pbcboxlen = parm->pbcboxlen;

    for (i=0; i<CHR_MAXLEN; i++) thee->units[i] = parm->units[i];
    thee->setunits = parm->setunits;

    // Electrostatic parts
    thee->gridpt = parm->gridpt;
    thee->setgridpt = parm->setgridpt;
    for (i=0; i<CHR_MAXLEN; i++) thee->map3dname[i] = parm->map3dname[i];
    thee->set3dmap = parm->set3dmap;


    thee->grid2Dct = parm->grid2Dct;
    for (i=0; i<PBAMPARM_MAXWRITE; i++)
    {
        for (j=0; j<CHR_MAXLEN; j++)
        { 
            thee->grid2Dname[i][j] = parm->grid2Dname[i][j];
            thee->grid2Dax[i][j] = parm->grid2Dax[i][j];
        }
        thee->grid2Dloc[i] = parm->grid2Dloc[i];
    }

    for (i=0; i<CHR_MAXLEN; i++) thee->dxname[i] = parm->dxname[i];
    thee->setdxname = parm->setdxname;

    // Dynamics parts
    thee->ntraj = parm->ntraj;
    thee->setntraj = parm->setntraj;

    for (i=0; i<CHR_MAXLEN; i++) thee->termcombine[i] = parm->termcombine[i];
    thee->settermcombine = parm->settermcombine;

    thee->diffct = parm->diffct;

    for (i=0; i<PBAMPARM_MAXMOL; i++)
    {
        for (j=0; j<CHR_MAXLEN; j++)
        { 
            thee->moveType[i][j] = parm->moveType[i][j];
        }
        thee->transDiff[i] = parm->transDiff[i];
        thee->rotDiff[i] = parm->rotDiff[i];
    }

    thee->termct = parm->termct;
    thee->setterm = parm->setterm;
    thee->confilct = parm->confilct;

    for (i=0; i<PBAMPARM_MAXWRITE; i++)
    {
        for (j=0; j<CHR_MAXLEN; j++)
        { 
            thee->termnam[i][j] = parm->termnam[i][j];
            thee->confil[i][j] = parm->confil[i][j];
        }
        thee->termVal[i] = parm->termVal[i];
        thee->termnu[i][0] = parm->termnu[i][0];
    }

    thee->setxyz = parm->setxyz;
    for (i = 0; i<PBAMPARM_MAXMOL; i++) 
    {
        thee->xyzct[i] = parm->xyzct[i];
        for (j = 0; j<PBAMPARM_MAXWRITE; j++) 
        {
            for (k = 0; k<CHR_MAXLEN; k++) 
            {
                thee->xyzfil[i][j][k] = parm->xyzfil[i][j][k];
            }
        }
    }

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
    } else if(Vstring_strcasecmp(tok, "dynamics") == 0){
      Vnm_print(2, "parsePBAM:  Dynamics has been moved out of the ELEC section!\n");
      return VRC_WARNING;
    } else {
      strncpy(thee->runtype, tok, CHR_MAXLEN);
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
      strncpy(thee->runname, tok, CHR_MAXLEN);
      thee->setrunname=1;
    }

    return VRC_SUCCESS;
}

VPRIVATE Vrc_Codes PBAMparm_parseRandorient(PBAMparm *thee, Vio *sock){
    const char* name = "randorient";
    thee->setrandorient=1;
    return VRC_SUCCESS;
}

VPRIVATE Vrc_Codes PBAMparm_parsePBCS(PBAMparm *thee, Vio *sock){
    const char* name = "pbc";
    char tok[VMAX_BUFSIZE];
    double tf;
    int td;
    if(Vio_scanf(sock, "%s", tok) == 0) {
        Vnm_print(2, "parsePBAM:  ran out of tokens on %s!\n", name);
        return VRC_WARNING;
    }
    
    if (sscanf(tok, "%d", &td) == 0) {
        Vnm_print(2, "parsePBAM:  Read non-int (%s) while parsing pbc keyword!\n", tok);
        return VRC_FAILURE;
    } else{
        thee->setpbcs = td;
    }

    if (sscanf(tok, "%lf", &tf) == 0){
        Vnm_print(2, "NOsh:  Read non-float (%s) while parsing %s keyword!\n", tok, name);
        return VRC_WARNING;
    }else{
        thee->pbcboxlen = tf;
    }
    return VRC_SUCCESS;
}

VPRIVATE Vrc_Codes PBAMparm_parseUnits(PBAMparm *thee, Vio *sock){
    const char* name = "units";
    char tok[VMAX_BUFSIZE];

    if(Vio_scanf(sock, "%s", tok) == 0) {
      Vnm_print(2, "parsePBAM:  ran out of tokens on %s!\n", name);
      return VRC_WARNING;
    } else {
      strncpy(thee->units, tok, CHR_MAXLEN);
      thee->setunits=1;
    }

    return VRC_SUCCESS;
}

VPRIVATE Vrc_Codes PBAMparm_parseGridPts(PBAMparm *thee, Vio *sock){
    const char* name = "dime";
    char tok[VMAX_BUFSIZE];
    int td;
    if(Vio_scanf(sock, "%s", tok) == 0) {
        Vnm_print(2, "parsePBAM:  ran out of tokens on %s!\n", name);
        return VRC_WARNING;
    }
    
    if (sscanf(tok, "%d", &td) == 0){
        Vnm_print(2, "NOsh:  Read non-integer (%s) while parsing %s keyword!\n", tok, name);
        return VRC_WARNING;
    }else{
        printf("Found a dime flag in parse: %d\n", td);
        thee->gridpt = td;
    }
    thee->setgridpt = 1;
    return VRC_SUCCESS;
}

VPRIVATE Vrc_Codes PBAMparm_parse3Dmap(PBAMparm *thee, Vio *sock){
    const char* name = "3dmap";
    char tok[VMAX_BUFSIZE];

	Vnm_print(2, "PBAM: 3dmap keyword has been deprecated! Please use in conjuction with the write keyword.");
	return VRC_FAILURE;

	/*
    if(Vio_scanf(sock, "%s", tok) == 0) {
      Vnm_print(2, "parsePBAM:  ran out of tokens on %s!\n", name);
      return VRC_WARNING;
    } else {
      strncpy(thee->map3dname, tok, CHR_MAXLEN);
      thee->set3dmap=1;
    }
	
    return VRC_SUCCESS;
	*/
}

VPRIVATE Vrc_Codes PBAMparm_parseGrid2D(PBAMparm *thee, Vio *sock){
    const char* name = "grid2d";
    char tok[VMAX_BUFSIZE];
    double tf;

    if(Vio_scanf(sock, "%s", tok) == 0) {
        Vnm_print(2, "parsePBAM:  ran out of tokens on %s!\n", name);
        return VRC_WARNING;
    } else {
      strncpy(thee->grid2Dname[thee->grid2Dct], tok, CHR_MAXLEN);
      thee->setgrid2Dname=1;
    }
    
    if(Vio_scanf(sock, "%s", tok) == 0) {
        Vnm_print(2, "parsePBAM:  ran out of tokens on %s!\n", name);
        return VRC_WARNING;
    } else {
      strncpy(thee->grid2Dax[thee->grid2Dct], tok, CHR_MAXLEN);
    }

    if(Vio_scanf(sock, "%s", tok) == 0) {
        Vnm_print(2, "parsePBAM:  ran out of tokens on %s!\n", name);
        return VRC_WARNING;
    }

    if (sscanf(tok, "%lf", &tf) == 0){
        Vnm_print(2, "NOsh:  Read non-float (%s) while parsing %s keyword!\n", tok, name);
        return VRC_WARNING;
    }else{
        thee->grid2Dloc[thee->grid2Dct] = tf;
        thee->grid2Dct = thee->grid2Dct+1;
    }
    return VRC_SUCCESS;
}

VPRIVATE Vrc_Codes PBAMparm_parseDX(PBAMparm *thee, Vio *sock){
	Vnm_print(2, "PBAM's dx keyword is deprecated. Please use the write keyword!\n");
	return VRC_FAILURE;
	/*
	const char* name = "dx";
    char tok[VMAX_BUFSIZE];

    if(Vio_scanf(sock, "%s", tok) == 0) {
      Vnm_print(2, "parsePBAM:  ran out of tokens on %s!\n", name);
      return VRC_WARNING;
    } else {
      strncpy(thee->dxname, tok, CHR_MAXLEN);
      thee->setdxname=1;
    }
    return VRC_SUCCESS;
	*/
}

VPRIVATE Vrc_Codes PBAMparm_parseTermcombine(PBAMparm *thee, Vio *sock){
    const char* name = "termcombine";
    char tok[VMAX_BUFSIZE];

    if(Vio_scanf(sock, "%s", tok) == 0) {
      Vnm_print(2, "parsePBAM:  ran out of tokens on %s!\n", name);
      return VRC_WARNING;
    } else {
      strncpy(thee->termcombine, tok, CHR_MAXLEN);
      thee->settermcombine=1;
    }
    return VRC_SUCCESS;
}

VPRIVATE Vrc_Codes PBAMparm_parseNtraj(PBAMparm *thee, Vio *sock){
    const char* name = "ntraj";
    char tok[VMAX_BUFSIZE];
    int td;
    if(Vio_scanf(sock, "%s", tok) == 0) {
        Vnm_print(2, "parsePBAM:  ran out of tokens on %s!\n", name);
        return VRC_WARNING;
    }
    
    if (sscanf(tok, "%d", &td) == 0){
        Vnm_print(2, "NOsh:  Read non-integer (%s) while parsing %s keyword!\n", tok, name);
        return VRC_WARNING;
    }else{
        thee->ntraj = td;
    }
    thee->setntraj = 1;
    return VRC_SUCCESS;
}

VPRIVATE Vrc_Codes PBAMparm_parseDiff(PBAMparm *thee, Vio *sock){
    const char* name = "diff";
    char tok[VMAX_BUFSIZE];
    int molind;
    double tf;

    if(Vio_scanf(sock, "%s", tok) == 0) {
        Vnm_print(2, "parsePBAM:  ran out of tokens on %s!\n", name);
        return VRC_WARNING;
    } 

    // // looking for index
    if (sscanf(tok, "%d", &molind) == 0){
        Vnm_print(2, "NOsh:  Read non-int (%s) while parsing %s keyword!\n", tok, name);
        return VRC_WARNING;
    }
    
    molind -= 1;
    // looking for move type = move, stat, rot
    if(Vio_scanf(sock, "%s", tok) == 0) {
        Vnm_print(2, "parsePBAM:  ran out of tokens on %s!\n", name);
        return VRC_WARNING;
    } else {
       strncpy(thee->moveType[molind], tok, CHR_MAXLEN);
      thee->diffct += 1;
    }

    if (strncmp(thee->moveType[molind], "move", 4) == 0)
    {
      if(Vio_scanf(sock, "%s", tok) == 0) {
          Vnm_print(2, "parsePBAM:  ran out of tokens on %s!\n", name);
          return VRC_WARNING;
      }
      if (sscanf(tok, "%lf", &tf) == 0){
          Vnm_print(2, "NOsh:  Read non-float (%s) while parsing %s keyword!\n", tok, name);
          return VRC_WARNING;
      }else{
          thee->transDiff[molind] = tf;
      }

      // rot diffusion coeff
      if(Vio_scanf(sock, "%s", tok) == 0) {
          Vnm_print(2, "parsePBAM:  ran out of tokens on %s!\n", name);
          return VRC_WARNING;
      }
      if (sscanf(tok, "%lf", &tf) == 0){
          Vnm_print(2, "NOsh:  Read non-float (%s) while parsing %s keyword!\n", tok, name);
          return VRC_WARNING;
      }else{
          thee->rotDiff[molind] = tf;
      }
    } else if (strncmp(thee->moveType[molind], "rot", 3) == 0)
    {
      if(Vio_scanf(sock, "%s", tok) == 0) {
          Vnm_print(2, "parsePBAM:  ran out of tokens on %s!\n", name);
          return VRC_WARNING;
      }
      if (sscanf(tok, "%lf", &tf) == 0){
          Vnm_print(2, "NOsh:  Read non-float (%s) while parsing %s keyword!\n", tok, name);
          return VRC_WARNING;
      }else{
          thee->rotDiff[molind] = tf;
          thee->transDiff[molind] = 0.0;
      }
    } else{
       thee->transDiff[molind] = 0.0;
       thee->rotDiff[molind] = 0.0;
    }

    return VRC_SUCCESS;
}

VPRIVATE Vrc_Codes PBAMparm_parseTerm(PBAMparm *thee, Vio *sock){
    const char* name = "term";
    char tok[VMAX_BUFSIZE];
    double tf;
    int td;

    // looking for term name
    if(Vio_scanf(sock, "%s", tok) == 0) {
        Vnm_print(2, "parsePBAM:  ran out of tokens on %s!\n", name);
        return VRC_WARNING;
    }else {
       if(strncmp(tok, "position", 8)==0){
    	   return PBAMparm_parseTerm(thee, sock);
       }else{
    	   strncpy(thee->termnam[thee->termct], tok, CHR_MAXLEN);
       }
    }

    if (strncmp(thee->termnam[thee->termct], "contact", 7) == 0)
    {
      if(Vio_scanf(sock, "%s", tok) == 0) {
          Vnm_print(2, "parsePBAM:  ran out of tokens on %s!\n", name);
          return VRC_WARNING;
      }else{
          strncpy(thee->confil[thee->confilct], tok, CHR_MAXLEN);
      }

      if(Vio_scanf(sock, "%s", tok) == 0) {
          Vnm_print(2, "parsePBAM:  ran out of tokens on %s!\n", name);
          return VRC_WARNING;
      }
      if (sscanf(tok, "%lf", &tf) == 0){
          Vnm_print(2, "NOsh:  Read non-float (%s) while parsing %s keyword!\n", tok, name);
          return VRC_WARNING;
      }else
      {
          thee->termVal[thee->termct] = tf;
          thee->termnu[thee->termct][0] = 0;
          thee->confilct += 1;
      }
    } else if (strncmp(thee->termnam[thee->termct], "time", 4) == 0)
    {
      if(Vio_scanf(sock, "%s", tok) == 0) {
          Vnm_print(2, "parsePBAM:  ran out of tokens on %s!\n", name);
          return VRC_WARNING;
      } 
      if (sscanf(tok, "%lf", &tf) == 0){
          Vnm_print(2, "NOsh:  Read non-float (%s) while parsing %s keyword!\n", tok, name);
          return VRC_WARNING;
      }else{
          thee->termVal[thee->termct] = tf;
          thee->termnu[thee->termct][0] = 0;
      }
    } else //if (strncmp(thee->termnam[thee->termct], "position", 8) == 0)
    {
      if(Vio_scanf(sock, "%s", tok) == 0) {
          Vnm_print(2, "parsePBAM:  ran out of tokens on %s!\n", name);
          return VRC_WARNING;
      }
      if (sscanf(tok, "%lf", &tf) == 0){
          Vnm_print(2, "NOsh:  Read non-float (%s) while parsing %s keyword!\n", tok, name);
          return VRC_WARNING;
      }else{
          thee->termVal[thee->termct] = tf;
      }

      if(Vio_scanf(sock, "%s", tok) == 0) {
          Vnm_print(2, "parsePBAM:  ran out of tokens on %s!\n", name);
          return VRC_WARNING;
      }
      if (sscanf(tok, "%d", &td) == 0){
          Vnm_print(2, "NOsh:  Read non-float (%s) while parsing %s keyword!\n", tok, name);
          return VRC_WARNING;
      }else{
          thee->termnu[thee->termct][0] = td-1;
      }
    }

    thee->setterm = 1;    
    thee->termct += 1;
    return VRC_SUCCESS;
}

VPRIVATE Vrc_Codes PBAMparm_parseXYZ(PBAMparm *thee, Vio *sock){
    const char* name = "xyz";
    char tok[VMAX_BUFSIZE];
    int td, mol;

    if(Vio_scanf(sock, "%s", tok) == 0) {
        Vnm_print(2, "parsePBAM:  ran out of tokens on %s!\n", name);
        return VRC_WARNING;
    } 

    // // looking for index
    if (sscanf(tok, "%d", &td) == 0){
        Vnm_print(2, "NOsh:  Read non-int (%s) while parsing %s keyword!\n", tok, name);
        return VRC_WARNING;
    } else{
        printf("This is my mol in parseXYZ: %d", td);
        mol = td-1;
    }
    
    // looking for move type = move, stat, rot
    if(Vio_scanf(sock, "%s", tok) == 0) {
        Vnm_print(2, "parsePBAM:  ran out of tokens on %s!\n", name);
        return VRC_WARNING;
    } else {
       strncpy(thee->xyzfil[mol][thee->xyzct[mol]], tok, CHR_MAXLEN);
      thee->xyzct[mol] += 1;
    }
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

    // General terms to parse
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
    }else if (Vstring_strcasecmp(tok, "units") == 0) {
        return PBAMparm_parseUnits(thee, sock);
    }

    // Electrostatic parsing
    else if (Vstring_strcasecmp(tok, "dime") == 0) {
        return PBAMparm_parseGridPts(thee, sock);
    }else if (Vstring_strcasecmp(tok, "3dmap") == 0) {
        return PBAMparm_parse3Dmap(thee, sock);
    }else if (Vstring_strcasecmp(tok, "grid2d") == 0) {
        return PBAMparm_parseGrid2D(thee, sock);
    }else if (Vstring_strcasecmp(tok, "dx") == 0) {
        return PBAMparm_parseDX(thee, sock);
    }

    // Dynamics parsing
    else if (Vstring_strcasecmp(tok, "ntraj") == 0) {
        return PBAMparm_parseNtraj(thee, sock);
    }else if (Vstring_strcasecmp(tok, "termcombine") == 0) {
        return PBAMparm_parseTermcombine(thee, sock);
    }else if (Vstring_strcasecmp(tok, "diff") == 0) {
        return PBAMparm_parseDiff(thee, sock);
    }else if (Vstring_strcasecmp(tok, "term") == 0) {
        return PBAMparm_parseTerm(thee, sock);
    }else if (Vstring_strcasecmp(tok, "xyz") == 0) {
        return PBAMparm_parseXYZ(thee, sock);
    }


    else
     return 0;
  
  /*else {
        Vnm_print(2, "parsePBAM:  Unrecognized keyword (%s)!\n", tok);
        return VRC_WARNING;
    }
    return VRC_FAILURE;
  */
}
