/* src/aaa_inc/maloccf.h.in.  Generated from configure.ac by autoheader.  */


/*
 * ***************************************************************************
 * MALOC = < Minimal Abstraction Layer for Object-oriented C >
 * Copyright (C) 1994--2000  Michael Holst
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 675 Mass Ave, Cambridge, MA 02139, USA.
 * 
 * rcsid="$Id: configure.ac,v 1.94 2006/07/14 06:23:15 mholst Exp $"
 * ***************************************************************************
 */

/*
 * ***************************************************************************
 * File:     acconfig.h
 *
 * Purpose:  Generates the main configuration header "maloccf.h" for MALOC.
 *
 * Notes:    See the comments at the top of the file "configure.ac" for
 *           an outline of the sequence of steps that turns acconfig.h
 *           into <src/aaa_inc/maloccf.h.in> and then eventually into
 *           <src/aaa_inc/maloccf.h> when you are using GNU autoconf.
 *
 *           This file can also form the basis for a manually-produced
 *           maloccf.h file.  In fact, a correct Win32 maloccf.h file can be
 *           generated simply by removing the two lines containing the
 *           GNU autoconf tags "TOP" and "BOTTOM".
 *
 *           The final autoconf (or manually) generated "maloccf.h" attempts
 *           to produce a correct header file layout for various UNIX-like
 *           and Win32 machines, giving access to several things beyond ISO
 *           C/C++, including BSD Signals, UNIX Domain sockets, INET TCP/IP
 *           sockets, and the WINSOCK implementation of INET TCP/IP sockets.
 *
 *           The MALOC library then provides a portable abstract interface
 *           to UNIX domain sockets, INET sockets, pipes, signals, and other
 *           system-dependent things that one usually wants to get to in a
 *           fairly standard C or C++ scientific software package.  Once
 *           MALOC is built, "maloccf.h" is no longer needed (it is not
 *           included in the set of API headers that are copied into the
 *           specified header install directory.  In other words, none of
 *           the MALOC headers forming the API include the config file
 *           "maloccf.h"; it is only included by the source files.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */

#ifndef _MALOCCF_H_
#define _MALOCCF_H_


/* Does accept() use unsigned int? */
#undef ACCEPT_USES_UINT

/* Does accept() use unsigned long? */
#undef ACCEPT_USES_ULONG

/* Define to 1 if you have the <arpa/inet.h> header file. */
#undef HAVE_ARPA_INET_H

/* Am I running in a Cygwin/Win32 environment? */
#undef HAVE_CYGWIN

/* Do I compile as a debug version? */
#undef HAVE_DEBUG

/* Define to 1 if you have the <dlfcn.h> header file. */
#undef HAVE_DLFCN_H

/* Does EMBED macro for embedding rcsid symbols into binaries work? */
#undef HAVE_EMBED

/* Define to 1 if you have the <fcntl.h> header file. */
#undef HAVE_FCNTL_H

/* Do I have the getcwd routine? */
#undef HAVE_GETCWD

/* Define to 1 if you have the <inttypes.h> header file. */
#undef HAVE_INTTYPES_H

/* Define to 1 if you have the <memory.h> header file. */
#undef HAVE_MEMORY_H

/* Do I have MPI header? */
#undef HAVE_MPI_H

/* Define to 1 if you have the <netdb.h> header file. */
#undef HAVE_NETDB_H

/* Define to 1 if you have the <netinet/in.h> header file. */
#undef HAVE_NETINET_IN_H

/* Do I have the O_NONBLOCK macro? */
#undef HAVE_O_NONBLOCK

/* Do I have history.h header? */
#undef HAVE_READLINE_HISTORY_H

/* Do I have readline.h header? */
#undef HAVE_READLINE_READLINE_H

/* Define to 1 if you have the <rpc/rpc.h> header file. */
#undef HAVE_RPC_RPC_H

/* Define to 1 if you have the <stdint.h> header file. */
#undef HAVE_STDINT_H

/* Define to 1 if you have the <stdlib.h> header file. */
#undef HAVE_STDLIB_H

/* Define to 1 if you have the <strings.h> header file. */
#undef HAVE_STRINGS_H

/* Define to 1 if you have the <string.h> header file. */
#undef HAVE_STRING_H

/* Define to 1 if you have the <sys/socket.h> header file. */
#undef HAVE_SYS_SOCKET_H

/* Define to 1 if you have the <sys/stat.h> header file. */
#undef HAVE_SYS_STAT_H

/* Define to 1 if you have the <sys/times.h> header file. */
#undef HAVE_SYS_TIMES_H

/* Define to 1 if you have the <sys/time.h> header file. */
#undef HAVE_SYS_TIME_H

/* Define to 1 if you have the <sys/types.h> header file. */
#undef HAVE_SYS_TYPES_H

