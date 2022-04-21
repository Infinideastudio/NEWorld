cmake_minimum_required(VERSION 3.10)

include(Kaleidoscope/Build/Config.cmake)
project(NEWorld)
kls_configure()

kls_define_modules(NoesisGUI)
kls_define_modules(Kaleidoscope)

kls_add_library_module(NEWorld.Base NW::Base)
kls_public_source_directory(NEWorld.Base NEWorld.Base)
target_link_libraries(NEWorld.Base PUBLIC klsxx::essential)
kls_add_executable_module(NEWorld)
kls_public_source_directory(NEWorld NEWorld.Game)
target_compile_definitions(NEWorld PRIVATE NEWORLD_GAME)
target_link_libraries(NEWorld PRIVATE klsxx::essential klsxx::thread klsxx::coroutine)

# copy assets
add_custom_command(
        TARGET NEWorld POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Assets $<TARGET_FILE_DIR:NEWorld>/Assets
)

# opengl
kls_vcpkg_package(glew)
kls_vcpkg_package(glfw3)
find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
target_link_libraries(NEWorld PRIVATE OpenGL::GL GLEW::GLEW glfw)

# nlohmann json
kls_vcpkg_package(nlohmann-json)
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(NEWorld PRIVATE nlohmann_json::nlohmann_json)

# All others
# we do not use the tsl on vcpkg as currently there are some problems on the public repo
kls_vcpkg_package(leveldb)
kls_vcpkg_package(openal-soft)
find_package(Threads REQUIRED)
find_package(OpenAL REQUIRED)
find_package(leveldb CONFIG REQUIRED)
target_include_directories(NEWorld PRIVATE ${OPENAL_INCLUDE_DIR} External)
target_link_libraries(NEWorld PRIVATE leveldb::leveldb NoesisGUI NEWorld.Base)
