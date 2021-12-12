cmake_minimum_required(VERSION 3.10)

project(NEWorld)

# Check IPO Support
if (NOT DEFINED NWCONF_IPO_CONFIGURED)
    set(NWCONF_IPO_CONFIGURED TRUE)
    cmake_policy(SET CMP0069 NEW)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT NWCONF_IPO_SUPPORT OUTPUT NWCONF_IPO_SUPPORT_MESSAGE)
    if (NWCONF_IPO_SUPPORT)
        message(STATUS "IPO IS SUPPORTED, ENABLED")
    else()
        message(STATUS "IPO IS NOT SUPPORTED: ${NWCONF_IPO_SUPPORT_MESSAGE}, DISABLED")
    endif()

    function(target_enable_ipo NAME)
        if (NWCONF_IPO_SUPPORT)
            set_property(TARGET ${NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION $<$<CONFIG:Debug>:FALSE>:TRUE)
        endif ()
    endfunction()
endif()


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/Build/CMake)

find_package(sqlite3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)
find_package(Freetype REQUIRED)

# find noesis core and its app
find_package(Noesis REQUIRED)
file(GLOB NOESIS_APP_INCLUDE 
External/Neosis/Src/Packages/*/*/Include
External/Neosis/Src/Packages/App/Theme/Data/Theme
External/Neosis/Src/Packages/App/Theme/Data/Theme/Fonts
)
file(GLOB NOESIS_APP_SRCS
External/Neosis/Src/Packages/App/Theme/Src/*.cpp
External/Neosis/Src/Packages/App/Providers/Src/*.cpp
External/Neosis/Src/Packages/App/Interactivity/Src/*.cpp
External/Neosis/Src/Packages/App/MediaElement/Src/*.cpp
External/Neosis/Src/Packages/Render/GLRenderDevice/Src/*.cpp
)

if (APPLE)
    find_package(Boost REQUIRED COMPONENTS filesystem)
endif()

if (MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /source-charset:utf-8")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /source-charset:utf-8")
endif()

file(GLOB_RECURSE SRC_BASE NEWorld.Base/*.*)
add_library(NEWorld.Base STATIC ${SRC_BASE})
target_include_directories(NEWorld.Base PUBLIC NEWorld.Base)
target_link_libraries(NEWorld.Base PRIVATE SQLite::SQLite3)

file(GLOB_RECURSE SOURCE NEWorld.Game/*.*)
add_executable(NEWorld ${SOURCE} ${NOESIS_APP_SRCS})

# copy assets
add_custom_command(TARGET NEWorld POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/Assets $<TARGET_FILE_DIR:NEWorld>/Assets)
file(COPY ${NOESIS_DLL} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# build noesis files
execute_process(COMMAND python bin2h.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

target_enable_ipo(NEWorld)
target_compile_definitions(NEWorld PRIVATE NEWORLD_GAME
    NS_APP_PROVIDERS_API=
    NS_APP_INTERACTIVITY_API=
    NS_APP_MEDIAELEMENT_API=
)
target_include_directories(NEWorld PRIVATE NEWorld.Game ${OPENAL_INCLUDE_DIR} External Noesis ${NOESIS_APP_INCLUDE})
target_link_libraries(NEWorld PRIVATE OpenGL::GL GLEW::GLEW glfw Freetype::Freetype NEWorld.Base Noesis)
if (APPLE)
    target_link_libraries(NEWorld PRIVATE Boost::filesystem)
endif()

