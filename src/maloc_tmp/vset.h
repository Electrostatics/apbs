/**
 * @defgroup Vset Vset class
 * @brief    A dynamic set object.
 */

/**
 *  @file       vset.h
 *  @ingroup    Vset
 *  @brief      Class Vset: a dynamic set object.
 *  @author     Michael Holst
 *  @note       None
 *  @version    $Id: vset.h,v 1.20 2010/08/12 05:40:37 fetk Exp $
 *  
 *  @attention
 *  @verbatim
 *
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
 *  @endverbatim
 */


#ifndef _VSET_H_
#define _VSET_H_

#include "maloc_base.h"
#include "vnm.h"
#include "vmem.h"

/*
 * ***************************************************************************
 * Class Vset: Parameters and datatypes
 * ***************************************************************************
 */

/**
 * @ingroup Vset
 * @author  Michael Holst
 * @brief   Contains public data members for Vset class
 */
struct sVset {

    /** @brief the memory manager                              */
    Vmem *vmem;      
    /** @brief did i make vmem or was it inherited             */
    int  iMadeVmem;  

    /** @brief the current "T" object in our collection        */
    int curT;

    /** @brief name of object we are managing                  */
    char nameT[VMAX_ARGLEN];
    /** @brief size of the object in bytes                     */
    int sizeT;     

    /** @brief total number of allocated blocks                */
    int numBlocks;  
    /** @brief the global "T" counter -- how many "T"s in list  */
    int numT;       
    /** @brief for i/o at appropriate block creation/deletion   */
    int prtT;        

    /** @brief number of objects to manage (user specified)     */
    int maxObjects;   
    /** @brief power of 2 for blocksize (e.g., =10, or =16)     */  
    int blockPower;  
    /** @brief blocksize is 2^(blockPower)                      */ 
    int blockSize; 
    /** @brief  num blocks = blockMax=(maxObjects/blockSize)    */  
    int blockMax;  
    /** @brief  =blockSize-1; for determining which block fast  */
    int blockModulo;  

    /** @brief list of pointers to blocks of storage we manage  */
    char **table;    

};

/**
 * @brief   Declaration of the Vset class as the Vset structure
 * @ingroup Vset
 * @author  Michael Holst
 */
typedef struct sVset Vset;

/****************************************************************/
/* Class Vset: Inlineable method (vset.c)                       */
/****************************************************************/

#if !defined(VINLINE_MALOC)
    /**
     * @ingroup Vset
     * @brief   Return the number of things currently in the list.
     * @author  Michael Holst
     * @note    Class Vset: Inlineable method (vset.c)
     * @return  the number of things currently in the list.
     * @param   thee  Pointer to the Vset object
     */
    VEXTERNC int Vset_num(Vset *thee);

    /**
     * @ingroup Vset
     * @brief   Access an object in an arbitrary place in the list.
     * @author  Michael Holst
     * @note    Class Vset: Inlineable method (vset.c)
     * @return  list of pointers to blocks of storage we manage
     * @param   thee  Pointer to the Vset object
     * @param   i     index of the object
     */
    VEXTERNC char *Vset_access(Vset *thee, int i);

    /**
     * @ingroup Vset
     * @brief   Create an object on the end of the list.     
     * @author  Michael Holst
     * @note    Class Vset: Inlineable method (vset.c)
     * @return  Pointer to a created Vset object on the end of the list
     * @param   thee Pointer to the Vset object
     */
    VEXTERNC char *Vset_create(Vset *thee);

    /**
     * @ingroup Vset
     * @brief   Return the first object in the set.  
     * @author  Michael Holst
     * @note    Class Vset: Inlineable method (vset.c)
     * @return  the first object in the set
     * @param   thee Pointer to the Vset object
     */
    VEXTERNC char *Vset_first(Vset *thee);

    /**
     * @ingroup Vset
     * @brief   Return the last object in the set.
     * @author  Michael Holst
     * @note    Class Vset: Inlineable method (vset.c)
     * @return  the last object in the set.
     * @param   thee Pointer to the Vset object
     */
    VEXTERNC char *Vset_last(Vset *thee);

    /**
     * @ingroup Vset
     * @brief   Return the next object in the set.
     * @author  Michael Holst
     * @note    Class Vset: Inlineable method (vset.c)
     * @return  the next object in the set.
     * @param   thee Pointer to the Vset object
     */
    VEXTERNC char *Vset_next(Vset *thee);

    /**
     * @ingroup Vset
     * @brief   Return the prev object in the set.
     * @author  Michael Holst
     * @note    Class Vset: Inlineable method (vset.c)
     * @return  the prev object in the set
     * @param   thee Pointer to the Vset object
     */
    VEXTERNC char *Vset_prev(Vset *thee);

    /**
     * @ingroup Vset
     * @brief   Return the first object in the set.
     * @author  Michael Holst
     * @note    Class Vset: Inlineable method (vset.c)
     * @return  the first object in the set
     * @param   thee Pointer to the Vset object
     */
    VEXTERNC char *Vset_peekFirst(Vset *thee);

    /**
     * @ingroup Vset
     * @brief   Return the last object in the set.
     * @author  Michael Holst
     * @note    Class Vset: Inlineable method (vset.c)
     * @return  the last object in the set.
     * @param   thee Pointer to the Vset object
     */
    VEXTERNC char *Vset_peekLast(Vset *thee);

    /**
     * @ingroup Vset
     * @brief   Delete an object from the end of the list.
     * @author  Michael Holst
     * @note    Class Vset: Inlineable method (vset.c)
     * @return  None
     * @param   thee Pointer to the Vset object
     */
    VEXTERNC void Vset_destroy(Vset *thee);
