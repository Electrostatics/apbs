/**
 * @defgroup Vmem Vmem class
 * @brief    This class provides a safe logged version of malloc and free.
 * 
 * This class provides a safe logged version of malloc and free, with
 * all standard routines supported with Vmem variants.
 */

/**
 * @file       vmem.h
 * @ingroup    Vmem
 * @brief      Class Vmem: A safer, object-oriented, malloc/free object.
 * @author     Michael Holst
 * @note       None
 * @version    $Id: vmem.h,v 1.21 2010/08/12 05:40:36 fetk Exp $
 *  
 * @attention
 * @verbatim
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
 * @endverbatim
 */

#ifndef _VMEM_H_
#define _VMEM_H_

#include "maloc_base.h"

/*
 * ***************************************************************************
 * Class Vmem: Parameters and datatypes
 * ***************************************************************************
 */


/**
 * @ingroup Vmem
 * @author  Michael Holst
 * @brief   Contains public data members for Vmem class
 */
struct sVmem {

    char name[VMAX_ARGLEN]; /**< name of class we manage malloc areas for   */

    size_t mallocBytes; /**< total size of all current malloc areas of class*/
    size_t freeBytes;   /**< total size of all freed malloc areas of class  */
    size_t highWater;   /**< high-water malloc bytemark for this class      */
    size_t mallocAreas; /**< total number of individual malloc areas        */

};

/** 
 * @brief   Declaration of the Vmem class as the Vmem structure
 * @ingroup Vmem
 * @author  Michael Holst
 */
typedef struct sVmem Vmem;

/*
 * ***************************************************************************
 * Class Vmem: Inlineable methods (vmem.c)
 * ***************************************************************************
 */

#if !defined(VINLINE_MALOC)
#else /* if defined(VINLINE_MALOC) */
#endif /* if !defined(VINLINE_MALOC) */

/*
 * ***************************************************************************
 * Class Vmem: Non-Inlineable methods (vmem.c)
 * ***************************************************************************
 */

/** 
 * @ingroup Vmem
 * @brief   Return total of all active Vmem malloc areas (current footprint)
 * @author  Michael Holst
 * @note    Class Vmem: Non-Inlineable methods (vmem.c)
 * @return  total of all active Vmem malloc areas (current footprint)
 */
VEXTERNC size_t Vmem_bytesTotal(void);

/** 
 * @ingroup Vmem
 * @brief   Return total of all Vmem malloc allocations
 * @author  Michael Holst
 * @note    Class Vmem: Non-Inlineable methods (vmem.c)
 * @return  total of all Vmem malloc allocations
 */
VEXTERNC size_t Vmem_mallocBytesTotal(void);

/** 
 * @ingroup Vmem
 * @brief   Return total of all Vmem free calls
 * @author  Michael Holst
 * @note    Class Vmem: Non-Inlineable methods (vmem.c)
 * @return  total of all Vmem free calls
 */
VEXTERNC size_t Vmem_freeBytesTotal(void);

/** 
 * @ingroup Vmem
 * @brief   Return the high-water byte mark (largest footprint)
 * @author  Michael Holst
 * @note    Class Vmem: Non-Inlineable methods (vmem.c)
 * @return  the high-water byte mark (largest footprint)
 */
VEXTERNC size_t Vmem_highWaterTotal(void);

/** 
 * @ingroup Vmem
 * @brief   Return total of all active Vmem malloc areas by groups
 * @author  Michael Holst
 * @note    Class Vmem: Non-Inlineable methods (vmem.c)
 * @return  total of all active Vmem malloc areas by groups
 */
VEXTERNC size_t Vmem_mallocAreasTotal(void);

/** 
 * @ingroup Vmem
 * @brief   Print current memory statistics for all Vmem malloc/free areas
 * @author  Michael Holst
 * @note    Class Vmem: Non-Inlineable methods (vmem.c)
 * @return  None
 */
VEXTERNC void Vmem_printTotal(void);

/** 
 * @ingroup Vmem
 * @brief   Construct the dynamic memory allocation logging object
 * @author  Michael Holst
 * @note    Class Vmem: Non-Inlineable methods (vmem.c)
 * @return  Pointer to a newly created Vmem object
 * @param   name Pointer to the object name
 */
VEXTERNC Vmem* Vmem_ctor(char *name);

