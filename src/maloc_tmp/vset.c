/*
 * ***************************************************************************
 * MALOC = < Minimal Abstraction Layer for Object-oriented C >
 * Copyright (C) 1994-- Michael Holst
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * rcsid="$Id: vset.c,v 1.16 2010/08/12 05:40:33 fetk Exp $"
 * ***************************************************************************
 */

/*
 * ***************************************************************************
 * File:     vset.c
 *
 * Purpose:  Class Vset: methods.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */

#include "vset.h"

/*
 * ***************************************************************************
 * Class Vset: Inlineable methods
 * ***************************************************************************
 */
#if !defined(VINLINE_MALOC)

/*
 * ***************************************************************************
 * Routine:  Vset_num
 *
 * Purpose:  Return the number of things currently in the list.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC int Vset_num(Vset *thee)
{
    return thee->numT;
}

/*
 * ***************************************************************************
 * Routine:  Vset_access
 *
 * Purpose:  Access an object in an arbitrary place in the list.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC char *Vset_access(Vset *thee, int i)
{
    if ((i >= 0) && (i < thee->numT))
        return &( thee->table[ i>>thee->blockPower               ]
                             [ thee->sizeT*(i&thee->blockModulo) ] );
    else
        return VNULL;
}

/*
 * ***************************************************************************
 * Routine:  Vset_create
 *
 * Purpose:  Create an object on the end of the list.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC char *Vset_create(Vset *thee)
{
    if (  (((thee->numT)>>thee->blockPower) >= thee->numBlocks)
       || (((thee->numT+1)%thee->prtT) == 0) ) {
        return Vset_createLast(thee);
    } else {
        (thee->numT)++;
        return Vset_access(thee,thee->numT-1);
    }
}

/*
 * ***************************************************************************
 * Routine:  Vset_destroy
 *
 * Purpose:  Delete an object from the end of the list.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vset_destroy(Vset *thee)
{
    if ( (((thee->numT-1)>>thee->blockPower) < thee->numBlocks-1)
        || (thee->numT == 1) || (((thee->numT)%thee->prtT) == 0)) {
        Vset_destroyLast(thee);
    } else {
        (thee->numT)--;
    }
}

/*
 * ***************************************************************************
 * Routine:  Vset_first
 *
 * Purpose:  Return the first object in the set.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC char *Vset_first(Vset *thee)
{
    thee->curT = 0;
    return Vset_access(thee, thee->curT);
}

/*
 * ***************************************************************************
 * Routine:  Vset_last
 *
 * Purpose:  Return the last object in the set.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC char *Vset_last(Vset *thee)
{
    thee->curT = thee->numT-1;
    return Vset_access(thee, thee->curT);
}

/*
 * ***************************************************************************
 * Routine:  Vset_next
 *
 * Purpose:  Return the next object in the set.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC char *Vset_next(Vset *thee)
{
    thee->curT++;
    if (thee->curT < thee->numT)
        return Vset_access(thee, thee->curT);
    else return VNULL;
}

/*
 * ***************************************************************************
 * Routine:  Vset_prev
 *
 * Purpose:  Return the prev object in the set.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC char *Vset_prev(Vset *thee)
{
    thee->curT--;
    if (thee->curT >= 0)
        return Vset_access(thee, thee->curT);
    else return VNULL;
}

/*
 * ***************************************************************************
 * Routine:  Vset_peekFirst
 *
 * Purpose:  Return the first object in the set.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC char *Vset_peekFirst(Vset *thee)
{
    return Vset_access(thee, 0);
}

/*
 * ***************************************************************************
 * Routine:  Vset_peekLast
 *
 * Purpose:  Return the last object in the set.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC char *Vset_peekLast(Vset *thee)
{
    return Vset_access(thee, thee->numT-1);
}

#endif /* if !defined(VINLINE_MALOC) */
/*
 * ***************************************************************************
 * Class Vset: Non-inlineable methods
 * ***************************************************************************
 */

/*
 * ***************************************************************************
 * Routine:  Vset_ctor
 *
 * Purpose:  Construct the set object.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC Vset* Vset_ctor(Vmem *vmem,
    const char *tname, int tsize, int tmaxNum, int ioKey)
{
    char name[VMAX_ARGLEN];
    Vset *thee = VNULL;
    int K;

    VDEBUGIO("Vset_ctor: CREATING object..");

    thee = Vmem_malloc( VNULL, 1, sizeof(Vset) );
    if (vmem == VNULL) {
        sprintf(name, "Vset:%s", tname);
        thee->vmem = Vmem_ctor( name );
        thee->iMadeVmem = 1;
    } else {
        thee->vmem = vmem;
        thee->iMadeVmem = 0;
    }

    VASSERT( tsize > 0 );
    VASSERT( tmaxNum > 0 );

    /* set object name and size */
    strncpy(thee->nameT, tname, VMAX_ARGLEN);
    thee->sizeT = tsize;

    /* set blockSize as a power of two for quick block division via shifts */
    thee->blockPower  = VBLOCK_POWER; 
    thee->blockSize   = (1 << thee->blockPower);

    /* determine maxObjects such that:   [maxObjects >= tmaxNum]       */
    /* AND such that for some integer K: [maxObjects  = K * blockSize] */
    /* (this most excellent elegant expression is due to mr. bond)     */
    K = (int)( (tmaxNum - 1)/(thee->blockSize) ) + 1;
    thee->maxObjects = K * thee->blockSize;
    VASSERT( thee->maxObjects >= tmaxNum );

    /* set blockMax and blockModulo using maxObjects and blockSize */
    thee->blockMax    = (thee->maxObjects / thee->blockSize);
    thee->blockModulo = (thee->blockSize - 1);

    /* create the table of blocks */
    thee->table = Vmem_malloc( thee->vmem, thee->blockMax, sizeof( char* ) );

    /* now initialize the block data */
    Vset_initData(thee);

    VDEBUGIO("..done.\n");

    /* some i/o */
    if (ioKey) {
        Vnm_print(0,
            "Vset_ctor: %d (%d) %s [%d bs/%s, %d %s/bl, %d bls, %d:o bs]\n",
            (thee->blockSize * thee->blockMax), tmaxNum,
            thee->nameT, thee->sizeT, thee->nameT,
            thee->blockSize, thee->nameT, thee->blockMax,
            (VPTRSIZE * thee->blockMax) );
    }

    return thee;
}

