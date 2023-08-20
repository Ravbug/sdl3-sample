# How to use these scripts

For the most part, just run the scripts in this directory from this directory. 

## Notes:
- visionOS
	- Requires CMake 3.28 or newer
- Web
 	1. Install emsdk somewhere.
  	2. On Windows hosts, run `config-web-win.bat` via the `emcmdprompt.bat` cmd in the emsdk root directory
	3. After the build completes, use `python3 -m http.server` in the build directory to make the page accessible. 