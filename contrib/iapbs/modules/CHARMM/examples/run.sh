#!/bin/sh

CHARMM=../../../../c35b2/exec/gnu/charmm

tests="apbs_elstat_auto apbs_elstat apbs_maps apbs_maps_read \
apbs_md_auto apbs_md apbs_vis"


for i in $tests
do
    echo ${i}.inp ...
    $CHARMM < ${i}.inp > ${i}.out
done

