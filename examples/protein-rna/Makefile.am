## ###########################################################################
## File:    Makefile.am
##
## Purpose: Automatically generate Makefile.in for autoconf using automake.
##
## Notes:   The final makefile produced by autoconf will generate basic
##                      testing results via make test.
##
##
## Author:  Todd Dolinsky
## ###########################################################################

APBSBIN=${top_builddir}/bin/${fetk_cpu_vendor_os}/apbs
SHELL=/bin/bash
TEMPLATE_IN=template.in
PQRS=model_outBoxB19.pqr model_outNB.pqr model_outNpep.pqr 
CLEANFILES = *.out apbs-*.in io.mc *.dat *.fit *.log 

.PRECIOUS:  apbs-%.in
.PHONY:  postprocess default clean all-out

all-local:  all-out postprocess
all-out: apbs-0.025.out apbs-0.050.out apbs-0.075.out apbs-0.100.out apbs-0.125.out apbs-0.150.out apbs-0.175.out apbs-0.225.out apbs-0.250.out apbs-0.275.out apbs-0.300.out apbs-0.325.out apbs-0.400.out apbs-0.500.out apbs-0.600.out apbs-0.700.out apbs-0.800.out postprocess
apbs-test: 
	@if test -z "${APBS}"; then echo; echo "*** Please set APBS environmental variable (e.g., 'export APBS=/path/to/apbs' or 'setenv APBS /path/to/apbs')"; echo; exit 1; fi
apbs-%.out: apbs-test apbs-%.in $(PQRS)
	${APBS} apbs-$*.in | tee $@
apbs-%.in: $(TEMPLATE_IN) $(PQRS)
	@echo *** Making ionic strength $* M input file:  $@
	cat $(TEMPLATE_IN) | sed -e "s/IONSTR/$*/g" > $@
postprocess:
	bash postprocess.sh

test:
	./test.sh ${APBSBIN} ${IGEN} noocd

testocd:
	./test.sh ${APBSBIN} ${IGEN} ocd

test.serial: test

$(srcdir)/Makefile.in:

Makefile:

