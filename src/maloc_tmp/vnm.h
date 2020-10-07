/**
 *  @file       vnm.h
 *  @ingroup    Vnm
 *  @brief      Header file for an ISO C [V]irtual [N]umerical [M]achine.
 *  @author     Michael Holst
 *  @note       None
 *  @version    $Id: vnm.h,v 1.22 2010/08/12 05:40:36 fetk Exp $
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


#ifndef _VNM_H_
#define _VNM_H_

#include "_maloc_base.h"
#include "maloccf.h"
#include "vnm.h"


/**
 * @ingroup Vnm
 * @brief   Signal and setjmp handling routine. Return the signal interrupt flag.
 * @author  Michael Holst
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) 
 * @return  Signal interrupt flag.
 */
VEXTERNC int Vnm_sigInt(void);

/**
 * @ingroup Vnm
 * @brief   Signal and setjmp handling routine. Set the signal interrupt flag.
 * @author  Michael Holst
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) 
 * @return  None
 */
VEXTERNC void Vnm_sigIntSet(void);

/**
 * @ingroup Vnm
 * @brief   Signal and setjmp handling routine. Clear the signal interrupt flag.    
 * @author  Michael Holst
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) 
 * @return  None
 */
VEXTERNC void Vnm_sigIntClear(void);

/**
 * @ingroup Vnm
 * @brief   Signal and setjmp handling routine. Return the "ok-to-jump" flag.
 * @author  Michael Holst
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) 
 * @return  "ok-to-jump" flag.
 */
VEXTERNC int Vnm_jmpOk(void);

/**
 * @brief   Signal and setjmp handling routine. Set the "okay-to-jump" flag.
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) 
 * @author  Michael Holst
 * @ingroup Vnm
 * @return  None
 */
VEXTERNC void Vnm_jmpOkSet(void);

/**
 * @ingroup Vnm
 * @brief   Signal and setjmp handling routine. Clear the "okay-to-jump" flag.     
 * @author  Michael Holst
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) 
 * @return  None
 */
VEXTERNC void Vnm_jmpOkClear(void);

/**
 * @ingroup Vnm
 * @brief   Initialize the signal handling data structures. 
 * @author  Michael Holst
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) 
 * @return  the signal handling data structures
 */
VEXTERNC jmp_buf *Vnm_signalInit(void);

/**
 * @ingroup Vnm
 * @brief   Register the signal handler with the operating system. 
 * @author  Michael Holst
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) 
 * @return  None
 */
VEXTERNC void Vnm_regHand(void);

/**
 * @ingroup Vnm
 * @brief   Handle events such as SIGINT. 
            We must have first been registered with "Vnm_signalInit".
 * @author  Michael Holst
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) 
 * @return  None
 */
VEXTERNC void Vnm_sigHand(int num);

/**
 * @brief A safe VPOW function (avoids division by zero)
 * @note  Useful constants and functions (timers, epsilon, token generators, i/o) 
 */
#define VPOW_SAFE(x,y) (Vnm_powsafe(x,y))

/**
 * @ingroup Vnm
 * @brief   A safe VPOW function (avoids division by zero).  
 * @author  Michael Holst
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) 
 * @return  output value of a VPOW function 
 * @param   x input parameter
 * @param   y input parameter
 */
VEXTERNC double Vnm_powsafe(double x, double y);

/**
 * @ingroup Vnm
 * @brief   Check out the sizes of various datatypes.  
 * @author  Michael Holst
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) 
 * @return  None
 */
VEXTERNC void Vnm_typeChk(void);

/**
 * @ingroup Vnm
 * @brief   Computes the unit roundoff of the machine in single  
            precision.  This is defined as the smallest positive machine
            number u such that  1.0d0 + u .ne. 1.0d0 (in single precision). \n\n
            A safe hardcoded machine epsilon as alternative:\n
            double value; \n
            value = 1.0e-9; \n
            return value; 
 * @author  Michael Holst
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) 
 * @return  the unit roundoff of the machine in single precision.
 */
VEXTERNC double Vnm_epsmac(void);

