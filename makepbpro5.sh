#!/bin/sh

mkdir -p pbpro5
cd pbpro5
cmake \
    -D CMAKE_TOOLCHAIN_FILE=../tools/toolchain-arm-obreey-linux-gnueabi-pocketbook.cmake \
    -D TARGET_TYPE=ARM \
    -D DEVICE_NAME=pb360 \
    -D MAX_IMAGE_SCALE_MUL=2 \
    -D CMAKE_BUILD_TYPE=Release \
    -D ENABLE_CHM=1 \
    -D ENABLE_ANTIWORD=1 \
    -D GUI=CRGUI_PB \
    -D POCKETBOOK_PRO=1 \
    -D POCKETBOOK_PRO_FW5=1 \
    ..
make

cd ..
if [ -f pbpro5/cr3gui/cr3-pb.app ]; then
    echo 'Strip binary'
    ../../SDK_481/bin/arm-obreey-linux-gnueabi-strip pbpro5/cr3gui/cr3-pb.app
fi
echo "Done"

bash updateReleases.sh pro5