/** 
 * @ingroup Vmem
 * @brief   Destruct the dynamic memory allocation logging object
 * @author  Michael Holst
 * @note    Class Vmem: Non-Inlineable methods (vmem.c)
 * @return  None
 * @param   thee Pointer to the Vmem object
 */
VEXTERNC void Vmem_dtor(Vmem **thee);

/** 
 * @ingroup Vmem
 * @brief   A safe logged version of malloc
 * @author  Michael Holst
 * @note    Class Vmem: Non-Inlineable methods (vmem.c)
 * @return  Void pointer to a new malloc area
 * @param   thee Pointer to the Vmem object
 * @param   num  number of the allocated address
 * @param   size size of one allocated address
 */
VEXTERNC void *Vmem_malloc(Vmem *thee, size_t num, size_t size);

/** 
 * @ingroup Vmem
 * @brief   A safe logged version of free
 * @author  Michael Holst
 * @note    Class Vmem: Non-Inlineable methods (vmem.c)
 * @return  None
 * @param   thee Pointer to the Vmem object
 * @param   num  number of the allocated address
 * @param   size size of one allocated address
 * @param   ram  Pointer to pointer of the reallocated memory 
 */
VEXTERNC void Vmem_free(Vmem *thee, size_t num, size_t size, void **ram);

/** 
 * @ingroup Vmem
 * @brief   A safe logged version of realloc (usually a bad idea to use this)
 * @author  Michael Holst
 * @note    Class Vmem: Non-Inlineable methods (vmem.c)
 * @return  Void pointer to a new malloc area
 * @param   thee    Pointer to the Vmem object
 * @param   num     number of the allocated address
 * @param   size    size of one allocated address
 * @param   ram     Pointer to pointer of the reallocated memory 
 * @param   newNum  number of the reallocated address
 */
VEXTERNC void *Vmem_realloc(Vmem *thee, size_t num, size_t size, void **ram,
    size_t newNum);

/** 
 * @ingroup Vmem
 * @brief   Return total of all ACTIVE malloc areas used by Vmem object
 * @author  Michael Holst
 * @note    Class Vmem: Non-Inlineable methods (vmem.c)
 * @return  total of all ACTIVE malloc areas used by Vmem object
 * @param   thee Pointer to the Vmem object
 */
VEXTERNC size_t Vmem_bytes(Vmem *thee);

/** 
 * @ingroup Vmem
 * @brief   Return total of all mallocs performed by Vmem object
 * @author  Michael Holst
 * @note    Class Vmem: Non-Inlineable methods (vmem.c)
 * @return  total of all mallocs performed by Vmem object
 * @param   thee Pointer to the Vmem object
 */
VEXTERNC size_t Vmem_mallocBytes(Vmem *thee);

/** 
 * @ingroup Vmem
 * @brief   Return total of all frees performed by Vmem object
 * @author  Michael Holst
 * @note    Class Vmem: Non-Inlineable methods (vmem.c)
 * @return  total of all frees performed by Vmem object
 * @param   thee Pointer to the Vmem object
 */
VEXTERNC size_t Vmem_freeBytes(Vmem *thee);

/** 
 * @ingroup Vmem
 * @brief   Return high-water malloc bytemark hit by Vmem object
 * @author  Michael Holst
 * @note    Class Vmem: Non-Inlineable methods (vmem.c)
 * @return  high-water malloc bytemark hit by Vmem object
 * @param   thee Pointer to the Vmem object
 */
VEXTERNC size_t Vmem_highWater(Vmem *thee);

/** 
 * @ingroup Vmem
 * @brief   Return total number of individual active malloc areas
 * @author  Michael Holst
 * @note    Class Vmem: Non-Inlineable methods (vmem.c)
 * @return  total number of individual active malloc areas
 * @param   thee Pointer to the Vmem object
 */
VEXTERNC size_t Vmem_mallocAreas(Vmem *thee);

/** 
 * @ingroup Vmem
 * @brief   Print current memory stats associated with this Vmem object
 * @author  Michael Holst
 * @note    Class Vmem: Non-Inlineable methods (vmem.c)
 * @return  None
 * @param   thee Pointer to the Vmem object
 */
VEXTERNC void Vmem_print(Vmem *thee);

#endif /* _VMEM_H_ */

