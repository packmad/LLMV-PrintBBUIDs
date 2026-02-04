#!/usr/bin/env bash

DOCKER_TAG="llvm_print-bb-uids"
docker build -t "$DOCKER_TAG" .

E_ACSL_ARG=""

# Optional flag handling
if [ "$1" = "--e-acsl" ]; then
  E_ACSL_ARG="--e-acsl"
  shift
fi

if [ "$#" -eq 1 ] && [[ -f $1 ]]; then
  FOLDER=$(cd "$(dirname "$1")" && pwd -P)
  FILE_NAME=$(basename "$1")

  docker run --rm \
    --volume "$FOLDER":/home/opam/input \
    -i "$DOCKER_TAG" \
    $E_ACSL_ARG "$FILE_NAME"
else
  echo "Usage: $(basename "$0") [--e-acsl] MAIN_C_FILE"
fi
