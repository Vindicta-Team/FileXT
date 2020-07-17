cmake_minimum_required(VERSION 3.2)

project(FileXT LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_library(FileXT SHARED 
   src/dllmain.cpp
   src/errorCodes.h
   src/filemgr.cpp
   src/framework.h
   src/pch.cpp
   src/value.h
)

if(${WIN32})
   # Windows
   add_definitions(-DWIN32_LEAN_AND_MEAN)
   set_target_properties(FileXT PROPERTIES LINK_FLAGS " /MANIFEST:NO /ERRORREPORT:NONE")
endif()