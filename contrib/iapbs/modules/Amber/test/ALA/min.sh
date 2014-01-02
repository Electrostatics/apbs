#!/bin/sh

export sander=$AMBERHOME/src/sander/sander.APBS

fname=min
cat > $fname <<EOF
 minimization test
 &cntrl
   maxcyc=1000, imin=1,
   cut=20.0,
   igb=0, ntb=0,
   ntpr=1,
 /

EOF

$sander -O -i $fname -o ${fname}.out < /dev/null
rm -f $fname