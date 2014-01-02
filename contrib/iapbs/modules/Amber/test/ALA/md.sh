#!/bin/sh

export sander=$AMBERHOME/src/sander/sander.APBS

fname=md
cat > $fname <<EOF
 MD test
 &cntrl
  imin=0, ntx=1, 
  ntt=1, temp0=300.0, tautp=0.2, 
  ntb=0, 
  nstlim=5000,
  ntwe=100, ntwx=100, ntpr=200, 
  cut=20.0, igb=0,
 /

EOF


$sander -O -i $fname -o ${fname}.out < /dev/null
rm -f $fname