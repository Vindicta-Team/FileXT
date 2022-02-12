cmake_minimum_required (VERSION 3.15)

set(CMAKE_SUPPRESS_REGENERATION true)


set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

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

add_subdirectory(FileXTTest)
