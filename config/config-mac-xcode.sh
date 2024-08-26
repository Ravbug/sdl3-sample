#!/bin/bash
cd "${0%/*}"

cmake -G "Xcode" -B ../build/mac  -S ..