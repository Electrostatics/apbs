#! /bin/bash

# Energy calculation for TM insertion into a membrane
# 
# Written by Michael Grabe (http://www.pitt.edu/~biohome/Dept/Frame/Faculty/grabe.htm) and modified by Nathan Baker

# This script selects a helix, draws the membrane around it by altering the maps from APBS.
# It then carries out born solvation energy calculations.
# It then picks another helix and does the whole set of calculations again.

# Set the paths to the APBS and membrane drawing executable
echo
echo "Here are the binary locations:"
set -o verbose
apbs_bin=apbs
draw_bin=./draw_membrane2
set +o verbose

# Specify settings for the membrane calculation
echo
echo "Here are the problem settings you've chosen:"
set -o verbose
zmem=-20           # lower leaflet height
Lmem=40            # length of the membrane
mdie=2.0           # membrane Dielectric
pdie=10.0          # protein dielectric value
sdie=80.0          # water dielectric value
memv=0.0           # applied membrane potential in KT/e [THIS WILL NOT WORK FOR YOU!]
I=0.1              # value of symmetric salt concentration [M]
R_top=0.0          # membrane exclusion radius in case your protein is a pore
R_bottom=0.0       # membrane exclusion radius in case your protein is a pore
glen_l=300         # Large grid length
glen_m=200         # Medium grid length
glen_s=100         # Small grid length
dime=97            # Number of grid points (Grabe example run with 65; Nathan increased to 97)
set +o verbose

# Generate the real APBS input files
echo
echo "Generating APBS input files..."
cat apbs_solv-TEMPLATE.in | \
	sed -e "s/GLEN_L/${glen_l}/g" | sed -e "s/GLEN_M/${glen_m}/g" | sed "s/GLEN_S/${glen_s}/g" | \
	sed -e "s/DIME/${dime}/g" | \
	sed -e "s/PDIE/${pdie}/g" | sed -e "s/SDIE/${sdie}/g" > apbs_solv.in
cat apbs_dummy-TEMPLATE.in | \
	sed -e "s/GLEN_L/${glen_l}/g" | sed -e "s/GLEN_M/${glen_m}/g" | sed "s/GLEN_S/${glen_s}/g" | \
	sed -e "s/DIME/${dime}/g" | \
	sed -e "s/PDIE/${pdie}/g" | sed -e "s/SDIE/${sdie}/g" > apbs_dummy.in

# Run each of the test calculations
for mytest in membrane-helix-0 membrane-helix-4 membrane-helix-8 membrane-helix-12 membrane-helix-16; do
	echo
	echo "Running ${mytest} test"

	echo "Copied molecule ${mytest}.pqr to start the calculations."
	cp ${mytest}.pqr TM.pqr

	# Generate dummy maps
	echo "Using ${apbs_bin} to generate Poisson-Boltzmann coefficient maps..."
	${apbs_bin} apbs_dummy.in > ${mytest}_dummy.out

	# Add membrane to maps
	echo "Using ${draw_bin} to write a membrane into the coefficient maps..."
	${draw_bin} dielx_L.dx $zmem $Lmem $pdie $memv $I $R_top $R_bottom
	${draw_bin} dielx_M.dx $zmem $Lmem $pdie $memv $I $R_top $R_bottom
	${draw_bin} dielx_S.dx $zmem $Lmem $pdie $memv $I $R_top $R_bottom

	# Use APBS with new maps
	echo "Carrying out APBS calculations with the membrane..."
	${apbs_bin} apbs_solv.in > ${mytest}_solv.out
	grep "Global" ${mytest}_solv.out

	# Clean-up
	echo "Cleaning up..."
	for diel in diel*dx; do
		mv -v ${diel} ${mytest}_${diel}
	done
	for pot in pot*dx; do
		mv -v ${pot} ${mytest}_${pot}
	done
	rm -v kappa*dx
	rm -v charge*dx
	rm -v change*dx
	rm -v TM.pqr

done

echo
grep "Global" *_solv.out
echo "Done!"