#else /* if defined(VINLINE_MALOC) */
    /**
     * @ingroup Vset
     * @brief   the global "T" counter -- how many "T"s in list
     * @author  Michael Holst
     * @note    Class Vset: Inlineable method (vset.c) 
     * @return  None
     * @param   thee Pointer to the Vset object
     */
#   define Vset_num(thee) ((thee)->numT)

    /**
     * @ingroup Vset
     * @brief   Access an object in an arbitrary place in the list.
     * @author  Michael Holst
     * @note    Class Vset: Inlineable method (vset.c)
     * @return  list of pointers to blocks of storage we manage
     * @param   thee  Pointer to the Vset object
     * @param   i     index of the object
     */
#   define Vset_access(thee,i) ( \
        ((i >= 0) && (i < thee->numT)) \
        ? &((thee)->table[ (i)>>(thee)->blockPower                 ] \
                         [ (thee)->sizeT*((i)&(thee)->blockModulo) ]) \
        : VNULL \
    )

    /**
     * @ingroup Vset
     * @brief   Create an object on the end of the list.
     * @author  Michael Holst
     * @note    Class Vset: Inlineable method (vset.c)
     * @return  Pointer to a created Vset object on the end of the list
     * @param   thee Pointer to the Vset object
     */
#   define Vset_create(thee) ( \
        (  ((((thee)->numT)>>(thee)->blockPower) >= (thee)->numBlocks) \
        || ((((thee)->numT+1)%(thee)->prtT) == 0) ) \
        ? (Vset_createLast((thee))) \
        : (++((thee)->numT), (Vset_access((thee),(thee)->numT-1))) \
    )

    /**
     * @ingroup Vset
     * @brief   Return the first object in the set
     * @author  Michael Holst
     * @note    Class Vset: Inlineable method (vset.c)
     * @return  the first object in the set
     * @param   thee Pointer to the Vset object
     */
#   define Vset_first(thee) ( \
        (thee)->curT = 0, \
        Vset_access((thee), (thee)->curT) \
    )

    /**
     * @ingroup Vset
     * @brief   Return the last object in the set
     * @author  Michael Holst
     * @note    Class Vset: Inlineable method (vset.c)
     * @return  the last object in the set
     * @param   thee Pointer to the Vset object
     */
#   define Vset_last(thee) ( \
        (thee)->curT = (thee)->numT-1, \
        Vset_access((thee), (thee)->curT) \
    )

    /**
     * @ingroup Vset
     * @brief   Return the next object in the set
     * @author  Michael Holst
     * @note    Class Vset: Inlineable method (vset.c)
     * @return  the next object in the set
     * @param   thee Pointer to the Vset object
     */
#   define Vset_next(thee) ( \
        (thee)->curT++, \
        ((thee)->curT < (thee)->numT) \
        ? Vset_access((thee), (thee)->curT) \
        : VNULL \
    )

    /**
     * @ingroup Vset
     * @brief   Return the prev object in the set
     * @author  Michael Holst
     * @note    Class Vset: Inlineable method (vset.c)
     * @return  the prev object in the set
     * @param   thee Pointer to the Vset object
     */
#   define Vset_prev(thee) ( \
        (thee)->curT--, \
        ((thee)->curT >= 0) \
        ? Vset_access((thee), (thee)->curT) \
        : VNULL \
    )

    /**
     * @ingroup Vset
     * @brief   Return the first object in the set.
     * @author  Michael Holst
     * @note    Class Vset: Inlineable method (vset.c)
     * @return  the first object in the set.
     * @param   thee Pointer to the Vset object
     */
