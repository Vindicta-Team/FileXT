cmake_minimum_required (VERSION 3.8)

project ("FileXT")

include_directories(${PROJECT_SOURCE_DIR}/FileXT)
file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/FileXT/*.cpp)

# Add source to this project's executable.
add_library(FileXT SHARED ${SRC_FILES})

target_precompile_headers(FileXT
  PUBLIC
    FileXT/pch.h
)

set_property(TARGET FileXT PROPERTY CXX_STANDARD 17)
