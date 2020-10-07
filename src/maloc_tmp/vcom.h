/**
 * @defgroup Vcom Vcom class
 * @brief    Virtual (currently just MPI) communications layer
 */

/**
 *  @file       vcom.h
 *  @ingroup    Vcom
 *  @brief      Class Vcom: virtual (currently just MPI) communications layer
 *  @authors    Nathan Baker and Michael Holst
 *  @note       None
 *  @version    $Id: vcom.h,v 1.38 2010/08/12 05:40:23 fetk Exp $
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


#ifndef _VCOM_H_
#define _VCOM_H_

#include <maloc_tmp/_maloc_base.h>
#include <maloc_tmp/vnm.h>
#include <maloc_tmp/vmem.h>
#include <maloc_tmp/vio.h>
#include <maloc_tmp/vset.h>

/** @brief A base value for MPI tags */
#define VCOM_MPI_TAG 111

/*
 * ***************************************************************************
 * Class Vcom: Parameters and datatypes
 * ***************************************************************************
 */

/**
 * @ingroup Vcom
 * @brief   Contains public data members for Vcom class
 * @author  Michael Holst
 * @note    None
 */
struct sVcom {

    /** @brief Local PE rank from MPI */
    int  mpi_rank;   
    /** @brief Total number of PEs in this communicator from MPI */
    int  mpi_size;   

    /** @brief Communication type. \n
              0 = not initialized \n
              1 = Message Passing Interface 1.1
    */
    int  type;         
    /** @brief note if any error has occurred on this vcom device */
    int  error; 
    /** @brief Private MPI core */
    void *core; 

};

/**
 * @brief   Declaration of the Vcom class as the Vcom structure  
 * @ingroup Vcom
 * @author  Michael Holst
 * @return  None
 */
typedef struct sVcom Vcom;

/*
 * ***************************************************************************
 * Class Vcom: Inlineable methods (vcom.c)
 * ***************************************************************************
 */

#if !defined(VINLINE_MALOC)
#else /* if defined(VINLINE_MALOC) */
#endif /* if !defined(VINLINE_MALOC) */


/** 
 * @ingroup Vcom
 * @brief   The Vmp initializer.
 * @author  Michael Holst
 * @note    Class Vcom: Non-Inlineable methods (vcom.c) 
 * @return  Success enumeration
 * @param   argc number of the command line arguments
 * @param   argv the command line arguments
 */
VEXTERNC int Vcom_init(int *argc, char ***argv);

/** 
 * @ingroup Vcom
 * @brief   The Vmp finalizer.
 * @author  Michael Holst
 * @note    Class Vcom: Non-Inlineable methods (vcom.c) 
 * @return  Success enumeration
 */
VEXTERNC int Vcom_finalize(void);

/** 
 * @ingroup Vcom
 * @brief   Construct the communications object.
            This routine sets up data members of class and initializes MPI.
 * @author  Michael Holst
 * @note    Class Vcom: Non-Inlineable methods (vcom.c) 
 * @return  Pointer to the new allocated communications object.
 * @param   commtype type of communications object
 */
VEXTERNC Vcom* Vcom_ctor(int commtype);

/** 
 * @ingroup Vcom
 * @brief   Construct the communications object.
            This routine sets up data members of class and initializes MPI.
            This is broken into two parts to be callable from FORTRAN.
 * @authors Nathan Baker and Michael Holst
 * @note    Class Vcom: Non-Inlineable methods (vcom.c) 
 * @return  Success enumeration
 * @param   thee     Pointer to the new allocated communications object.
 * @param   commtype type of communications object
 */
VEXTERNC int Vcom_ctor2(Vcom* thee, int commtype);

/** 
 * @ingroup Vcom
 * @brief   Destroy the communications object 
 * @authors Nathan Baker and Michael Holst
 * @note    Class Vcom: Non-Inlineable methods (vcom.c) 
 * @return  None
 * @param   thee Pointer to the communications object.
 */
VEXTERNC void Vcom_dtor(Vcom **thee);

/** 
 * @ingroup Vcom
 * @brief   Destroy the communications object. 
            This is broken into two parts to be callable from FORTRAN.
 * @authors Nathan Baker and Michael Holst
 * @note    Class Vcom: Non-Inlineable methods (vcom.c) 
 * @return  None
 * @param   thee Pointer to the communications object.
 */
VEXTERNC void Vcom_dtor2(Vcom *thee);

/** 
 * @ingroup Vcom
 * @brief   Send a buffer.  Returns 1 on success. 
 * @authors Nathan Baker and Michael Holst
 * @note    Class Vcom: Non-Inlineable methods (vcom.c) 
 * @return  1 if successful
 * @param   thee Pointer to the communications object.
 * @param   des  rank of receiving processor 
 * @param   buf  buffer containing message 
 * @param   len  number of items (of declared type) in buffer 
 * @param   type type of items in message. \n 
            0 => MPI_BYTE, 1 => MPI_INT, 2 => MPI_DOUBLE, 3 => MPI_CHAR
 * @param   block toggles blocking on (=1) and off (=0) 
 */
