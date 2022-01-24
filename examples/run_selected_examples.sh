#!/bin/bash

APBS_EXE="apbs"
BASE_PATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
export PATH="$BASE_PATH/../../../bin:$PATH"
cd $BASE_PATH

echo $BASE_PATH
which apbs

run_example() {
  echo # blank line
  run_subdir=$1
  in_file=$2

  curr_dir=`pwd`
  echo "current dir: $curr_dir"
  cd $run_subdir
  if [[ $? -ne 0 ]]; then
    echo "Invalid subdirectory: $run_subdir}"
    ((total_status+=1))
    return 1
  fi
  echo "Running in: $(pwd)"

  echo -n "$APBS_EXE $in_file "
  SECONDS=0
  $APBS_EXE $in_file >> ${curr_dir}/${in_file}.OUTPUT.txt 2>&1
  status=$?
  duration=$SECONDS

  echo "$(($duration / 60)) minutes and $(($duration % 60)) seconds elapsed."
  if [[ "$status" -ne "0" ]]; then
    echo "     failed: $status"
    echo "     printing execution log"
    cat ${curr_dir}/${in_file}.OUTPUT.txt
  fi

  ((total_status+=$status))
  cd $curr_dir
}

total_status=0

# pbam and pbsam examples are currently disabled; pbam/pbsam integration needs further development
#run_example "pbsam-barn_bars" "barn_bars_electro.in"
run_example "bem" "1a63_NanoShaper_SES.in"
run_example "geoflow" "glycerol.in"
run_example "born" "apbs-mol-fem-extmesh.in"
run_example "born" "apbs-mol-fem.in"
run_example "born" "apbs-mol-auto.in"
run_example "FKBP" "1d7h-dmso-mol.in"
run_example "bem-pKa" "2LZT-noASH66.in"
run_example "actin-dimer" "apbs-mol-auto.in"
#run_example "actin-dimer" "apbs-smol-parallel.in"
#run_example "pbam" "toy_electrostatic.in"
#run_example "pbsam-gly/msms" "gly_electrostatic.in"

echo "exit: $total_status"

exit $total_status

# This is a record of which examples run, and which fail + why
# Last updated from commit 91de12f98e1f23ee3ade0b89bc23f915f24ca041

#run_example "pbsam-barn_bars/barn_bars_electro.in" (fails, 13)
#   NOsh_parseELEC: The method ("mg","fem", "pygbe", "bem", "geoflow" "pbam", "pbsam") or "name" must be the first keyword in the ELEC section
#   Error while parsing input file.
#run_example "bem/1a63_NanoShaper_SES.in" (runs)
#run_example "geoflow/glycerol.in" (seg fault, failed, 139)
#   Program received signal SIGSEGV, Segmentation fault.
#   Valist_getNumberAtoms (thee=thee@entry=0x100000014) at /src/src/generic/valist.c:111
#   111	    return thee->number;
#   (gdb) bt
#   #0  Valist_getNumberAtoms (thee=thee@entry=0x100000014) at /src/src/generic/valist.c:111
#   #1  0x000055989f52fdca in runGeometricFlowWrapAPBS (geoflowParams=..., molecules=0x100000014)
#       at /src/src/geoflow/geoflow_wrap_apbs.cpp:78
#   #2  0x000055989f4db593 in solveGeometricFlow (molecules=0x7ffd4f828420, nosh=<optimized out>, pbeparm=0x5598a1721ac0, 
#       apolparm=0x5598a17150f0, parm=0x5598a17150d0) at /src/src/routines.c:5645
#   #3  0x000055989f4b12aa in main (argc=<optimized out>, argv=<optimized out>) at /src/src/main.c:818
#run_example "born/apbs-mol-fem-extmesh.in" (seg fault, fails, 134;VASSERT: ASSERTION FAILURE!  filename /home/runner/work/FETK/FETK/mc/src/gem/gem.c, line 915, ((*thee) != ((void *)0)), 134)
#   Breakpoint 1, Gem_dtor (thee=thee@entry=0x7ffe69194230) at /src/build-docker/_deps/fetk-src/mc/src/gem/gem.c:915
#   915	    VASSERT( (*thee) != VNULL );
#   (gdb) bt
#   #0  Gem_dtor (thee=thee@entry=0x7ffe69194230) at /src/build-docker/_deps/fetk-src/mc/src/gem/gem.c:915
#   #1  0x0000559b6a395777 in killFE (nosh=0x7fc835391010, pbe=<optimized out>, fetk=<optimized out>, gm=<optimized out>)
#       at /src/src/routines.c:4083
#   #2  0x0000559b6a36e9ce in main (argc=<optimized out>, argv=<optimized out>) at /src/src/main.c:967
####
#   Breakpoint 3, main (argc=<optimized out>, argv=<optimized out>) at /src/src/main.c:967
#   967	    killFE(nosh, pbe, fetk, gm);
#   (gdb) p gm
#   $3 = {0x0 <repeats 20 times>}
####
#   Elements of gm are never initialized.  Why?  Is that intended?  Is that a problem?
#   Can we solve the problem by removing the requirement in the gm destructor that the pointer be non-null?
#   It could just only do destruction if it's not null (which it already checks after the assertion).
#   Code is fetk/mc/src/gem/gem.c
#run_example "born/apbs-mol-fem.in" (runs)
#run_example "born/apbs-mol-auto.in" (runs)
#run_example "FKBP/1d7h-dmso-mol.in" (runs)
#run_example "bem-pKa/2LZT-noASH66.in" (runs)
#run_example "actin-dimer/apbs-mol-auto.in" (runs)
#run_example "actin-dimer/apbs-smol-parallel.in" (seg fault, fails, 139; not compiled with MPI)
#run_example "pbam/toy_electrostatic.in" (fails, 13)
#   PBAM: 3dmap keyword has been deprecated! Please use in conjuction with the write keyword.NOsh:  Unrecognized keyword: 3dmap
#   Error while parsing input file.
#run_example "pbsam-gly/msms/gly_electrostatic.in" (fails, 13)
#   NOsh_parseELEC: The method ("mg","fem", "pygbe", "bem", "geoflow" "pbam", "pbsam") or "name" must be the first keyword in the ELEC section
#   Error while parsing input file.
