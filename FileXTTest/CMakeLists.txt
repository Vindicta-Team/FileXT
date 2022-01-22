cmake_minimum_required (VERSION 3.8)
project ("FileXT Test")

include_directories(${PROJECT_SOURCE_DIR}/../FileXT)
file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/*.cpp ${PROJECT_SOURCE_DIR}/*.h)

add_executable(FileXTTest ${SRC_FILES})

if (WIN32)
    target_link_libraries(FileXTTest FileXT)
endif (WIN32)
if (UNIX)
    target_link_libraries(FileXTTest FileXT dl)
endif (UNIX)

set_property(TARGET FileXTTest PROPERTY CXX_STANDARD 17)

get_target_property(FileXTType FileXT TYPE)
if(FileXTType STREQUAL "SHARED_LIBRARY")
  add_custom_command(TARGET FileXTTest POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy_if_different
                         $<TARGET_FILE:FileXT> $<TARGET_FILE_DIR:FileXTTest>
                     COMMENT "Copying FileXT alongside FileXTTest."
                     VERBATIM)
endif()