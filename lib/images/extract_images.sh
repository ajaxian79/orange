#!/usr/bin/env bash

WORKING_DIR=$(dirname "$0")

cd "${WORKING_DIR}";

for raw in *.embed
do
  cat "$raw" | awk -v RS= '{print > ("raw-" NR ".hex")}'

  for f in *.hex
  do
    IMAGE_NAME=`cat $f | grep const | sed -E 's/(const [a-z8_]* g_)([A-Za-z0-9_]*)(.*)/\2/g'`
    echo "Processing $IMAGE_NAME"
    cat "$f" | sed '/^[c{}].*/d' | sed 's/0x//g' | sed 's/,//g' | xxd -r -p > "${IMAGE_NAME}.png"
    rm "$f"
  done

done
