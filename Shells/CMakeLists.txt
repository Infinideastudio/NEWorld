cmake_minimum_required(VERSION 3.9)

# Target: NEWorld
project(NEWorld)
add_executable(NEWorld client.cpp)
target_link_libraries(NEWorld Core)
target_compile_definitions(NEWorld PRIVATE -DNWCompartmentLoggerPrefix="NEWorld")

# Target: NEWorldShell
project(NEWorldShell)
add_executable(NEWorldShell shell.cpp)
target_link_libraries(NEWorldShell CLI Core)
target_compile_definitions(NEWorldShell PRIVATE -DNWCompartmentLoggerPrefix="NEWorldShell")
