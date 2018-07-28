#!/bin/sh

if [ -z "$CHARMM_EXE" ] ; then
    CHARMM_EXE=../../../../../../../../charmm/exec/gnu/charmm
fi

tests="apbs_elstat_auto apbs_elstat apbs_maps apbs_maps_read \
apbs_md_auto apbs_md apbs_vis"


for i in $tests
do
    echo ${i}.inp ...
    $CHARMM_EXE < ${i}.inp > ${i}.out
done

