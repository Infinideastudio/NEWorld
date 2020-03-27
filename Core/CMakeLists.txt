cmake_minimum_required(VERSION 3.9)

add_custom_target(
    Core.CodeGen ALL
    InboundGen ${CMAKE_CURRENT_SOURCE_DIR}/Protocol/Lists ${CMAKE_CURRENT_SOURCE_DIR}
)

file(GLOB_RECURSE SRC_COMMON ${CMAKE_CURRENT_SOURCE_DIR}/Common/*.*)
file(GLOB_RECURSE SRC_GAME ${CMAKE_CURRENT_SOURCE_DIR}/Game/*.*)
set(SRC_CORE ${SRC_COMMON} ${SRC_GAME})

add_library(Core SHARED ${SRC_CORE})
target_enable_ipo(Core)
target_compile_definitions(Core
    PRIVATE -DNWCOREEXPORTS
    PRIVATE -DNWAPIEXPORTS
    PRIVATE -DNWCompartmentLoggerPrefix="Core")

# Link libraries for Core
find_package(rpclib CONFIG REQUIRED)
find_package(sqlite3 CONFIG REQUIRED)
find_package(wangle CONFIG REQUIRED)
target_link_libraries(Core rpclib::rpc NRT.CFX NRT.Math sqlite3 wangle::wangle)
target_include_directories(Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(Core Core.CodeGen)