/* Define to 1 if you have the <sys/un.h> header file. */
#undef HAVE_SYS_UN_H

/* Define to 1 if you have <sys/wait.h> that is POSIX.1 compatible. */
#undef HAVE_SYS_WAIT_H

/* Define to 1 if you have the <unistd.h> header file. */
#undef HAVE_UNISTD_H

/* Do I have the XDR datastructure in the RPC package? */
#undef HAVE_XDR

/* Define to 1 if you have the <\> header file. */
#undef HAVE__

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#undef LT_OBJDIR

/* MSDOS Macro for Cygwin and icl - obsolete */
#undef MSDOS

/* Name of package */
#undef PACKAGE

/* Define to the address where bug reports for this package should be sent. */
#undef PACKAGE_BUGREPORT

/* Define to the full name of this package. */
#undef PACKAGE_NAME

/* Define to the full name and version of this package. */
#undef PACKAGE_STRING

/* Define to the one symbol short name of this package. */
#undef PACKAGE_TARNAME

/* Define to the version of this package. */
#undef PACKAGE_VERSION

/* Define to 1 if the `S_IS*' macros in <sys/stat.h> do not work properly. */
#undef STAT_MACROS_BROKEN

/* Define to 1 if you have the ANSI C header files. */
#undef STDC_HEADERS

/* Version number of package */
#undef VERSION

/* WIN32 Macro for Cygwin and icl */
#undef WIN32

/* Define to 1 if `lex' declares `yytext' as a `char *' by default, not a
   `char[]'. */
#undef YYTEXT_POINTER

/* Define to `int' if <sys/types.h> does not define. */
#undef mode_t

/* Define to `int' if <sys/types.h> does not define. */
#undef pid_t

/* Define to `unsigned int' if <sys/types.h> does not define. */
#undef size_t

/* NAB custom Windows configuration */
#define HAVE_WIN32 1
#define HAVE_GETCWD 1
#define HAVE_O_NONBLOCK 1
#define HAVE_MODE_T 1
#define HAVE_DIRECT_H 1
#define HAVE_PROCESS_H 1
#define HAVE_WINSOCK_H 1
#define HAVE_IO_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_FCNTL_H 1
#define HAVE_RPC_H 1

/*
 * ***************************************************************************
 * Handle some additional things manually (Win32, NeXT, etc)
 * ***************************************************************************
 */

/* Win32 configuration (non-CygWin) */
#if !defined(HAVE_CYGWIN)
#   if defined(WIN32) || defined(__WATCOMC__)

        /* Set the main key for specifying WIN32 code */
#       define HAVE_WIN32

        /* Deal with some basic problems with UNIX/WIN32 compatibility */
#       define HAVE_O_NONBLOCK 1
#       define HAVE_MODE_T 1
#       define HAVE_GETCWD 1

        /* WATCOM does STAT macros right; Microsoft does not */
#       if !defined(__WATCOMC__)
#           define STAT_MACROS_BROKEN 1
#       endif

        /* Required headers that exist in both UNIX and WIN32 */
#       define HAVE_SYS_TYPES_H 1
#       define HAVE_SYS_STAT_H 1
#       define HAVE_FCNTL_H 1
#       define HAVE_RPC_H 1

        /* Required headers that exist only in WIN32 (non-CygWin) */
#       define HAVE_DIRECT_H 1
#       define HAVE_PROCESS_H 1
#       define HAVE_WINSOCK_H 1
#       define HAVE_IO_H 1

#   endif
#endif

#if defined(NeXT) || defined(__NeXT__)
#   define HAVE_NEXT
#endif

/*
 * ***************************************************************************
 * Deal with macros we need that are sometimes missing
 * ***************************************************************************
 */

/* Deal with broken stat macros on some platforms */
#if !defined(STAT_MACROS_BROKEN)
#   define VS_ISREG(a) (((a) & S_IFMT) == S_IFREG)
#else
#   define VS_ISREG(a) (0)
#endif

/* Deal a missing macro on some unix platforms (NeXT, etc) */
#if !defined(HAVE_O_NONBLOCK)
#   define VO_NONBLOCK 00004
#else
#   define VO_NONBLOCK O_NONBLOCK
#endif

/*
 * ***************************************************************************
 * Define some RCS tag embedding and debug I/O macros
 * ***************************************************************************
 */

/* Embedded RCS tags ("ident filename" prints module versions in filename) */
#if defined(HAVE_EMBED)
#    define VEMBED(rctag) \
         VPRIVATE const char* rctag; \
         static void* use_rcsid=(0 ? &use_rcsid : (void*)&rcsid);
#else
#    define VEMBED(rctag)
#endif

/* Produce additional debugging I/O */
#if defined(HAVE_DEBUG)
#    define VDEBUGIO(str) fprintf(stderr,str)
#else
#    define VDEBUGIO(str)
#endif

#endif /* _MALOCCF_H_ */

