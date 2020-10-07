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
 * rcsid="$Id: vio.c,v 1.32 2010/08/12 05:40:30 fetk Exp $"
 * ***************************************************************************
 */

/*
 * ***************************************************************************
 * File:     vio.c
 *
 * Purpose:  Class Vio: methods.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */

#include "_maloc_base.h"
#include "vio.h"

#if defined(HAVE_UNISTD_H)
#   include <unistd.h>
#endif

#if defined(HAVE_SYS_TYPES_H)
#   include <sys/types.h>
#endif

#if defined(HAVE_SYS_STAT_H)
#   include <sys/stat.h>
#endif

#if defined(HAVE_FCNTL_H)
#   include <fcntl.h>
#endif

#if defined(HAVE_SYS_SOCKET_H)
#   include <sys/socket.h>
#endif

#if defined(HAVE_SYS_UN_H)
#   include <sys/un.h>
#endif

#if defined(HAVE_NETINET_IN_H)
#   include <netinet/in.h>
#endif

#if defined(HAVE_ARPA_INET_H)
#   include <arpa/inet.h>
#endif

#if defined(HAVE_NETDB_H)
#   include <netdb.h>
#endif


#if defined(HAVE_RPC_RPC_H)
#   include <rpc/rpc.h>
#elif defined(HAVE_RPC_H)
#   include <rpc.h>
#endif


/*#include <rpc/rpc.h>*/

#if defined(HAVE_WINSOCK_H)
#   include <winsock.h>
#endif

#if defined(HAVE_IO_H)
#   include <io.h>
#endif

/* ASC modes as dual to the XDR modes */
typedef enum ASCmode {
    ASC_NO_MODE,
    ASC_DECODE,
    ASC_ENCODE
} ASCmode;

/* ASC structure as dual to the XDR structure */
typedef struct ASC {
    ASCmode mode;                    /* ASC_DECODE or ASC_ENCODE            */
    int     pos;                     /* position of next character in buf   */
    int     size;                    /* size of buf                         */
    char    *buf;                    /* the character buffer                */
    char    whiteChars[VMAX_ARGNUM]; /* white space character set           */
    char    commChars[VMAX_ARGNUM];  /* comment character set               */
    int     error;                   /* APBS-KTS: Currently for EOF cond    */
} ASC;

/* use ASC in place of XDR if XDR does not exist */
#if !defined(HAVE_XDR)
#   define XDR           ASC
#   define XDR_DECODE    ASC_DECODE
#   define XDR_ENCODE    ASC_ENCODE
#   define xdrmem_create ascmem_create
#   define xdr_destroy   asc_destroy
#   define xdr_getpos    asc_getpos
#   define xdr_setpos    asc_setpos
#   define xdr_string    asc_string
#   define xdr_char      asc_char
#   define xdr_int       asc_int
#   define xdr_float     asc_float
#   define xdr_double    asc_double
#endif

/* communication startup */
VPRIVATE int VIOstarted = 0;

/* default comment char set and white space char set */
VPRIVATE char *VIOwhiteChars = " \t\n";
VPRIVATE char *VIOcommChars  = "";

/* initialization, signals, and other tools */
VPRIVATE void VIOregHand(void);
VPRIVATE void VIOunregHand(void);
VPRIVATE void VIOsigHand(int num);
VPRIVATE int VIOgethostname(char *name, unsigned int len);
VPRIVATE const char *VIOstrerrno(int err);

/* buffer management */
VPRIVATE void Vio_initIoPutBuffers(Vio *thee);
VPRIVATE void Vio_purgePutBuffer(Vio *thee);
VPRIVATE int Vio_writePutBuffer(Vio *thee, char *buf, int bufsize);

/* ASC analogs to the core XDR routines */
VPRIVATE void ascmem_create(ASC *thee, char *buf, int size, ASCmode mode);
VPRIVATE void asc_destroy(ASC *thee);
VPRIVATE int asc_getpos(ASC *thee);
VPRIVATE int asc_setpos(ASC *thee, int pos);
VPRIVATE int asc_string(ASC *thee, char **sval, int size);
VPRIVATE int asc_char(ASC *thee, char *cval);
VPRIVATE int asc_int(ASC *thee, int *ival);
VPRIVATE int asc_float(ASC *thee, float *fval);
VPRIVATE int asc_double(ASC *thee, double *dval);

/* some additional ASC routines (no analogs in XDR) */
VPRIVATE void asc_setWhiteChars(ASC *thee, char *whiteChars);
VPRIVATE void asc_setCommChars(ASC *thee, char *commChars);
VPRIVATE char *asc_getToken(ASC *thee, char *tok, int toksize);

/* two low-level file-descriptor i/o jewels from Rick Steven's book */
VPRIVATE int readn(int fd, void *vptr, unsigned int n);
VPRIVATE int writen(int fd, void *vptr, unsigned int n) ;

/*
 * ***************************************************************************
 * Class Vio: Inlineable methods
 * ***************************************************************************
 */
#if !defined(VINLINE_MALOC)

#endif /* if !defined(VINLINE_MALOC) */
/*
 * ***************************************************************************
 * Class Vio: Non-inlineable methods
 * ***************************************************************************
 */

/*
 * ***************************************************************************
 * Routine:  Vio_start
 *
 * Purpose:  Startup the Vio communication layer.
 *
 * Notes:    This routine initializes some internal variables and buffers.
 *           This routine also deals with the interception of fatal
 *           SIGPIPE signals which are generated on some systems when a
 *           socket connection is broken by one of the send/receive pair.
 *
 *           We don't want to abort in this situation, but rather do some
 *           damage control.  Hence we register our own SIGPIPE interrupt
 *           handler in Vio_start(), and re-register it every time a SIGPIPE
 *           is intercepted.  We re-register it because some systems reset
 *           the interrupt mask after a single interrupt is generated.
 *           We un-register the SIGPIPE handler in Vio_stop().
 *
 *           To use the Vio library, you need to call Vio_start() before
 *           you make any calls to the Vio_ctor().  Calling the Vio_ctor()
 *           (or any other Vio method) before Vio_start() generates an error.
 *           For example, you can call this routine as follows:
 *
 *               Vio_start()
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vio_start(void)
{
    /* repeated initializations ARE legal (no error; possibly frees memory) */
    /* VASSERT( !VIOstarted ); */

    /* mark as initialized */
    VIOstarted = 1;

    /* register the SIGPIPE handler to avoid aborts on a SIGPIPE */
    VIOregHand();

    /* normal return */
    return;
}

/*
 * ***************************************************************************
 * Routine:  Vio_stop
 *
 * Purpose:  Shutdown the Vio communication layer.
 *
 * Notes:    This routine was written primarily to deal with some
 *           communication runtime environments which require a shutdown.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vio_stop(void)
{
    /* repeated de-initializations ARE legal (no error; just a no-op) */
    /* VASSERT( VIOstarted ); */

    /* un-initialize us */
    VIOstarted = 0;

    /* un-register the SIGPIPE handler */
    VIOunregHand();

    /* normal return */
    return;
}

/*
 * ***************************************************************************
 * Routine:  VIOregHand
 *
 * Purpose:  Register the signal handler with the operating system.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPRIVATE void VIOregHand(void)
{
#if !defined(HAVE_WINSOCK_H)
    VASSERT( signal(SIGPIPE,&VIOsigHand) != SIG_ERR );
#endif
}

/*
 * ***************************************************************************
 * Routine:  VIOunregHand
 *
 * Purpose:  Un-Register the signal handler with the operating system.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPRIVATE void VIOunregHand(void)
{
#if !defined(HAVE_WINSOCK_H)
    VASSERT( signal(SIGPIPE,SIG_DFL) != SIG_ERR );
#endif
}

/*
 * ***************************************************************************
 * Routine:  VIOsigHand
 *
 * Purpose:  Handle events such as SIGPIPE.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPRIVATE void VIOsigHand(int num)
{
    /* some i/o */
    fprintf(stderr,"VIOsigHand: yow! caught a hot SIGPIPE....diffused it.\n");

    /* just re-register interrupt handler in case it was cleared by default */
    VIOregHand();
}

