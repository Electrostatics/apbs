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
 * rcsid="$Id: vcom.c,v 1.25 2010/08/12 05:40:21 fetk Exp $"
 * ***************************************************************************
 */

/*
 * ***************************************************************************
 * File:     vcom.c
 *
 * Purpose:  Class Vcom: methods.
 *
 * Author:   Nathan Baker and Michael Holst
 * ***************************************************************************
 */

#include "vcom.h"

/*
 * ***************************************************************************
 * Class Vcom: Inlineable methods
 * ***************************************************************************
 */
#if !defined(VINLINE_MALOC)

#endif /* if !defined(VINLINE_MALOC) */

/*
 * ***************************************************************************
 * Class Vcom: Non-inlineable methods
 * ***************************************************************************
 */

/*
 * ***************************************************************************
 * Routine:  Vcom_init
 *
 * Purpose:  The Vmp initializer.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC int Vcom_init(int *argc, char ***argv)
{
#if defined(HAVE_MPI_H)
    return (MPI_SUCCESS == MPI_Init(argc,argv));
#else
    return 1;
#endif
}

/*
 * ***************************************************************************
 * Routine:  Vcom_finalize
 *
 * Purpose:   The Vmp finalizerr.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC int Vcom_finalize(void)
{
#if defined(HAVE_MPI_H)
    return (MPI_SUCCESS == MPI_Finalize());
#else
    return 1;
#endif
}

/*
 * ***************************************************************************
 * Routine:  Vcom_ctor
 *
 * Purpose:  Construct the communications object
 *
 * Notes:    This routine sets up data members of class and initializes MPI.  
 *
 * Author:   Nathan Baker and Michael Holst
 * ***************************************************************************
 */
VPUBLIC Vcom* Vcom_ctor(int commtype)
{
    int rc;
    Vcom *thee = VNULL;

    /* Set up the structure */
    thee       = Vmem_malloc( VNULL, 1, sizeof(Vcom) );
    thee->core = Vmem_malloc( VNULL, 1, sizeof(Vcom_core) );
    
    /* Call the real constructor */
    rc = Vcom_ctor2(thee, commtype);

    /* Destroy the guy if something went wrong */
    if (rc == 0) {
        Vmem_free( VNULL, 1, sizeof(Vcom_core), (void**)&(thee->core) );
        Vmem_free( VNULL, 1, sizeof(Vcom), (void**)&thee );
    }
 
    return thee;
}

/*
 * ***************************************************************************
 * Routine:  Vcom_ctor2
 *
 * Purpose:  Construct the communications object
 *
 * Notes:    This routine sets up data members of class and initializes MPI.
 *
 *           This is broken into two parts to be callable from FORTRAN.
 *
 * Author:   Nathan Baker and Michael Holst
 * ***************************************************************************
 */
VPUBLIC int Vcom_ctor2(Vcom *thee, int commtype)
{
    int rc = 0;

#if defined(HAVE_MPI_H)
    char estr[MPI_MAX_ERROR_STRING];
    int elen, dummy;
    Vcom_core *core = thee->core;
#endif

    /*
     * See what type of communications we should use.  Maybe each type of
     * communications object should have its own ctor2() function.
     */
    switch ( commtype ) {
      case 1:             /* MPI 1.1 */
        thee->type = commtype;

#if defined(HAVE_MPI_H)
        /* Start up MPI */
        rc = MPI_Initialized(&dummy);
        if (rc != MPI_SUCCESS) {
            MPI_Error_string(rc, estr, &elen);
            Vnm_print(2, "Vcom_ctor2: MPI_Init returned error: %s\n", 
              estr);
            return 0;
        } 

        /* Get the total number of processors */
        rc = MPI_Comm_size(MPI_COMM_WORLD, &(thee->mpi_size));
        if (rc != MPI_SUCCESS) {
            MPI_Error_string(rc, estr, &elen);
            Vnm_print(2, "Vcom_ctor2: MPI_Comm_size returned error: %s\n", 
              estr);
            return 0;
        }

        /* Get my processor rank */
        rc = MPI_Comm_rank(MPI_COMM_WORLD, &(thee->mpi_rank));
        if (rc != MPI_SUCCESS) {
            MPI_Error_string(rc, estr, &elen);
            Vnm_print(2, "Vcom_ctor2: MPI_Comm_rank returned error: %s\n", 
              estr);
            return 0;
        }

        /* Construct the communications group including all processors */
        core->mpi_comm = MPI_COMM_WORLD;

        /* Initialize Vnm with MPI rank */
        Vnm_setIoTag(thee->mpi_rank, thee->mpi_size);

        /* Some i/o */
        Vnm_print(2,"Vcom_ctor2: process %d of %d is ALIVE!\n",
            thee->mpi_rank, thee->mpi_size);

        rc = 1;
        break;

#else  /* defined(HAVE_MPI_H) */

        /* this might not be an error if this is a sequential code... */
        rc = 1;
        break;

#endif /* defined(HAVE_MPI_H) */

      default:
        Vnm_print(2, "Vcom_ctor2: Invalid communications type!\n");
        rc = 0;

    } /* switch (commtype) */

    return rc;
}

