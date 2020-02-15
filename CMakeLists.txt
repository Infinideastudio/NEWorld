cmake_minimum_required(VERSION 3.9)

set(CMAKE_MACOSX_RPATH ON)
set(CMAKE_CXX_STANDARD 17)
set(PROJECT_DIR ${CMAKE_SOURCE_DIR})
set(SOURCE_DIR "${PROJECT_DIR}/Core")

if(UNIX AND NOT APPLE)
    link_libraries(dl stdc++fs)
endif()

if(_VCPKG_ROOT_DIR)
	set(VCPKG_USED ON)
	message("Vcpkg detected.")
else()
	message("Vcpkg not detected.")
endif()

if(NOT VCPKG_USED)
	list(APPEND CMAKE_MODULE_PATH "${PROJECT_DIR}/Build/CMake")
endif()

project(NEWorld)
set(NEWORLD_PRODUCT_DIR ${CMAKE_BINARY_DIR}/Product-$<CONFIG>)

# Set Build Output Path
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/SDK/Lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${NEWORLD_PRODUCT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${NEWORLD_PRODUCT_DIR})

include(3rdParty/NRT/Build/Configure.cmake)

# For the whole project
include_directories(${SOURCE_DIR} "${PROJECT_DIR}/3rdParty/Headers")
find_package(Threads REQUIRED)

# Runtime
add_subdirectory(3rdParty/NRT NRT)

# Target: Core
file(GLOB_RECURSE SRC_CORE ${SOURCE_DIR}/*.*)

add_library(Core SHARED ${SRC_CORE})
target_enable_ipo(Core)
target_compile_definitions(Core
    PRIVATE -DNWCOREEXPORTS
    PRIVATE -DNWAPIEXPORTS
    PRIVATE -DNWCompartmentLoggerPrefix="Core")

# Link libraries for Core
find_package(rpclib CONFIG REQUIRED)
find_package(SQLite3 REQUIRED) # use the cmake built-in search
target_link_libraries(Core Threads::Threads rpclib::rpc NRT.CFX SQLite::SQLite3)

add_subdirectory(Tools)
add_subdirectory(Shells)
add_subdirectory(Modules)
