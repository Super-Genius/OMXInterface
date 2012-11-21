#!/bin/bash

echo "Fetching Android system headers"
git clone --depth=1 --branch gingerbread-release git://github.com/CyanogenMod/android_frameworks_base.git ../android-source/frameworks/base
git clone --depth=1 --branch gingerbread-release git://github.com/CyanogenMod/android_system_core.git ../android-source/system/core

echo "Fetching Android libraries for linking"
# Libraries from any froyo/gingerbread device/emulator should work
# fine, since the symbols used should be available on most of them.
if [ ! -d "../android-libs" ]; then
    if [ ! -f "../update-cm-7.0.3-N1-signed.zip" ]; then
        wget http://download.cyanogenmod.com/get/update-cm-7.0.3-N1-signed.zip -P../
    fi
    unzip ../update-cm-7.0.3-N1-signed.zip system/lib/libstagefright.so -d../android-libs
    unzip ../update-cm-7.0.3-N1-signed.zip system/lib/libutils.so -d../android-libs
fi
