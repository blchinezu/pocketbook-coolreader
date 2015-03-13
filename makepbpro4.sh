#!/bin/sh

mkdir -p pbpro4
cd pbpro4
cmake \
    -D INKVIEW_LIB=$HOME/PBDEV/FRSCSDK/arm-none-linux-gnueabi/sysroot/usr/lib/libinkview.so \
    -D CMAKE_TOOLCHAIN_FILE=../tools/toolchain-arm-gnu-eabi-pocketbook.cmake \
    -D TARGET_TYPE=ARM \
    -D DEVICE_NAME=pb360 \
    -D MAX_IMAGE_SCALE_MUL=2 \
    -D CMAKE_BUILD_TYPE=Release \
    -D ENABLE_CHM=1 \
    -D ENABLE_ANTIWORD=1 \
    -D GUI=CRGUI_PB \
    -D POCKETBOOK_PRO=1 \
    ..
make

cd ..
if [ -f pbpro4/cr3gui/cr3-pb.app ]; then
    echo 'Strip binary'
    ../../FRSCSDK/bin/arm-none-linux-gnueabi-strip pbpro4/cr3gui/cr3-pb.app
fi
echo 'Done'

