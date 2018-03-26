cmake_minimum_required(VERSION 3.9)

# Target: NEWorldShell
project(NEWorldShell)
file(GLOB_RECURSE SRC_CLI Source/*.*)
add_executable(NEWorldShell ${SRC_CLI})
target_link_libraries(NEWorldShell Core)
target_compile_definitions(NEWorldShell PRIVATE -DNWCompartmentLoggerPrefix="NEWorldShell")
