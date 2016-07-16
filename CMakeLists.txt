cmake_minimum_required(VERSION 3.1)

#set_property(GLOBAL PROPERTY CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
#set(LINK_BOOST 1)

if(LINK_BOOST)
    set(EXTLIB boost_filesystem boost_system)
endif()

if(UNIX)
    set(DL dl)
endif(UNIX)
set(GLOBAL_DEFINITIONS -DNOMINMAX)

# ---
project(uilib)
aux_source_directory(./src/uilib/src SRC_UILIB)
include_directories(./src/uilib/include)
add_definitions(${GLOBAL_DEFINITIONS})
link_libraries(${DL} ${EXTLIB} GL GLEW SDL2 SDL2main SDL2_image)
add_library(uilib STATIC ${SRC_UILIB})
# ---
project(nwshared)
aux_source_directory(./src/shared SRC_SHARED)
add_definitions(${GLOBAL_DEFINITIONS})
link_libraries(${DL} ${EXTLIB})
add_library(nwshared STATIC ${SRC_SHARED})
# ---
project(nwclient)
aux_source_directory(./src/client SRC_CLIENT)
add_definitions(${GLOBAL_DEFINITIONS})
include_directories(./src/shared ./uilib/include)
link_libraries(${DL} ${EXTLIB} pthread)
add_library(NEWorld SHARED ${SRC_CLIENT})
target_link_libraries(NEWorld nwshared uilib)
# ---
project(nwserver)
aux_source_directory(./src/server SRC_SERVER)
add_definitions(${GLOBAL_DEFINITIONS})
include_directories(./src/shared)
link_libraries(${DL} ${EXTLIB} pthread)
add_library(NEWorldServer SHARED ${SRC_SERVER})
target_link_libraries(NEWorldServer nwshared)
# ---
project(launcher)
aux_source_directory(./src/launcher SRC_LAUNCHER)
add_definitions(${GLOBAL_DEFINITIONS})
link_libraries(${DL} ${EXTLIB})

add_executable(launcher ${SRC_LAUNCHER})

add_dependencies(NEWorld nwshared)
add_dependencies(NEWorld nwshared)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-fPIC")
endif()
