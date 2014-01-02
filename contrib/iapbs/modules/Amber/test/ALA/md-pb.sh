#!/bin/sh

export sander=$AMBERHOME/src/sander/sander.APBS

fname=md-pb

cat > $fname <<EOF
Sample PB implicit solvent dynamics
&cntrl
ntx=1, irest=0, imin=0,
ntpr=500, ntwx=500, nscm=100, ntwr=5000,
dt=0.001, nstlim=1000,
temp0=300, tempi=0, ntt=1, tautp=0.1,
igb=10, cut=0, ntb=0,
ntc=2, ntf=2, tol=0.000001
/
&pb
npbverb=0, nsnbr=25, nsnba=5, npbgrid=100,
npopt=0, istrng=0, epsout=80.0, epsin=1.0,
space=1., fillratio=4,
sprob=1.6, radiopt=1,
accept=0.001
/


EOF

$sander -O -i $fname -o ${fname}.out < /dev/null

rm -f $fname

