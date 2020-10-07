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
 * rcsid="$Id: vnm.c,v 1.20 2010/08/12 05:40:32 fetk Exp $"
 * ***************************************************************************
 */

/*
 * ***************************************************************************
 * File:     vnm.c
 *
 * Purpose:  Virtual numerical machine.
 *
 *           In particular, this module defines an imaginary ideal 
 *           Virtual Machine for numerical codes, independent of the 
 *           underlying hardware.  All resources are provided abstractly.
 *
 * Notes:    The vnm library provides abstractions only for
 *           ANSI/ISO/Standard C; no extensions are provided for.
 *           In particular, "getenv" and "system", being almost the 
 *           only ISO C facilities for communicating with the underlying
 *           command shell, are the only facilities provided for in
 *           the abstraction layer.  All other facilities are layered
 *           on top of vnm in other libraries.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */

#include "maloc_base.h"
#include "vnm.h"

#if defined(HAVE_UNISTD_H)
#   include <unistd.h> 
#endif

#if defined(HAVE_SYS_TYPES_H)
#   include <sys/types.h> 
#endif

#if defined(HAVE_SYS_TIME_H)
#   include <sys/time.h> 
#endif

#if defined(HAVE_SYS_TIMES_H)
#   include <sys/times.h> 
#endif

#if defined(HAVE_SYS_STAT_H)
#   include <sys/stat.h> 
#endif

#if defined(HAVE_WINSOCK_H)
    VEXTERNC char *getcwd(char *buf, int size);
    VEXTERNC int chdir(const char *path);
    VEXTERNC int mkdir(const char *pathname);
#endif

/* console management */
VPRIVATE FILE *cons[4];
VPRIVATE int consIni[4];
VPRIVATE int consRedirect = 1;

/* timers */
VPRIVATE long before[VTIMERS];

/* My unique name */
VPRIVATE int ioTag = -1;
VPRIVATE int nTags =  0;

/* signal handler and jump data */
VPRIVATE int vSigInt = 0;
VPRIVATE int vJmpOk  = 0;
VPRIVATE jmp_buf vJmpBuf;

/* sorting */
VPRIVATE void Vnm_qsortR(int *u, int left, int right);
VPRIVATE void Vnm_qsortOrdR(int *u, int *ord, int left, int right);
VPRIVATE void Vnm_dqsortR(double *u, int left, int right);
VPRIVATE void Vnm_dqsortOrdR(double *u, int *ord, int left, int right);

/*
 * ***************************************************************************
 * Routine:  Vnm_sigInt, Vnm_sigIntSet, Vnm_sitIntClear
 *           Vnm_jmpOk, Vnm_jmpOkSet, Vnm_jmpOkClear
 *
 * Purpose:  Signal and setjmp handling routines.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */

/*
 * ***************************************************************************
 * Routine:  Vnm_sigInt
 *
 * Purpose:  Return the signal interupt flag.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC int Vnm_sigInt(void)
{
     return vSigInt;
}

/*
 * ***************************************************************************
 * Routine:  Vnm_sigIntSet
 *
 * Purpose:  Set the signal interrupt flag.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vnm_sigIntSet(void)
{
    vSigInt=1;
}

/*
 * ***************************************************************************
 * Routine:  Vnm_sitIntClear
 *
 * Purpose:  Clear the signal interrupt flag.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vnm_sigIntClear(void)
{
    vSigInt=0;
}

/*
 * ***************************************************************************
 * Routine:  Vnm_jmpOk
 *
 * Purpose:  Return the "okay-to-jump" flag.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC int Vnm_jmpOk(void)
{
    return vJmpOk;
}

/*
 * ***************************************************************************
 * Routine:  Vnm_jmpOkSet
 *
 * Purpose:  Set the "okay-to-jump" flag.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vnm_jmpOkSet(void)
{
    vJmpOk=1;
}

/*
 * ***************************************************************************
 * Routine:  Vnm_jmpOkClear
 *
 * Purpose:  Clear the "okay-to-jump" flag.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vnm_jmpOkClear(void)
{
    vJmpOk=0;
}

/*
 * ***************************************************************************
 * Routine:  Vnm_signalInit
 *
 * Purpose:  Initialize the signal handling data structures.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC jmp_buf *Vnm_signalInit(void)
{
    Vnm_sigIntClear();
    Vnm_jmpOkClear();
    Vnm_regHand();
    return &vJmpBuf;
}

/*
 * ***************************************************************************
 * Routine:  Vnm_regHand
 *
 * Purpose:  Register the signal handler with the operating system.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vnm_regHand(void)
{
    VASSERT( signal(SIGINT,&Vnm_sigHand) != SIG_ERR );
}

/*
 * ***************************************************************************
 * Routine:  Vnm_sigHand
 *
 * Purpose:  Handle events such as SIGINT.
 *           We must have first been registered with "Vnm_signalInit".
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vnm_sigHand(int num)
{
#if 0
    Vnm_print(2,"Vnm_sigHand: caught signal=<%d>\n",num);
#endif

    /* re-register the interrupt handler in case it was cleared by default */
    Vnm_regHand();

    /* FIRST: handle SIGINT for killing loops */
    Vnm_sigIntSet();

    /* SECOND: handle SIGINT for returning to input prompt via longJmp */
    if (Vnm_jmpOk()) {
        Vnm_jmpOkClear();
        longjmp(vJmpBuf, 1);
    } else {
        return;
    }
}

