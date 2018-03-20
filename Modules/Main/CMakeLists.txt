cmake_minimum_required(VERSION 3.9)

# Target: Main
file(GLOB SRC_MAIN Source/*.*)
add_library(Main SHARED ${SRC_MAIN})
target_link_libraries(Main Core)
target_compile_definitions(Main PRIVATE -DNWCompartmentLoggerPrefix="Main")
