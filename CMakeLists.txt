cmake_minimum_required(VERSION 3.16)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# prevent installing to system directories. 
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL "")

# Declare the project
project(sdl-min)

if (APPLE OR EMSCRIPTEN)
    set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")    # Disable shared builds on platforms where it does not make sense to use them
endif()

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

# Add your sources to the target
target_sources(${EXECUTABLE_NAME} 
PRIVATE 
    src/main.cpp
    src/iosLaunchScreen.storyboard
)
# What is iosLaunchScreen.storyboard? This file describes what Apple's mobile platforms
# should show the user while the application is starting up. If you don't include one,
# then you get placed in a compatibility mode that does not allow HighDPI.
# This file is referenced inside Info.plist.in, where it is marked as the launch screen file.
# It is also ignored on non-Apple platforms. 

# To get an app icon on Apple platforms, add it to your executable.
# Afterward, the image file in Info.plist.in.
if(APPLE)
    target_sources("${EXECUTABLE_NAME}" PRIVATE "src/logo.png")
endif()

# Set C++ version
target_compile_features(${EXECUTABLE_NAME} PUBLIC cxx_std_20)

# on Web targets, we need CMake to generate a HTML webpage. 
if(EMSCRIPTEN)
	set(CMAKE_EXECUTABLE_SUFFIX ".html" CACHE INTERNAL "")
endif()

# Configure SDL by calling its CMake file.
# we use EXCLUDE_FROM_ALL so that its install targets and configs don't
# pollute upwards into our configuration.
add_subdirectory(SDL EXCLUDE_FROM_ALL)

# SDL_ttf (used for rendering text) depends on Freetype. If you don't want SDL_ttf, then remove this section.
add_subdirectory(freetype EXCLUDE_FROM_ALL)
if(NOT TARGET Freetype::Freetype)
    add_library(Freetype::Freetype ALIAS freetype)
endif()
set(SDLTTF_VENDORED OFF CACHE INTERNAL "")
set(FREETYPE_LIBRARY freetype)
set(FREETYPE_INCLUDE_DIRS "${CMAKE_BINARY_DIR}/freetype/include/" "${CMAKE_CURRENT_LIST_DIR}/freetype/include/")
add_subdirectory(SDL_ttf EXCLUDE_FROM_ALL)	

# SDL_mixer (used for playing audio)
set(SDLMIXER_OPUS OFF)
add_subdirectory(ogg EXCLUDE_FROM_ALL)
set(SDLMIXER_VENDORED OFF)
set(SDLMIXER_MIDI_NATIVE OFF)
add_subdirectory(SDL_mixer EXCLUDE_FROM_ALL)

# Link SDL to our executable. This also makes its include directory available to us. 
target_link_libraries(${EXECUTABLE_NAME} PUBLIC 
	SDL3::SDL3
	SDL3_ttf::SDL3_ttf  # remove if you are not using SDL_ttf
    SDL3_mixer::SDL3_mixer
)
target_compile_definitions(${EXECUTABLE_NAME} PUBLIC SDL_MAIN_USE_CALLBACKS)

# Dealing with assets
# We have some non-code resources that our application needs in order to work. How we deal with those differs per platform.
if (APPLE)
    # on Apple targets, the application bundle has a "resources" subfolder where we can place our assets.
    # SDL_GetBasePath() gives us easy access to that location.
    set(input_root "${CMAKE_CURRENT_LIST_DIR}/src")
    macro(add_resource FILE)
        file(RELATIVE_PATH relpath "${input_root}" "${FILE}")
        get_filename_component(relpath "${relpath}" DIRECTORY)
        target_sources(${EXECUTABLE_NAME} PRIVATE ${FILE})
        set_property(SOURCE ${FILE} PROPERTY MACOSX_PACKAGE_LOCATION "Resources/${relpath}")
    endmacro()
    add_resource("${CMAKE_CURRENT_LIST_DIR}/src/Inter-VariableFont.ttf")
    add_resource("${CMAKE_CURRENT_LIST_DIR}/src/the_entertainer.ogg")
elseif(EMSCRIPTEN)
    # on the web, we have to put the files inside of the webassembly
    # somewhat unintuitively, this is done via a linker argument.
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE 
        "--preload-file \"${CMAKE_CURRENT_LIST_DIR}/src/Inter-VariableFont.ttf@/\""
        "--preload-file \"${CMAKE_CURRENT_LIST_DIR}/src/the_entertainer.ogg@/\""
    )
else()
    if (ANDROID)
        file(MAKE_DIRECTORY "${MOBILE_ASSETS_DIR}")    # we need to create the project Assets dir otherwise CMake won't copy our assets.
    endif()

    # for platforms that do not have a good packaging format, all we can do is copy the assets to the process working directory.
    macro(copy_helper filename)
        if (ANDROID)
            set(outname "${MOBILE_ASSETS_DIR}/${filename}")    # MOBILE_ASSETS_DIR is set in the gradle file
        else()
            set(outname "${CMAKE_BINARY_DIR}/$<CONFIGURATION>/${filename}")
        endif()
        add_custom_command(POST_BUILD
            TARGET "${EXECUTABLE_NAME}"
		    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_LIST_DIR}/src/${filename}" "${outname}"
		    DEPENDS "${filename}"
	    )
    endmacro()
    copy_helper("Inter-VariableFont.ttf")
    copy_helper("the_entertainer.ogg")
endif()

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

# On macOS Platforms, ensure that the bundle is valid for distribution by calling fixup_bundle.
# note that fixup_bundle does not work on iOS, so you will want to use static libraries 
# or manually copy dylibs and set rpaths
if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    INSTALL(CODE 
        "include(BundleUtilities)
        fixup_bundle(\"$<TARGET_BUNDLE_DIR:${EXECUTABLE_NAME}>\" \"\" \"\")
        " 
        COMPONENT Runtime
    )
endif()
install(TARGETS ${EXECUTABLE_NAME} BUNDLE DESTINATION ./install)

