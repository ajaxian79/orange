#!/usr/bin/env bash

WORKING_DIR=$(dirname "$0")

cd "${WORKING_DIR}";

echo "" > "resources.embed"

for raw in *.png
do
  IMAGE_NAME=`echo "$raw" | sed -E 's/([A-Z_a-z0-9]*)(\.png)/\1/g'`
  echo "Processing $IMAGE_NAME"

  echo "const uint8_t g_${IMAGE_NAME}[] =" > "${IMAGE_NAME}.h"
  echo "{" >> "${IMAGE_NAME}.h"
  cat "$raw" | hexdump -ve '16/1 "0x%02x, " "\n"' | sed '$ s/.$//' | sed 's/, 0x  //g' >> "${IMAGE_NAME}.h"
  echo "};" >> "${IMAGE_NAME}.h"

  cat "${IMAGE_NAME}.h" >> "resources.embed"
  echo "" >> "resources.embed"
  echo "" >> "resources.embed"
  rm "${IMAGE_NAME}.h"

done
