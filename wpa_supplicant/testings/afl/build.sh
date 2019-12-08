#!/bin/bash

WPA_CLI_DIR="${PWD}/../.."
SRC="wpa_cli_afl_wrapper.c"
OBJ=${SRC%.c}.o
CFLAG="-DMAX_ARGC=10 -fprofile-arcs -ftest-coverage "
AFL_INPUT_DIR="afl-in"
AFL_INPUT_DICT_DIR="afl-in-dict"
AFL_INPUT_DICT="${AFL_INPUT_DICT_DIR}/wpa_cli.dict"
AFL_OUTPUT_DIR="afl-out"
AFL_COV_SLLEP_TIME_SEC="15"

build_wpa_cli_without_supplicant() {
    # copy compiler flags from wpa_supplicant/Makefilels
    . ${WPA_CLI_DIR}/.config
    CFLAG_WPACLI=${CFLAG}
    if [ $CONFIG_CTRL_IFACE == y ]
    then
        CFLAG_WPACLI+="-DCONFIG_CTRL_IFACE "
    fi  
    TARGET_FILE="${WPA_CLI_DIR}/wpa_cli.c"
    TARGET_OBJ="${TARGET_FILE%.c}"
    LIBRARY_FILES+="${TARGET_FILE} "
    LIBRARY_FILES+="${WPA_CLI_DIR}/../src/common/cli.c "
    LIBRARY_FILES+="${WPA_CLI_DIR}/../src/utils/edit.c "
    LIBRARY_FILES+="${WPA_CLI_DIR}/../src/utils/common.c "
    LIBRARY_FILES+="${WPA_CLI_DIR}/../src/utils/os_unix.c "
    LIBRARY_FILES+="${WPA_CLI_DIR}/testings/wap_cli_wrapper_common.c "
    INCLUDE_PATH="-I${WPA_CLI_DIR}/../src -I${WPA_CLI_DIR}/../src/utils "
    INCLUDE_PATH+="-I${WPA_CLI_DIR}/testings/include "
    # ?? If build by afl-gcc, encounter a SIGSEGV at __afl_setup_first before entering the main() function
    gcc ${CFLAG_WPACLI} ${INCLUDE_PATH} ${LIBRARY_FILES} -o ${TARGET_OBJ} 
}

if test $1 = "cov"
then
    xdg-open ${AFL_OUTPUT_DIR}/cov/web/index.html
elif test $1 = "clean"
then
    rm -rf afl-out *.o *.gcda *.gcno
    mkdir afl-out
elif test $1 = "run"
then
    if test $2 = "ignore"
    then
        export AFL_I_DONT_CARE_ABOUT_MISSING_CRASHES=1
        export AFL_SKIP_CPUFREQ=1
    fi
    # ?? afl-cov does not count the coverage of wpa_cli during off-line analysis. Maybe using the execv is the problem for off-line analysis?
    gnome-terminal -- bash -c "afl-cov -v -d ${AFL_OUTPUT_DIR} --overwrite --live --ignore-core-pattern --sleep ${AFL_COV_SLLEP_TIME_SEC} --coverage-cmd \"./${OBJ} -f AFL_FILE -a -b -c\" --code-dir ${WPA_CLI_DIR};"
    afl-fuzz -i ${AFL_INPUT_DIR} -o ${AFL_OUTPUT_DIR} -- ${PWD}/${OBJ} @@
elif test $1 = "run-dict"
then
    if test $2 = "ignore"
    then
        export AFL_I_DONT_CARE_ABOUT_MISSING_CRASHES=1
        export AFL_SKIP_CPUFREQ=1
    fi
    # ?? afl-cov does not count the coverage of wpa_cli during off-line analysis. Maybe using the execv is the problem for off-line analysis?
    gnome-terminal -- bash -c "afl-cov -v -d ${AFL_OUTPUT_DIR} --overwrite --live --ignore-core-pattern --sleep ${AFL_COV_SLLEP_TIME_SEC} --coverage-cmd \"./${OBJ} -f AFL_FILE -a -b -c\" --code-dir ${WPA_CLI_DIR};"
    afl-fuzz -i ${AFL_INPUT_DIR} -x ${AFL_INPUT_DICT} -o ${AFL_OUTPUT_DIR}  -- ${PWD}/${OBJ} @@
else
    # Build wrapper with afl-gcc
    afl-gcc $CFLAG $SRC -o $OBJ   
    if test $1 = "standalone"
    then      
        build_wpa_cli_without_supplicant
    else
        # Build wpa_cli with afl-gcc
        echo ${WPA_CLI_DIR}
        cd ${WPA_CLI_DIR}
        make clean
        CC="afl-gcc" make wpa_cli -j8
    fi
fi