/*
 * ***************************************************************************
 * Routine:  VIOgethostname
 *
 * Purpose:  Get the hostname of this machine.
 *           Returns 0 on success.  Returns -1 on error.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPRIVATE int VIOgethostname(char *name, unsigned int len)
{
#if defined(HAVE_WINSOCK_H)
    return gethostname(name,(int)len);
#else
    return gethostname(name,len);
#endif
}

/*
 * ***************************************************************************
 * Routine:  VIOstrerrno
 *
 * Purpose:  Return the error string corresponding to the error number.
 *
 * Notes:    This is a partial implementation of the "iberty" library
 *           function "strerrno" that exists on most Linux boxes.  It is
 *           simply a mapping of error number to error string.  It is
 *           very useful for debugging UNIX and INET socket code.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPRIVATE const char *VIOstrerrno(int err)
{
    static char errstr[VMAX_ARGLEN];

    if      (err == EFAULT           ) strcpy(errstr,"EFAULT");
    else if (err == EINTR            ) strcpy(errstr,"EINTR");
    else if (err == EINVAL           ) strcpy(errstr,"EINVAL");
    else if (err == ENOENT           ) strcpy(errstr,"ENOENT");
    else if (err == EPIPE            ) strcpy(errstr,"EPIPE");
    else if (err == ENOMEM           ) strcpy(errstr,"ENOMEM");
    else if (err == EAGAIN           ) strcpy(errstr,"EAGAIN");
    else if (err == EBADF            ) strcpy(errstr,"EBADF");

#if defined(HAVE_WINSOCK_H)
    else if (err == WSAENETDOWN      ) strcpy(errstr,"WSAENETDOWN");
    else if (err == WSAEFAULT        ) strcpy(errstr,"WSAEFAULT");
    else if (err == WSAENOTCONN      ) strcpy(errstr,"WSAENOTCONN");
    else if (err == WSAEINTR         ) strcpy(errstr,"WSAEINTR");
    else if (err == WSAEINPROGRESS   ) strcpy(errstr,"WSAEINPROGRESS");
    else if (err == WSAENETRESET     ) strcpy(errstr,"WSAENETRESET");
    else if (err == WSAENOTSOCK      ) strcpy(errstr,"WSAENOTSOCK");
    else if (err == WSAEOPNOTSUPP    ) strcpy(errstr,"WSAEOPNOTSUPP");
    else if (err == WSAESHUTDOWN     ) strcpy(errstr,"WSAESHUTDOWN");
    else if (err == WSAEWOULDBLOCK   ) strcpy(errstr,"WSAEWOULDBLOCK");
    else if (err == WSAEMSGSIZE      ) strcpy(errstr,"WSAEMSGSIZE");
    else if (err == WSAEINVAL        ) strcpy(errstr,"WSAEINVAL");
    else if (err == WSAETIMEDOUT     ) strcpy(errstr,"WSAETIMEDOUT");
    else if (err == WSAECONNABORTED  ) strcpy(errstr,"WSAECONNABORTED");
    else if (err == WSAECONNREFUSED  ) strcpy(errstr,"WSAECONNREFUSED");
    else if (err == WSAECONNRESET    ) strcpy(errstr,"WSAECONNRESET");
    else if (err == WSANOTINITIALISED) strcpy(errstr,"WSANOTINITIALISED");
#else
    else if (err == ENETDOWN         ) strcpy(errstr,"ENETDOWN");
    else if (err == ENOTCONN         ) strcpy(errstr,"ENOTCONN");
    else if (err == EINPROGRESS      ) strcpy(errstr,"EINPROGRESS");
    else if (err == ENETRESET        ) strcpy(errstr,"ENETRESET");
    else if (err == ENOTSOCK         ) strcpy(errstr,"ENOTSOCK");
    else if (err == EOPNOTSUPP       ) strcpy(errstr,"EOPNOTSUPP");
    else if (err == ESHUTDOWN        ) strcpy(errstr,"ESHUTDOWN");
    else if (err == EWOULDBLOCK      ) strcpy(errstr,"EWOULDBLOCK");
    else if (err == EMSGSIZE         ) strcpy(errstr,"EMSGSIZE");
    else if (err == ETIMEDOUT        ) strcpy(errstr,"ETIMEDOUT");
    else if (err == ECONNABORTED     ) strcpy(errstr,"ECONNABORTED");
    else if (err == ECONNREFUSED     ) strcpy(errstr,"ECONNREFUSED");
    else if (err == ECONNRESET       ) strcpy(errstr,"ECONNRESET");
    else if (err == ENOBUFS          ) strcpy(errstr,"ENOBUFS");
#endif

    else sprintf(errstr,"VIO_UNKNOWN_ERROR(%d)",err);
    return errstr;
}

/*
 * ***************************************************************************
 * Routine:  Vio_ctor
 *
 * Purpose:  Construct the [sdio/file/buff/unix/inet] container object.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC Vio* Vio_ctor(const char *socktype, const char *datafrmt,
    const char *hostname, const char *filename, const char *rwkey)
{
    Vio *thee = VNULL;

    /* make sure Vio was started */
    VJMPERR1( VIOstarted );

    thee = (Vio*)calloc( 1, sizeof(Vio) );
    VJMPERR2( thee != VNULL );
    VJMPERR3( Vio_ctor2(thee, socktype, datafrmt, hostname, filename, rwkey) );

    /* normal return */
    return thee;

  VERROR1:
    fprintf(stderr,"Vio_ctor: Vio library has not been started.\n");
    return VNULL;

  VERROR2:
    fprintf(stderr,"Vio_ctor: malloc of Vio structure failed.\n");
    return VNULL;

  VERROR3:
    fprintf(stderr,"Vio_ctor: Vio_ctor2() failed.\n");
    Vio_dtor(&thee);
    return VNULL;
}

/*
 * ***************************************************************************
 * Routine:  Vio_ctor2
 *
 * Purpose:  Construct the [sdio/file/buff/unix/inet] container object.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC int Vio_ctor2(Vio *thee, const char *socktype, const char *datafrmt,
    const char *hostname, const char *filename, const char *rwkey)
{
    int n, ival;
    char host[VMAX_ARGLEN];
    struct linger ling;
    unsigned int len;
    struct hostent *hpTmp;
#if defined(HAVE_WINSOCK_H)
    WSADATA wsaData;
#endif
#if defined(HAVE_SYS_UN_H)
    char username[VMAX_ARGLEN];
#endif

    /* make sure Vio was started */
    VJMPERR1( VIOstarted );

    /* initialize all Vio fields */
    thee->type     = VIO_NO_TYPE;
    thee->frmt     = VIO_NO_FRMT;
    thee->rwkey    = VIO_NO_RW;
    thee->error    = 0;
    thee->dirty    = 0;
    thee->fp       = VNULL;
    thee->axdr     = VNULL;
    thee->so       = -1;
    thee->soc      = -1;
    thee->name     = VNULL;

    /* initialize the internal buffer (for BUFF datatype) */
    thee->VIObuffer = VNULL;
    thee->VIObufferLen = 0;
    thee->VIObufferPtr = 0;

    /* initialize the socktype field */
    if (!strcmp(socktype,"SDIO")) {
        thee->type = VIO_SDIO;
    } else if (!strcmp(socktype,"FILE")) {
        thee->type = VIO_FILE;
    } else if (!strcmp(socktype,"BUFF")) {
        thee->type = VIO_BUFF;
    } else if (!strcmp(socktype,"UNIX")) {
        thee->type = VIO_UNIX;
    } else if (!strcmp(socktype,"INET")) {
        thee->type = VIO_INET;
    } else {
        fprintf(stderr,"Vio_ctor2: Incorrect socktype given <%s>\n",socktype);
        VJMPERR2( 0 );
    }

    /* initialize the datafrmt field */
    if (!strcmp(datafrmt,"ASC")) {
        thee->frmt = VIO_ASC;
    } else if (!strcmp(datafrmt,"XDR")) {
        thee->frmt = VIO_XDR;
    } else {
        fprintf(stderr,"Vio_ctor2: Incorrect datafrmt given <%s>\n", datafrmt);
        VJMPERR2( 0 );
    }

    /* initialize the r/w field */
    if (!strcmp(rwkey,"r")) {
        thee->rwkey = VIO_R;
    } else if (!strcmp(rwkey,"w")) {
        thee->rwkey = VIO_W;
    } else {
        fprintf(stderr,"Vio_ctor2: Incorrect rwkey given <%s>\n", rwkey);
        VJMPERR2( 0 );
    }

    /* need to call this stupid Win32 function before gethostname... */
#if defined(HAVE_WINSOCK_H)
    if ( WSAStartup(0x0101, &wsaData) != 0 ) {
        fprintf(stderr, "Vio_ctor2: WSAStartup fail INET sock <%s>"
            " dueto <%s>\n", thee->file,VIOstrerrno(errno));
        VJMPERR2( 0 );
    }
