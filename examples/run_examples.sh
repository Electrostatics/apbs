#!/bin/bash

APBS_EXE="apbs"
BASE_PATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
export PATH="$BASE_PATH/../../../bin:$PATH"
cd $BASE_PATH
EXCLUDE="pygbe"

DIRS=`find . -maxdepth 1 -type d -name "[a-zA-Z]*" | awk -F'/' '{print $NF}' | grep -v $EXCLUDE`

for dir in $DIRS
do
  cd $BASE_PATH
  cd $dir
  for infile in `ls -1 *.in`
  do
    echo -n "$APBS_EXE $dir/$infile "
    SECONDS=0
    #/dev/null 2>&1
    $APBS_EXE $infile > OUTPUT.txt 2>&1
    status=$?
    duration=$SECONDS
    echo "$(($duration / 60)) minutes and $(($duration % 60)) seconds elapsed."
    if [[ "$status" -ne "0" ]]; then
      echo "***** $infile, $status: failed"
    fi
  done
done

