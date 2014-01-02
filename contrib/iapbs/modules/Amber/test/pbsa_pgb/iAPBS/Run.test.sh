#!/bin/sh
#
# Run iAPBS test on pbsa_pgb Amber test
#
# $Id: $

template=iapbs-template.in
in=iapbs.in


if [ -z "$AMBERHOME" ] ; then
    echo "\$AMBERHOME is not defined"
    exit
fi

export MCSH_HOME=/dev/null

sed "s/LINE/ radiopt=2, pqr='pgb-parse.pqr',/" $template > $in
$AMBERHOME/exe/sander.APBS -O -i iapbs.in -c pgb.rst -p prmtop -o iapbs.2.parse.out

sed "s/LINE/ radiopt=3, pqr='pgb-parse.pqr',/" $template > $in
$AMBERHOME/exe/sander.APBS -O -i iapbs.in -c pgb.rst -p prmtop -o iapbs.3.parse.out

sed "s/LINE/ radiopt=2, pqr='pgb-amber.pqr',/" $template > $in
$AMBERHOME/exe/sander.APBS -O -i iapbs.in -c pgb.rst -p prmtop -o iapbs.2.amber.out

sed "s/LINE/ radiopt=3, pqr='pgb-amber.pqr',/" $template > $in
$AMBERHOME/exe/sander.APBS -O -i iapbs.in -c pgb.rst -p prmtop -o iapbs.3.amber.out

grep EPB iapbs.*.out | awk '{print $10}' > results
grep NPOLAR iapbs.*.out | awk '{print $4}' >> results


echo "Diffing ..."
diff out.save/results.save results

/bin/rm -f mdin restrt mdinfo mdcrd