#endif

    /* get "my" local hostname */
    if ((VIOgethostname(thee->lhost,sizeof(thee->lhost))) < 0) {
        fprintf(stderr,
            "Vio_ctor2: Gethostname fail INET sock <%s> dueto <%s>\n",
            thee->file, VIOstrerrno(errno));
        strcpy(thee->lhost,"unknown");
    } else if ((hpTmp=gethostbyname(thee->lhost))==VNULL) {
        fprintf(stderr,
            "Vio_ctor2: Gethostbyname fail INET sock <%s> dueto <%s>\n",
            thee->file, VIOstrerrno(errno));
        strcpy(thee->lhost,"unknown");
    } else strcpy(thee->lhost,hpTmp->h_name);

    /* default remote hostname */
    strcpy(thee->rhost,"unknown");

    /* initialize the buffer space */
    Vio_initIoPutBuffers(thee);

    /* SDIO READ/WRITE SETUP */
    if (thee->type==VIO_SDIO) {

        if (thee->rwkey==VIO_R) {
            thee->fp = stdin;
        } else { /* (thee->rwkey==VIO_W) */
            thee->fp = stdout;
        }
        VJMPERR2( thee->fp != VNULL );

    /* FILE READ/WRITE SETUP */
    } else if (thee->type==VIO_FILE) {

        /* filename is the i/o file name */
        if (strlen(filename) >= VMAX_ARGLEN) {
            fprintf(stderr, "Vio_ctor2: Filename <%d> exceeds max <%d>!\n",
                (int)strlen(filename), VMAX_ARGLEN);
            VJMPERR2( 0 );
        }
        strncpy(thee->file, filename, VMAX_ARGLEN);
        if (thee->rwkey==VIO_R) {
            thee->fp = fopen(thee->file, "r");
        } else { /* (thee->rwkey==VIO_W) */
            thee->fp = fopen(thee->file, "w");
        }
        VJMPERR2( thee->fp != VNULL );

    /* BUFF READ/WRITE SETUP */
    } else if (thee->type==VIO_BUFF) {

        /* filename is the internal buffer number for the buffer */
        thee->VIObufferPtr = 0;

    /* UNIX SOCKET READ/WRITE SETUP */
    } else if (thee->type==VIO_UNIX) {

#if defined(HAVE_SYS_UN_H)

        /* filename is socketName-userName in the directory /tmp */

        VASSERT( Vnm_getuser(username, sizeof(username)) );
        sprintf(thee->file, "/tmp/%s-%s", filename, username);

        /* create the socket address structure */
        thee->name = (struct sockaddr_un *)
            calloc( 1, sizeof(struct sockaddr_un) );
        VJMPERR2( thee->name != VNULL );

        /* Get a socket structure */
        if ((thee->so=socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
            fprintf(stderr,"Vio_ctor2: fail to find UNIX sock dueto <%s>.\n",
                VIOstrerrno(errno));
            VJMPERR2( 0 );
        } else {

            /* set REUSEADDR so sockets can be closed and reopened */
            ival = 1;  /* just need a nonzero value */
            if ( setsockopt(thee->so,SOL_SOCKET,SO_REUSEADDR,
              (void*)&ival,sizeof(ival)) < 0 ) {
                fprintf(stderr, "Vio_ctor2: Setsockopt1 fail UNIX sock <%s>"
                   " dueto <%s>\n", thee->file, VIOstrerrno(errno));
                VJMPERR2( 0 );
            }

            /* turn on LINGER so WRITES complete before socks close */
            ling.l_onoff  = 1;   /* just need a nonzero value */
            ling.l_linger = 30;  /* linger time in seconds */
            if ( setsockopt(thee->so,SOL_SOCKET,SO_LINGER,
              (void*)&ling,sizeof(ling)) < 0) {
                fprintf(stderr, "Vio_ctor2: Setsockopt2 fail UNIX sock <%s>"
                   " dueto <%s>\n", thee->file, VIOstrerrno(errno));
                VJMPERR2( 0 );
            }

            /* Setup for the socket */
            memset(thee->name, '\0', sizeof(struct sockaddr_un));
            ((struct sockaddr_un *)(thee->name))->sun_family = AF_UNIX;
            strcpy(((struct sockaddr_un *)(thee->name))->sun_path,
                thee->file);

            /* if we are reader, WE are responsible for creating socket */
            /* the reader must do: (unlink/)setsockopt/bind/listen */
            if (thee->rwkey==VIO_R) {

                /* define socket file; remove previous socket */
                unlink(thee->file);

                /* determine structure size; AF_UNIX is variable length */
                len = sizeof(((struct sockaddr_un *)
                              (thee->name))->sun_family)
                    + strlen(((struct sockaddr_un *)
                              (thee->name))->sun_path);

                /* Bind socket to address (must setsockopts before bind) */
                if (bind(thee->so,(struct sockaddr *)(thee->name),len)<0) {
                    fprintf(stderr,
                        "Vio_ctor2: Bind fail UNIX sock <%s> dueto <%s>\n",
                        thee->file, VIOstrerrno(errno));
                    VJMPERR2( 0 );
                } else {

                    /* Tell socket to start listening for connections */
                    if (listen(thee->so,5) < 0) {
                        fprintf(stderr,
                        "Vio_ctor2: List fail UNIX sock <%s> dueto <%s>\n",
                        thee->file, VIOstrerrno(errno));
                        VJMPERR2( 0 );
                    }
                }
            /*
             * if we got to here, we can assume reader has done
             * all of: (unlink/)setsockopt/bind/listen
             */
            }
        }
#endif

    /* INET SOCKET READ/WRITE SETUP */
    } else if (thee->type==VIO_INET) {

        /* filename is the port number for the socket */
        if (strlen(filename) >= VMAX_ARGLEN) {
            fprintf(stderr, "Vio_ctor2: Filename <%d> exceeds max <%d>!\n",
                (int)strlen(filename), VMAX_ARGLEN);
            VJMPERR2( 0 );
        }
        strncpy(thee->file, filename, VMAX_ARGLEN);

        /* create the socket address structure */
        thee->name = (struct sockaddr_in *)
            calloc( 1, sizeof(struct sockaddr_in) );
        VJMPERR2( thee->name != VNULL );

        /* Look for sockets */
        if ((thee->so=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            fprintf(stderr,"Vio_ctor2: fail to find INET sock dueto <%s>\n",
                VIOstrerrno(errno));
            VJMPERR2( 0 );
        } else {

            /* set REUSEADDR so sockets can be closed and reopened */
            ival = 1;  /* just need a nonzero value */
            if ( setsockopt(thee->so,SOL_SOCKET,SO_REUSEADDR,
              (void*)&ival,sizeof(ival)) < 0 ) {
                fprintf(stderr, "Vio_ctor2: Setsockopt3 fail INET sock <%s>"
                   " dueto <%s>\n", thee->file, VIOstrerrno(errno));
                VJMPERR2( 0 );
            }

            /* turn on LINGER so WRITES complete before sockets close */
            ling.l_onoff  = 1;   /* just need a nonzero value */
            ling.l_linger = 30;  /* linger time in seconds */
            if ( setsockopt(thee->so,SOL_SOCKET,SO_LINGER,
              (void*)&ling,sizeof(ling)) < 0 ) {
                fprintf(stderr, "Vio_ctor2: Setsockopt4 fail INET sock <%s>"
                   " dueto <%s>\n", thee->file, VIOstrerrno(errno));
                VJMPERR2( 0 );
            }

            /* Setup for the socket */
            memset(thee->name, '\0', sizeof(struct sockaddr_in));
            ((struct sockaddr_in *)(thee->name))->sin_family = AF_INET;
            ((struct sockaddr_in *)(thee->name))->sin_port
                = htons( (unsigned short) (VPORTNUMBER + atoi(thee->file)) );

            /* if we are the reader, WE must create the socket */
            /* the reader must do: setsockopt/bind/listen */
            if (thee->rwkey==VIO_R) {

                /* use wildcard address */
                n = INADDR_ANY;
                memcpy(&((struct sockaddr_in *)(thee->name))->sin_addr,
                    &n, sizeof(long));

                /* determine structure size; AF_INET is fixed length */
                len = sizeof(struct sockaddr_in);

                /* Bind socket to address (must setsockopts before bind) */
                if (bind(thee->so,(struct sockaddr *)(thee->name),len)<0) {
                    fprintf(stderr,
                        "Vio_ctor2: Bind fail INET sock <%s> dueto <%s>\n",
                        thee->file, VIOstrerrno(errno));
                    VJMPERR2( 0 );
                } else {

                    /* Tell socket to start listening for connections */
                    if (listen(thee->so,5) < 0) {
                        fprintf(stderr,
                            "Vio_ctor2: List fail INET sock <%s> dueto <%s>\n",
                            thee->file, VIOstrerrno(errno));
                        VJMPERR2( 0 );
                    }
                }

            /* assume reader has done: setsockopt/bind/listen */
            } else {

                /* network address of port -- "localhost" means WE have port */
                if (!strcmp(hostname,"localhost")) {
                    strcpy(host,thee->lhost);
                } else {
                    strcpy(host,hostname);
                }

                /* get IP address corresponding to this server hostname */
                if ((hpTmp=gethostbyname(host))==VNULL) {
                    fprintf(stderr,
                        "Vio_ctor2: Gethostbyname fail INET sock <%s>"
                        " dueto <%s>\n", thee->file, VIOstrerrno(errno));
                    VJMPERR2( 0 );
                } else {

                    /* just need to save address of host that has socket */
                    memcpy(
                        &(((struct sockaddr_in *)(thee->name))->sin_addr),
                        hpTmp->h_addr_list[0], (unsigned int)hpTmp->h_length);

                    /* save the hostname for the port for later i/o */
                    strcpy(thee->rhost,hpTmp->h_name);
                }
            }
        }
    }

    /* initialize <asc,xdr> datastructures; must do almost at the end */
    if (thee->frmt==VIO_ASC) {
        thee->axdr = (ASC*)calloc( 1, sizeof(ASC) );
        VJMPERR2( thee->axdr != VNULL );
        if (thee->rwkey==VIO_R) {
            ascmem_create(thee->axdr, thee->ioBuffer, VMAX_BUFSIZE, ASC_DECODE);
        } else { /* if (thee->rwkey==VIO_W) */
            ascmem_create(thee->axdr, thee->ioBuffer, VMAX_BUFSIZE, ASC_ENCODE);
        }
    } else if (thee->frmt==VIO_XDR) {
        thee->axdr = (XDR*)calloc( 1, sizeof(XDR) );
        VJMPERR2( thee->axdr != VNULL );
        if (thee->rwkey==VIO_R) {
            xdrmem_create(thee->axdr, thee->ioBuffer, VMAX_BUFSIZE, XDR_DECODE);
        } else { /* if (thee->rwkey==VIO_W) */
            xdrmem_create(thee->axdr, thee->ioBuffer, VMAX_BUFSIZE, XDR_ENCODE);
        }
    }

    /* lastly: default white/comment char sets (careful! propogates to axdr) */
    Vio_setWhiteChars(thee, VIOwhiteChars);
    Vio_setCommChars(thee,  VIOcommChars);

    /* return without error */
    return 1;

  VERROR1:
    fprintf(stderr,"Vio_ctor2: Vio library has not been started.\n");
    return 0;

  VERROR2:
    fprintf(stderr,"Vio_ctor2: some error occurred.\n");
    return 0;
}

/*
 * ***************************************************************************
 * Routine:  Vio_dtor
 *
 * Purpose:  Destroy the [sdio/file/buff/unix/inet] container object.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vio_dtor(Vio **thee)
{
    if ((*thee) != VNULL) {
        if ((*thee)->VIObuffer != VNULL) {
            free( (*thee)->VIObuffer );
            (*thee)->VIObuffer = VNULL;
        }
        Vio_dtor2(*thee);
        free( (*thee) );
        (*thee) = VNULL;
    }
}

/*
 * ***************************************************************************
 * Routine:  Vio_dtor2
 *
 * Purpose:  Destroy the [sdio/file/buff/unix/inet] container object.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vio_dtor2(Vio *thee)
{
    if (thee != VNULL) {

        /* free the <ASC,XDR> structures */
        if ( thee->axdr != VNULL ) {
            if ( thee->frmt == VIO_ASC ) {
                asc_destroy( (ASC*)(thee->axdr) );
            } else if ( thee->frmt == VIO_XDR ) {
                xdr_destroy( (XDR*)(thee->axdr) );
            }
            free( thee->axdr );
            thee->axdr = VNULL;
        }

        /* finish up */
        if (thee->type==VIO_SDIO) {
            /* no-op */
        } else if (thee->type==VIO_FILE) {
            if ( thee->fp != VNULL ) {
                if ( fclose(thee->fp) != 0 ) {
                    fprintf(stderr, "Vio_dtor2: fclose fail device <%s>"
                        " dueto <%s>\n", thee->file,VIOstrerrno(errno));
                }
            }
        } else if (thee->type==VIO_BUFF) {
            /* CMIKE: WHAT ABOUT FREEING THE BUFFER SPACE??? */
            thee->VIObufferPtr = 0;
        } else if ( (thee->type==VIO_UNIX)
                 || (thee->type==VIO_INET) ) {
            if ( thee->soc >= 0 ) {
#if defined(HAVE_WINSOCK_H)
                if ( closesocket(thee->soc) != 0 ) {
                    fprintf(stderr, "Vio_dtor2: closesocket1 fail device <%s>"
                        " dueto <%s>\n", thee->file,
                        VIOstrerrno(errno));
                }
#else
                if ( close(thee->soc) != 0 ) {
                    fprintf(stderr, "Vio_dtor2: close1 fail device <%s>"
                        " dueto <%s>\n", thee->file,
                    VIOstrerrno(errno));
                }
#endif
            }
            if ( thee->so >= 0 ) {
#if defined(HAVE_WINSOCK_H)
                if ( closesocket(thee->so) != 0 ) {
                    fprintf(stderr, "Vio_dtor2: closesocket2 fail device <%s>"
                        " dueto <%s>\n", thee->file,
                        VIOstrerrno(errno));
                }
#else
                if ( close(thee->so) != 0 ) {
                    fprintf(stderr, "Vio_dtor2: close2 fail device <%s>"
                        " dueto <%s>\n", thee->file,
                        VIOstrerrno(errno));
                }
#endif
            }

            /* remove the device file for domain sockets */
            if (thee->type==VIO_UNIX)
                if (thee->rwkey==VIO_R)
                    unlink(thee->file);

        } else {
            fprintf(stderr,"Vio_dtor2: Bad type found <%d>\n", thee->type);
        }

        if ( (thee->type==VIO_UNIX)
          || (thee->type==VIO_INET) ) {
            if (thee->name != VNULL) {
                free( thee->name );
            }
            thee->name = VNULL;
        }

        /* we called WSAStartup() in constructor; must always be paired */
#if defined(HAVE_WINSOCK_H)
        WSACleanup();
#endif
    }
}

