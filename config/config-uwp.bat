@echo OFF

cmake -DCMAKE_SYSTEM_NAME="WindowsStore" -DCMAKE_SYSTEM_VERSION="10.0" -B ..\build\uwp -S ..