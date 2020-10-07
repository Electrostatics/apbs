/**
 * @defgroup Vio Vio class
 * @brief    This class provides an I/O layer for files/bufferes/pipes/sockets.
 * 
 * This class provides an abstraction of I/O to give acess to
 * files, buffers, pipes, UNIX sockets, and INET sockets.
 */

/**
 * @file       vio.h
 * @ingroup    Vio
 * @brief      Class Vio: virtual <SDIO/FILE/BUFF/UNIX/INET> I/O layer.
 * @version    $Id: vio.h,v 1.28 2010/08/12 05:40:35 fetk Exp $
 * @author     Michael Holst
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

#ifndef _VIO_H_
#define _VIO_H_

#include "_maloc_base.h"
#include "maloccf.h"
#include "vnm.h"

/*
 * ***************************************************************************
 * Class Vio: Parameters and datatypes
 * ***************************************************************************
 */

/** @brief our portbase;  5000 < VPORTNUMBER < 49152 */
#define VPORTNUMBER 14916   
/** @brief number of internal buffers (BUFF datatype) */
#define VIO_MAXBUF 10       

/**
 * @ingroup Vio
 * @author  Michael Holst
 * @brief   Parameter for I/O type (sdio,buff,file,unix,inet)
 */
typedef enum VIOtype {
    VIO_NO_TYPE, 
    VIO_SDIO,
    VIO_BUFF,
    VIO_FILE,
    VIO_UNIX,
    VIO_INET
} VIOtype;

/**
 * @ingroup Vio
 * @author  Michael Holst
 * @brief   Parameter for compression type (XDR,ASC)
 */
typedef enum VIOfrmt {
    VIO_NO_FRMT,
    VIO_XDR,
    VIO_ASC
} VIOfrmt;

/**
 * @ingroup Vio
 * @author  Michael Holst
 * @brief   Parameter for rw type (R,RW)
 */
typedef enum VIOrwkey {
    VIO_NO_RW,
    VIO_R,
    VIO_W
} VIOrwkey;

/**
 * @ingroup Vio
 * @author  Michael Holst
 * @brief   Contains public data members for Vio class
 */
struct sVio {

    VIOtype type;       /**< file (or device) type.                          
                         *   VIO_NO_TYPE = not initialized.
                         *   VIO_SDIO    = standard I/O.
                         *   VIO_FILE    = file I/O.                   
                         *   VIO_BUFF    = buffer I/O.                      
                         *   VIO_UNIX    = UNIX (domain) socket I/O.
                         *   VIO_INET    = INET (network) socket I/O.        */

    VIOfrmt frmt;       /**< data format.
                         *   VIO_NO_FRMT = not initialized.                
                         *   VIO_ASC     = ASCII (FILE,BUFF,UNIX,INET).
                         *   VIO_XDR     = BINARY (FILE,BUFF,UNIX,INET).     */

    VIOrwkey rwkey;     /**< r/w key.
                         *   VIO_NO_R = not initialized.
                         *   VIO_R    = read (FILE,BUFF,UNIX,INET)
                         *   VIO_W    = write (FILE,BUFF,UNIX,INET)          */

    char file[VMAX_ARGLEN];   /**< file or device name (FILE,BUFF,UNIX,INET) */
    char lhost[VMAX_ARGLEN];  /**< local hostname (me) (UNIX,INET)           */
    char rhost[VMAX_ARGLEN];  /**< remote hostname (other guy) (UNIX,INET)   */

    int error;        /**< note if any error has occurred on this vio device */
    int dirty;        /**< dirty read bit -- have we read file yet (FILE)    */

    FILE *fp;         /**< file pointer (SDIO,FILE)                          */
    int so;           /**< primary unix domain or inet socket (UNIX,INET)    */
    int soc;          /**< subsocket created for socket reading (UNIX,INET)  */
    void *name;       /**< &sockaddr_un or &sockaddr_in (UNIX,INET)          */
    void *axdr;       /**< ASC/XDR structure pointer (ASC,XDR)               */

    char whiteChars[VMAX_ARGNUM]; /**< white space character set (ASC)       */
    char commChars[VMAX_ARGNUM];  /**< comment character set (ASC,XDR)       */

    char ioBuffer[VMAX_BUFSIZE];  /**< I/O buffer (ASC,XDR)                  */
    int ioBufferLen;              /**< I/O buffer length (ASC,XDR)           */

    char putBuffer[VMAX_BUFSIZE]; /**< final write buffer (ASC,XDR)          */
    int putBufferLen;             /**< final write buffer length (ASC,XDR)   */

    char *VIObuffer;    /**< (BUFF) */
    int VIObufferLen;   /**< (BUFF) */
    int VIObufferPtr;   /**< (BUFF) */

};

