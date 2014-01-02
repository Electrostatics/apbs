## ###########################################################################
## File:    Makefile.am
##
## Purpose: Automatically generate Makefile.in for autoconf using automake.
##
## Notes:   The final makefile produced by autoconf will generate basic
##			testing results via make test.
##
##
## Author:  Todd Dolinsky
## ###########################################################################

APBS=${top_builddir}/bin/${fetk_cpu_vendor_os}/apbs
CLEANFILES = *-PE* *.out *.log io.mc

test:
	./test.sh ${APBS} ${IGEN} noocd

testocd:
	./test.sh ${APBS} ${IGEN} ocd
	
testtime:
	/usr/bin/time -v ./test.sh ${APBS} ${IGEN} noocd


$(srcdir)/Makefile.in:

Makefile:

