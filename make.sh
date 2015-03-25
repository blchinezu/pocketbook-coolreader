#!/bin/sh

if [ "$1" = "" -o "$1" = "360" ]; then
    
    mkdir -p pb360
    cd pb360
    cmake \
        -D CMAKE_TOOLCHAIN_FILE=../tools/toolchain-arm-pocketbook.cmake \
        -D TARGET_TYPE=ARM \
        -D DEVICE_NAME=pb360 \
        -D MAX_IMAGE_SCALE_MUL=2 \
        -D CMAKE_BUILD_TYPE=Release \
        -D ENABLE_CHM=1 \
        -D ENABLE_ANTIWORD=1 \
        -D GUI=CRGUI_PB \
        -D CMAKE_CXX_FLAGS_RELEASE:STRING="-fomit-frame-pointer -O1" \
        ..
    make

    cd ..
    if [ -f pb360/cr3gui/cr3-pb.app ]; then
        echo 'Strip binary'
        ../../PBSDK/bin/arm-linux-strip pb360/cr3gui/cr3-pb.app
    fi
    echo 'Done'

    bash updateReleases.sh 360

fi

if [ "$1" = "" -o "$1" = "pro4" ]; then

    mkdir -p pbpro4
    cd pbpro4
    cmake \
        -D CMAKE_TOOLCHAIN_FILE=../tools/toolchain-arm-gnu-eabi-pocketbook.cmake \
        -D TARGET_TYPE=ARM \
        -D DEVICE_NAME=pb360 \
        -D MAX_IMAGE_SCALE_MUL=2 \
        -D CMAKE_BUILD_TYPE=Release \
        -D ENABLE_CHM=1 \
        -D ENABLE_ANTIWORD=1 \
        -D GUI=CRGUI_PB \
        -D ENABLE_PB_DB_STATE=1 \
        -D BACKGROUND_CACHE_FILE_CREATION=1 \
        -D POCKETBOOK_PRO=1 \
        ..
    make

    cd ..
    if [ -f pbpro4/cr3gui/cr3-pb.app ]; then
        echo 'Strip binary'
        ../../FRSCSDK/bin/arm-none-linux-gnueabi-strip pbpro4/cr3gui/cr3-pb.app
    fi
    echo 'Done'

    bash updateReleases.sh pro4

fi

if [ "$1" = "" -o "$1" = "pro5" ]; then

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
        -D ENABLE_PB_DB_STATE=1 \
        -D BACKGROUND_CACHE_FILE_CREATION=1 \
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

fi
