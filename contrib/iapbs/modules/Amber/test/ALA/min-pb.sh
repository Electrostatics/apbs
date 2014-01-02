#!/bin/sh

export sander=$AMBERHOME/src/sander/sander.APBS

fname=min-pb

cat > $fname <<EOF
 PB test
 &cntrl
   maxcyc=100, imin=1,
   cut=10.0,
   igb=10, ntb=0,
   ntpr=1,
 ntx=1, irest=0, ntmin=2, ntc=1, ntf=1, 
/
 &pb
 npbverb=1,
 epsout=80.0, epsin=1.0,
 istrng=0, sprob=1.6, radiopt=1,
 space=0.5, nbuffer=0, accept=0.001,
 cutnb=0, dbfopt=1, npopt=2,
/

EOF

$sander -O -i $fname -o ${fname}.out < /dev/null

rm -f $fname