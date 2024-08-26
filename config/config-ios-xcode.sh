#!/bin/bash
cd "${0%/*}"

cmake -G "Xcode" -DCMAKE_SYSTEM_NAME="iOS" -B ../build/ios -S ..
