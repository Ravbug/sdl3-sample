#!/bin/bash
cd "${0%/*}"
cd ..

mkdir -p build
mkdir -p build/ios
cd build/ios
cmake -G "Xcode" -DCMAKE_SYSTEM_NAME="iOS" ../..