/*
 * ***************************************************************************
 * Routine:  Vnm_powsafe
 *
 * Purpose:  A safe VPOW function (avoids division by zero).
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC double Vnm_powsafe(double x, double y)
{
    if ((y < 0.) && (x < VSMALL)) {
        return 1.0;
    } else {
        return VPOW( x, y );
    }
}

/*
 * ***************************************************************************
 * Routine:  Vnm_typeChk
 *
 * Purpose:  Check out the sizes of various datatypes.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vnm_typeChk(void)
{
    Vnm_print(0,"Vnm_typeChk: Asserting the following type sizes:\n");
    Vnm_print(0,"Vnm_typeChk: SizeOf(char)          = %d (1 on 32-bit arch)\n",
        sizeof(char));
    Vnm_print(0,"Vnm_typeChk: SizeOf(short)         = %d (2 on 32-bit arch)\n",
        sizeof(short));
    Vnm_print(0,"Vnm_typeChk: SizeOf(int)           = %d (4 on 32-bit arch)\n",
        sizeof(int));
    Vnm_print(0,"Vnm_typeChk: SizeOf(unsigned char) = %d (1 on 32-bit arch)\n",
        sizeof(unsigned char));
    Vnm_print(0,"Vnm_typeChk: SizeOf(unsigned short)= %d (2 on 32-bit arch)\n",
        sizeof(unsigned short));
    Vnm_print(0,"Vnm_typeChk: SizeOf(unsigned int)  = %d (4 on 32-bit arch)\n",
        sizeof(unsigned int));
    Vnm_print(0,"Vnm_typeChk: SizeOf(float)         = %d (4 on 32-bit arch)\n",
        sizeof(float));
    Vnm_print(0,"Vnm_typeChk: SizeOf(double)        = %d (8 on 32-bit arch)\n",
        sizeof(double));

    Vnm_print(0,"Vnm_typeChk: Noting also the following type sizes:\n");
    Vnm_print(0,"Vnm_typeChk: SizeOf(long)          = %d (4 on 32-bit arch)\n",
        sizeof(long));
    Vnm_print(0,"Vnm_typeChk: SizeOf(unsigned long) = %d (4 on 32-bit arch)\n",
        sizeof(unsigned long));
    Vnm_print(0,"Vnm_typeChk: SizeOf(int*)          = %d (4 on 32-bit arch)\n",
        sizeof(int*));
    Vnm_print(0,"Vnm_typeChk: SizeOf(void*)         = %d (4 on 32-bit arch)\n",
        sizeof(void*));
    Vnm_print(0,"Vnm_typeChk: SizeOf(size_t)        = %d (4 on 32-bit arch)\n",
        sizeof(size_t));

    VASSERT( sizeof(char)              == 1 );
    VASSERT( sizeof(short)             == 2 );
    VASSERT( sizeof(int)               == 4 );
    VASSERT( sizeof(unsigned char)     == 1 );
    VASSERT( sizeof(unsigned short)    == 2 );
    VASSERT( sizeof(unsigned int)      == 4 );
    VASSERT( sizeof(float)             == 4 );
    VASSERT( sizeof(double)            == 8 );

    /* VASSERT( sizeof(long)           == 4 ); */
    /* VASSERT( sizeof(unsigned long)  == 4 ); */
    /* VASSERT( sizeof(int*)           == 4 ); */
    /* VASSERT( sizeof(void*)          == 4 ); */
    /* VASSERT( sizeof(size_t)         == 4 ); */
}

