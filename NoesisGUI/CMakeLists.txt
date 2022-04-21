cmake_minimum_required(VERSION 3.10)

project(NEWorld)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/CMake)

kls_add_library_module(NoesisGUI NW::Noesis)

set(NOESIS_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/SDK" CACHE PATH "Root to the NoesisGui sdk")
find_package(Noesis REQUIRED)
file(GLOB NOESIS_APP_INCLUDE
        SDK/Src/Packages/*/*/Include
        SDK/Src/Packages/App/Theme/Data/Theme
        SDK/Src/Packages/App/Theme/Data/Theme/Fonts
        )
file(GLOB NOESIS_APP_SRCS
        SDK/Src/Packages/App/Theme/Src/*.cpp
        SDK/Src/Packages/App/Providers/Src/*.cpp
        SDK/Src/Packages/App/Interactivity/Src/*.cpp
        SDK/Src/Packages/App/MediaElement/Src/*.cpp
        SDK/Src/Packages/App/ApplicationLauncher/Src/NotifyPropertyChangedBase.cpp
        SDK/Src/Packages/Render/GLRenderDevice/Src/*.cpp
        )
target_sources(NoesisGUI PRIVATE ${NOESIS_APP_SRCS})
target_compile_definitions(NoesisGUI PUBLIC
        NS_APP_THEME_API=
        NS_APP_PROVIDERS_API=
        NS_APP_INTERACTIVITY_API=
        NS_APP_MEDIAELEMENT_API=
        NS_APP_APPLICATIONLAUNCHER_API=
        NS_RENDER_GLRENDERDEVICE_API=
        )
# For some reason we still need to put all these as public
# TODO(Maybe we should put a wrapper library around it)
target_include_directories(NoesisGUI PUBLIC ${NOESIS_APP_INCLUDE})
target_link_libraries(NoesisGUI PUBLIC Noesis)
execute_process(COMMAND python bin2h.py WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
add_custom_command(
        TARGET NoesisGUI POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${NOESIS_DLL} ${KLS_OUT_PRODUCT_DIR}
)
