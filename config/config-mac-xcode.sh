#!/bin/bash
cd "${0%/*}"
cd ..

mkdir -p build
mkdir -p build/mac
cd build/mac
cmake -G "Xcode" ../..