/*
 * ***************************************************************************
 * Routine:  Vnm_epsmac
 *
 * Purpose:  Computes the unit roundoff of the machine in single
 *           precision.  This is defined as the smallest positive machine
 *           number u such that  1.0d0 + u .ne. 1.0d0 (in single precision).
 *
 *           A safe hardcoded machine epsilon as alternative:
 *
 *                double value;
 *                value = 1.0e-9;
 *                return value;
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC double Vnm_epsmac(void)
{
    double u, comp, value;
    u = 1.0;
    while (1) {
        u = u * 0.5;
        comp = 1.0 + u;
        if (comp == 1.0) break;
    }
    value = u*2.0;
    return value;
}

/*
 * ***************************************************************************
 * Routine:  Vnm_gentokens
 *
 * Purpose:  Generate an [argv,argc] pair from a character string "buf"
 *           (assumed NULL-terminated) in which tokens are separated by 
 *           whitespace "white" with possible comments "comment" occuring.
 *           THE INPUT STRING IS MODIFIED HERE!
 *
 * Notes:    Again, the input string "buf" IS MODIFIED; white space characters
 *           (defined in the input string "white") are replaced by the NULL 
 *           character '\0'.  The output "argv" is simply a list of pointers
 *           to the start of the tokens in "buf", which are NULL-terminated 
 *           after we replace the white space with NULLs.
 *
 *           We follow convention and "NULL"-terminate "argv" by setting
 *           the pointer following the last token to "VNULL".  The return
 *           value is "argc", the number of tokens found (not including the
 *           terminating NULL pointer).  For safety you must pass in the
 *           maximal length of argv in the parameter "argvmax".
 *
 *           If we encounter a token which begins with a comment character
 *           (defined in the input string "comment"), then we ignore the 
 *           rest of the tokens in the input buffer "buf".  This is suitable
 *           for parsing shell languages such as sh/ksh/bash which have
 *           comments that start with e.g. "#" and continue until a newline.
 *
 *           We DO NOT use the C library function strtok in this routine.
 *           (There are some bad implementations of strtok around apparently;
 *           the internal state variables maintained by strtok can get very
 *           messed up if you use strtok in multiple places in a code.)
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC int Vnm_gentokens(char *buf, char **argv, 
    const int argvmax, const char *white, const char *comment)
{
    int  i, j, ntok, state, done, bufsize;

    for (i=0; i<argvmax; i++) {
        argv[i] = VNULL;
    }
    bufsize = strlen(buf);
    VJMPERR1( buf[bufsize] == '\0' );
    ntok = 0;
    state = 0;
    done = 0;
    i=0;
    while ((i<bufsize) && (!done)) {
        if (strchr(comment,buf[i])) done = 1;
        else {
            if (!strchr(white,buf[i]) && (state==0)) {
                state = 1;
                argv[ntok] = (buf+i);
                ntok++;
            }
            if (strchr(white,buf[i])) {
                buf[i] = '\0';
                state = 0;
            }
            i++;
        }
    }
    argv[ntok] = VNULL;
    VJMPERR1( ntok < argvmax );
    for (j=i; j<bufsize; j++) {
        buf[j] = '\0';
    }

    /* return with no errors */
    return ntok;

  VERROR1:
    Vnm_print(2,"Vnm_gentokens: problem with buffer management.\n");
    return 0;
}