/*
 * ***************************************************************************
 * Routine:  Vio_setWhiteChars
 *
 * Purpose:  Define the white character set.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vio_setWhiteChars(Vio *thee, char *whiteChars)
{
    if (thee != VNULL) {
        strncpy(thee->whiteChars, whiteChars, VMAX_ARGNUM);

        /* propogate the character set down to the ASC structure */
        VASSERT( thee->axdr != VNULL );
        if (thee->frmt == VIO_ASC) {
            asc_setWhiteChars(thee->axdr, whiteChars);
        } else if (thee->frmt == VIO_XDR) {
#if !defined(HAVE_XDR)
            asc_setWhiteChars(thee->axdr, whiteChars);
#endif
        } else { VASSERT( 0 ); }
    }
}

/*
 * ***************************************************************************
 * Routine:  Vio_setCommChars
 *
 * Purpose:  Define the comment character set.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vio_setCommChars(Vio *thee, char *commChars)
{
    if (thee != VNULL) {
        strncpy(thee->commChars, commChars, VMAX_ARGNUM);

        /* propogate the character set down to the ASC structure */
        VASSERT( thee->axdr != VNULL );
        if (thee->frmt == VIO_ASC) {
            asc_setCommChars(thee->axdr, commChars);
        } else if (thee->frmt == VIO_XDR) {
#if !defined(HAVE_XDR)
            asc_setCommChars(thee->axdr, commChars);
#endif
        } else { VASSERT( 0 ); }
    }
}

/*
 * ***************************************************************************
 * Routine:  Vio_accept
 *
 * Purpose:  Accept any waiting connect attempt to our socket on our machine.
 *
 * Notes:    The nonblock parameter has the following interpretation:
 *           (Only for <UNIX/INET>; othewise it is ignored.)
 *
 *               nonblock==0  ==> block until a connect is attempted
 *               nonblock==1  ==> DO NOT block at all
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC int Vio_accept(Vio *thee, int nonblock)
{
    int rc;

#if defined(HAVE_WINSOCK_H)
    unsigned long  blockKey;
#else
    int flags = 0;
    struct sockaddr_in peer;
    struct hostent *hpTmp;
#endif

#if defined(ACCEPT_USES_ULONG)
    unsigned long len;
#elif defined(ACCEPT_USES_UINT)
    unsigned int len;
#elif defined(ACCEPT_USES_INT)
    int len;
#else
    unsigned int len;
#endif

    /* reset error tag */
    thee->error = 0;

    thee->soc = -1;
    rc = -1;

    Vio_initIoPutBuffers(thee);
    VJMPERR2( thee->rwkey == VIO_R );

    if ( (thee->type==VIO_SDIO)
      || (thee->type==VIO_FILE)
      || (thee->type==VIO_BUFF) ) {

        /* ONLY for file i/o, we need to look at and set the dirty bit */
        /* (this keeps us from reading the file twice) */
        if (thee->type==VIO_FILE) {
            if ((!thee->dirty) && (!feof(thee->fp))) {
                thee->dirty = 1;
                rc = 1;
            }
        } else {
            rc = 1;
        }

    } else if (thee->type==VIO_UNIX) {

#if defined(HAVE_SYS_UN_H)
        /* Make this a non-blocking socket just for the accept call */
        if (nonblock) {
            flags = fcntl( thee->so, F_GETFL, 0 );
            fcntl( thee->so, F_SETFL, flags | VO_NONBLOCK );
        }

        /* accept */
        len = sizeof(struct sockaddr_un);
        rc = accept(thee->so,(struct sockaddr *)(thee->name),&len);
        thee->soc = rc;
        if ((!nonblock) && (rc < 0)) {
            fprintf(stderr, "Vio_accept: Accept fail UNIX sock <%s>"
                " dueto <%s>\n", thee->file, VIOstrerrno(errno));
            VJMPERR2( 0 );
        }

        /* restore blocking -- must nonblock for LINGER to work! */
        if (nonblock) {
            fcntl( thee->so, F_SETFL, flags );
        }
#endif

    } else if (thee->type==VIO_INET) {

        /* Make this non-blocking socket just for accept call */
        if (nonblock) {
#if defined(HAVE_WINSOCK_H)
            blockKey = 1;
            if ( ioctlsocket( thee->so, FIONBIO, &blockKey ) != 0 ) {
                fprintf(stderr, "Vio_accept: Ioctlsocket1 fail INET sock <%s>"
                   " dueto <%s>\n", thee->file, VIOstrerrno(errno));
                VJMPERR2( 0 );
            }
#else
            flags = fcntl( thee->so, F_GETFL, 0 );
            fcntl( thee->so, F_SETFL, flags | VO_NONBLOCK );
#endif
        }

        len = sizeof(struct sockaddr_in);
        rc = accept(thee->so, (struct sockaddr *)(thee->name), &len);
        thee->soc = rc;
        if ((!nonblock) && (rc < 0)) {
            fprintf(stderr, "Vio_accept: Accept fail INET sock <%s>"
                " dueto <%s>\n", thee->file, VIOstrerrno(errno));
            VJMPERR2( 0 );
        }

        /* restore blocking -- must nonblock for LINGER to work! */
        if (nonblock) {
#if defined(HAVE_WINSOCK_H)
            blockKey = 0;
            if ( ioctlsocket( thee->so, FIONBIO, &blockKey ) != 0 ) {
                fprintf(stderr, "Vio_accept: Ioctlsocket2 fail INET sock <%s>"
                   " dueto <%s>\n", thee->file, VIOstrerrno(errno));
                VJMPERR2( 0 );
            }
#else
            fcntl( thee->so, F_SETFL, flags );
#endif
        }

        /* if we found a writer, get his hostname (just for i/o) */
        if (rc >= 0) {
#if defined(HAVE_WINSOCK_H)
            strcpy(thee->rhost,"unknown");
#else
            len = sizeof(struct sockaddr_in);
            if (getpeername(thee->soc,(struct sockaddr *)(&peer),&len)<0) {
                fprintf(stderr, "Vio_accept: Getpeername fail INET <%s>"
                    " dueto <%s>\n", thee->file, VIOstrerrno(errno));
                VJMPERR2( 0 );
            } else if (VNULL==(hpTmp=gethostbyname(inet_ntoa(peer.sin_addr)))){
                fprintf(stderr, "Vio_accept: Gethostbyname fail INET <%s>"
                    " dueto <%s>\n", thee->file, VIOstrerrno(errno));
                VJMPERR2( 0 );
            } else {
                strcpy(thee->rhost,hpTmp->h_name);
            }
#endif
        }

    } else {
        fprintf(stderr,"Vio_accept: Bad type found <%d>\n", thee->type);
        VJMPERR2( 0 );
    }

    /* normal return */
    return rc;

  VERROR2:
    thee->error = 1;
    return -1;
}

/*
 * ***************************************************************************
 * Routine:  Vio_acceptFree
 *
 * Purpose:  Free the socket child that was used for the last accept.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vio_acceptFree(Vio *thee)
{
    /* VJMPERR2( !thee->error ); */  /* Need to close the socket... */
    VJMPERR2( thee->rwkey == VIO_R );

    if ( (thee->type==VIO_SDIO)
      || (thee->type==VIO_FILE)
      || (thee->type==VIO_BUFF) ) {
        /* no-op */
    } else if ( (thee->type==VIO_UNIX)
             || (thee->type==VIO_INET) ) {
        if ( thee->soc >= 0 ) {
#if defined(HAVE_WINSOCK_H)
            if ( closesocket(thee->soc) != 0 ) {
                fprintf(stderr, "Vio_acceptFree: closesocket fail device <%s>"
                    " dueto <%s>\n", thee->file,VIOstrerrno(errno));
                VJMPERR2( 0 );
            }
#else
            if ( close(thee->soc) != 0 ) {
                fprintf(stderr, "Vio_acceptFree: close fail device <%s>"
                    " dueto <%s>\n", thee->file,VIOstrerrno(errno));
                VJMPERR2( 0 );
            }
#endif
        }
    } else {
        fprintf(stderr,"Vio_acceptFree: Bad type found <%d>\n", thee->type);
        VJMPERR2( 0 );
    }

    thee->soc = -1;

    /* normal return */
    Vio_initIoPutBuffers(thee);
    return;

  VERROR2:
    Vio_initIoPutBuffers(thee);
    thee->error = 1;
    return;
}

