cmake_minimum_required(VERSION 3.9)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Product/Modules)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Product/Modules)

# Target: Main
file(GLOB_RECURSE SRC_MAIN ${SOURCE_DIR}/main/*.cpp)
add_library(Main SHARED ${SRC_MAIN})
target_link_libraries(Main Core)
target_compile_definitions(Main PRIVATE -DNWCompartmentLoggerPrefix="Main")

# Target: GUI
file(GLOB_RECURSE SRC_GUI ${SOURCE_DIR}/gui/*.cpp)
add_library(GUI SHARED ${SRC_GUI})
target_compile_definitions(GUI PRIVATE -DNWCompartmentLoggerPrefix="GUI")

# Link libraries for GUI
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
target_include_directories(GUI PUBLIC ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})
target_link_libraries(GUI Core Threads::Threads
    ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${GLEW_LIBRARY} ${OPENGL_LIBRARIES})
