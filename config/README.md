# How to use these scripts

For the most part, just run the scripts in this directory from this directory. 

## Notes:
- visionOS
	- Requires CMake 3.28 or newer
- Web
 	1. Install emsdk somewhere.
  	2. On Windows hosts, run `config-web-win.bat` via the `emcmdprompt.bat` cmd in the emsdk root directory
	3. On Unix hosts, first run `source emsdk_env.sh` (found in the emsdk root directory), then run `config-web-unix.sh` 
	4. After the build completes, use `python3 -m http.server` in the build directory to make the page accessible.
- Android
	- There is no easy CMake config for this platform. Instead, follow the steps as outlined below. See the android GitHub action for more details. 
	1. Install Android Studio + NDK from https://developer.android.com/studio
 	2. Copy `build.gradle` to `SDL/android-project/app/`. If you look inside `build.gradle`, you'll see `"../../../CMakeLists.txt"` as the path to the CMakeLists file. This points to the CMakeLists in this repo's root directory after it is copied. 
  	3. `cd SDL/android-project/`
  	4. `./gradlew assembleDebug`. You'll get an apk which you can then install onto a device. 
