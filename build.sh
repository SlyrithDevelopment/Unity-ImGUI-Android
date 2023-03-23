#!/bin/bash
# this build script only for unix/like system

CWD=$(pwd)/src

build ()
{
    ANDROID_NDK=$1
    ARCH_ABI=$2
    cmake \
        -H$CWD \
        -DCMAKE_FIND_ROOT_PATH=$CWD/../build/$ARCH_ABI/.cxx \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
        -DANDROID_ABI=$ARCH_ABI \
        -DANDROID_NDK=$ANDROID_NDK \
        -DANDROID_PLATFORM=android-23 \
        -DCMAKE_ANDROID_ARCH_ABI=$ARCH_ABI \
        -DCMAKE_ANDROID_NDK=$ANDROID_NDK \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=$CWD/../out/$ARCH_ABI \
        -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=$CWD/../out/$ARCH_ABI \
        -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=$CWD/../out/$ARCH_ABI \
        -DCMAKE_MAKE_PROGRAM=/usr/bin/ninja \
        -DCMAKE_SYSTEM_NAME=Android \
        -DCMAKE_SYSTEM_VERSION=23 \
        -B$CWD/../build/$ARCH_ABI \
        -GNinja
}

if [[ $# -eq 0 ]];
then
    echo -e "$0 <path/to/ndk>"
    exit -1
fi

for abi in armeabi-v7a arm64-v8a x86 x86_64
do
    build $1 $abi && ninja -j7 -C $CWD/../build/$abi
done