/**
 * @ingroup Vnm
 * @brief   Generate an [argv,argc] pair from a character string "buf" 
            (assumed NULL-terminated) in which tokens are separated by
            whitespace "white" with possible comments "comment" occuring.
            THE INPUT STRING IS MODIFIED HERE!
 * @author  Michael Holst
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o)\n\n
            Again, the input string "buf" IS MODIFIED; white space characters
            (defined in the input string "white") are replaced by the NULL 
            character '\\0'.  The output "argv" is simply a list of pointers  
            to the start of the tokens in "buf", which are NULL-terminated  
            after we replace the white space with NULLs. \n\n
            We follow convention and "NULL"-terminate "argv" by setting 
            the pointer following the last token to "VNULL".  The return 
            value is "argc", the number of tokens found (not including the 
            terminating NULL pointer).  For safety you must pass in the
            maximal length of argv in the parameter "argvmax".\n\n
            If we encounter a token which begins with a comment character 
            (defined in the input string "comment"), then we ignore the
            rest of the tokens in the input buffer "buf".  This is suitable   
            for parsing shell languages such as sh/ksh/bash which have
            comments that start with e.g. "#" and continue until a newline.\n\n
            We DO NOT use the C library function strtok in this routine.
            (There are some bad implementations of strtok around apparently;
            the internal state variables maintained by strtok can get very  
            messed up if you use strtok in multiple places in a code.)
 * @return  number of tokens
 * @param   buf     buffer containing message
 * @param   argv    the command line arguments
 * @param   argvmax maximal number of the command line arguments
 * @param   white   Pointer to the input string
 * @param   comment token which begins with a comment character
 */
VEXTERNC int Vnm_gentokens(char *buf, char **argv, 
    const int argvmax, const char *white, const char *comment);

/**
 * @brief the maiximal timer constant
 * @note  Useful constants and functions (timers, epsilon, token generators, i/o) 
 */
#define VTIMERS 100

/**
 * @ingroup Vnm
 * @brief   Starts the timer on the particular machine.  
 * @author  Michael Holst
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) 
 * @return  None
 * @param   timer index for the starting timer
 * @param   name  Pointer to the object
 */
VEXTERNC void Vnm_tstart(int timer, const char *name);

/**
 * @ingroup Vnm
 * @brief   Stops the timer on the particular machine.
 * @author  Michael Holst
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) 
 * @return  None
 * @param   timer index for the starting timer
 * @param   name  Pointer to the object
 */
VEXTERNC void Vnm_tstop(int timer, const char *name);

/**
 * @ingroup Vnm
 * @brief   Ask the system for the username.
 * @author  Michael Holst
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) 
 * @return  the username of the system
 * @param   user     Pointer to the username of the system
 * @param   usermax  index for maximal size of user name 
 */
VEXTERNC char *Vnm_getuser(char *user, int usermax);

/**
 * @author  Michael Holst
 * @brief   Ask the system for the operating system name.
 * @ingroup Vnm
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) 
 * @return  the operating system name
 * @param   os     Pointer to the OS type
 * @param   osmax  index for maximal size of OS name
 */
VEXTERNC char *Vnm_getos(char *os, int osmax);

/**
 * @ingroup Vnm
 * @brief   Ask the system for the hostname.
 * @author  Michael Holst
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) 
 * @return  the hostname
 * @param   host     Pointer to the hostname.
 * @param   hostmax  index for maximal size of host name
 */
VEXTERNC char *Vnm_gethost(char *host, int hostmax);

/**
 * @ingroup Vnm=
 * @brief   Ask the system for the home directory.
 * @author  Michael Holst
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o)\n\n
            The following preference order is used to set the home directory: \n\n
            MCSH_HOME (the user must define this in his environment) \n
            CWD       (always defined as the current working directory) \n\n
            We consider it an error if we can't return something useful;  
            therefore we will VASSERT(path!=VNULL) before returning.\n\n
            We settle on a home directory the first time we are called,
            and then we simply return this fixed home directory forever.  
            In other words, the first call to Vnm_gethome, regardless of   
            who makes the call, establishes the home directory for everyone 
            else (as long as everyone goes through Vnm_gethome!).
 * @return  the home directory
 * @param   path     Pointer to the path
 * @param   pathmax  index for the size of path
 */
