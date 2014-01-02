#!/bin/sh
#
# run tleap and generate ALA prm files
#

export AMBERHOME=/home/rok/src/SandBox/amber9_030906/amber9

cat > leap.input << EOF

pep = sequence { ALA}
saveAmberParm pep prmtop.ala prmcrd.ala

savepdb ALA ala.pdb

quit

EOF

$AMBERHOME/exe/tleap -f leap.input

rm -f leap.input