/*
 * ***************************************************************************
 * Routine:  Vcom_resize
 *
 * Purpose:  Resize (shrink) the communications group to include only newsize
 *           number of processors
 *
 * Notes:    Obsolete processes are given rank of -1 and size of 0
 *
 * Returns:  1 if sucessful
 *
 * Author:   Nathan Baker 
 * ***************************************************************************
 */
VPUBLIC int Vcom_resize(Vcom *thee, int newsize)
{
#if defined(HAVE_MPI_H) 
    int color;
    MPI_Comm oldcomm;
    Vcom_core *core = thee->core;
#endif

    switch (thee->type) {
        case 1:  /* MPI 1.1 */
#if defined(HAVE_MPI_H)
            /* This is a no-op for obsolete processes */
            if (core->mpi_comm == MPI_COMM_NULL) return 1;
            Vcom_barr(thee);
            /* Split the communications group.  We will ignore all processes
             * with rank outside the desired size */
            if (newsize > thee->mpi_size) {
                Vnm_print(2, "Vcom_resize:  Requested number of processors (%d) greater than original size (%d)!\n", newsize, thee->mpi_size);
                return 0;
            }
            if (thee->mpi_rank < newsize) color = 0; 
            else color = MPI_UNDEFINED;
            MPI_Comm_dup(core->mpi_comm, &oldcomm);
            if (MPI_Comm_split(oldcomm, color, 0, &(core->mpi_comm)) 
              != MPI_SUCCESS) {
                Vnm_print(2, "Vcom_resize:  Failed to split communicator!\n");
                return 0;
            } 
            MPI_Comm_free(&oldcomm);
            if (core->mpi_comm != MPI_COMM_NULL) {
                MPI_Comm_rank(core->mpi_comm, &(thee->mpi_rank));
                MPI_Comm_size(core->mpi_comm, &(thee->mpi_size));
            } else {
                thee->mpi_rank = -1;
                thee->mpi_size = 0;
            }
            Vnm_print(0, "Vcom_resize: New comm size = %d\n", thee->mpi_size);
            return 1;
#else
            Vnm_print(2, "Vcom_resize: Not compiled with MPI!\n");
            return 0;
#endif
            break;
        default:
            Vnm_print(2, "Vcom_resize: Invalid communications type!\n");
            return 0;
    }
}

/*
 * ***************************************************************************
 * Routine:  Vcom_dtor
 *
 * Purpose:  Destroy the communications object
 *
 * Author:   Nathan Baker and Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vcom_dtor(Vcom **thee)
{
    if ((*thee) != VNULL) {
        Vcom_dtor2(*thee);
        Vmem_free( VNULL, 1, sizeof(Vcom_core), (void**)&((*thee)->core) );
        Vmem_free( VNULL, 1, sizeof(Vcom), (void**)thee );
    }
}

/*
 * ***************************************************************************
 * Routine:  Vcom_dtor2
 *
 * Purpose:  Destroy the communications object
 *
 * Notes:    This is broken into two parts to be callable from FORTRAN.
 *
 * Author:   Nathan Baker and Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vcom_dtor2(Vcom *thee)
{
#if defined(HAVE_MPI_H)
    int err;
    Vcom_core *core = thee->core;
#endif

    /*
     * Do various things depending on what type of communications object
     * this is.  Maybe each object type should have its own dtor2() function.
     */
    switch (thee->type) {

      case 1:  /* MPI 1.1 */
#if defined(HAVE_MPI_H)
        /* Destroy the communicator */
        if ((core->mpi_comm != MPI_COMM_NULL) && 
            (core->mpi_comm != MPI_COMM_WORLD)) {
            Vnm_print(0, "Vcom_dtor2:  Freeing MPI communicator...\n");
            MPI_Comm_free(&(core->mpi_comm));
        }

#if 0
        err = MPI_Finalize();
        if (err != MPI_SUCCESS) {
            Vnm_print(2, "Vcom_dtor2: MPI_Finalize returned %d\n", err);
        }
#endif
#endif
      default:
        return;
    }
} 