/*
 * ***************************************************************************
 * Routine:  Vio_connect
 *
 * Purpose:  Connect to some socket on a remote machine (or on our machine).
 *
 * Notes:    The nonblock parameter has the following interpretation:
 *           (Only for <UNIX/INET>; othewise it is ignored.)
 *
 *               nonblock==0  ==> block until our connection is accepted
 *               nonblock==1  ==> DO NOT block at all
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC int Vio_connect(Vio *thee, int nonblock)
{
    int rc;
#if defined(HAVE_WINSOCK_H)
    unsigned long len;
#else
    int len;
    int flags = 0;
#endif

    /* reset error tag */
    thee->error = 0;

    rc = -1;

    Vio_initIoPutBuffers(thee);
    VJMPERR2( thee->rwkey == VIO_W );

    if ( (thee->type==VIO_SDIO)
      || (thee->type==VIO_FILE)
      || (thee->type==VIO_BUFF) ) {
        rc = 1;
    } else if (thee->type==VIO_UNIX) {

#if defined(HAVE_SYS_UN_H)
        /* Make this a non-blocking socket just for the connect call */
        if (nonblock) {
            flags = fcntl( thee->so, F_GETFL, 0 );
            fcntl( thee->so, F_SETFL, flags | VO_NONBLOCK );
        }

        /* blocking connect */
        len = sizeof(struct sockaddr_un);
        rc = connect(thee->so, (struct sockaddr *)(thee->name),len);
        if ((!nonblock) && (rc < 0)) {
            fprintf(stderr, "Vio_connect: Conn fail UNIX sock <%s>"
                " dueto <%s>\n", thee->file, VIOstrerrno(errno));
            VJMPERR2( 0 );
        }

        /* restore blocking -- must nonblock for LINGER to work! */
        if (nonblock) {
            fcntl( thee->so, F_SETFL, flags );
        }
#endif

    } else if (thee->type==VIO_INET) {

        /* make this a non-blocking socket just for the connect call */
        if (nonblock) {
#if defined(HAVE_WINSOCK_H)
            len = 1;
            if ( ioctlsocket( thee->so, FIONBIO, &len ) != 0 ) {
                fprintf(stderr, "Vio_connect: Ioctlsocket1 fail INET sock <%s>"
                   " dueto <%s>\n", thee->file, VIOstrerrno(errno));
                VJMPERR2( 0 );
            }
#else
            flags = fcntl( thee->so, F_GETFL, 0 );
            fcntl( thee->so, F_SETFL, flags | VO_NONBLOCK );
#endif
        }

        /* blocking connect */
        len = sizeof(struct sockaddr_in);
        rc = connect(thee->so, (struct sockaddr *)(thee->name),len);
        if ((!nonblock) && (rc < 0)) {
            fprintf(stderr, "Vio_connect: Conn fail INET sock <%s>"
                 " dueto <%s>\n", thee->file, VIOstrerrno(errno));
            VJMPERR2( 0 );
        }

        /* restore blocking -- must nonblock for LINGER to work! */
        if (nonblock) {
#if defined(HAVE_WINSOCK_H)
            len = 0;
            if ( ioctlsocket( thee->so, FIONBIO, &len ) != 0 ) {
                fprintf(stderr, "Vio_connect: Ioctlsocket2 fail INET sock <%s>"
                   " dueto <%s>\n", thee->file, VIOstrerrno(errno));
                VJMPERR2( 0 );
            }
#else
            fcntl( thee->so, F_SETFL, flags );
#endif
        }

    } else {
        fprintf(stderr,"Vio_connect: Bad type found <%d>\n", thee->type);
        VJMPERR2( 0 );
    }

    /* normal return */
    return rc;

  VERROR2:
    thee->error = 1;
    return -1;
}

/*
 * ***************************************************************************
 * Routine:  Vio_connectFree
 *
 * Purpose:  Purge any output buffers (for <UNIX/INET>, else a no-op).
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vio_connectFree(Vio *thee)
{
    /* VJMPERR2( !thee->error ); */  /* Need to close the socket... */
    VJMPERR2( thee->rwkey == VIO_W );

    if ( (thee->type==VIO_SDIO)
      || (thee->type==VIO_FILE)
      || (thee->type==VIO_BUFF) ) {
        /* no-op */
    } else if ( (thee->type==VIO_UNIX)
             || (thee->type==VIO_INET) ) {
        Vio_purgePutBuffer(thee);
    } else {
        fprintf(stderr,"Vio_connectFree: Bad type found <%d>\n", thee->type);
        VJMPERR2( 0 );
    }

    /* normal return */
    Vio_initIoPutBuffers(thee);
    return;

  VERROR2:
    Vio_initIoPutBuffers(thee);
    thee->error = 1;
    return;
}

/*
 * ***************************************************************************
 * Routine:  Vio_findNewline
 *
 * Purpose:  finds the new line char if any from current position in iobuffer
 *
 * Author:   Juan Brandi
 * ***************************************************************************
 */
VPUBLIC int Vio_findNewLine(Vio *thee){

	char nullChar = '\n';
	int pntr = asc_getpos((ASC*)thee->axdr);
	int isNewline = 0;

	if(thee == VNULL){
		Vnm_print(2, "Vio_findNewLine:  Got NULL pointer!\n");
		return isNewline;
	}

	char *buf;
	buf = thee->ioBuffer;

	while(pntr < thee->ioBufferLen && isNewline == 0){
		if(memcmp(&buf[pntr], &nullChar,sizeof(char)) == 0){
			asc_setpos((ASC*)thee->axdr, pntr);
			isNewline = 1;
		}
		pntr++;
	}

	return isNewline;

}

/*
 * ***************************************************************************
 * Routine:  Vio_scanf
 *
 * Purpose:  Mimic "scanf" from an arbitrary Vio device.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC int Vio_scanf(Vio *thee, char *parms, ... )
{
    va_list ap;
    char arg0, arg1, arg2, *cval, *sval, buf[VMAX_BUFSIZE];
    int i, len, tokCount, *ival;
    float *fval;
    double *dval;

    VJMPERR2( !thee->error );
    VJMPERR2( thee->rwkey == VIO_R );

    /* get the value of the current pointer that points into the ioBuffer */
    len = 0;
    if (thee->frmt == VIO_ASC) {
        len = asc_getpos((ASC*)thee->axdr);
    } else if (thee->frmt == VIO_XDR) {
        len = xdr_getpos((XDR*)thee->axdr);
    } else { VASSERT( 0 ); }

    /* if the buffer is completely empty (i.e., first time here) fill it up */
    if ( thee->ioBufferLen == 0 ) {

        /* read the data */
        thee->ioBufferLen = Vio_read( thee, thee->ioBuffer, VMAX_BUFSIZE );

        /* set the buffer point to 0 */
        if (thee->frmt == VIO_ASC) {
            VJMPERR1( asc_setpos((ASC*)thee->axdr, 0) );
        } else if (thee->frmt == VIO_XDR) {
            VJMPERR1( xdr_setpos((XDR*)thee->axdr, 0) );
        } else { VASSERT( 0 ); }

    /* if current point is more than halfway through buf, read in more data */
    } else if ( len > (VMAX_BUFSIZE/2) ) {

        /* sanity check */
        VJMPERR1( len <= thee->ioBufferLen );

        /* copy unread part of ioBuffer into temp buf and clear ioBuffer */
        for (i=len; i<thee->ioBufferLen; i++)
            buf[i-len] = thee->ioBuffer[i];
        memset(thee->ioBuffer,  '\0', sizeof(thee->ioBuffer));

        /* read temp buffer back, reseting to the beginning of ioBuffer */
        thee->ioBufferLen = thee->ioBufferLen - len;
        for (i=0; i<thee->ioBufferLen; i++)
            thee->ioBuffer[i] = buf[i];

        /* reset the buffer point to 0 */
        if (thee->frmt == VIO_ASC) {
            VJMPERR1( asc_setpos((ASC*)thee->axdr, 0) );
        } else if (thee->frmt == VIO_XDR) {
            VJMPERR1( xdr_setpos((XDR*)thee->axdr, 0) );
        } else { VASSERT( 0 ); }

        /* finally, read in the new data, starting at end of current data */
        thee->ioBufferLen += Vio_read(thee,
            thee->ioBuffer+thee->ioBufferLen, VMAX_BUFSIZE-thee->ioBufferLen );

    /* we (hopefully?) have enough in buffer to work with; do nothing here */
    } else {
        /* no-op */
    }

    /* we ALWAYS have to pick the format specifier apart <ASC,XDR> ... */
    tokCount = 0;
    len = strlen(parms);
    va_start(ap, parms);
    i = 0;
    while ( i < len ) {
        arg0 = parms[i];
        if ( arg0 == ' ' ) {
            i+=1;
        } else if ( arg0 == '\n' ) {
            i+=1;
        } else if ( i+1 < len ) {
            arg1 = parms[i+1];
            if ( arg1 == 's' ) {
                sval = va_arg(ap, char*);
                if ((i == len-3) && ( parms[len-1] == '\n' )) {
                    if (thee->frmt == VIO_ASC) {
                        VASSERT( 0 ); /* is this ever executed??? */
                    } else if (thee->frmt == VIO_XDR) {
                        VASSERT( 0 ); /* is this ever executed??? */
                    } else { VASSERT( 0 ); }
                } else {
                    if (thee->frmt == VIO_ASC) {
                        VJMPERR1( asc_string(thee->axdr, &sval, VMAX_BUFSIZE) );
                    } else if (thee->frmt == VIO_XDR) {
                        VJMPERR1( xdr_string(thee->axdr, &sval, VMAX_BUFSIZE) );
                    } else { VASSERT( 0 ); }
                }
                tokCount++;
                i+=2;
            } else if ( arg1 == 'c' ) {
                cval = va_arg(ap, char*);
                if (thee->frmt == VIO_ASC) {
                    VJMPERR1( asc_char( thee->axdr, cval ) );
                } else if (thee->frmt == VIO_XDR) {
                    VJMPERR1( xdr_char( thee->axdr, cval ) );
                } else { VASSERT( 0 ); }
                tokCount++;
                i+=2;
            } else if ( arg1 == 'd' ) {
                ival = va_arg(ap, int*);
                if (thee->frmt == VIO_ASC) {
                    VJMPERR1( asc_int( thee->axdr, ival ) );
                } else if (thee->frmt == VIO_XDR) {
                    VJMPERR1( xdr_int( thee->axdr, ival ) );
                } else { VASSERT( 0 ); }
                tokCount++;
                i+=2;
            } else if ( arg1 == 'f' ) {
                fval = va_arg(ap, float*);
                if (thee->frmt == VIO_ASC) {
                    VJMPERR1( asc_float( thee->axdr, fval ) );
                } else if (thee->frmt == VIO_XDR) {
                    VJMPERR1( xdr_float( thee->axdr, fval ) );
                } else { VASSERT( 0 ); }
                tokCount++;
                i+=2;
            } else if ( arg1 == 'e' ) {
                fval = va_arg(ap, float*);
                if (thee->frmt == VIO_ASC) {
                    VJMPERR1( asc_float( thee->axdr, fval ) );
                } else if (thee->frmt == VIO_XDR) {
                    VJMPERR1( xdr_float( thee->axdr, fval ) );
                } else { VASSERT( 0 ); }
                tokCount++;
                i+=2;
            } else if (( arg1 == 'l' ) && ( i+2 < len )) {
                arg2 = parms[i+2];
                if ( arg2 == 'e' ) {
                    dval = va_arg(ap, double*);
                    if (thee->frmt == VIO_ASC) {
                        VJMPERR1( asc_double( thee->axdr, dval ) );
                    } else if (thee->frmt == VIO_XDR) {
                        VJMPERR1( xdr_double( thee->axdr, dval ) );
                    } else { VASSERT( 0 ); }
                    tokCount++;
                    i+=3;
                } else { VJMPERR1( 0 ); }
            } else { VJMPERR1( 0 ); }
        } else { VJMPERR1( 0 ); }
    }
    va_end(ap);

    /* return without error */
    return tokCount;

  VERROR1:
    va_end(ap);
    /*
     * APBS-KTS: Check to see if we are here because of an EOF condition.  If
     * so, then just quietly go away.
     */
    if (thee->frmt == VIO_ASC && ((ASC *)thee->axdr)->error != EOF)
        fprintf(stderr,"Vio_scanf: Format problem with input.\n");
  VERROR2:
    thee->error = 1;
    return 0;
}

