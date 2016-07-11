cmake_minimum_required(VERSION 3.1)

set_property(GLOBAL PROPERTY CXX_STANDARD 11)

project(nwshared)
aux_source_directory(./src/shared SRC_SHARED)
link_libraries(dl boost_filesystem boost_system)
add_library(nwshared STATIC ${SRC_SHARED})

project(nwclient)
aux_source_directory(./src/client SRC_CLIENT)
include_directories(./src/shared)
link_libraries(dl boost_filesystem boost_system)
add_library(nwclient SHARED ${SRC_CLIENT})

project(nwserver)
aux_source_directory(./src/server SRC_SERVER)
include_directories(./src/shared)
link_libraries(dl boost_filesystem boost_system pthread)
add_library(nwserver SHARED ${SRC_SERVER})

project(launcher)
aux_source_directory(./src/launcher SRC_LAUNCHER)
link_libraries(dl boost_filesystem boost_system)
add_executable(launcher ${SRC_LAUNCHER})

add_dependencies(nwclient nwshared)
add_dependencies(nwserver nwshared)
target_link_libraries(nwserver nwshared)
target_link_libraries(nwclient nwshared)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-fPIC")
endif()