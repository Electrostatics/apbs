#!/bin/bash

rm -f conc-energykJ.dat
rm -f lnconc-energyRT.dat
rm -f log10conc-energykcal.dat

for outfile in apbs-*.out; do
	stem=${outfile%.out}
	ionstr=${stem#apbs-}
	energy_kJ=`cat ${outfile} | grep Global | awk '{print $6}'`
	echo ${ionstr} ${energy_kJ} >> conc-energykJ.dat
	echo ${ionstr} ${energy_kJ} | awk '{ printf("%1.12E %1.12E\n", log($1), $2/2.494) }' >> lnconc-energyRT.dat
	echo ${ionstr} ${energy_kJ} | awk '{ printf("%1.12E %1.12E\n", log($1)/2.303, $2/4.184) }' >> log10conc-energykcal.dat
done

cat lnconc-energyRT.dat | python fit.py > lnconc-energyRT.fit
n=`grep "slope:" lnconc-energyRT.fit | awk '{print $2}'`
dn=`grep "slope error:" lnconc-energyRT.fit | awk '{print $3}'`
echo "n = -(${n} +/- ${dn})"
cat log10conc-energykcal.dat | python fit.py > log10conc-energykcal.fit
dGdl=`grep "slope:" log10conc-energykcal.fit | awk '{print $2}'`
ddGdl=`grep "slope error:" log10conc-energykcal.fit | awk '{print $3}'`
echo "dG/d(log10 [KCl]) = ${dGdl} +/- (${ddGdl})"