/** 
 * @brief   Declaration of the Vio class as the Vio structure
 * @ingroup Vio
 * @author  Michael Holst
 */
typedef struct sVio Vio;

/*
 * ***************************************************************************
 * Class Vio: Inlineable methods (vio.c)
 * ***************************************************************************
 */

#if !defined(VINLINE_MALOC)
#else /* if defined(VINLINE_MALOC) */
#endif /* if !defined(VINLINE_MALOC) */

/*
 * ***************************************************************************
 * Class Vio: Non-Inlineable methods (vio.c)
 * ***************************************************************************
 */

/** 
 * @ingroup Vio
 * @brief   Start Vio communication layer (init internal variables/buffers)
 * @author  Michael Holst
 * @note    Class Vio: Non-Inlineable methods (vio.c)
 * @return  None
 */
VEXTERNC void Vio_start(void);

/** 
 * @ingroup Vio
 * @brief   Shutdown Vio communication layer
 * @author  Michael Holst
 * @note    Class Vio: Non-Inlineable methods (vio.c)
 * @return  None
 */
VEXTERNC void Vio_stop(void);

/** 
 * @ingroup Vio
 * @brief   Construct the Vio object
 * @author  Michael Holst
 * @note    Class Vio: Non-Inlineable methods (vio.c)
 * @return  Pointer to newly constructed Vio object
 * @param   socktype Pointer to the socket type
 * @param   datafrmt Pointer to the data format
 * @param   hostname Pointer to network address of port
 * @param   filename Pointer to the i/o file name
 * @param   rwkey    Pointer to the read/write options
 */
VEXTERNC Vio* Vio_ctor(const char *socktype, const char *datafrmt, 
    const char *hostname, const char *filename, const char *rwkey);

/** 
 * @ingroup Vio
 * @brief   Work routine that Vio_ctor calls to do most of the construction
 * @author  Michael Holst
 * @note    Class Vio: Non-Inlineable methods (vio.c) 
 * @return  1 on success, 0 on failure
 * @param   thee     Pointer to the Vio object
 * @param   socktype Pointer to the socket type
 * @param   datafrmt Pointer to the data format
 * @param   hostname Pointer to network address of port
 * @param   filename Pointer to the i/o file name
 * @param   rwkey    Pointer to the read/write options
 */
VEXTERNC int Vio_ctor2(Vio *thee, const char *socktype, const char *datafrmt, 
    const char *hostname, const char *filename, const char *rwkey);

/** 
 * @ingroup Vio
 * @brief   Destruct the Vio object
 * @author  Michael Holst
 * @note    Class Vio: Non-Inlineable methods (vio.c) 
 * @return  None
 * @param   thee Pointer to the Vio object
 */
VEXTERNC void Vio_dtor(Vio **thee);

/** 
 * @ingroup Vio
 * @brief   Work routine that Vio_dtor calls to do most of the destruction
 * @author  Michael Holst
 * @note    Class Vio: Non-Inlineable methods (vio.c) 
 * @return  None
 * @param   thee Pointer to the Vio object
 */
VEXTERNC void Vio_dtor2(Vio *thee);

/** 
 * @ingroup Vio
 * @brief   Set the white character set for I/O stream
 * @author  Michael Holst
 * @note    Class Vio: Non-Inlineable methods (vio.c) 
 * @return  None
 * @param   thee       Pointer to the Vio object
 * @param   whiteChars white space character set
 */
VEXTERNC void Vio_setWhiteChars(Vio *thee, char *whiteChars);

/** 
 * @ingroup Vio
 * @brief   Set the comment character set for I/O stream
 * @author  Michael Holst
 * @note    Class Vio: Non-Inlineable methods (vio.c) 
 * @return  None
 * @param   thee      Pointer to the Vio object
 * @param   commChars comment character set
 */
VEXTERNC void Vio_setCommChars(Vio *thee, char *commChars);

/** 
 * @ingroup Vio
 * @brief   Accept any waiting connect attempt to our socket on our machine
 * @author  Michael Holst
 * @note    Class Vio: Non-Inlineable methods (vio.c) 
 * @return  1 on success, -1 on failure
 * @param   thee     Pointer to the Vio object
 * @param   nonblock index for a non-blocking socket
 *          Only for <UNIX/INET>; othewise it is ignored.
 *          nonblock==0  ==> block until a connect is attempted
 *          nonblock==1  ==> DO NOT block at all
 */
VEXTERNC int Vio_accept(Vio *thee, int nonblock);

/** 
 * @ingroup Vio
 * @brief   Free the socket child that was used for the last accept
 * @author  Michael Holst
 * @note    Class Vio: Non-Inlineable methods (vio.c) 
 * @return  None
 * @param   thee Pointer to the Vio object
 */
VEXTERNC void Vio_acceptFree(Vio *thee);

