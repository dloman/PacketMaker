cmake_minimum_required(VERSION 3.0)

set(MODULE_NAME PacketMaker)
add_definitions(-DMODULE_NAME=\"PacketMaker\")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_definitions(-Wall)
add_definitions(-fPIC)

add_definitions(-std=c++14)

project(PacketMaker)

include_directories(src)

set(CMAKE_DEBUG_POSTFIX d)

add_library(
  PacketMaker
  src/PacketMaker.hpp
  )

set_target_properties(PacketMaker PROPERTIES LINKER_LANGUAGE CXX)

add_executable(
  PacketMakerTest
  Test/PacketMakerTest.cpp
  )

target_link_libraries(
  PacketMakerTest
  PacketMaker
  )
