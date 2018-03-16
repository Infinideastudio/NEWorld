cmake_minimum_required(VERSION 3.9)

set(CMAKE_MACOSX_RPATH ON)
set(CMAKE_CXX_STANDARD 17)
set(PROJECT_DIR ${CMAKE_SOURCE_DIR})
set(SOURCE_DIR "${PROJECT_DIR}/Core")

if(UNIX)
    link_libraries(dl stdc++fs)
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_DIR}/Build/CMake")

project(NEWorld)

# Set Build Output Path
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/SDK/Lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Product)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Product)

# For the whole project
include_directories(${SOURCE_DIR} "${PROJECT_DIR}/3rdParty")
find_package(Threads REQUIRED)

# Target: Core
file(GLOB_RECURSE SRC_CORE ${SOURCE_DIR}/client/*.*
                             ${SOURCE_DIR}/Common/*.*
                             ${SOURCE_DIR}/Game/*.*)

add_library(Core SHARED ${SRC_CORE})
target_compile_definitions(Core
    PRIVATE -DNWCOREEXPORTS
    PRIVATE -DNWCompartmentLoggerPrefix="Core")

# Link libraries for Core
find_package(rpclib REQUIRED)
target_include_directories(Core PUBLIC ${RPCLIB_INCLUDE_DIR})
target_link_libraries(Core Threads::Threads ${RPCLIB_LIBS})
target_compile_definitions(Core PUBLIC ${RPCLIB_COMPILE_DEFINITIONS})

# Target: CLI
file(GLOB_RECURSE SRC_CLI Modules/CLI/*.*)
add_library(CLI SHARED ${SRC_CLI})
target_link_libraries(CLI Core)
target_compile_definitions(CLI PRIVATE -DNWCompartmentLoggerPrefix="CLI")

add_subdirectory(Shells)
add_subdirectory(Modules)