/*
 * ***************************************************************************
 * Routine:  Vnm_tstart
 *
 * Purpose:  Starts the timer on the particular machine.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vnm_tstart(int timer, const char *name)
{
    VASSERT( (timer>=0) && (timer<VTIMERS) );
    Vnm_print(0, "Vnm_tstart: starting timer %d (%s)..\n", timer, name);
    before[timer] = clock();
}

/*
 * ***************************************************************************
 * Routine:  Vnm_tstop
 *
 * Purpose:  Stops the timer on the particular machine.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vnm_tstop(int timer, const char *name)
{
    long after;
    double cputme;

    VASSERT( (timer>=0) && (timer<VTIMERS) );
    after  = clock();
    cputme = (double)(after-before[timer]) / (double)(CLOCKS_PER_SEC);
    Vnm_print(0, "Vnm_tstop: stopping timer %d (%s).  CPU TIME = %e\n", 
        timer, name, cputme);
}

/*
 * ***************************************************************************
 * Routine:  Vnm_getuser
 *
 * Purpose:  Ask the system for the username.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC char *Vnm_getuser(char *user, int usermax)
{
    char *name = VNULL;
    VASSERT( usermax <= VMAX_ARGLEN );

    name = getenv("USER");
    if (name != VNULL) strncpy(user,name,usermax);
    else strncpy(user,"mcuser",usermax);
    return user;
}

/*
 * ***************************************************************************
 * Routine:  Vnm_getos
 *
 * Purpose:  Ask the system for the operating system name.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC char *Vnm_getos(char *os, int osmax)
{
    char *name = VNULL;
    VASSERT( osmax <= VMAX_ARGLEN );

    name = getenv("OSTYPE");
    if (name != VNULL) strncpy(os,name,osmax);
    else strncpy(os,"UNIX",osmax);
    return os;
}

/*
 * ***************************************************************************
 * Routine:  Vnm_gethost
 *
 * Purpose:  Ask the system for the hostname.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC char *Vnm_gethost(char *host, int hostmax)
{
    int i, j;
    char *name = VNULL;
    VASSERT( hostmax <= VMAX_ARGLEN );

    name = getenv("HOSTNAME");
    if (name != VNULL) {
        strncpy(host,name,hostmax);
    } else {
        name = getenv("HOST");
        if (name != VNULL) {
            strncpy(host,name,hostmax);
        } else {
            strncpy(host,"HOST",hostmax);
        }
    }
    j = (int)strlen(host);
    for (i=0; i<j; i++) {
        if (host[i] == '.') host[i] = '\0';
    }
    return host;
}

/*
 * ***************************************************************************
 * Routine:  Vnm_gethome
 *
 * Purpose:  Ask the system for the home directory.
 *
 * Note:     The following preference order is used to set the home directory:
 *
 *               MCSH_HOME (the user must define this in his environment)
 *               CWD       (always defined as the current working directory)
 *
 *           We consider it an error if we can't return something useful;
 *           therefore we will VASSERT(path!=VNULL) before returning.
 *
 *           We settle on a home directory the first time we are called,
 *           and then we simply return this fixed home directory forever.
 *           In other words, the first call to Vnm_gethome, regardless of
 *           who makes the call, establishes the home directory for everyone
 *           else (as long as everyone goes through Vnm_gethome!).
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC char *Vnm_gethome(char *path, int pathmax)
{
    char *home = VNULL;
    static char vnmHome[VMAX_ARGLEN];
    static int init=0;

    VASSERT( pathmax <= VMAX_ARGLEN );

    if (!init) {
        init = 1;
        home = getenv("MCSH_HOME");
        if (home == VNULL) {
            home = Vnm_getcwd(vnmHome,pathmax);
            VASSERT( home != VNULL );
        } else {
            strncpy(vnmHome,home,pathmax);
        }
    }
    strncpy(path,vnmHome,pathmax);
    home = path;
    return path;
}

/*
 * ***************************************************************************
 * Routine:  Vnm_getcwd
 *
 * Purpose:  Ask the system for the current working directory.
 *
 * Note:     Consider it an error if we can't return something useful;
 *           therefore we will VASSERT(path!=VNULL) before returning.
 *
 *           Note that unlike Vnm_gethome, a call to Vnm_getcwd returns
 *           the current directory, possibly modified from call to call.
 *
 *           I.e., calls to Vnm_chdir can change the current working
 *           directory; Vnm_getcwd returns the current directory, whatever
 *           that me be.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC char *Vnm_getcwd(char *path, int pathmax)
{
    char *cwd = VNULL;
    VASSERT( pathmax <= VMAX_ARGLEN );

#if defined(HAVE_GETCWD)
    cwd = getcwd(path, (unsigned int)pathmax);
#else
    cwd = getwd(path);
#endif

    VASSERT( cwd != VNULL );
    return path;
}

/*
 * ***************************************************************************
 * Routine:  Vnm_chdir
 *
 * Purpose:  Interact with the system to change the working directory.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC int Vnm_chdir(const char *path)
{
    return chdir(path);
}

/*
 * ***************************************************************************
 * Routine:  Vnm_mkdir
 *
 * Purpose:  Interact with the system to make a new directory.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC int Vnm_mkdir(const char *path)
{
#if defined(HAVE_WINSOCK_H)
    return mkdir(path);
#else
    mode_t mode = 0777;
    return mkdir(path,mode);
#endif
}

/*
 * ***************************************************************************
 * Routine:  Vnm_system
 *
 * Purpose:  An improved ANSI-C "system" call.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC int Vnm_system(const char *cmd)
{
    return system(cmd);
}   

/*
 * ***************************************************************************
 * Routine:  Vnm_systemBack
 *
 * Purpose:  A background variant of the ANSI-C "system" call.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC int Vnm_systemBack(const char *cmd)
{
    char cmdbuf[VMAX_BUFSIZE];

#if defined(HAVE_WINSOCK_H)
    strcpy(cmdbuf, "start /B ");
    strcat(cmdbuf, cmd);
#else
    strcpy(cmdbuf, cmd);
    strcat(cmdbuf, " &");
#endif
    return Vnm_system(cmdbuf);
}   

/*
 * ***************************************************************************
 * Routine:  Vnm_systemKill
 *
 * Purpose:  Something like a UNIX "killall" call.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC int Vnm_systemKill(const char *cmd)
{
    char cmdbuf[VMAX_BUFSIZE];

#if defined(HAVE_WINSOCK_H)
    strcpy(cmdbuf, "killall ");
    strcat(cmdbuf, cmd);
    (void)Vnm_system(cmdbuf);
#else
    strcpy(cmdbuf, "killall ");
    strcat(cmdbuf, cmd);
    strcat(cmdbuf, "> /dev/null 2>&1");
    (void)Vnm_system(cmdbuf);
    strcpy(cmdbuf, "killall ./");
    strcat(cmdbuf, cmd);
    strcat(cmdbuf, "> /dev/null 2>&1");
    (void)Vnm_system(cmdbuf);
#endif
    return 0;
}   

/*
 * ***************************************************************************
 * Routine:  Vnm_exec
 *
 * Purpose:  An improved UNIX "exec" call.
 *
 * Notes:    This routine does not return except on error.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC int Vnm_exec(int argc, char **argv)
{
#if !defined(HAVE_WINSOCK_H)
    if (strstr(argv[0], "/")) {
        execv (argv[0],argv);
    } else {
        execvp(argv[0],argv);
    }
#endif
    /* Vnm_system("play sorry.au"); */

    return -1;
}   

