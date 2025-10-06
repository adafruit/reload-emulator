#!/bin/sh
set -e
TAG=fruitjam
BUILD=build
export CFLAGS="-include $(pwd)/fruitjam_cflags.h -g3 -ggdb"
export CXXFLAGS="-include $(pwd)/fruitjam_cflags.h -g3 -ggdb"
cmake -S platforms/rp2040 -B $BUILD \
    -DCMAKE_BUILD_TYPE=MinSizeRel \
    -DPICO_SDK_PATH=platforms/rp2040/lib/pico-sdk \
    -DPICO_EXTRAS_PATH=platforms/rp2040/lib/pico-extras \
    -DPICOTOOL_FETCH_FROM_GIT_PATH="$(pwd)/picotool" \
    -DBOARD=adafruit_fruit_jam -DPICO_BOARD=adafruit_fruit_jam \
    -DUSE_HSTX=1 \
    ${CMAKE_ARGS} "$@"

if ! [ -e tools/dsk2nib/src/dsk2nib ]; then
    (cd tools/dsk2nib/src && gcc main.c -o dsk2nib)
fi

if ! [ -f src/roms/apple2ee_roms.h ] ; then
    echo "Making ROM header"
    python mkrom.py > src/roms/apple2ee_roms.h
fi

echo "Making nib files"
python mkdisk.py

echo "Main course"
make -C $BUILD -j$(nproc) apple2e