VEXTERNC int Vcom_send(Vcom *thee, int des, void *buf, int len, int type, 
  int block);

/** 
 * @ingroup Vcom
 * @brief   Receive a (character) buffer. \n
            The blocking flag is present, but not used.  All receives are
            assumed to be blocking.  A non-blocking receive would be *very*
            ugly to implement (signals or something?).
 * @authors Nathan Baker and Michael Holst                                
 * @note    Class Vcom: Non-Inlineable methods (vcom.c) 
 * @return  1 if successful
 * @param   thee Pointer to the communications object.
 * @param   src  rank of receiving processor 
 * @param   buf  pointer to buffer of previously allocated memory 
 * @param   len  number of items (of declared type) in buffer 
 * @param   type  type of items in message. \n        
            0 => MPI_BYTE, 1 => MPI_INT, 2 => MPI_DOUBLE, 3 => MPI_CHAR
 * @param   block  toggles blocking on (=1) and off (=0) 
 */
VEXTERNC int Vcom_recv(Vcom *thee, int src, void *buf, int len, int type, 
  int block);

/** 
 * @ingroup Vcom
 * @brief   Perform a blocking probe to get the length (in number of items of
            specified type) of an incoming message and place it in the  
            argument ``length". 
 * @author  Nathan Baker
 * @note    Class Vcom: Non-Inlineable methods (vcom.c) 
 * @return  Success enumeration
 * @param   thee    Pointer to the communications object.
 * @param   src     rank of receiving processor
 * @param   length  Pointer to perform a blocking probe
 * @param   type  type of items in message. \n
            0 => MPI_BYTE, 1 => MPI_INT, 2 => MPI_DOUBLE, 3 => MPI_CHAR
 */
VEXTERNC int Vcom_getCount(Vcom *thee, int src, int *length, int type);

/** 
 * @ingroup Vcom
 * @brief   Perform a reduction of the data across all processors.  This is
            equivalent (and in the case of MPI is identical to) MPI_Allreduce.
            Basically, the specified operations are appleed to each member of  
            the sendbuf across all processors and the results are written to
            recvbuf.
 * @author  Nathan Baker
 * @note    Class Vcom: Non-Inlineable methods (vcom.c) 
 * @return  Success enumeration
 * @param   thee  Pointer to the communications object
 * @param   sendbuf  buffer containing `length` items of the specified type
            to be operated on
 * @param   recvbuf  buffer containing `length` items of the specified type
            after operation 
 * @param   length  number of items
 * @param   type  type of items in message \n
            0 => MPI_BYTE, 1 => MPI_INT, 2 => MPI_DOUBLE, 3 => MPI_CHAR 
 * @param   op  operation to perform \n
            0 => MPI_SUM, 1 => MPI_PROD, 2 => MPI_MIN, 3 => MPI_MAX
 */
VEXTERNC int Vcom_reduce(Vcom *thee, void *sendbuf, void *recvbuf, int length, 
  int type, int op);

/** 
 * @ingroup Vcom
 * @brief   Get the number of PEs in communicator 
 * @authors Nathan Baker Michael Holst 
 * @note    Class Vcom: Non-Inlineable methods (vcom.c) 
 * @return  Number of PEs or -1 if error
 * @param   thee Pointer to the communications object
 */
VEXTERNC int Vcom_size(Vcom *thee);

/** 
 * @ingroup Vcom
 * @brief   Resize (shrink) the communications group to include only newsize 
            number of processors. \n
            Obsolete processes are given rank of -1 and size of 0 
 * @author  Nathan Baker
 * @note    Class Vcom: Non-Inlineable methods (vcom.c) 
 * @return  1 if successful
 * @param   thee    Pointer to the communications object
 * @param   newsize number of processors
 */
VEXTERNC int Vcom_resize(Vcom *thee, int newsize);

/** 
 * @ingroup Vcom
 * @brief   Get the ID of the local PE   
 * @authors Nathan Baker and Michael Holst
 * @note    Class Vcom: Non-Inlineable methods (vcom.c) 
 * @return  Get the ID of the local PE
 * @param   thee Pointer to the communications object
 */
VEXTERNC int Vcom_rank(Vcom *thee);

/** 
 * @ingroup Vcom
 * @brief   Synchronization barrier.
 * @author  Michael Holst
 * @note    Class Vcom: Non-Inlineable methods (vcom.c) 
 * @return  1 if successful.
 * @param   thee Pointer to the communications object
 */
VEXTERNC int Vcom_barr(Vcom *thee);

/* This struct was taken from the private header vcom_p.h */
typedef struct Vcom_core {
#if defined(HAVE_MPI_H)
    MPI_Status  mpi_status;       /* MPI_Status object   (4-int struct)     */
    MPI_Request mpi_request;      /* MPI_Request object  (union of structs) */
    MPI_Comm    mpi_comm;         /* MPI_Comm object     (int)              */
#else
    int mpi_status;
    int mpi_request;
    int mpi_comm;
#endif
} Vcom_core;

#endif /* _VCOM_H_ */

