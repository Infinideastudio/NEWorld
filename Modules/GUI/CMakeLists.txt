cmake_minimum_required(VERSION 3.9)

# Target: GUI
add_custom_target(GUICopyAsset ALL CopyAssets
        ${CMAKE_BINARY_DIR}/Product
        ${CMAKE_CURRENT_SOURCE_DIR}
        infinideas.gui
        )
file(GLOB_RECURSE SRC_GUI Source/*.*)
add_library(GUI SHARED ${SRC_GUI})
target_compile_definitions(GUI
        PRIVATE -DNWAPIEXPORTS
        PRIVATE -DNWCompartmentLoggerPrefix="GUI")

# Link libraries for GUI
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
target_include_directories(GUI PUBLIC
        ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS}
        ${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})
target_link_libraries(GUI Core Threads::Threads
        ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARIES} ${GLEW_LIBRARY} ${OPENGL_LIBRARIES})
add_dependencies(GUI GUICopyAsset)


