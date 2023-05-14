@echo OFF
cd ..
mkdir build
mkdir build\win
cd build\win
cmake -DCMAKE_SYSTEM_NAME="WindowsStore" -DCMAKE_SYSTEM_VERSION="10.0" ..\..