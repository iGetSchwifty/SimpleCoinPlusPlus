add_library(server server.cpp
                   )

target_include_directories(server
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/miner"
        "${CMAKE_SOURCE_DIR}/src/utils/server-helpers"
)