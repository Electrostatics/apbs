## ###########################################################################
## File:    Makefile.am
##
## Purpose: Automatically generate Makefile.in for autoconf using automake.
##
## Author:  Nathan Baker, Todd Dolinsky
## ###########################################################################

AUTOMAKE_OPTIONS = no-dependencies
SUFFIXES         = .c .C .o .so
INCLUDES         = @inc_list@ $(PY_CFLAGS) -I${top_srcdir}/bin 
CCLD             = $(PY_CC)
CLEANFILES       = _apbslib.so
SUBDIRS          = vgrid 

noinst_PROGRAMS       = _apbslib.so
_apbslib_so_SOURCES   = apbslib.c
_apbslib_so_LINK      = $(PY_CC) $(CFLAGS) $(PY_LINKFLAGS) -o $@ $(PY_SHARED)
_apbslib_so_LDADD     = ${top_builddir}/bin/.libs/libapbsmainroutines.a ${top_builddir}/src/aaa_lib/.libs/libapbs.a ${top_builddir}/contrib/lib/libmaloc.a ${top_builddir}/contrib/blas/.libs/libapbsblas.a $(PY_LDFLAGS) ${z_lib} 
