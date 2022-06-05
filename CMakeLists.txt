cmake_minimum_required (VERSION 3.15)

set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(Is64Bit ON CACHE BOOL "Used via cmake commandline during project generation to determine 64 or 32bit.")

project (FileXT)

include_directories(${PROJECT_SOURCE_DIR}/FileXT)
file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/FileXT/*.cpp ${PROJECT_SOURCE_DIR}/FileXT/*.h)

# Add source to this project's executable.
add_library(FileXT SHARED ${SRC_FILES})

target_precompile_headers(FileXT
  PUBLIC
    FileXT/pch.h
)

if (${Is64Bit})
    set(output_name "filext_x64")
else()
    set(output_name "filext")
    
    if (UNIX)
        target_link_options(FileXT PUBLIC "-m32")
        target_compile_options(FileXT PUBLIC "-m32")
    endif()
endif()

set_target_properties(FileXT
    PROPERTIES
    PREFIX ""
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/bin/$<CONFIG>"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/bin/$<CONFIG>"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/bin/$<CONFIG>"
)

set_target_properties(FileXT PROPERTIES OUTPUT_NAME ${output_name})

set_property(TARGET FileXT PROPERTY CXX_STANDARD 17)

add_subdirectory(FileXTTest)
