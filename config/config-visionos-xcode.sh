#!/bin/bash

# note: requires CMake 3.28 or newer

cd "${0%/*}"
cd ..

mkdir -p build
mkdir -p build/visionOS
cd build/visionOS
cmake -G "Xcode" -DCMAKE_SYSTEM_NAME="visionOS" ../..
