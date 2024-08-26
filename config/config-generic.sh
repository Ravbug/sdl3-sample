#!/bin/bash
cd "${0%/*}"/

cmake -B "../build/`uname`" -S ..