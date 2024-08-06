#!/bin/bash


INPUT_FILE="/input/$1"
INPUT_FILE_BC="/input/$1.bc"
INPUT_FILE_BCI="/input/$1.bci"
INPUT_FILE_OBJ="/input/$1.o"
INPUT_FILE_EXE="/input/$1.exe"


if [ "$#" -eq 1 ] && [[ -f $INPUT_FILE ]]; then
  FOLDER=$(cd $(dirname "$1") && pwd -P)
  
  $LLVM_DIR/bin/clang -O0 -emit-llvm $INPUT_FILE -c -o $INPUT_FILE_BC

  $LLVM_DIR/bin/opt -load-pass-plugin $PASS_DIR/build/libPrintBBUIDs.so --passes="print-bb-uids" $INPUT_FILE_BC -o $INPUT_FILE_BCI
  
  $LLVM_DIR/bin/llc -filetype=obj $INPUT_FILE_BCI -o $INPUT_FILE_OBJ
  
  $LLVM_DIR/bin/clang $INPUT_FILE_OBJ -o $INPUT_FILE_EXE -no-pie

  rm $INPUT_FILE_BC $INPUT_FILE_BCI $INPUT_FILE_OBJ 
else
  echo "Missing input file?!"
fi
