cmake_minimum_required(VERSION 3.9)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${NEWORLD_PRODUCT_DIR}/Modules)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${NEWORLD_PRODUCT_DIR}/Modules)
set(CMAKE_SHARED_LIBRARY_SUFFIX ".nwModule")

file(GLOB MODULE_DIRS *)
foreach(DIR IN LISTS MODULE_DIRS)
    if(NOT DIR MATCHES ".*CMakeLists.txt")
        add_subdirectory(${DIR})
    endif()
endforeach()
