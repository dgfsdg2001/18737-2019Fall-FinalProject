#!/bin/bash

#
# clang 
#
CLANG="clang-6.0"
CFLAG="-emit-llvm -c -g -O0 -Xclang -disable-O0-optnone "
INCLUDE_PATH="-I./../../src -I./../../src/utils"
TARGET_FILE="wpa_cli_klee_wrapper.c"
LIBRARY_FILES="./../wpa_cli.c "
LIBRARY_FILES+="./../../src/common/cli.c "
LIBRARY_FILES+="./../../src/utils/edit.c "
LIBRARY_FILES+="./../../src/utils/common.c "
#LIBRARY_FILES+="./get_opt.c "
#LIBRARY_FILES+="./strcmp.c "
#LIBRARY_FILES+="./strchr.c "
#LIBRARY_FILES+="./strdup.c "
#LIBRARY_FILES+="./strcasecmp.c "
#LIBRARY_FILES+="./strlen.c "
#LIBRARY_FILES+="./strncmp.c "
#LIBRARY_FILES+="./strrchr.c "
#LIBRARY_FILES+="./strstr.c "
#LIBRARY_FILES+="./atoi.c "

# copy compiler flags from wpa_supplicant/Makefile
. ./../.config
if [ $CONFIG_CTRL_IFACE == y ]
then
    CFLAG+="-DCONFIG_CTRL_IFACE "
fi
$CLANG $CFLAG $INCLUDE_PATH $LIBRARY_FILES $TARGET_FILE

#
# klee
#
KLEE_FLAG="-only-output-states-covering-new -optimize -libc=uclibc -posix-runtime -readable-posix-inputs -external-calls=all -entry-point=main_wrapper"
KLEE_SYMBOLIC_ENVIRONMENT="-sym-args 1 1 6 -iwlan0"
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

echo $INCLUDE_LIB_BC
klee $INCLUDE_LIB_BC $KLEE_FLAG $TARGET_BC $KLEE_SYMBOLIC_ENVIRONMENT > /dev/null