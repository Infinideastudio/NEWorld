cmake_minimum_required(VERSION 3.9)

set(PROJECT_DIR ${CMAKE_SOURCE_DIR})

project(NEWorld)
# Parent project configuration
include(3rdParty/NRT/Build/Configure.cmake)

# For the whole project
include_directories(${SOURCE_DIR} "${PROJECT_DIR}/3rdParty/Headers")

# Runtime
nw_add_subdirectory(3rdParty/NRT NRT)

# Project Components
nw_add_subdirectory(Core)
nw_add_subdirectory(Tools)
nw_add_subdirectory(Shells)
nw_add_subdirectory(Modules)
nw_add_subdirectory(Tests)
