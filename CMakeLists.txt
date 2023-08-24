cmake_minimum_required(VERSION 3.16)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIGURATION>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIGURATION>)

# Declare the project
project(sdl-min)

# Set the name of the executable
set(EXECUTABLE_NAME ${PROJECT_NAME})

# Create an executable or a shared library based on the platform and add our sources to it
if (ANDROID)
	# The SDL java code is hardcoded to load libmain.so on android, so we need to change EXECUTABLE_NAME
	set(EXECUTABLE_NAME main)
	add_library(${EXECUTABLE_NAME} SHARED)
else()
	add_executable(${EXECUTABLE_NAME})
endif()

target_sources(${EXECUTABLE_NAME} 
PRIVATE 
    src/main.cpp
    src/iosLaunchScreen.storyboard
    src/Sample.appxManifest
)

# What is iosLaunchScreen.storyboard? This file describes what Apple's mobile platforms
# should show the user while the application is starting up. If you don't include one,
# then you get placed in a compatibility mode that does not allow HighDPI.
# This file is referenced inside Info.plist.in, where it is marked as the launch screen file.
# It is also ignored on non-Apple platforms. 

# What is Sample.appxManifest? This file describes deployment settings for Universal Windows Platform
# applications, like splash screen and icon resolutions, which are required for the application to run. 
# It is ignored on other platforms. Below, we add those images. 
if (WINDOWS_STORE)
    target_sources("${EXECUTABLE_NAME}" PRIVATE "src/logo44.png" "src/logo150.png" "src/logo50.png" "src/logo620x300.png")
endif()

# use C++11
target_compile_features(${EXECUTABLE_NAME} PUBLIC cxx_std_11)

if(CMAKE_SYSTEM_NAME MATCHES Emscripten)
	set(CMAKE_EXECUTABLE_SUFFIX ".html" CACHE INTERNAL "")
endif()

# If targeting Windows UWP, enable Windows Runtime Compilation
if(WINDOWS_STORE)
    target_compile_options(${EXECUTABLE_NAME} PUBLIC "/ZW" "/EHsc")
endif()

# Configure SDL by calling its CMake file.
# we use EXCLUDE_FROM_ALL so that its install targets and configs don't
# pollute upwards into our configuration.
add_subdirectory(SDL EXCLUDE_FROM_ALL)

# Link SDL to our executable. This also makes its include directory available to us. 
target_link_libraries(${EXECUTABLE_NAME} PUBLIC SDL3::SDL3)

# set some extra configs for each platform
set_target_properties(${EXECUTABLE_NAME} PROPERTIES 
    # On macOS, make a proper .app bundle instead of a bare executable
    MACOSX_BUNDLE TRUE
    # Set the Info.plist file for Apple Mobile platforms. Without this file, your app
    # will not launch. 
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/src/Info.plist.in"

    # in Xcode, create a Scheme in the schemes dropdown for the app.
    XCODE_GENERATE_SCHEME TRUE
    # Identification for Xcode
    XCODE_ATTRIBUTE_BUNDLE_IDENTIFIER "com.ravbug.sdl3-sample"
	XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.ravbug.sdl3-sample"
	XCODE_ATTRIBUTE_CURRENTYEAR "${CURRENTYEAR}"
)

# on Visual Studio, set our app as the default project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "${EXECUTABLE_NAME}")

# On Apple Platforms, ensure that 
# the bundle is valid for distribution.
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
INSTALL(CODE 
    "include(BundleUtilities)
    fixup_bundle(\"${CMAKE_INSTALL_PREFIX}/$<CONFIGURATION>/${EXECUTABLE_NAME}.app\" \"\" \"\")
    " 
    COMPONENT Runtime
)
endif()
