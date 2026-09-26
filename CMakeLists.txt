cmake_minimum_required(VERSION 3.20)
project(dkomrade CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(dkomrade STATIC
    src/KernelOffsetsLoader.cpp
    src/CidTableWalker.cpp
    src/ListEntryEditor.cpp
    src/CidEntryEditor.cpp
    src/ProcessHider.cpp
)

add_library(dkomrade::dkomrade ALIAS dkomrade)

target_include_directories(dkomrade PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(dkomrade PUBLIC cxx_std_20)

if(MSVC)
    target_compile_options(dkomrade PRIVATE /W4 /permissive-)
else()
    target_compile_options(dkomrade PRIVATE -Wall -Wextra -Wpedantic)
endif()