/** 
 * @ingroup Vio
 * @brief   Connect to some socket on a remote machine (or on our machine)
 * @author  Michael Holst
 * @note    Class Vio: Non-Inlineable methods (vio.c) 
 * @return  1 on success, -1 on failure
 * @param   thee     Pointer to the Vio object
 * @param   nonblock index for a non-blocking socket
 *          Only for <UNIX/INET>; othewise it is ignored.
 *          nonblock==0  ==> block until a connect is attempted
 *          nonblock==1  ==> DO NOT block at all
 */
VEXTERNC int Vio_connect(Vio *thee, int nonblock);

/** 
 * @ingroup Vio
 * @brief   Purge any output buffers (for <UNIX/INET>, else a no-op)
 * @author  Michael Holst
 * @note    Class Vio: Non-Inlineable methods (vio.c) 
 * @return  None
 * @param   thee Pointer to the Vio object
 */
VEXTERNC void Vio_connectFree(Vio *thee);

/** 
 * @ingroup Vio
 * @brief   Mimic "scanf" from an arbitrary Vio device
 * @author  Michael Holst
 * @note    Class Vio: Non-Inlineable methods (vio.c) 
 * @return  Number of tokens read
 * @param   thee  Pointer to the Vio object
 * @param   parms Pointer to input parameters
 */
VEXTERNC int Vio_scanf(Vio *thee, char *parms, ...);

/** 
 * @ingroup Vio
 * @brief   Mimic "printf" from an arbitrary Vio device
 * @author  Michael Holst
 * @note    Class Vio: Non-Inlineable methods (vio.c) 
 * @return  Number of tokens printed
 * @param   thee  Pointer to the Vio object
 * @param   parms Pointer to output parameters
 */
VEXTERNC int Vio_printf(Vio *thee, char *parms, ...);

/** 
 * @ingroup Vio
 * @brief   Read (up to) bufsize characters into buf from input device
 * @author  Michael Holst
 * @note    Class Vio: Non-Inlineable methods (vio.c) 
 * @return  Number of bytes read
 * @param   thee    Pointer to the Vio object
 * @param   buf     buffer containing message 
 * @param   bufsize number of items (of declared type) in buffer 
 */
VEXTERNC int Vio_read(Vio *thee, char *buf, int bufsize);

/** 
 * @ingroup Vio
 * @brief   Write bufsize characters from buf to output device
 * @author  Michael Holst
 * @note    Class Vio: Non-Inlineable methods (vio.c) 
 * @return  Number of bytes writen
 * @param   thee    Pointer to the Vio object
 * @param   buf     buffer containing message 
 * @param   bufsize number of items (of declared type) in buffer 
 */
VEXTERNC int Vio_write(Vio *thee, char *buf, int bufsize);

/** 
 * @ingroup Vio
 * @brief   Set the pointer to the internal buffer
 * @author  Michael Holst
 * @note    Class Vio: Non-Inlineable methods (vio.c) 
 * @return  None
 * @param   thee    Pointer to the Vio object
 * @param   buf     buffer containing message 
 * @param   bufsize number of items (of declared type) in buffer 
 */
VEXTERNC void Vio_bufTake(Vio *thee, char *buf, int bufsize);

/** 
 * @ingroup Vio
 * @brief   Return the pointer to the internal buffer
 * @author  Michael Holst
 * @note    Class Vio: Non-Inlineable methods (vio.c) 
 * @return  Poitner to the internal buffer
 * @param   thee Pointer to the Vio object
 */
VEXTERNC char* Vio_bufGive(Vio *thee);

/** 
 * @ingroup Vio
 * @brief   Return the length to the internal buffer
 * @author  Michael Holst
 * @note    Class Vio: Non-Inlineable methods (vio.c) 
 * @return  Length of the internal buffer
 * @param   thee Pointer to the Vio object
 */
VEXTERNC int Vio_bufSize(Vio *thee);

/** 
 * @ingroup Vio
 * @brief   Socket open for read or write
 * @author  Michael Holst
 * @note    Class Vio: Non-Inlineable methods (vio.c) 
 * @return  Socket for reading/writing the external data
 * @param   key    Pointer to the read/write option
 * @param   iodev  Pointer to open device for read/write
 * @param   iofmt  Pointer to i/o data format
 * @param   iohost Pointer to i/o address of port
 * @param   iofile Pointer to the i/o file name
 */
VEXTERNC Vio *Vio_socketOpen(char *key,
    const char *iodev, const char *iofmt,
    const char *iohost, const char *iofile);

/** 
 * @ingroup Vio
 * @brief   Socket close from read or write
 * @author  Michael Holst
 * @note    Class Vio: Non-Inlineable methods (vio.c) 
 * @return  None
 * @param   sock Socket for reading/writing the external data
 */
VEXTERNC void Vio_socketClose(Vio **sock);

#endif /* _VIO_H_ */

