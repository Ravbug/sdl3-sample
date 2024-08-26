#!/bin/bash
cd "${0%/*}"

cmake -G "Xcode" -DCMAKE_SYSTEM_NAME="tvOS" -B ../build/tvos -S ..
