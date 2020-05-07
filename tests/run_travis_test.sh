#!/bin/bash

# No validation going on here -- it's run by Travis CI.

# The first argument is the directory that contains the apbs executable.
# The second is the test configuration file to use.

echo $1
echo $2
#if [ "$TRAVIS_OS_NAME" = "linux" ] && [ "$FETK" = "ON" ]; then
  export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$1/../fetk/lib
#fi

PATH=$PATH:$1 python3 apbs_tester.py -t $2
let result=$?

if [ "$result" -eq 0 ]; then
    ! grep FAILED test.log
    exit $?
fi

exit $result
