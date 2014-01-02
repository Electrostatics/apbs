#!/bin/sh

namd=$NAMD_BIN

short="apbs-solvation.conf apbs-visualization.conf"
long="apbs-md.conf"

arg=$1;
case $arg in
    all|long)
	tests="$short $long"
	;;
    short)
	tests="$short"
	;;
    *)
	tests="$short"
	;;
esac;

awkdiff=../../../../test/ndiff.awk

export MCSH_HOME=/dev/null

# error for result comparison
ABSERR=1.0e-6

for i in $tests
do
    base=`basename $i .conf`
    echo "Working on $base ..."
    $namd $i > $base.out
    echo -n "Diffing ... "
    tmpfile=`mktemp ./tmp.XXX` || exit 1
    tmpsave=`mktemp ./tmp.XXX` || exit 1
    tmpout=`mktemp ./tmp.XXX` || exit 1
    egrep -v "Info:|WallClock:" save/${base}.out.save > $tmpsave
    egrep -v "Info:|WallClock:" ${base}.out > $tmpout
    awk -f $awkdiff -v ABSERR=${ABSERR} $tmpsave $tmpout > $tmpfile
    if [ -s $tmpfile ] ; then
	mv $tmpfile ${base}.out.diff
	echo "failed, see ${base}.out.diff."
    else
	echo "passed."
	rm $tmpfile

    fi
    rm -f $tmpsave $tmpout
done
