#!/bin/bash

#
# clang 
#
CLANG="clang-6.0"
CFLAG="-emit-llvm -c -g -O0 -Xclang -disable-O0-optnone  "
CFLAG_COV="-g -fprofile-arcs -ftest-coverage -O0 -Xclang -disable-O0-optnone " # remove -c to build a whole binary

WPA_CLI_DIR="${PWD}/../.."
TARGET_FILE="wpa_cli_klee_wrapper.c"
LIBRARY_FILES="${WPA_CLI_DIR}/wpa_cli.c "
LIBRARY_FILES+="${WPA_CLI_DIR}/../src/common/cli.c "
LIBRARY_FILES+="${WPA_CLI_DIR}/../src/utils/edit.c "
LIBRARY_FILES+="${WPA_CLI_DIR}/../src/utils/common.c "
LIBRARY_FILES+="${WPA_CLI_DIR}/../src/utils/os_unix.c "
LIBRARY_FILES+="${WPA_CLI_DIR}/testings/wap_cli_wrapper_common.c "
INCLUDE_PATH="-I${WPA_CLI_DIR}/../src -I${WPA_CLI_DIR}/../src/utils "
INCLUDE_PATH+="-I${WPA_CLI_DIR}/testings/include "

# copy compiler flags from wpa_supplicant/Makefilels
. ${WPA_CLI_DIR}/.config
if [ $CONFIG_CTRL_IFACE == y ]
then
    CFLAG+="-DCONFIG_CTRL_IFACE "
    CFLAG_COV+="-DCONFIG_CTRL_IFACE "
fi

if test $1 = "cov"
then
    # Remove old gocv files
    rm -f *.gcno *.gcda

    # Build standalone wpa_cli without commuication between wpa_supplicant
    $CLANG $CFLAG_COV $INCLUDE_PATH $LIBRARY_FILES $TARGET_FILE -o ${TARGET_FILE%.c}.o

    # Replay test case and run lcov
    klee-replay ./${TARGET_FILE%.c}.o ./klee-last/*.ktest
    lcov --directory . --gcov-tool ./llvm-gcov-6.0.sh --capture -o cov.info
    genhtml cov.info -o output
    xdg-open output/index.html
else
    $CLANG $CFLAG $INCLUDE_PATH $LIBRARY_FILES $TARGET_FILE
    KLEE_FLAG="-only-output-states-covering-new -optimize -libc=uclibc -posix-runtime -readable-posix-inputs -external-calls=all -entry-point=main_wrapper"
    KLEE_SYMBOLIC_ENVIRONMENT="-sym-args 1 2 6 -iwlan0"
    TARGET_BC=${TARGET_FILE%.c}.bc
    INCLUDE_LIB_BC=""
    read -a lib_file_array <<< "$LIBRARY_FILES"
    for (( n=0; n < ${#lib_file_array[*]}; n++ ))
    do
        file="${lib_file_array[n]}"
        file="${file##*/}"
        INCLUDE_LIB_BC+=" -link-llvm-lib="
        INCLUDE_LIB_BC+=${file%.c}.bc 
    done
    klee $INCLUDE_LIB_BC $KLEE_FLAG $TARGET_BC $KLEE_SYMBOLIC_ENVIRONMENT > /dev/null
fi