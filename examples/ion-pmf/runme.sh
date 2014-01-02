#!/bin/bash

apbspath=`which apbs`
if test -z "${apbspath}"; then
	echo ""
	echo "Please make sure the apbs binary is in your path"
fi

#coord='-3.000 -2.750 -2.500 -2.250 -2.000 -1.750 -1.50 -1.250 -1.00 -0.750 -0.500 -0.250 0.000 0.250 0.500 0.750 1.000 1.250 1.500 1.750 2.000 2.250 2.500 2.750 3.000'
coord='-3.000 -2.500 -2.000 -1.50 -1.00 -0.500 0.000 0.500 1.000 1.500 2.000 2.500 3.000 3.500 4.000'

for i in $coord; do
	# TO TRANSLATE IN THE X DIRECTION:
	echo "ATOM      1  ION   ION     1      -3.000   0.000   0.000  1.00 2.00" > complex.pdb
	echo "ATOM      1  ION   ION     1      $i    0.000   0.000  1.00 2.00" >> complex.pdb
	outfile=OUTPUT_${i}
	apbs apbs.in > ${outfile} 2>&1
	echo "x = ${i}"
	grep "^  sasa " ${outfile}
	grep "^  qf" ${outfile}
	grep "^  db" ${outfile}
	grep "^  ib" ${outfile}
	grep "^  Global net ELEC energy" ${outfile}
	grep "^  Global net APOL energy" ${outfile}
done