/*
 * ***************************************************************************
 * Routine:  Vcom_send
 *
 * Purpose:  Send a buffer.  Returns 1 on success.
 *
 * Args:     des   = rank of receiving processor
 *           buf   = buffer containing message
 *           len   = number of items (of declared type) in buffer
 *           type  = type of items in message
 *                   0 => MPI_BYTE
 *                   1 => MPI_INT
 *                   2 => MPI_DOUBLE
 *                   3 => MPI_CHAR
 *           block = toggles blocking on (=1) and off (=0)
 *
 * Returns:  1 if successful
 *
 * Author:   Nathan Baker and Michael Holst
 * ***************************************************************************
 */
VPUBLIC int Vcom_send(Vcom *thee, int des, void *buf, int len, int type, 
    int block)
{
#if defined(HAVE_MPI_H)
    int tag = VCOM_MPI_TAG;  /* MPI tag */
    MPI_Datatype datatype;
    Vcom_core *core = thee->core;
#endif

    int err = 1;             /* Error flag (success = 1) */

    /* Bail if we've received any errors */
    VASSERT(thee != VNULL);

    if (thee->error != 0) {
        Vnm_print(2, "Vcom_send:  Have non-zero error state (%d)!\n",
          thee->error);
        return 0;
    }

    /* Figure out data type to use */
#if defined(HAVE_MPI_H)
    switch(type) {
        case 0: 
            datatype = MPI_BYTE;
            break;
        case 1: 
            datatype = MPI_INT;
            break;
        case 2:
            datatype = MPI_DOUBLE;
            break;
        case 3:
            datatype = MPI_CHAR;
            break;
        default:
            Vnm_print(2, "Vcom_send: Bogus datatype (%d), bailing!\n", type);
            return 0;
    }
#endif

    /* Send routine depends on comm object type */
    switch(thee->type) {

      case 1: /* MPI 1.1 */
#if defined(HAVE_MPI_H)
        if (core->mpi_comm == MPI_COMM_NULL) return 1;
        /* To block or not to block... */
        if (block == 1) {
            err = MPI_Send(buf, len, datatype, des, tag, core->mpi_comm);
            err = (MPI_SUCCESS == err);
        } else { /* if (block == 1) */
            err = MPI_Isend(buf, len, datatype, des, tag, core->mpi_comm,
                  &(core->mpi_request));
            err = (MPI_SUCCESS == err);
        } /* if (block == 1) */
#else
        Vnm_print(2, "Vcom_send: Vcom not compiled with MPI!\n");
        return 0;
#endif
        break; 
      default:
        Vnm_print(2, "Vcom_send: Invalid communications type!\n");
        return 0;
    }

    return err;
}

/*
 * ***************************************************************************
 * Routine:  Vcom_recv
 *
 * Purpose:  Receive a (character) buffer.  Returns 1 on success.  
 *
 * Args:     src   = rank of sending processor
 *           buf   = pointer to buffer of previously allocated memory
 *           len   = number of items (of declared type) in buffer
 *           type  = type of items in message
 *                   0 => MPI_BYTE
 *                   1 => MPI_INT
 *                   2 => MPI_DOUBLE
 *                   3 => MPI_CHAR
 *           block = toggles blocking on (=1) and off (=0)
 *
 * Returns:  1 if successful
 * 
 * Notes:    The blocking flag is present, but not used.  All receives are
 *           assumed to be blocking.  A non-blocking receive would be *very* 
 *           ugly to implement (signals or something?).
 *
 * Author:   Nathan Baker and Michael Holst
 * ***************************************************************************
 */
