#!/bin/bash

INPUT_FILE="input/$1"
INPUT_FILE_EACSL="input/$1.e-acsl.c"
INPUT_FILE_BC="input/$1.bc"
INPUT_FILE_BCI="input/$1.bci"
INPUT_FILE_OBJ="input/$1.o"
INPUT_FILE_EXE="input/$1.exe"

EACSL_LINKING_FLAGS="-DE_ACSL_SEGMENT_MMODEL -std=c99 -m64 -g -O2 -fno-builtin -fno-merge-constants -Wall -Wno-long-long -Wno-attributes -Wno-nonnull -Wno-undef -Wno-unused -Wno-unused-function -Wno-unused-result -Wno-unused-value -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable -Wno-implicit-function-declaration -Wno-empty-body -I/home/opam/.opam/4.14/share/frama-c/share/e-acsl /home/opam/.opam/4.14/share/frama-c/share/e-acsl/e_acsl_rtl.c /home/opam/.opam/4.14/lib/frama-c-e-acsl/libeacsl-dlmalloc.a -lgmp -lm"

if [ "$#" -eq 1 ] && [[ -f $INPUT_FILE ]]; then
  FOLDER=$(cd $(dirname "$1") && pwd -P)

  . /home/opam/.opam/opam-init/init.sh

  frama-c -keep-unused-functions none -machdep gcc_x86_64 '-cpp-extra-args= -std=c99 -D_DEFAULT_SOURCE -D__NO_CTYPE -D__FC_MACHDEP_X86_64 ' $INPUT_FILE -rte -e-acsl -e-acsl-share=/home/opam/.opam/4.14/share/frama-c/share/e-acsl -then-last -print -ocode $INPUT_FILE_EACSL
  
  $LLVM_DIR/bin/clang -O0 -emit-llvm $INPUT_FILE_EACSL -c -o $INPUT_FILE_BC

  $LLVM_DIR/bin/opt -load-pass-plugin $PASS_DIR/build/libPrintBBUIDs.so --passes="print-bb-uids" $INPUT_FILE_BC -o $INPUT_FILE_BCI
  
  $LLVM_DIR/bin/llc -filetype=obj $INPUT_FILE_BCI -o $INPUT_FILE_OBJ
  
  $LLVM_DIR/bin/clang $EACSL_LINKING_FLAGS $INPUT_FILE_OBJ -o $INPUT_FILE_EXE -no-pie

  rm $INPUT_FILE_BC $INPUT_FILE_BCI $INPUT_FILE_OBJ 
else
  echo "Missing input file?!"
fi