/*
 * ***************************************************************************
 * Routine:  Vio_printf
 *
 * Purpose:  Mimic "printf" to an arbitrary Vio device.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC int Vio_printf(Vio *thee, char *parms, ... )
{
    va_list ap;
    char buf[VMAX_BUFSIZE];
    int len;

    char arg0, arg1, arg2, cval, *sval;
    int i, tokCount, ival;
    float fval;
    double dval;

    VJMPERR2( !thee->error );
    VJMPERR2( thee->rwkey == VIO_W );

    /* if ASCII data then use vsprintf to handle format specifier exactly */
    if (thee->frmt == VIO_ASC) {
        va_start(ap, parms);
        vsprintf(buf, parms, ap);
        va_end(ap);
        len = strlen(buf);
        return Vio_writePutBuffer(thee,buf,len);
    }

    /* if XDR data then we have to pick the format specifier apart... */
    len = strlen(parms);
    va_start(ap, parms);
    tokCount = 0;
    i = 0;
    while ( i < len ) {
        arg0 = parms[i];
        if ((arg0 == '%') && (i+1 < len)) {
            arg1 = parms[i+1];
            if ( arg1 == '%' ) {
                i+=2;
            } else {
                while (!strchr("scdfel",arg1)) {
                    i+=1;
                    arg1 = parms[i+1];
                    VJMPERR1( i+1 < len );
                }
                if ( arg1 == 's' ) {
                    sval = va_arg(ap, char*);
                    /* don't put comment strings into xdr files */
                    if (!strchr(thee->commChars,sval[0])) {
                        VJMPERR1( xdr_string(thee->axdr, &sval, strlen(sval)) );
                        tokCount++;
                    }
                    i+=2;
                } else if ( arg1 == 'c' ) {
                    /* are char args always passed as int? ... */
                    cval = (char)va_arg(ap, int);  /* CAST FROM INT */
                    VJMPERR1( xdr_char( thee->axdr, &cval ) );
                    tokCount++;
                    i+=2;
                } else if ( arg1 == 'd' ) {
                    ival = va_arg(ap, int);
                    VJMPERR1( xdr_int( thee->axdr, &ival ) );
                    tokCount++;
                    i+=2;
                } else if ( arg1 == 'f' ) {
                    /* are float args always passed as double? ... */
                    fval = (float)va_arg(ap, double);  /* CAST FROM DOUBLE */
                    VJMPERR1( xdr_float( thee->axdr, &fval ) );
                    tokCount++;
                    i+=2;
                } else if ( arg1 == 'e' ) {
                    /* are float args always passed as double? ... */
                    fval = (float)va_arg(ap, double);  /* CAST FROM DOUBLE */
                    VJMPERR1( xdr_float( thee->axdr, &fval ) );
                    tokCount++;
                    i+=2;
                } else if (( arg1 == 'l' ) && ( i+2 < len )) {
                    arg2 = parms[i+2];
                    if ( arg2 == 'e' ) {
                        dval = va_arg(ap, double);
                        VJMPERR1( xdr_double( thee->axdr, &dval ) );
                        tokCount++;
                        i+=3;
                    } else { VJMPERR1( 0 ); }
                } else { VJMPERR1( 0 ); }
            }
        } else {
            i+=1;
        }
    }
    va_end(ap);

    /* finally write out the XDR buffer */
    VJMPERR1( 0<=(len=xdr_getpos((XDR*)thee->axdr)) );
    VJMPERR1( Vio_writePutBuffer(thee,thee->ioBuffer,len) == len );
    VJMPERR1( xdr_setpos((XDR*)thee->axdr, 0) );

    /* return without error */
    return len;

  VERROR1:
    va_end(ap);
    fprintf(stderr,"Vio_printf: Format problem with output.\n");

  VERROR2:
    thee->error = 1;
    return 0;
}

/*
 * ***************************************************************************
 * Routine:  Vio_read
 *
 * Purpose:  Read (up to) bufsize characters into buf from input device.
 *
 * Notes:    The number of bytes read is returned.
 *
 *           It is not necessarily an error if the number of bytes read
 *           is less than bufsize (EOF may have been encountered).
 *
 *           Acts exactly like fread() or read().
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC int Vio_read(Vio *thee, char *buf, int bufsize)
{
    int rc, i, ilen;

    VJMPERR2( !thee->error );
    VJMPERR2( thee->rwkey == VIO_R );

    rc = 0;
    if (bufsize > 0) {
        if ( (thee->type==VIO_SDIO)
          || (thee->type==VIO_FILE) ) {
            rc = fread(buf, sizeof(char), (unsigned int)bufsize, thee->fp);
            /* CMIKE: if (rc!=bufsize), make SURE EOF was reached! */
        } else if (thee->type==VIO_BUFF) {
            ilen = VMIN2( bufsize, thee->VIObufferLen - thee->VIObufferPtr );
            for (i=0; i<ilen; i++)
                buf[i] = thee->VIObuffer[thee->VIObufferPtr + i];
            thee->VIObufferPtr += ilen;
            rc = ilen;
        } else if ( (thee->type==VIO_UNIX)
                 || (thee->type==VIO_INET) ) {
            rc = readn(thee->soc, buf, (unsigned int)bufsize);
            /* CMIKE: if (rc!=bufsize), make SURE EOF was reached! */
        } else {
            fprintf(stderr,"Vio_read: Bad type found <%d>\n", thee->type);
            rc = 0;
            VJMPERR2( 0 );
        }
    }

    /* return without error */
    return rc;

  VERROR2:
    thee->error = 1;
    return 0;
}

/*
 * ***************************************************************************
 * Routine:  Vio_write
 *
 * Purpose:  Write bufsize characters from buf to output device.
 *
 * Notes:    The number of bytes written is returned.
 *
 *           On success, the returned bytecount is the same as the number
 *           of bytes in the input buffer.
 *
 *           On failure, the returned bytecount is less than the number
 *           of bytes in the input buffer.
 *
 *           Acts exactly like fwrite() or write().
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC int Vio_write(Vio *thee, char *buf, int bufsize)
{
    int rc, i, isize;
    char *tmpBuf;

    VJMPERR2( !thee->error );
    VJMPERR2( thee->rwkey == VIO_W );

    rc = 0;
    if (bufsize > 0) {
        if ( (thee->type==VIO_SDIO)
          || (thee->type==VIO_FILE) ) {
            rc = fwrite(buf, sizeof(char), (unsigned int)bufsize, thee->fp);
            VJMPERR1( rc == bufsize );
        } else if (thee->type==VIO_BUFF) {
            while ( bufsize > (thee->VIObufferLen - thee->VIObufferPtr) ) {
                isize = VMAX2( 1, 2*(thee->VIObufferLen) );
                tmpBuf = (char*)calloc( isize, sizeof(char) );
                VJMPERR1( tmpBuf != VNULL );
                for (i=0; i<thee->VIObufferLen; i++)
                    tmpBuf[i] = thee->VIObuffer[i];
                free( thee->VIObuffer );
                thee->VIObuffer = tmpBuf;
                thee->VIObufferLen = isize;
            }
            for (i=0; i<bufsize; i++)
                thee->VIObuffer[thee->VIObufferPtr + i] = buf[i];
            thee->VIObufferPtr += bufsize;
            rc = bufsize;
            VJMPERR1( rc == bufsize );
        } else if ( (thee->type==VIO_UNIX)
                 || (thee->type==VIO_INET) ) {
            rc = writen(thee->so, buf, (unsigned int)bufsize);
            VJMPERR1( rc == bufsize );
        } else {
            fprintf(stderr,"Vio_write: Bad type found <%d>\n", thee->type);
            rc = 0;
            VJMPERR2( 0 );
        }
    }

    /* return without error */
    return rc;

  VERROR1:
    fprintf(stderr,"Vio_write: Error occurred (bailing out).\n");
  VERROR2:
    thee->error = 1;
    return 0;
}

/*
 * ***************************************************************************
 * Routine:  Vio_initIoPutBuffers
 *
 * Purpose:  Initialize the internal buffer.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPRIVATE void Vio_initIoPutBuffers(Vio *thee)
{
    /* initialize the buffer space */
    memset(thee->ioBuffer,  '\0', sizeof(thee->ioBuffer));
    memset(thee->putBuffer, '\0', sizeof(thee->putBuffer));
    thee->ioBufferLen = 0;
    thee->putBufferLen = 0;
}

/*
 * ***************************************************************************
 * Routine:  Vio_purgePutBuffer
 *
 * Purpose:  Purge the internal buffer.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPRIVATE void Vio_purgePutBuffer(Vio *thee)
{
    int len;

    VJMPERR2( !thee->error );
    VJMPERR2( thee->rwkey == VIO_W );

    len = thee->putBufferLen;
    if ( (thee->type==VIO_UNIX)
      || (thee->type==VIO_INET) ) {
        if ( Vio_write(thee,thee->putBuffer,len) != len ) {
            fprintf(stderr,
               "Vio_purgePutBuffer: Vio_write fail UNIX/INET sock <%s>"
               " dueto <%s>\n", thee->file, VIOstrerrno(errno));
            VJMPERR2( 0 );
        }
        memset(thee->putBuffer, '\0', sizeof(thee->putBuffer));
    } else {
        fprintf(stderr,"Vio_purgePutBuffer: Bad type found <%d>\n",thee->type);
        VJMPERR2( 0 );
    }

    /* return without error */
    return;

  VERROR2:
    thee->error = 1;
    return;
}

