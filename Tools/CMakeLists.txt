cmake_minimum_required(VERSION 3.9)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/SDK/Tools/$<CONFIG>)

add_executable(CopyAssets CopyAssets.cpp)

add_executable(
    InboundGen
    PacketCodeGen/Type.h
    PacketCodeGen/Type.cpp
    PacketCodeGen/Parse.h
    PacketCodeGen/Gen.cpp
)
