cmake_minimum_required(VERSION 3.1)

#set_property(GLOBAL PROPERTY CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD 14)
#set(LINK_BOOST 1) #'cmake .. -DLINK_BOOST=1' if you need.
#set(EXECUTABLE 1) #Just for debugging!

if(LINK_BOOST)
    link_libraries(boost_system boost_filesystem boost_date_time)
endif()

if(UNIX)
    link_libraries(dl)
    set(AUDIO -D__LINUX_ALSA__)
endif(UNIX)

if(MS_VC)
    add_definitions(-DNOMINMAX)
endif(MS_VC)

if(EXTRA_INCDIR)
    include_directories(${EXTRA_INCDIR})
endif()

# ---
project(maximilian)
aux_source_directory(./src/maximilian/src SRC_MAXIMILIAN)
add_library(maximilian STATIC ${SRC_MAXIMILIAN})
target_include_directories(maximilian PUBLIC ./src/maximilian/include)
target_compile_definitions(maximilian PUBLIC ${AUDIO})
# ---
project(uilib)
aux_source_directory(./src/uilib/src SRC_UILIB)
add_library(uilib STATIC ${SRC_UILIB})
target_link_libraries(uilib GL GLEW SDL2 SDL2main SDL2_image)
target_include_directories(uilib PUBLIC ./src/uilib/include PUBLIC ./src/maximilian/include)
# ---
project(nwshared)
aux_source_directory(./src/shared SRC_SHARED)
add_library(nwshared STATIC ${SRC_SHARED})
# ---
project(nwclient)
aux_source_directory(./src/client SRC_CLIENT)
if(EXECUTABLE)
    add_executable(NEWorld ${SRC_CLIENT})
else()
    add_library(NEWorld SHARED ${SRC_CLIENT})
endif()
target_link_libraries(NEWorld pthread nwshared uilib freetype)
target_include_directories(NEWorld PUBLIC ./src/shared PUBLIC ./src/uilib/include PUBLIC ./src/maximilian/include)
# ---
project(nwserver)
aux_source_directory(./src/server SRC_SERVER)
if(EXECUTABLE)
    add_executable(NEWorldServer ${SRC_SERVER})
else()
    add_library(NEWorldServer SHARED ${SRC_SERVER})
endif()
target_link_libraries(NEWorldServer nwshared pthread)
target_include_directories(NEWorldServer PUBLIC ./src/shared)
# ---
project(launcher)
aux_source_directory(./src/launcher SRC_LAUNCHER)
add_executable(launcher ${SRC_LAUNCHER})
target_include_directories(launcher PUBLIC ./src/shared)

add_dependencies(NEWorld nwshared)
add_dependencies(NEWorldServer nwshared)
add_dependencies(uilib maximilian)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-fPIC")
endif()
