## SDL3 App From Source Minimal Example
This is a minimal example for building and using SDL3 from source 
using C++ and CMake. It also demonstrates setting up things like macOS/iOS
bundles.
See `src/main.cpp` for the code. 

### Building And Running
First install your compiler of choice and CMake. Then, follow the commands below:
```sh
# You need to clone with submodules, otherwise SDL will not download.
git clone https://github.com/Ravbug/sdl3-sample --depth=1 --recurse-submodules
cd sdl3-sample
mkdir build
cd build
cmake ..
```
You can also use an init script inside `config/`. Then open the IDE project inside `build/` 
(If you had CMake generate one) and run!

## Supported Platforms
I have tested the following:
- macOS
- iOS
- tvOS
- visionOS
- Windows
- Linux
- UWP
- Web browsers (via Emscripten)*
- Android*

*See further instructions in `config/`

## Updating SDL
Just update the submodule:
```sh
cd SDL
git pull
```
You don't need to use a submodule, you can also copy the source in directly. This
repository uses a submodule to keep its size to a minimum. Note that as of writing, SDL3 is
in development, so expect APIs to change. 


## Reporting issues
Is something not working? Create an Issue or send a Pull Request on this repository!