VEXTERNC char *Vnm_gethome(char *path, int pathmax);

/**
 * @ingroup Vnm
 * @brief   Ask the system for the current working directory.
 * @author  Michael Holst
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o)\n\n
            Consider it an error if we can't return something useful; 
            therefore we will VASSERT(path!=VNULL) before returning. \n\n
            Note that unlike Vnm_gethome, a call to Vnm_getcwd returns
            the current directory, possibly modified from call to call.\n\n
            I.e., calls to Vnm_chdir can change the current working  
            directory; Vnm_getcwd returns the current directory, whatever 
            that might be.
 * @return  the current working directory
 * @param   path     Pointer to the path
 * @param   pathmax  index for the size of path
 */
VEXTERNC char *Vnm_getcwd(char *path, int pathmax);

/**
 * @ingroup Vnm
 * @brief   Interact with the system to change the working directory. 
 * @author  Michael Holst
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) 
 * @return  Success enumeration
 * @param   path Pointer to the path
 */
VEXTERNC int Vnm_chdir(const char *path);

/**
 * @ingroup Vnm
 * @brief   Interact with the system to make a new directory.  
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) 
 * @author  Michael Holst
 * @return  Success enumeration
 * @param   path Pointer to the path
 */
VEXTERNC int Vnm_mkdir(const char *path);

/**
 * @ingroup Vnm
 * @brief   An improved ANSI-C "system" call.
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) 
 * @author  Michael Holst
 * @return  Success enumeration
 * @param   cmd Pointer to the command
 */
VEXTERNC int Vnm_system(const char *cmd);

/**
 * @ingroup Vnm
 * @brief   A background variant of the ANSI-C "system" call.  
 * @author  Michael Holst
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) 
 * @return  Success enumeration
 * @param   cmd Pointer to the command
 */
VEXTERNC int Vnm_systemBack(const char *cmd);

/**
 * @ingroup Vnm
 * @brief   Something like a UNIX "killall" call.
 * @author  Michael Holst
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) 
 * @return  Success enumeration
 * @param   cmd Pointer to the command
 */
VEXTERNC int Vnm_systemKill(const char *cmd);

/**
 * @ingroup Vnm
 * @brief   An improved UNIX "exec" call. 
            This routine does not return except on error.
 * @author  Michael Holst
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) 
 * @return  no return except on error
 * @param   argc number of the command line arguments
 * @param   argv the command line arguments
 */
VEXTERNC int Vnm_exec(int argc, char **argv);

/**
 * @ingroup Vnm
 * @brief   Implement a sleep function with microsecond resolution. 
 * @author  Michael Holst
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) \n
            This is hacked out of the "sleep_us" example in
            Rick Steven's Advance Unix Programming book. 
 * @return  None
 * @param   nusecs number of microseconds
 */
VEXTERNC void Vnm_sleep(int nusecs);

/**
 * @ingroup Vnm
 * @brief   Return my I/O tag.
 * @author  Michael Holst
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) 
 * @return  my I/O tag.
 */
VEXTERNC int Vnm_ioTag(void);

/**
 * @ingroup Vnm
 * @brief   Return the total number of tags.
 * @author  Michael Holst
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) 
 * @return  total number of tags.
 */
VEXTERNC int Vnm_nTags(void);

/**
 * @ingroup Vnm
 * @brief   Set my id.
 * @author  Michael Holst
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) 
 * @return  None
 * @param   myTag   index for the tag
 * @param   numTags number of tags
 */
VEXTERNC void Vnm_setIoTag(int myTag, int numTags);

/**
 * @ingroup Vnm
 * @brief   Open an I/O console.
 * @author  Michael Holst
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) 
 * @verbatim
   We MUST NOT use VASSERT (or Vnm_print!) in this routine.   

   The following codes are used:                                               

   unit#      C output unit      
   -------    -------------                                                

   unit==0    garbage   -- Non-interactive i/o; lots of stuff                        
                           (can be redirected to ${MCSH_HOME/io.mc)
   
   unit==1    stdout    -- standard output (Interactive I/O)            

   unit==2    stderr    -- standard error (IMPORTANT interactive I/O)          

   unit==3    history   -- History file ${MCSH_HOME}/hist.mcsh                       

   unit==else /dev/null -- Error...                                
   @endverbatim                 
 * @return  None
 * @param   unit index for the file unit
 */