/*
 * ***************************************************************************
 * Routine:  Vnm_sleep
 *
 * Purpose:  Implement a sleep function with microsecond resolution.
 *
 * Notes:    This is hacked out of the "sleep_us" example in
 *           Rick Steven's Advance Unix Programming book.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vnm_sleep(int nusecs)
{
#if defined(HAVE_WINSOCK_H)
#else
    struct timeval tval;
    tval.tv_sec  = ((unsigned int)nusecs) / 1000000;
    tval.tv_usec = ((unsigned int)nusecs) % 1000000;
    select(0, VNULL, VNULL, VNULL, &tval);
#endif
}

/*
 * ***************************************************************************
 * Routine:  Vnm_ioTag
 *
 * Purpose:  Return my I/O tag.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC int Vnm_ioTag(void)
{
    return ioTag;
}

/*
 * ***************************************************************************
 * Routine:  Vnm_nTags
 *
 * Purpose:  Return the total number of tags.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC int Vnm_nTags(void)
{
    return nTags;
}

/*
 * ***************************************************************************
 * Routine:  Vnm_setIoTag
 *
 * Purpose:  Set my id.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vnm_setIoTag(int myTag, int numTags)
{
    ioTag = myTag;
    nTags = numTags;
}

/*
 * ***************************************************************************
 * Routine:  Vnm_open
 *
 * Purpose:  Open an I/O console.
 *
 * NOTE:     We MUST NOT use VASSERT (or Vnm_print!) in this routine.
 *
 *           The following codes are used:
 *
 *           unit#      C output unit
 *           -------    -------------
 *
 *           unit==0    garbage   -- Non-interactive i/o; lots of stuff
 *                                   (can be redirected to ${MCSH_HOME/io.mc)
 *
 *           unit==1    stdout    -- standard output (Interactive I/O)
 *
 *           unit==2    stderr    -- standard error (IMPORTANT interactive I/O)
 *
 *           unit==3    history   -- History file ${MCSH_HOME}/hist.mcsh
 *
 *           unit==else /dev/null -- Error...
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC FILE *Vnm_open(const int unit)
{
    static int openIni = VFALSE;
    int i;
    char str[256], fname[256], apnd[256], myhome[VMAX_ARGLEN];
    time_t now;

    if ( !((0<=unit)&&(unit<=3)) )
        fprintf(stderr,"Vnm_open: Bad UNIT <%d> specified.\n", unit);

    /* initialize once */
    if (!openIni) {
        for (i=0; i<4; i++) {
            consIni[i] = VFALSE;
            cons[i]    = VNULL;
        }
        openIni = VTRUE;
    }

    /* open the file unit */
    if (cons[unit] == VNULL) {

        /* get a reasonable home directory for ALL file i/o (dot files, etc) */
        VASSERT( Vnm_gethome(myhome, sizeof(myhome)) );

        /* do we need to append our id to all of the dot files */
        if ((Vnm_ioTag() >= 0) && (Vnm_nTags() > 1)) {
            sprintf(apnd,"_%d",Vnm_ioTag());
        } else {
            apnd[0] = '\0';
        }

        if (unit == 0) {
            if (consRedirect) {
                sprintf(fname,"%s/%s%s",myhome,"io.mc",apnd);
                if (!consIni[unit]) {
                    cons[unit]=fopen( fname, "a" /*"w"*/ );
                } else {
                    cons[unit]=fopen( fname, "a" );
                }
            } else {
                cons[unit] = stderr;
            }
        } else if (unit == 1) {
            cons[unit] = stdout;
        } else if (unit == 2) {
            cons[unit] = stderr;
        } else if (unit == 3) {
            sprintf(fname,"%s/%s%s",myhome,"hist.mcsh",apnd);
            if (!consIni[unit]) {
                cons[unit]=fopen(fname, "a" /*"w"*/); 
            } else {
                cons[unit]=fopen(fname, "a"); 
            }
        } else fprintf(stderr,"Vnm_open: Bad UNIT <%d> specified.\n", unit);

        /* Write some info for the first line in the file (if not stdout). */
        if (!consIni[unit]) {
            if ( cons[unit] != VNULL ) {
                consIni[unit] = VTRUE;
                if ((unit == 0) && (consRedirect)) {
                    fprintf(cons[unit],"####################################"
                        "##########################################\n");
                    fprintf(cons[unit],"# MC-shell I/O capture file.\n");
                    now = time(VNULL);
                    sprintf(str,"# Creation Date and Time:  %s",ctime(&now));
                    fprintf(cons[unit],"%s\n",str);
                    fprintf(cons[unit],"####################################"
                        "##########################################\n");
                } else if (unit == 3) {
                    fprintf(cons[unit],"#! /bin/mcsh\n");
                    fprintf(cons[unit],"####################################"
                        "##########################################\n");
                    fprintf(cons[unit],"# MC-shell history file.\n");
                    now = time(VNULL);
                    sprintf(str,"# Creation Date and Time:  %s",ctime(&now));
                    fprintf(cons[unit],"%s\n",str);
                    fprintf(cons[unit],"####################################"
                        "##########################################\n");
                }
            }
        }
    }
    return cons[unit];
}