/*
 * ***************************************************************************
 * Routine:  Vio_writePutBuffer
 *
 * Purpose:  Write bufsize characters from buf to output device.
 *
 * Notes:    The number of bytes written is returned.
 *
 *           On success, the returned bytecount is the same as the number
 *           of bytes in the input buffer.
 *
 *           On failure, the returned bytecount is less than the number
 *           of bytes in the input buffer.
 *
 *           Acts exactly like fwrite() or write().
 *
 * Comment:  This is simply a buffered version of Vio_write().
 *           The Vio object maintains the buffer safely internally.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPRIVATE int Vio_writePutBuffer(Vio *thee, char *buf, int bufsize)
{
    int i, curLen;

    VJMPERR2( !thee->error );
    VJMPERR2( thee->rwkey == VIO_W );

    /* attempt to buffer the i/o to get some speed */
    if ( (thee->type==VIO_SDIO)
      || (thee->type==VIO_FILE)
      || (thee->type==VIO_BUFF) ) {

        if ( Vio_write(thee,buf,bufsize) != bufsize ) {
            fprintf(stderr,
                "Vio_writePutBuffer: Vio_write(1) fail FILE sock <%s>"
                " dueto <%s>\n", thee->file, VIOstrerrno(errno));
            VJMPERR2( 0 );
        }

    } else if ( (thee->type==VIO_UNIX)
             || (thee->type==VIO_INET) ) {

        /* incoming data is larger than our buffer */
        if (bufsize > (int)sizeof(thee->putBuffer)) {

            /* just do a normal unbuffered socket write */
            if ( Vio_write(thee,buf,bufsize) != bufsize ) {
                fprintf(stderr, "Vio_writePutBuffer: Vio_write(2) fail"
                    " UNIX/INET sock <%s>"
                    " dueto <%s>\n", thee->file, VIOstrerrno(errno));
                VJMPERR2( 0 );
            }

        /* incoming data will fit in our buffer */
        } else {

            curLen = thee->putBufferLen;

            /* it fits in now -- just cat it to the end of the buffer */
            if ( (curLen + bufsize) <= (int)sizeof(thee->putBuffer) ) {
                for (i=0; i<bufsize; i++)
                    thee->putBuffer[curLen+i] = buf[i];
                thee->putBufferLen += bufsize;

            /* it won't fit until we write out the existing buffer */
            } else {
                if ( Vio_write(thee,thee->putBuffer,curLen) != curLen ) {
                    fprintf(stderr, "Vio_writePutBuffer: Vio_write(3)"
                     " fail UNIX/INET sock <%s>"
                     " dueto <%s>\n", thee->file, VIOstrerrno(errno));
                    VJMPERR2( 0 );
                }
                thee->putBufferLen = 0;
                memset(thee->putBuffer, '\0', sizeof(thee->putBuffer));
                for (i=0; i<bufsize; i++)
                    thee->putBuffer[i] = buf[i];
                thee->putBufferLen += bufsize;
            }
        }

    } else {
        fprintf(stderr,"Vio_writePutBuffer: Bad type found <%d>\n",thee->type);
        VJMPERR2( 0 );
    }

    /* return without error */
    return bufsize;

  VERROR2:
    thee->error = 1;
    return 0;
}

/*
 * ***************************************************************************
 * Routine:  ascmem_create, asc_destroy, asc_getpos, asc_setpos
 *           asc_string, asc_char, asc_int, asc_float, asc_double
 *           asc_setWhiteChars, asc_setCommChars, asc_getToken
 *
 * Purpose:  An ASC (i.e. ASCII) dual to the XDR routines.
 *
 * Notes:    These routines basically function idential to the XDR routines,
 *           except that after calling the constructor <ascmem_create>, one
 *           must call two additional routines, <asc_setWhiteChars> and
 *           <asc_setCommChars>, to specify the strings representing the
 *           whitespace in the ASCII stream separating the tokens, and a set
 *           of possible comment characters which generate skips to a newline.
 *
 *           The only complicated routine is <asc_genToken>, on which
 *           most of the other things rest.
 *
 *           Both ASC_ENCODE (write) and ASC_DECODE (read) directions work.
 *           In ASC_ENCODE mode, the tokens are separated by a single newline
 *           (for lack of anything more intelligent to do...).
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */

/*
 * ***************************************************************************
 * Routine:  ascmem_create
 *
 * Purpose:  Create the ASC structure.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPRIVATE void ascmem_create(ASC *thee, char *buf, int size, ASCmode mode)
{
    thee->mode = mode;
    thee->pos  = 0;
    thee->size = size;
    thee->buf  = buf;
    memset(thee->whiteChars, '\0', VMAX_ARGNUM);
    memset(thee->commChars,  '\0', VMAX_ARGNUM);
    thee->error = 0;     /* APBS-KTS: initialize to "no error" */
}

/*
 * ***************************************************************************
 * Routine:  asc_destroy
 *
 * Purpose:  Destroy the ASC structure.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPRIVATE void asc_destroy(ASC *thee)
{
    thee->mode = ASC_NO_MODE;
    thee->pos  = 0;
    thee->size = 0;
    thee->buf  = VNULL;
    memset(thee->whiteChars, '\0', VMAX_ARGNUM);
    memset(thee->commChars,  '\0', VMAX_ARGNUM);
    thee->error = 0;     /* APBS-KTS: reset to "no error" */
}

/*
 * ***************************************************************************
 * Routine:  asc_getpos
 *
 * Purpose:  Return the current position in the ASC stream.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPRIVATE int asc_getpos(ASC *thee)
{
    return thee->pos;
}

/*
 * ***************************************************************************
 * Routine:  asc_setpos
 *
 * Purpose:  Set the current position in the ASC stream.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPRIVATE int asc_setpos(ASC *thee, int pos)
{
    thee->pos = pos;
    return 1;
}

/*
 * ***************************************************************************
 * Routine:  asc_string
 *
 * Purpose:  DECODE or ENCODE a string.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPRIVATE int asc_string(ASC *thee, char **sval, int size)
{
    int i, len;
    char tok[VMAX_BUFSIZE];

    if (thee->mode == ASC_DECODE) {
        VJMPERR1( VNULL != asc_getToken(thee, tok, VMAX_BUFSIZE) );
        sscanf(tok,"%s",(*sval));
        //memcpy((*sval), tok, strlen(tok));
    } else if (thee->mode == ASC_ENCODE) {
        sprintf(tok,"%s\n",*sval);
        len = strlen(tok);
        for (i=0; i<len; i++)
            thee->buf[thee->pos+i] = tok[i];
        thee->pos += len;
    }
    return 1;

  VERROR1:
    return 0;
}

/*
 * ***************************************************************************
 * Routine:  asc_char
 *
 * Purpose:  DECODE or ENCODE a char.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPRIVATE int asc_char(ASC *thee, char *cval)
{
    int i, len;
    char tok[VMAX_BUFSIZE];

    if (thee->mode == ASC_DECODE) {
        VJMPERR1( VNULL != asc_getToken(thee, tok, VMAX_BUFSIZE) );
        sscanf(tok,"%c",cval);
    } else if (thee->mode == ASC_ENCODE) {
        sprintf(tok,"%c\n",*cval);
        len = strlen(tok);
        for (i=0; i<len; i++)
            thee->buf[thee->pos+i] = tok[i];
        thee->pos += len;
    }
    return 1;

  VERROR1:
    return 0;
}

/*
 * ***************************************************************************
 * Routine:  asc_int
 *
 * Purpose:  DECODE or ENCODE an int.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPRIVATE int asc_int(ASC *thee, int *ival)
{
    int i, len;
    char tok[VMAX_BUFSIZE];

    if (thee->mode == ASC_DECODE) {
        VJMPERR1( VNULL != asc_getToken(thee, tok, VMAX_BUFSIZE) );
        sscanf(tok,"%d",ival);
    } else if (thee->mode == ASC_ENCODE) {
        sprintf(tok,"%d\n",*ival);
        len = strlen(tok);
        for (i=0; i<len; i++)
            thee->buf[thee->pos+i] = tok[i];
        thee->pos += len;
    }
    return 1;

  VERROR1:
    return 0;
}

/*
 * ***************************************************************************
 * Routine:  asc_float
 *
 * Purpose:  DECODE or ENCODE a float.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPRIVATE int asc_float(ASC *thee, float *fval)
{
    int i, len;
    char tok[VMAX_BUFSIZE];

    if (thee->mode == ASC_DECODE) {
        VJMPERR1( VNULL != asc_getToken(thee, tok, VMAX_BUFSIZE) );
        sscanf(tok,"%e",fval);
    } else if (thee->mode == ASC_ENCODE) {
        sprintf(tok,"%e\n",*fval);
        len = strlen(tok);
        for (i=0; i<len; i++)
            thee->buf[thee->pos+i] = tok[i];
        thee->pos += len;
    }
    return 1;

  VERROR1:
    return 0;
}

/*
 * ***************************************************************************
 * Routine:  asc_double
 *
 * Purpose:  DECODE or ENCODE a double.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPRIVATE int asc_double(ASC *thee, double *dval)
{
    int i, len;
    char tok[VMAX_BUFSIZE];

    if (thee->mode == ASC_DECODE) {
        VJMPERR1( VNULL != asc_getToken(thee, tok, VMAX_BUFSIZE) );
        sscanf(tok,"%le",dval);
    } else if (thee->mode == ASC_ENCODE) {
        sprintf(tok,"%e\n",*dval);
        len = strlen(tok);
        for (i=0; i<len; i++)
            thee->buf[thee->pos+i] = tok[i];
        thee->pos += len;
    }
    return 1;

  VERROR1:
    return 0;
}

/*
 * ***************************************************************************
 * Routine:  asc_setWhiteChars
 *
 * Purpose:  Define the white character set.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPRIVATE void asc_setWhiteChars(ASC *thee, char *whiteChars)
{
    strncpy(thee->whiteChars, whiteChars, VMAX_ARGNUM);
}

/*
 * ***************************************************************************
 * Routine:  asc_setCommChars
 *
 * Purpose:  Define the comment character set.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPRIVATE void asc_setCommChars(ASC *thee, char *commChars)
{
    strncpy(thee->commChars, commChars, VMAX_ARGNUM);
}

/*
 * ***************************************************************************
 * Routine:  asc_getToken
 *
 * Purpose:  Get the next token from the input stream.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPRIVATE char* asc_getToken(ASC *thee, char *tok, int toksize)
{
    /*
     * APBS-KTS: In general all of the tests that check if we are less than
     * thee->size are lame.  It should be checking against the amount of data
     * in the buffer, not the size of the buffer.
     */
    int i, ii, jj, done;
    if (thee->mode == ASC_DECODE) {

        /* first clear the token buffer */
        memset(tok, '\0', toksize);

        /* set "ii" ptr to the first token character */
        ii = thee->pos;
        done = 0;
        while ( !done ) {

            /* if whiteChar then just skip that character */
            if ( strchr(thee->whiteChars,thee->buf[ii]) ) {
                ii++;
                VJMPERR1( ii < thee->size );

            /* if commChar then skip to the next newline and keep going */
            } else if ( strchr(thee->commChars,thee->buf[ii]) ) {
                ii++;
                VJMPERR1( ii < thee->size );
                while ( thee->buf[ii] != '\n' ) {
                    ii++;
                    VJMPERR1( ii < thee->size );
                }

            /* this must be the first token character */
            } else {
                done = 1;
            }
        }

        /* set "jj" ptr to the first character (white or comm) after token */
        jj = ii+1;
        done = 0;
        while ( !done ) {
            /*
             * APBS-KTS: The below line is sort of dumb.  It has the effect of
             * requiring that there be a whiteChar or commChar character after
             * the final token.  But only if the file data just happens to
             * completely fill up thee->buf.  This would be unlikely, but if it
             * did happen, it would be a pain to debug.
             */
            VJMPERR1( jj < thee->size );

            /* if whiteChar then we are done */
            if ( strchr(thee->whiteChars,thee->buf[jj]) ) {
                done = 1;

            /* if commChar then we are done */
            } else if ( strchr(thee->commChars,thee->buf[jj]) ) {
                done = 1;

            /* this must be another token character */
            } else {
                jj++;
            }
        }

        /* error control */
        VJMPERR1( (jj-ii) <= toksize );
        VJMPERR1( jj <= thee->size );

        /* copy the characters between ii and jj to the output string */
        for (i=ii; i<jj; i++)
            tok[i-ii] = thee->buf[i];
/*        tok[jj] = '\0';
 *
 * APBS-KTS: Strictly speaking the above line is a bug.  It's also unnecessary.
 * It only works, and is redundant, because of the memset on tok above.
 * For what it's worth, it should be, assuming you didn't have the memset,
 *
 * tok[jj-ii] = '\0';
 */

        /* update the position pointer */
        thee->pos = jj;

    } else if (thee->mode == ASC_ENCODE) {
        fprintf(stderr,"asc_getToken: Don't know how to ENCODE yet!\n");
    }

    return tok;

  VERROR1:
    /*
     * APBS-KTS: Getting to the end of the buffer without a token should not be
     * a big deal.  It's an EOF condition.  Mark it thus and get on with it.
     *
     * fprintf(stderr,"asc_getToken: Error occurred (bailing out).\n");
     */
    thee->error = EOF;
    return VNULL;
}

