cmake_minimum_required(VERSION 3.10)

project(NEWorld)
include(${CMAKE_CURRENT_SOURCE_DIR}/NEWorld.Base/CMake/Config.cmake)

# Include NEWorld Base Library
add_subdirectory(NEWorld.Base)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/Build/CMake)

find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)

# find noesis core and its app
find_package(Noesis REQUIRED)
file(GLOB NOESIS_APP_INCLUDE 
External/Noesis/Src/Packages/*/*/Include
External/Noesis/Src/Packages/App/Theme/Data/Theme
External/Noesis/Src/Packages/App/Theme/Data/Theme/Fonts
)
file(GLOB NOESIS_APP_SRCS
External/Noesis/Src/Packages/App/Theme/Src/*.cpp
External/Noesis/Src/Packages/App/Providers/Src/*.cpp
External/Noesis/Src/Packages/App/Interactivity/Src/*.cpp
External/Noesis/Src/Packages/App/MediaElement/Src/*.cpp
External/Noesis/Src/Packages/App/ApplicationLauncher/Src/NotifyPropertyChangedBase.cpp
External/Noesis/Src/Packages/Render/GLRenderDevice/Src/*.cpp
)

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
    NS_APP_APPLICATIONLAUNCHER_API=
)
target_include_directories(NEWorld PRIVATE NEWorld.Game ${OPENAL_INCLUDE_DIR} External Noesis ${NOESIS_APP_INCLUDE})
target_link_libraries(NEWorld PRIVATE OpenGL::GL GLEW::GLEW glfw NEWorld.Base Noesis)

