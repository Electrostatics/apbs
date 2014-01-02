#!/bin/sh
#
# run tleap and generate 2ALA prm files
#

export AMBERHOME=/home/rok/src/SandBox/amber9_030906/amber9

cat > leap.input << EOF

pep = sequence { ALA ALA }
saveAmberParm pep prmtop.2ala prmcrd.2ala

savepdb pep 2ala.pdb

quit

EOF

$AMBERHOME/exe/tleap -f leap.input

rm -f leap.input