/*
 * ***************************************************************************
 * Routine:  readn
 *
 * Purpose:  A fixed-up file-descriptor read (for UNIX/INET).
 *
 * Notes:    Fixes the "short read" problem if the operating system
 *           is interrupted during the read.  Calls the usual
 *           file-descriptor read repeatedly until n characters are
 *           actually read in.  Returns the number of characters
 *           actually read in.  Returns -1 on error.
 *
 *           Includes my WINSOCK fixes (err, rather hacks).
 *
 * Author:   Michael Holst (first of two jewels from Rick Stevens' book)
 * ***************************************************************************
 */
VPRIVATE int readn(int fd, void *vptr, unsigned int n)
{
    char *ptr;
    unsigned int nleft;
    int  nread;

    ptr = vptr;
    nleft = n;
    while (nleft > 0) {
        if ((nread = recv(fd,ptr,nleft,0)) < 0) {
#if defined(HAVE_WINSOCK_H)
            if (WSAGetLastError() == WSAEINTR) {
                nread = 0;
            } else if (WSAGetLastError() == WSAEWOULDBLOCK) {
                nread = 0;
            } else { return(-1); }
#else
            if (errno == EINTR) {
                nread = 0;
            } else if (errno == EWOULDBLOCK) {
                nread = 0;
            } else { return(-1); }
#endif
        } else if (nread == 0) {
            break;
        }
        nleft -= nread;
        ptr += nread;
    }
    return (n-nleft);
}

/*
 * ***************************************************************************
 * Routine:  writen
 *
 * Purpose:  A fixed-up file-descriptor write (for UNIX/INET).
 *
 * Notes:    Fixes the "short write" problem if the operating system
 *           has buffer overflow problems.  Calls the usual
 *           file-descriptor write repeatedly until the input buffer
 *           actually gets written out.  Returns the number of
 *           characters actually written out.  Returns -1 on error.
 *
 * Author:   Michael Holst (second of two jewels from Rick Stevens' book)
 * ***************************************************************************
 */
VPRIVATE int writen(int fd, void *vptr, unsigned int n)
{
    char *ptr;
    unsigned int nleft;
    int  nwritten;

    ptr = vptr;
    nleft = n;
    while (nleft > 0) {
        if ((nwritten = send(fd,ptr,nleft,0)) <= 0) {
            if (errno == EINTR) {
                nwritten = 0;
            } else {
                return(-1);
            }
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return(n);
}

/*
 * ***************************************************************************
 * Routine:  Vio_bufTake
 *
 * Purpose:  Set the pointer to the internal buffer.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vio_bufTake(Vio *thee, char *buf, int bufsize)
{
    /* make sure Vio was started */
    VJMPERR1( VIOstarted );

    /* clear the internal buffer */
    if (thee->VIObuffer != VNULL) {
        free( thee->VIObuffer );
        thee->VIObuffer = VNULL;
    }

    /* now set the buffer */
    thee->VIObuffer    = buf;
    thee->VIObufferLen = bufsize;
    thee->VIObufferPtr = 0;

    /* return without error */
    return;

  VERROR1:
    fprintf(stderr,"Vio_bufTake: Vio library has not been started.\n");
    return;
}

/*
 * ***************************************************************************
 * Routine:  Vio_bufGive
 *
 * Purpose:  Return the pointer to the internal buffer.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC char* Vio_bufGive(Vio *thee)
{
    char *tmp;

    /* make sure Vio was started */
    VJMPERR1( VIOstarted );

    /* grab the pointer */
    tmp = thee->VIObuffer;

    /* reset things for the hand-off */
    thee->VIObufferLen = 0;
    thee->VIObuffer = VNULL;

    /* return without error */
    return tmp;

  VERROR1:
    fprintf(stderr,"Vio_bufGive: Vio library has not been started.\n");
    return VNULL;
}

/*
 * ***************************************************************************
 * Routine:  Vio_bufSize
 *
 * Purpose:  Return the length of the internal buffer.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC int Vio_bufSize(Vio *thee)
{
    /* make sure Vio was started */
    VJMPERR1( VIOstarted );

    /* return without error */
    return thee->VIObufferLen;

  VERROR1:
    fprintf(stderr,"Vio_bufSize: Vio library has not been started.\n");
    return 0;
}

/*
 * ***************************************************************************
 * Routine:  Vio_socketOpen
 *
 * Purpose:  Socket open for read or write.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC Vio *Vio_socketOpen(char *key,
    const char *iodev, const char *iofmt,
    const char *iohost, const char *iofile)
{
    static Vio *sock;

    /* make sure Vio was started */
    VJMPERR1( VIOstarted );

    /* setup for a read */
    if (!strcmp("r",key)) {

        /* Open device for READ */
        if ( VNULL == (sock=Vio_ctor(iodev,iofmt,iohost,iofile,"r")) ) {
            fprintf(stderr,"Vio_socketOpen: Problem opening(read) <%s>\n",
                iofile);
            VJMPERR2( 0 );
        }

        /* START READ (blocking accept) */
        if ( 0 > Vio_accept(sock,0) ) {
            fprintf(stderr,"Vio_socketOpen: Problem accepting(read) <%s>\n",
                iofile);
            /* destroy the socket before we return */
            Vio_dtor( &sock );
            VJMPERR2( 0 );
        }

    /* setup for a write */
    } else if (!strcmp("w",key)) {

        /* Open device for WRITE */
        if ( VNULL == (sock=Vio_ctor(iodev,iofmt,iohost,iofile,"w")) ) {
            fprintf(stderr,"Vio_socketOpen: Problem opening(write) <%s>\n",
                iofile);
            VJMPERR2( 0 );
        }

        /* START WRITE (blocking connect) */
        if ( 0 > Vio_connect(sock,0) ) {
            fprintf(stderr,"Vio_socketOpen: Problem connecting(write) <%s>\n",
                iofile);
            /* destroy the socket before we return */
            Vio_dtor( &sock );
            VJMPERR2( 0 );
        }

    } else {
        fprintf(stderr,"Vio_socketOpen: Internal logic error.\n");
        VJMPERR2( 0 );
    }

    /* some i/o */
#if 0
    fprintf(stderr,"Vio_socketOpen: iodev =<%s>\n", iodev);
    fprintf(stderr,"Vio_socketOpen: iofmt =<%s>\n", iofmt);
    fprintf(stderr,"Vio_socketOpen: iohost=<%s>\n", iohost);
    fprintf(stderr,"Vio_socketOpen: iofile=<%s>\n", iofile);
#endif

    /* return without error */
    return sock;

  VERROR1:
    fprintf(stderr,"Vio_socketOpen: Vio library has not been started.\n");
    return VNULL;

  VERROR2:
    fprintf(stderr,"Vio_socketOpen: bailing out.\n");
    return VNULL;
}

/*
 * ***************************************************************************
 * Routine:  Vio_socketClose
 *
 * Purpose:  Socket close from read or write.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vio_socketClose(Vio **sock)
{
    /* make sure Vio was started */
    VJMPERR1( VIOstarted );

    VJMPERR2( VNULL != *sock );

    /* FINISH READ (release subsocket if we had one) */
    if ((*sock)->rwkey == VIO_R) {
        Vio_acceptFree(*sock);

    /* FINISH WRITE */
    } else if ((*sock)->rwkey == VIO_W) {
        Vio_connectFree(*sock);

    /* Problems... */
    } else {
        VJMPERR2( 0 );
    }

    /* return without error */
    Vio_dtor(sock);
    return;

  VERROR1:
    fprintf(stderr,"Vio_socketClose: Vio library has not been started.\n");
    return;

  VERROR2:
    fprintf(stderr,"Vio_socketClose: bailing out.\n");
    return;
}