VPUBLIC int Vcom_recv(Vcom *thee, int src, void *buf, int len, int type,
    int block)
{
    int err = 0;             /* Error flag (success = 1) */

#if defined(HAVE_MPI_H)
    int tag = VCOM_MPI_TAG;  /* MPI tag */
    MPI_Datatype datatype;
    Vcom_core *core = thee->core;
#endif

    /* Bail if we've received any errors */
    VASSERT(thee != VNULL);
    if (thee->error != 0) {
        Vnm_print(2, "Vcom_send:  Have non-zero error state (%d)!\n",
          thee->error);
        return 0;
    }


#if defined(HAVE_MPI_H)
    switch(type) {
        case 0:
            datatype = MPI_BYTE;
            break;
        case 1:
            datatype = MPI_INT;
            break;
        case 2:
            datatype = MPI_DOUBLE;
            break;
        case 3:
            datatype = MPI_CHAR;
            break;
        default:
            Vnm_print(2, "Vcom_recv: Bogus datatype (%d), bailing!\n", type);
            return 0;
    }
#endif

    /* Send routine depends on comm object type */
    switch(thee->type) {

      case 1: /* MPI 1.1 */
#if defined(HAVE_MPI_H)
        if (core->mpi_comm == MPI_COMM_NULL) return 1;
        /* To block or not to block... */
        if (block == 1) {
            err = MPI_Recv(buf, len, datatype, src, tag, 
                  core->mpi_comm, &(core->mpi_status));
            err = (MPI_SUCCESS == err);
        } else {
            Vnm_print(2, "Vcom_recv: WARNING! Non-blocking receive not implemented!\n");
            return 0;
        }
#else
        Vnm_print(2, "Vcom_recv: Vcom not compiled with MPI!\n");
        return 0;
#endif

        break; 
      default:
        Vnm_print(2, "Vcom_recv: Invalid communications type!\n");
        return 0;
    }
    return err;
}

/*
 * ***************************************************************************
 * Routine:  Vcom_size
 *
 * Purpose:  Get the number of PEs in communicator
 *
 * Returns:  Number of PEs or -1 if error
 * 
 * Author:   Nathan Baker and Michael Holst
 * ***************************************************************************
 */
VPUBLIC int Vcom_size(Vcom *thee)
{
#if defined(HAVE_MPI_H)
    Vcom_core *core = thee->core;
#endif

    VASSERT(thee != VNULL);

    if ( thee->type == 1) {
#if defined(HAVE_MPI_H)
        if (core->mpi_comm == MPI_COMM_NULL) return 0;
        return thee->mpi_size;
#else
        return 1;
#endif
    } else { return -1; }
}

/*
 * ***************************************************************************
 * Routine:  Vcom_rank
 *
 * Purpose:  Get the ID of the local PE
 *
 * Returns:  PE rank or -1 if error
 * 
 * Author:   Nathan Baker and Michael Holst
 * ***************************************************************************
 */
VPUBLIC int Vcom_rank(Vcom *thee)
{
#if defined(HAVE_MPI_H)
    Vcom_core *core = thee->core;
#endif

    VASSERT(thee != VNULL);

    if ( thee->type == 1) {
#if defined(HAVE_MPI_H)
        if (core->mpi_comm == MPI_COMM_NULL) return -1;
        return thee->mpi_rank;
#else
        return 0;
#endif
    } else { return -1; }
}

/*
 * ***************************************************************************
 * Routine:  Vcom_barr
 *
 * Purpose:  Synchronization barrier.
 *
 * Returns:  1 if succesful
 * 
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC int Vcom_barr(Vcom *thee)
{
#if defined(HAVE_MPI_H)
    int err;
    Vcom_core *core = thee->core;
#endif

    VASSERT(thee != VNULL);

    if ( thee->type == 1) {

#if defined(HAVE_MPI_H)
        if (core->mpi_comm != MPI_COMM_NULL) {
            err = MPI_Barrier(core->mpi_comm);
            return (err == MPI_SUCCESS);
        } else return 1;
#else
        Vnm_print(2, "Vcom_barr: Vcom not compiled with MPI!\n");
        return 0;
#endif
    } else {
        Vnm_print(2, "Vcom_barr: Invalid communications type!\n");
        return 0;
    }
    return 0;
}

/*
 * ***************************************************************************
 * Routine:  Vcom_getCount
 *
 * Purpose:  Perform a blocking probe to get the length (in number of items of
 *           specified type) of an incoming message and place it in the 
 *           argument ``length".
 *
 *           type  = type of items in message
 *                   0 => MPI_BYTE
 *                   1 => MPI_INT
 *                   2 => MPI_DOUBLE
 *                   3 => MPI_CHAR
 * 
 * Author:   Nathan Baker
 * ***************************************************************************
 */
