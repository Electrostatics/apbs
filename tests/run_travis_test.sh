#!/bin/bash

# No validation going on here -- it's run by Travis CI.

# The first argument is the directory that contains the apbs executable.
# The second is the test configuration file to use.

echo $1
echo $2
if [ "$TRAVIS_OS_NAME" = "linux" ] && [ "$FETK" = "ON" ]; then
  export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$1/../fetk/lib
fi
PATH=$PATH:$1 python apbs_tester.py -c $2
! grep FAILED test.log
