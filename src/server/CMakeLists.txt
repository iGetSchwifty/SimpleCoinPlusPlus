add_library(server server.cpp
                   miner/miner.cpp
                   ../utils/base64/base64.cpp
                   ../utils/easy-ecc/ecc.c
                   ../utils/sha256/sha256.cpp)

target_include_directories(server
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_SOURCE_DIR}/src/utils/base64"
        "${CMAKE_CURRENT_SOURCE_DIR}/miner"
        "${CMAKE_SOURCE_DIR}/src/utils/server-helpers"
        "${CMAKE_SOURCE_DIR}/src/utils/data"
        "${CMAKE_SOURCE_DIR}/src/utils/easy-ecc"
        "${CMAKE_SOURCE_DIR}/src/utils/json"
        "${CMAKE_SOURCE_DIR}/src/utils/sha256"
)