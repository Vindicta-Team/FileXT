cmake_minimum_required(VERSION 3.2)

project(FileXT)

add_library(FileXT SHARED 
    dllmain.cpp
    errorCodes.h
    filemgr.cpp
    framework.h
    pch.cpp
    value.h
)
set_target_properties(FileXT PROPERTIES CXX_STANDARD 17)

if(${WIN32})
	# Windows
	add_definitions(-DWIN32_LEAN_AND_MEAN)
	set_target_properties(FileXT PROPERTIES LINK_FLAGS " /MANIFEST:NO /ERRORREPORT:NONE")
endif()



