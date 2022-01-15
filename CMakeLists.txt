cmake_minimum_required (VERSION 3.8)

set(CMAKE_SUPPRESS_REGENERATION true)

project ("FileXT")

include_directories(${PROJECT_SOURCE_DIR}/FileXT)
file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/FileXT/*.cpp ${PROJECT_SOURCE_DIR}/FileXT/*.h)

# Add source to this project's executable.
add_library(FileXT SHARED ${SRC_FILES})

target_precompile_headers(FileXT
  PUBLIC
    FileXT/pch.h
)

set_property(TARGET FileXT PROPERTY CXX_STANDARD 17)

# This option set in CMakePresets.json is specifically for linux x86 support.
if (DEFINED FILEXT_X86)
    set_target_properties(FileXT PROPERTIES COMPILE_OPTIONS "-m32" LINK_FLAGS "-m32")
endif (DEFINED FILEXT_X86)
