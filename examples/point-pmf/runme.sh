#!/bin/bash

apbspath=`which apbs`
if test -z "${apbspath}"; then
	echo ""
	echo "Please add the apbs binary to your path..."
	echo ""
	exit
fi

coord='-2.000 -1.00 0.000 1.000 2.000'

echo "One charge is fixed at x = -3.00..."
for i in $coord; do
  # TO TRANSLATE IN THE X DIRECTION:
  echo "ATOM      1  I   ION     1      -3.000   0.000   0.000  1.00  0.00" > mol1.pqr
  echo "ATOM      1  I   ION     1      $i    0.000   0.000  1.00 0.00" > mol2.pqr
  cat mol1.pqr mol2.pqr > complex.pqr
  outfile=OUTPUT_${i}
  apbs apbs.in > ${outfile}
  echo "x = ${i}"
  grep "Global" ${outfile}
done