/*
 * ***************************************************************************
 * Routine:  Vnm_close
 *
 * Purpose:  Close an I/O console.
 *
 * NOTE:     We MUST NOT use VASSERT (or Vnm_print!) in this routine.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC int Vnm_close(const int unit)
{
    int retcode;

    if ( !((0<=unit)&&(unit<=3)) ) {
        fprintf(stderr,"Vnm_close: Bad UNIT <%d> specified.\n", unit);
    }

    if (  (cons[unit] != VNULL) 
       && (cons[unit] != stdin)
       && (cons[unit] != stdout)
       && (cons[unit] != stderr) ) {
        retcode = fclose(cons[unit]);
    } else {
        retcode = 1;
    }
    cons[unit] = VNULL;
    return retcode;
}

/*
 * ***************************************************************************
 * Routine:  Vnm_flush
 *
 * Purpose:  Attempt to flush the specified i/o stream.
 *
 * NOTE:     We MUST NOT use VASSERT (or Vnm_print!) in this routine.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vnm_flush(const int unit)
{
    if ( !((0<=unit)&&(unit<=3)) ) {
        fprintf(stderr,"Vnm_flush: Bad UNIT <%d> specified.\n", unit);
    }

    if (cons[unit] != VNULL) {
        fflush(cons[unit]);
    }
}

/*
 * ***************************************************************************
 * Routine:  Vnm_redirect
 *
 * Purpose:  Set/unset the redirect flag for UNIT zero.
 *           When redirected, I/O goes to the file: ${MCSH_HOME}/io.mc
 *
 * NOTE:     We MUST NOT use VASSERT (or Vnm_print!) in this routine.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vnm_redirect(const int flag)
{
    if ( !((flag==0)||(flag==1)) ) {
        fprintf(stderr,"Vnm_redirect: Bad FLAG <%d> specified.\n", flag);
    }

    consRedirect = flag;
}

/*
 * ***************************************************************************
 * Routine:  Vnm_print
 *
 * Purpose:  External interface to the console i/o routine.
 *
 * NOTE:     We MUST NOT use VASSERT (or Vnm_print!) in this routine.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vnm_print(const int unit, const char *format, ...)
{
    va_list argList;
    FILE    *fp;

    if ( !((0<=unit)&&(unit<=3)) ) {
        fprintf(stderr,"Vnm_print: Bad UNIT <%d> specified.\n", unit);
    }

    fp = Vnm_open(unit);
    if (fp != VNULL) {
        va_start(argList, format);
        vfprintf(fp, format, argList);
        va_end(argList);
        Vnm_close(unit);
    }
    Vnm_flush(unit);
}

/*
 * ***************************************************************************
 * Routine:  Vnm_tprint
 *
 * Purpose:  Add our ioTag to Vnm_print output.
 *
 * Notes:    For a tag to be added, both of the following conditions
 *           must hold:
 *
 *               Vnm_ioTag() >= 0   (I.e., I must have been given a tag)
 *               Vnm_nTags() >  1   (I must not be the only one given a tag)
 *
 * NOTE:     We MUST NOT use VASSERT (or Vnm_print!) in this routine.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vnm_tprint(const int unit, const char *format, ...)
{
    va_list argList;
    FILE    *fp;

    if ( !((0<=unit)&&(unit<=3)) ) {
        if ((Vnm_ioTag() >= 0) && (Vnm_nTags() > 1)) {
            fprintf(stderr, "[%d] ", Vnm_ioTag());
        }
        fprintf(stderr,"Vnm_tprint: Bad UNIT <%d> specified.\n", unit);
    }

    fp = Vnm_open(unit);
    if (fp != VNULL) {
        if ((Vnm_ioTag() >= 0) && (Vnm_nTags() > 1)) {
            fprintf(fp, "[%d] ", Vnm_ioTag());
        }
        va_start(argList, format);
        vfprintf(fp, format, argList);
        va_end(argList);
        Vnm_close(unit);
    }
    Vnm_flush(unit);
}

/*
 * ***************************************************************************
 * Routine:  Vnm_qsort
 *
 * Purpose:  Front-end to quick sort integer array from [-large] to [+large].
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vnm_qsort(int *u, int size)
{
    int i, itmp;

    /* find largest entry; place on right for qSortRecurse */
    for (i=0; i<size-1; i++) {
        if (u[i] > u[size-1]) {
            itmp = u[size-1];
            u[size-1] = u[i];
            u[i] = itmp;
        }
    }

    /* now call qSortRecurse */
    Vnm_qsortR(u, 0, size-2);
}