VEXTERNC FILE *Vnm_open(const int unit);

/**
 * @ingroup Vnm
 * @brief   Close an I/O console. 
            We MUST NOT use VASSERT (or Vnm_print!) in this routine. 
 * @author  Michael Holst
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) 
 * @return  Success enumeration
 * @param   unit index for the file unit
 */
VEXTERNC int Vnm_close(const int unit);

/**
 * @ingroup Vnm
 * @brief   Attempt to flush the specified i/o stream.
            We MUST NOT use VASSERT (or Vnm_print!) in this routine. 
 * @author  Michael Holst
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) 
 * @return  None
 * @param   unit index for the file unit
 */
VEXTERNC void Vnm_flush(const int unit);

/**
 * @ingroup Vnm
 * @brief   Set/unset the redirect flag for UNIT zero. 
            When redirected, I/O goes to the file: ${MCSH_HOME}/io.mc.
            We MUST NOT use VASSERT (or Vnm_print!) in this routine. 
 * @author  Michael Holst
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) 
 * @return  None
 * @param   flag index for the redirect flag
 */
VEXTERNC void Vnm_redirect(const int flag);

/**
 * @ingroup Vnm
 * @brief   External interface to the console i/o routine.  
            We MUST NOT use VASSERT (or Vnm_print!) in this routine. 
 * @author  Michael Holst
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) 
 * @return  None
 * @param   unit   index for the file unit
 * @param   format Pointer to the print format
 */
VEXTERNC void Vnm_print(const int unit, const char *format, ...);

/**
 * @ingroup Vnm
 * @brief   Add our ioTag to Vnm_print output. 
            We MUST NOT use VASSERT (or Vnm_print!) in this routine. 
 * @author  Michael Holst
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) 
            For a tag to be added, both of the following conditions must hold:\n
            Vnm_ioTag() >= 0   (I.e., I must have been given a tag) \n
            Vnm_nTags() >  1   (I must not be the only one given a tag) \n
 * @return  None
 * @param   unit   index for the file unit
 * @param   format Pointer to the print format
 */
VEXTERNC void Vnm_tprint(const int unit, const char *format, ...);

/**
 * @ingroup Vnm
 * @brief   Front-end to quick sort integer array from [-large] to [+large].  
 * @author  Michael Holst
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) 
 * @return  None
 * @param   u     Pointer to quick sort integer array
 * @param   size  size of the integer array
 */
VEXTERNC void Vnm_qsort(int *u, int size);

/**
 * @ingroup Vnm
 * @brief   Front-end to quick sort integer array from [-large] to [+large]. 
 * @author  Michael Holst
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) 
 * @return  None * @author  Michael Holst
 * @param   u     Pointer to quick sort integer array
 * @param   ord   Pointer to reordered array
 * @param   size  size of the integer array
 */
VEXTERNC void Vnm_qsortOrd(int *u, int *ord, int size);

/**
 * @ingroup Vnm
 * @brief   Front-end to quick sort integer array from [-large] to [+large].   
 * @author  Michael Holst
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) 
 * @return  None
 * @param   u     Pointer to quick sort integer array
 * @param   size  size of the integer array
 */
VEXTERNC void Vnm_dqsort(double *u, int size);

/**
 * @ingroup Vnm
 * @brief   Front-end to quick sort integer array from [-large] to [+large].
 * @author  Michael Holst
 * @note    Useful constants and functions (timers, epsilon, token generators, i/o) 
 * @return  None
 * @param   u     Pointer to quick sort integer array
 * @param   ord   Pointer to reordered array
 * @param   size  size of the integer array
 */
VEXTERNC void Vnm_dqsortOrd(double *u, int *ord, int size);

#endif /* _VNM_H_ */

