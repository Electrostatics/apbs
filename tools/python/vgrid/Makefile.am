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
CLEANFILES       = _vgrid.so

if PY_PRESENT
   noinst_PROGRAMS     = _vgrid.so
   _vgrid_so_SOURCES   = vgridlib.c
   _vgrid_so_LINK      = $(PY_CC) $(CFLAGS) $(PY_LINKFLAGS) -o $@ $(PY_SHARED)
   _vgrid_so_LDADD     = ${top_builddir}/bin/.libs/libapbsmainroutines.a ${top_builddir}/src/aaa_lib/.libs/libapbs.a $(PY_LDFLAGS) ${top_builddir}/contrib/lib/libmaloc.a @lib_list@
endif
