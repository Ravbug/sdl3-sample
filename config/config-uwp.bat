@echo OFF
cd ..
mkdir build
mkdir build\uwp
cd build\uwp
cmake -DCMAKE_SYSTEM_NAME="WindowsStore" -DCMAKE_SYSTEM_VERSION="10.0" ..\..