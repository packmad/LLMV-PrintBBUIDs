#!/bin/bash

DOCKER_TAG="llvm_print-bb-uids"

if [ "$#" -eq 1 ] && [[ -f $1 ]]; then
  docker build -t $DOCKER_TAG .
  FOLDER=$(cd $(dirname "$1") && pwd -P)
  FILE_NAME=$(basename "$1")
  docker run --rm --volume "$FOLDER":/input -i $DOCKER_TAG "$FILE_NAME"
else
  echo "Usage: $(basename "$0") MAIN_C_FILE"
fi
