cmake_minimum_required(VERSION 3.9)

# Target: Main
add_custom_target(MainCopyAsset ALL CopyAssets
        ${NEWORLD_PRODUCT_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        infinideas.main
        )
file(GLOB SRC_MAIN Source/*.*)
add_library(Main SHARED ${SRC_MAIN})
target_link_libraries(Main Core)
target_compile_definitions(Main PRIVATE -DNWCompartmentLoggerPrefix="Main")
add_dependencies(Main MainCopyAsset)
