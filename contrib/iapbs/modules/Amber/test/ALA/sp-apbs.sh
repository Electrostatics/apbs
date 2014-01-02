#!/bin/sh

export sander=$AMBERHOME/src/sander/sander.APBS

fname=sp-apbs

cat > $fname <<EOF
 APBS SP test
 &cntrl
   maxcyc=0, imin=1,
   cut=12.0,
   igb=6, ntb=0,
   ntpr=1,
 /
 &apbs
    apbs_debug = 5, apbs_print=1,
    dime = 33, 33, 33,
    cglen = 10.0, 10.0, 10.0,
    fglen = 8.0, 8.0, 8.0,
    sp_apbs = .TRUE.,
 &end

EOF

$sander -O -i $fname -o ${fname}.out < /dev/null

rm -f $fname
