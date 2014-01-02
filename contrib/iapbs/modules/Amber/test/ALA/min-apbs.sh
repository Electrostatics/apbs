#!/bin/sh

export sander=$AMBERHOME/src/sander/sander.APBS

fname=min-apbs

cat > $fname <<EOF
 APBS minim test
 &cntrl
   maxcyc=5, imin=1,
   cut=12.0,
   igb=6, ntb=0,
   ntpr=1,
 /
 &apbs
    apbs_debug = 1, apbs_print=0,
    geom_upd_limit = 1.0d-4,
    evdw_upd_limit = 0.01,
    dime = 33, 33, 33,
    cglen = 10.0, 10.0, 10.0,
    fglen = 9.0, 9.0, 9.0,
    radiopt=0
    cmeth=1,
    nion=2,
    ionq  = 1.0, -1.0,
    ionc  = 0.15, 0.15,
    ionrr = 2.0, 2.0,
 &end

EOF

$sander -O -i $fname -o ${fname}.out < /dev/null

rm -f $fname
