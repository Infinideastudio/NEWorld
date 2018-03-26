cmake_minimum_required(VERSION 3.9)

file(GLOB MODULE_DIRS *)
foreach(DIR IN LISTS MODULE_DIRS)
    if(NOT DIR MATCHES ".*CMakeLists.txt")
        add_subdirectory(${DIR})
    endif()
endforeach()
