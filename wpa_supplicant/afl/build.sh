#!/bin/bash

SRC="wpa_cli_afl_wrapper.c"
OBJ=${SRC%.c}.o
CFLAG="-DMAX_ARGC=10 -fprofile-arcs -ftest-coverage "

build_wpa_cli_without_supplicant() {
    # copy compiler flags from wpa_supplicant/Makefilels
    . ./../.config
    CFLAG_WPACLI=${CFLAG}
    if [ $CONFIG_CTRL_IFACE == y ]
    then
        CFLAG_WPACLI+="-DCONFIG_CTRL_IFACE "
    fi  
    CFLAG_WPACLI+="-DTEST_STANDALONE"
    TARGET_FILE="./../wpa_cli.c"
    LIBRARY_FILES=${SRC}
    LIBRARY_FILES+=" ./../../src/common/cli.c "
    LIBRARY_FILES+=" ./../../src/utils/edit.c "
    LIBRARY_FILES+=" ./../../src/utils/common.c "
    LIBRARY_FILES+=" ./../../src/utils/os_unix.c "
    INCLUDE_PATH="-I./../../src -I./../../src/utils"
    # ?? If build by afl-gcc, encounter a SIGSEGV at __afl_setup_first before entering the main() function
    gcc ${CFLAG_WPACLI} ${INCLUDE_PATH} ${LIBRARY_FILES} ${TARGET_FILE} -o ${TARGET_FILE%.c}   
}

if test $1 = "cov"
then
    xdg-open afl-out/cov/web/index.html
elif test $1 = "run"
then
    if test $2 = "ignore"
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
    if test $1 = "standalone"
    then      
        build_wpa_cli_without_supplicant
    else
        # Build wpa_cli with afl-gcc
        cd ..
        make clean
        CC="afl-gcc" make wpa_cli -j8
    fi
fi