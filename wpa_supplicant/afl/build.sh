#!/bin/bash

SRC="wpa_cli_afl_wrapper.c"
OBJ=${SRC%.c}.o
CFLAG="-DMAX_ARGC=10 -fprofile-arcs -ftest-coverage"

if test $1 = "run"
then
    if $2 = "ignore"
    then
        export AFL_I_DONT_CARE_ABOUT_MISSING_CRASHES=1
        export AFL_SKIP_CPUFREQ=1
    fi
    # ?? afl-cov does not count the coverage of wpa_cli during off-line analysis. Maybe using the execv is the problem for off-line analysis?
    cp $OBJ ./afl-cov/
    gnome-terminal -- bash -c 'cd afl-cov; afl-cov -v -d ./../afl-out --overwrite --live --ignore-core-pattern --sleep 30 --coverage-cmd "./wpa_cli_afl_wrapper.o -f AFL_FILE -a -b -c" --code-dir ./../../;'
    afl-fuzz -i afl-in -o afl-out -- ${PWD}/${OBJ} @@
else
    # Build wrapper with afl-gcc
    afl-gcc $CFLAG $SRC -o $OBJ

    # Build wpa_cli with afl-gcc
    cd ..
    make clean
    CC="afl-gcc" make wpa_cli -j8
fi