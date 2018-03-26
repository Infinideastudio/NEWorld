cmake_minimum_required(VERSION 3.9)

# Target: NEWorld
add_custom_target(NEWorldCopyAsset ALL CopyAssets
        ${NEWORLD_PRODUCT_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        infinideas.gui
        )
file(GLOB_RECURSE SRC_NEWorld Source/*.*)
add_executable(NEWorld ${SRC_NEWorld})
target_compile_definitions(NEWorld
        PRIVATE -DNWAPIEXPORTS
        PRIVATE -DNWCompartmentLoggerPrefix="NEWorld")

# Link libraries for NEWorld
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
target_include_directories(NEWorld PUBLIC
        ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS}
        ${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})
target_link_libraries(NEWorld Core Threads::Threads
        ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARIES} ${GLEW_LIBRARY} ${OPENGL_LIBRARIES})
add_dependencies(NEWorld NEWorldCopyAsset)


