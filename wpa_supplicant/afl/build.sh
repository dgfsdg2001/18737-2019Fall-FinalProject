#!/bin/bash

if test $1 = "wpa_cli"
then
    cd ..
    make clean
    CC="afl-gcc" make wpa_cli -j8
else 
    SRC="wpa_cli_afl_wrapper.c"
    OBJ=${SRC%.c}.o
    CFLAG="-DMAX_ARGC=10"
    afl-gcc $CFLAG $SRC -o $OBJ
    if test $1 = "ignore"
    then
        export AFL_I_DONT_CARE_ABOUT_MISSING_CRASHES=1
        export AFL_SKIP_CPUFREQ=1
    fi
    afl-fuzz -i afl-in -o afl-out -- ${PWD}/${OBJ} @@
fi
