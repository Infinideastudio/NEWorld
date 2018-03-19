cmake_minimum_required(VERSION 3.9)

# Target: NEWorld
project(NEWorld)
add_executable(NEWorld client.cpp)
target_link_libraries(NEWorld Core)
target_compile_definitions(NEWorld PRIVATE -DNWCompartmentLoggerPrefix="NEWorld")

# Target: NEWorldShell
project(NEWorldShell)
# Target: CLI
file(GLOB_RECURSE SRC_CLI CLI/*.*)
add_executable(NEWorldShell ${SRC_CLI})
target_link_libraries(NEWorldShell Core)
target_compile_definitions(NEWorldShell PRIVATE -DNWCompartmentLoggerPrefix="NEWorldShell")
