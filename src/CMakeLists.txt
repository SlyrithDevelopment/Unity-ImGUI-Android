# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

# Declares and names the project.

project("void")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        void

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        main.cpp
        utils.cpp
        hooks.cpp
        menu.cpp
        imgui/imgui.cpp
        imgui/imgui_demo.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_impl_android.cpp
        imgui/imgui_impl_opengl3.cpp
        libs/KittyMemory/KittyMemory.cpp
        libs/KittyMemory/MemoryPatch.cpp
        libs/KittyMemory/MemoryBackup.cpp
        libs/KittyMemory/KittyUtils.cpp
        libs/KittyMemory/KittyScanner.cpp
        libs/KittyMemory/KittyArm64.cpp)

option(DOBBY_GENERATE_SHARED "Build shared library" ON)
option(DOBBY_DEBUG "Enable debug logging" OFF)
option(NearBranch "Enable near branch trampoline" ON)
option(FullFloatingPointRegisterPack "Save and pack all floating-point registers" OFF)
option(Plugin.SymbolResolver "Enable symbol resolver" ON)
option(Plugin.ImportTableReplace "Enable import table replace " OFF)
option(Plugin.Android.BionicLinkerUtil "Enable android bionic linker util" OFF)
option(BUILD_EXAMPLE "Build example" OFF)
option(BUILD_TEST "Build test" OFF)

if(NOT TARGET dobby)
set(DOBBY_DIR libs/Dobby)
macro(SET_OPTION option value)
  set(${option} ${value} CACHE INTERNAL "" FORCE)
endmacro()
SET_OPTION(DOBBY_DEBUG OFF)
SET_OPTION(DOBBY_GENERATE_SHARED OFF)
add_subdirectory(${DOBBY_DIR} dobby)
get_property(DOBBY_INCLUDE_DIRECTORIES
  TARGET dobby
  PROPERTY INCLUDE_DIRECTORIES)
include_directories(
  .
  ${DOBBY_INCLUDE_DIRECTORIES}
  $<TARGET_PROPERTY:dobby,INCLUDE_DIRECTORIES>
)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/includes)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)


# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        void

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
        dobby
        android
        EGL
        GLESv3)