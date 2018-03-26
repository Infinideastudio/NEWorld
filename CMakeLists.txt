cmake_minimum_required(VERSION 3.9)

set(CMAKE_MACOSX_RPATH ON)
set(CMAKE_CXX_STANDARD 17)
set(PROJECT_DIR ${CMAKE_SOURCE_DIR})
set(SOURCE_DIR "${PROJECT_DIR}/Core/Source")

if(UNIX)
    link_libraries(dl stdc++fs)
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_DIR}/Build/CMake")

project(NEWorld)
set(NEWORLD_PRODUCT_DIR ${CMAKE_BINARY_DIR}/Product-$<CONFIG>)
# Set Build Output Path
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/SDK/Lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${NEWORLD_PRODUCT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${NEWORLD_PRODUCT_DIR})

# For the whole project
include_directories(${SOURCE_DIR} "${PROJECT_DIR}/3rdParty")
find_package(Threads REQUIRED)

# Target: Core
file(GLOB_RECURSE SRC_CORE ${SOURCE_DIR}/*.*)

add_library(Core SHARED ${SRC_CORE})
target_compile_definitions(Core
    PRIVATE -DNWCOREEXPORTS
    PRIVATE -DNWAPIEXPORTS
    PRIVATE -DNWCompartmentLoggerPrefix="Core")

# Link libraries for Core
find_package(rpclib REQUIRED)
target_include_directories(Core PUBLIC ${RPCLIB_INCLUDE_DIR})
target_link_libraries(Core Threads::Threads ${RPCLIB_LIBS})
target_compile_definitions(Core PUBLIC ${RPCLIB_COMPILE_DEFINITIONS})

add_subdirectory(Tools)
add_subdirectory(Shells)
add_subdirectory(Modules)