/*
 * ***************************************************************************
 * Routine:  Vnm_qsortOrd
 *
 * Purpose:  Front-end to quick sort integer array from [-large] to [+large].
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vnm_qsortOrd(int *u, int *ord, int size)
{
    int i, itmp;

    /* find largest entry; place on right for qSortRecurse */
    for (i=0; i<size-1; i++) {
        if (u[i] > u[size-1]) {
            itmp = u[size-1];
            u[size-1] = u[i];
            u[i] = itmp;
            itmp = ord[size-1];
            ord[size-1] = ord[i];
            ord[i] = itmp;
        }
    }

    /* now call qSortRecurse */
    Vnm_qsortOrdR(u, ord, 0, size-2);
}

/*
 * ***************************************************************************
 * Routine:  Vnm_dqsort
 *
 * Purpose:  Front-end to quick sort integer array from [-large] to [+large].
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vnm_dqsort(double *u, int size)
{
    int i;
    double tmp;

    /* find largest entry; place on right for qSortRecurse */
    for (i=0; i<size-1; i++) {
        if (u[i] > u[size-1]) {
            tmp = u[size-1];
            u[size-1] = u[i];
            u[i] = tmp;
        }
    }

    /* now call qSortRecurse */
    Vnm_dqsortR(u, 0, size-2);
}

/*
 * ***************************************************************************
 * Routine:  Vnm_dqsortOrd
 *
 * Purpose:  Front-end to quick sort integer array from [-large] to [+large].
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPUBLIC void Vnm_dqsortOrd(double *u, int *ord, int size)
{
    int i, itmp;
    double tmp;

    /* find largest entry; place on right for qSortRecurse */
    for (i=0; i<size-1; i++) {
        if (u[i] > u[size-1]) {
            tmp = u[size-1];
            u[size-1] = u[i];
            u[i] = tmp;
            itmp = ord[size-1];
            ord[size-1] = ord[i];
            ord[i] = itmp;
        }
    }

    /* now call qSortRecurse */
    Vnm_dqsortOrdR(u, ord, 0, size-2);
}