/*
 * ***************************************************************************
 * Routine:  Vset_dtor
 *
 * Purpose:  Destroy the set object.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vset_dtor(Vset **thee)
{
    VASSERT( (*thee) != VNULL );
    if ((*thee) != VNULL) {
        Vset_reset(*thee);
        Vmem_free( (*thee)->vmem, (*thee)->blockMax, sizeof(char*),
            (void**)&((*thee)->table) );

        VDEBUGIO("Vset_dtor: DESTROYING object..");
        if ((*thee)->iMadeVmem) Vmem_dtor( &((*thee)->vmem) );
        Vmem_free( VNULL, 1, sizeof(Vset), (void**)thee );
        VDEBUGIO("..done.\n");

        (*thee) = VNULL;
    }
}

/*
 * ***************************************************************************
 * Routine:  Vset_createLast
 *
 * Purpose:  Create an object on the end of the list.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC char *Vset_createLast(Vset *thee)
{
    /* get block number and index into block */
    int blockIdx = thee->sizeT*(thee->numT & thee->blockModulo);
    int blockNum = thee->numT >> thee->blockPower;

    /* do we need a new block yet? */
    if (blockNum >= thee->numBlocks) {
        VASSERT( blockNum == thee->numBlocks );
        thee->table[blockNum] = Vmem_malloc(thee->vmem,
                                    thee->blockSize, thee->sizeT);
        VASSERT ( thee->table[blockNum] != VNULL );
        thee->numBlocks++;
        VASSERT ( thee->numBlocks <= thee->blockMax );
    }

    /* increase global object count by one */
    thee->numT++;

    /* some i/o if needed */
    /* *** Vnm_print(0,"Vset_create: numT = %d\n", thee->numT); *** */
    if ( (thee->numT%thee->prtT) == 0 )
        Vnm_print(0,"[%s:c%d]",thee->nameT,thee->numT);

    /* return new obj */
    return &(thee->table[blockNum][blockIdx]);
}

/*
 * ***************************************************************************
 * Routine:  Vset_destroyLast
 *
 * Purpose:  Free up the object currently on the end of the list.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vset_destroyLast(Vset *thee)
{
    int blockNum;

    /* some i/o if needed */
    /* *** Vnm_print(0,"Vset_destroy: numT = %d\n", thee->numT); *** */
    if ( (thee->numT%thee->prtT) == 0 )
        Vnm_print(0,"[%s:d%d]",thee->nameT,thee->numT);

    /* decrease global object count by one */
    thee->numT--;

    /* has block been completely emptied? */
    blockNum = thee->numT >> thee->blockPower;
    if (blockNum < thee->numBlocks-1) {
        VASSERT( blockNum == thee->numBlocks-2 );
        thee->numBlocks--;
        Vmem_free( thee->vmem, thee->blockSize, thee->sizeT,
            (void**)&(thee->table[thee->numBlocks]) );
        thee->table[thee->numBlocks] = VNULL;
    } else if (thee->numT == 0) {
        VASSERT( thee->numBlocks == 1 );
        thee->numBlocks = 0;
        Vmem_free( thee->vmem, thee->blockSize, thee->sizeT,
            (void**)&(thee->table[0]) );
        thee->table[0] = VNULL;
    }
}

/*
 * ***************************************************************************
 * Routine:  Vset_initData
 *
 * Purpose:  Initialize the Vset data (thee).
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vset_initData(Vset *thee)
{
    int j;
    thee->numBlocks = 0;
    thee->curT      = 0;
    thee->numT      = 0;
    thee->prtT      = 10000;
    for (j=0; j<thee->blockMax; j++) thee->table[j]=VNULL;
}

/*
 * ***************************************************************************
 * Routine:  Vset_reset
 *
 * Purpose:  Release all Ram controlled by this (thee) and re-initialize.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vset_reset(Vset *thee)
{
    int j;
    while (Vset_num(thee) > 0) Vset_destroy(thee);
    for (j=0; j<thee->blockMax; j++)
        if (thee->table[j] != VNULL)
             Vmem_free( thee->vmem, thee->blockSize, thee->sizeT,
                 (void**)&(thee->table[j]) );
    Vset_initData(thee);
}

/*
 * ***************************************************************************
 * Routine:  Vset_check
 *
 * Purpose:  Get and return the RAM Control Block (thee) information.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vset_check(Vset *thee,
    int *tnum, int *tsize, int *tVecUse, int *tVecMal, int *tVecOhd)
{
    (*tnum)    = thee->numT;
    (*tsize)   = thee->sizeT;
    (*tVecUse) = thee->sizeT * thee->numT;
    (*tVecMal) = thee->sizeT * (thee->numBlocks * thee->blockSize);
    (*tVecOhd) = VPTRSIZE * thee->blockMax;
}

/*
 * ***************************************************************************
 * Routine:  Vset_memChk
 *
 * Purpose:  Print the exact current malloc usage.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vset_memChk(Vset *thee)
{
    if (thee->iMadeVmem) Vmem_print(thee->vmem);
}