#   define Vset_peekFirst(thee) ( \
        Vset_access((thee), 0) \
    )

    /**
     * @ingroup Vset
     * @brief   Return the last object in the set
     * @author  Michael Holst
     * @note    Class Vset: Inlineable method (vset.c)
     * @return  the last object in the set
     * @param   thee Pointer to the Vset object
     */
#   define Vset_peekLast(thee) ( \
        Vset_access((thee), (thee)->numT-1) \
    )

    /**
     * @ingroup Vset
     * @brief   Free up the object currently on the end of the list
     * @author  Michael Holst
     * @note    Class Vset: Inlineable method (vset.c)
     * @return  no return
     * @param   thee Pointer to the Vset object
     */
#   define Vset_destroy(thee) ( \
        ( ((((thee)->numT-1)>>(thee)->blockPower) < (thee)->numBlocks-1) \
          || ((thee)->numT == 1) || ((((thee)->numT)%(thee)->prtT) == 0) ) \
        ? (Vset_destroyLast((thee))) : (void)(((thee)->numT)--) \
    )
#endif /* if !defined(VINLINE_MALOC) */

/**
 * @ingroup Vset
 * @brief   Construct the set object.
 * @author  Michael Holst
 * @note    Class Vset: Non-Inlineable method (vset.c) 
 * @return  Pointer to a new allocated Vset object
 * @param   vmem     Memory management object
 * @param   tname    name of object we are managing
 * @param   tsize    size of the object in bytes
 * @param   tmaxNum  number of objects to manage (user specified)
 * @param   ioKey    index for i/o
 */
VEXTERNC Vset* Vset_ctor(Vmem *vmem,
    const char *tname, int tsize, int tmaxNum, int ioKey);

/**
 * @ingroup Vset
 * @brief   Destroy the set object.
 * @author  Michael Holst
 * @note    Class Vset: Non-Inlineable method (vset.c) 
 * @return  None
 * @param   thee Pointer to the Vset object
 */
VEXTERNC void Vset_dtor(Vset **thee);

/**
 * @ingroup Vset
 * @brief   Create an object on the end of the list
 * @author  Michael Holst
 * @note    Class Vset: Non-Inlineable method (vset.c) 
 * @return  Pointer to the created Vset object
 * @param   thee Pointer to the Vset object
 */
VEXTERNC char *Vset_createLast(Vset *thee);

/**
 * @ingroup Vset
 * @brief   Free up the object currently on the end of the list.
 * @author  Michael Holst
 * @note    Class Vset: Non-Inlineable method (vset.c) 
 * @return  None
 * @param   thee Pointer to the Vset object
 */
VEXTERNC void Vset_destroyLast(Vset *thee);

/**
 * @ingroup Vset
 * @brief   Initialize the Vset data (thee).
 * @author  Michael Holst
 * @note    Class Vset: Non-Inlineable method (vset.c) 
 * @return  None 
 * @param   thee Pointer to the Vset object
 */
VEXTERNC void Vset_initData(Vset *thee);

/**
 * @ingroup Vset
 * @brief   Release all Ram controlled by this (thee) and re-initialize.
 * @author  Michael Holst
 * @note    Class Vset: Non-Inlineable method (vset.c) 
 * @return  None
 * @param   thee Pointer to the Vset object
 */
VEXTERNC void Vset_reset(Vset *thee);

/**
 * @ingroup Vset
 * @brief   Get and return the RAM Control Block (thee) information.
 * @author  Michael Holst
 * @note    Class Vset: Non-Inlineable method (vset.c) 
 * @return  None
 * @param   thee     Pointer to the Vset object
 * @param   tnum     the global "T" counter -- how many "T"s in list
 * @param   tsize    size of the object in bytes
 * @param   tVecUse  size of the total objects
 * @param   tVecMal  size of the total RAM Control Block
 * @param   tVecOhd  maximal size of RAM Control Block
 */
VEXTERNC void Vset_check(Vset *thee,
    int *tnum, int *tsize, int *tVecUse, int *tVecMal, int *tVecOhd);

/**
 * @ingroup Vset
 * @brief   Print the exact current malloc usage.
 * @author  Michael Holst
 * @note    Class Vset: Non-Inlineable method (vset.c) 
 * @return  None
 * @param   thee Pointer to the Vset object
 */
VEXTERNC void Vset_memChk(Vset *thee);

#endif /* _VSET_H_ */