/*
 * ***************************************************************************
 * Routine:  Vnm_qsortR
 *
 * Purpose:  RECURSIVE Quick sort a vector from [-large] to [+large].
 *
 * Notes:    Sorts u[left],...,u[right] in nondecreasing order.
 *           IMPORTANT NOTE: it is assumed that u[left] <= u[right+1]
 *           THEREFORE, you must have set u[right+1] to be greater than
 *           or equal to all entries u[0],...,u[right].
 *
 *           pivot=u[left] is arbitrarily chosen as the pivot key.
 *           i,j=used to partition sublist so at all times: 
 *
 *                u[m] <= pivot <= u[n],   m<i, n>j.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPRIVATE void Vnm_qsortR(int *u, int left, int right)
{
    int i, j, pivot, tmp;
    if (left < right) {
        i = left;
        j = right+1;
        pivot = u[left];
        do {
            do { i++; } while (u[i] < pivot);
            do { j--; } while (u[j] > pivot);
            if (i<j) {
                tmp = u[i]; 
                u[i] = u[j]; 
                u[j] = tmp; 
            }
        } while (i<j);
        tmp = u[left]; 
        u[left] = u[j]; 
        u[j] = tmp; 
        Vnm_qsortR(u, left, j-1);
        Vnm_qsortR(u, j+1, right);
    }
}

/*
 * ***************************************************************************
 * Routine:  Vnm_qsortOrdR
 *
 * Purpose:  RECURSIVE Quick sort a vector from [-large] to [+large].
 *
 * Notes:    Sorts u[left],...,u[right] in nondecreasing order.
 *           IMPORTANT NOTE: it is assumed that u[left] <= u[right+1]
 *           THEREFORE, you must have set u[right+1] to be greater than
 *           or equal to all entries u[0],...,u[right].
 *
 *           pivot=u[left] is arbitrarily chosen as the pivot key.
 *           i,j=used to partition sublist so at all times: 
 *
 *                u[m] <= pivot <= u[n],   m<i, n>j.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPRIVATE void Vnm_qsortOrdR(int *u, int *ord, int left, int right)
{
    int i, j, pivot, tmp, itmp;
    if (left < right) {
        i = left;
        j = right+1;
        pivot = u[left];
        do {
            do { i++; } while (u[i] < pivot);
            do { j--; } while (u[j] > pivot);
            if (i<j) {
                tmp = u[i]; 
                u[i] = u[j]; 
                u[j] = tmp; 
                itmp = ord[i]; 
                ord[i] = ord[j]; 
                ord[j] = itmp; 
            }
        } while (i<j);
        tmp = u[left]; 
        u[left] = u[j]; 
        u[j] = tmp; 
        itmp = ord[left]; 
        ord[left] = ord[j]; 
        ord[j] = itmp; 
        Vnm_qsortOrdR(u, ord, left, j-1);
        Vnm_qsortOrdR(u, ord, j+1, right);
    }
}

/*
 * ***************************************************************************
 * Routine:  Vnm_dqsortR
 *
 * Purpose:  RECURSIVE Quick sort a vector from [-large] to [+large].
 *
 * Notes:    Sorts u[left],...,u[right] in nondecreasing order.
 *           IMPORTANT NOTE: it is assumed that u[left] <= u[right+1]
 *           THEREFORE, you must have set u[right+1] to be greater than
 *           or equal to all entries u[0],...,u[right].
 *
 *           pivot=u[left] is arbitrarily chosen as the pivot key.
 *           i,j=used to partition sublist so at all times: 
 *
 *                u[m] <= pivot <= u[n],   m<i, n>j.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPRIVATE void Vnm_dqsortR(double *u, int left, int right)
{
    int i, j;
    double pivot, tmp;
    if (left < right) {
        i = left;
        j = right+1;
        pivot = u[left];
        do {
            do { i++; } while (u[i] < pivot);
            do { j--; } while (u[j] > pivot);
            if (i<j) {
                tmp = u[i]; 
                u[i] = u[j]; 
                u[j] = tmp; 
            }
        } while (i<j);
        tmp = u[left]; 
        u[left] = u[j]; 
        u[j] = tmp; 
        Vnm_dqsortR(u, left, j-1);
        Vnm_dqsortR(u, j+1, right);
    }
}

/*
 * ***************************************************************************
 * Routine:  Vnm_dqsortOrdR
 *
 * Purpose:  RECURSIVE Quick sort a vector from [-large] to [+large].
 *
 * Notes:    Sorts u[left],...,u[right] in nondecreasing order.
 *           IMPORTANT NOTE: it is assumed that u[left] <= u[right+1]
 *           THEREFORE, you must have set u[right+1] to be greater than
 *           or equal to all entries u[0],...,u[right].
 *
 *           pivot=u[left] is arbitrarily chosen as the pivot key.
 *           i,j=used to partition sublist so at all times: 
 *
 *                u[m] <= pivot <= u[n],   m<i, n>j.
 *
 * Author:   Michael Holst
 * ***************************************************************************
 */
VPRIVATE void Vnm_dqsortOrdR(double *u, int *ord, int left, int right)
{
    int i, j, itmp;
    double pivot, tmp;
    if (left < right) {
        i = left;
        j = right+1;
        pivot = u[left];
        do {
            do { i++; } while (u[i] < pivot);
            do { j--; } while (u[j] > pivot);
            if (i<j) {
                tmp = u[i]; 
                u[i] = u[j]; 
                u[j] = tmp; 
                itmp = ord[i]; 
                ord[i] = ord[j]; 
                ord[j] = itmp; 
            }
        } while (i<j);
        tmp = u[left]; 
        u[left] = u[j]; 
        u[j] = tmp; 
        itmp = ord[left]; 
        ord[left] = ord[j]; 
        ord[j] = itmp; 
        Vnm_dqsortOrdR(u, ord, left, j-1);
        Vnm_dqsortOrdR(u, ord, j+1, right);
    }
}

