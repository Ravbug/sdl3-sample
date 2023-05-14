#!/bin/bash
cd "${0%/*}"
cd ..

mkdir -p build
mkdir -p build/tvos
cd build/tvos
cmake -G "Xcode" -DCMAKE_SYSTEM_NAME="tvOS" ../..