VPUBLIC int Vcom_getCount(Vcom *thee, int src, int *length, int type)
{
#if defined(HAVE_MPI_H)
    MPI_Datatype datatype;
    Vcom_core *core = thee->core;
#endif

    VASSERT(thee != VNULL);

#if defined(HAVE_MPI_H)
    switch(type) {
        case 0:
            datatype = MPI_BYTE;
            break;
        case 1:
            datatype = MPI_INT;
            break;
        case 2:
            datatype = MPI_DOUBLE;
            break;
        case 3:
            datatype = MPI_CHAR;
            break;
        default:
            Vnm_print(2,"Vcom_getCount: Bogus datatype (%d), bailing!\n",type);
            return 0;
    }
#endif

    if ( thee->type == 1) {
#if defined(HAVE_MPI_H)
        if (core->mpi_comm == MPI_COMM_NULL) return 1;
        MPI_Probe(src, VCOM_MPI_TAG, core->mpi_comm, &(core->mpi_status));
        return MPI_Get_count(&(core->mpi_status), datatype, length);
#else
        Vnm_print(2, "Vcom_getCount: Vcom not compiled with MPI!\n");
        return -1;
#endif
    } else { return -1; }
}

/*
 * ***************************************************************************
 * Routine:  Vcom_reduce
 *
 * Purpose:  Perform a reduction of the data across all processors.  This is
 *           equivalent (and in the case of MPI is identical to) MPI_Allreduce.
 *           Basically, the specified operations are appleed to each member of
 *           the sendbuf across all processors and the results are written to
 *           recvbuf.
 *
 * Args:     sendbuf = buffer containing `length` items of the specified type
 *                     to be operated on
 *           recvbuf = buffer containing `length` items of the specified type
 *                     after operation
 *           length = number of items 
 *           type  = type of items in message
 *                   0 => MPI_BYTE
 *                   1 => MPI_INT
 *                   2 => MPI_DOUBLE
 *                   3 => MPI_CHAR
 *           op = operation to perform
 *                0 => MPI_SUM
 *                1 => MPI_PROD
 *                2 => MPI_MIN
 *                3 => MPI_MAX
 * 
 * Author:   Nathan Baker
 * ***************************************************************************
 */
VPUBLIC int Vcom_reduce(Vcom *thee, void *sendbuf, void *recvbuf, int length,
    int type, int op)
{
    int memsize;

#if defined(HAVE_MPI_H)
    MPI_Datatype datatype;
    MPI_Op optype;
    Vcom_core *core = thee->core;
#endif

    VASSERT(thee != VNULL);

#if defined(HAVE_MPI_H)
    switch(type) {
        case 0:
            datatype = MPI_BYTE;
            break;
        case 1:
            datatype = MPI_INT;
            break;
        case 2:
            datatype = MPI_DOUBLE;
            break;
        case 3:
            datatype = MPI_CHAR;
            break;
        default:
            Vnm_print(2, "Vcom_recv: Bogus datatype (%d), bailing!\n", type);
            return 0;
    }
    switch(op) {
        case 0: 
            optype = MPI_SUM;
            break;
        case 1: 
            optype = MPI_PROD;
            break;
        case 2: 
            optype = MPI_MIN;
            break;
        case 3: 
            optype = MPI_MAX;
            break;
        default:
            Vnm_print(2, "Vcom_reduce: Bogus optype (%d), bailing!\n", type);
            return 0;
    }
#endif

    if ( thee->type == 1) {

#if defined(HAVE_MPI_H)

        if (core->mpi_comm == MPI_COMM_NULL) return 1;
        return MPI_Allreduce(sendbuf, recvbuf, length, datatype, optype, 
          core->mpi_comm);

#else
        Vnm_print(0, "Vcom_reduce:  Not compiled with MPI, doing simple copy.\n");
        switch(type) {
            case 0:
                memsize = 1;
                break;
            case 1:
                memsize = sizeof(int);
                break;
            case 2:
                memsize = sizeof(double);
                break;
            case 3:
                memsize = sizeof(char);
                break;
            default:
                Vnm_print(2, "Vcom_recv: Bogus datatype (%d), bailing!\n", type);
                return 0;
        }

        memcpy(recvbuf, sendbuf, memsize*length);
        return 1;

#endif

    } else { return -1; }
}

