#!/bin/sh

export sander=$AMBERHOME/src/sander/sander.APBS

fname=min-apbs

cat > $fname <<EOF
 APBS minim test
 &cntrl
   maxcyc=100, imin=1,
   cut=12.0,
   igb=6, ntb=0,
   ntpr=1,
 /
 &apbs
    apbs_debug = 0, apbs_print=2,
    apbs_upd_limit = 1.d-6,
    dime = 33, 33, 33,
    cglen = 20.0, 20.0, 20.0,
    fglen = 18.0, 18.0, 16.0,
    cmeth=1,
    nion=2,
    ionq  = 1.0, -1.0,
    ionc  = 0.15, 0.15,
    ionrr = 2.0, 2.0,
 &end

EOF

$sander -O -i $fname -o ${fname}.out < /dev/null

rm -f $fname
