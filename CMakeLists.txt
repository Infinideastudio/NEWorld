cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 14)
set(SOURCE_DIR ./src)
set(CMFILE CMakeLists.txt)

option(LINK_BOOST "Enable link compiler switch of Boost library instead of auto-link" OFF)
option(EXECUTABLE "Link the executable for client and server" OFF)

if(LINK_BOOST STREQUAL "ON")
    link_libraries(boost_system boost_filesystem boost_date_time)
endif()

if(EXTRA_INCDIR)
    include_directories(BEFORE ${EXTRA_INCDIR})
endif()

if(EXTRA_LIBDIR)
    link_directories(${EXTRA_LIBDIR})
endif()

if(UNIX)
    link_libraries(dl)
endif()

if(MS_VC)
    add_definitions(-DNOMINMAX)
endif()

include(${SOURCE_DIR}/uilib/${CMFILE})
include(${SOURCE_DIR}/shared/${CMFILE})
include(${SOURCE_DIR}/client/${CMFILE})
include(${SOURCE_DIR}/server/${CMFILE})
include(${SOURCE_DIR}/launcher/${CMFILE})

add_dependencies(nwclient nwshared)
add_dependencies(nwserver nwshared)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()
