#!/bin/sh

export sander=$AMBERHOME/src/sander/sander.APBS

fname=vis-pb

cat > $fname <<EOF 
Sample PB visualization input
&cntrl
ntx=1, irest=0,
imin=1, ntmin=2, maxcyc=0,
ntpr=1, igb=10, ntb=0,
ntc=1, ntf=1
/
&pb
npbverb=1, istrng=0, epsout=80.0, epsin=1.0,
space=1., accept=0.001,
sprob=1.4, cutnb=9,
phiout=1, phiform=0
/


EOF

$sander -O -i $fname -o ${fname}.out < /dev/null

rm -f $fname
