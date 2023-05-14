#!/bin/bash
cd "${0%/*}"
cd ..

name=`uname`
mkdir -p build
mkdir -p build/$name
cd build/$name
cmake ../..