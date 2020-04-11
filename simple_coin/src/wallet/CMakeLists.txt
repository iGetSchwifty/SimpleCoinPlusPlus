add_library(wallet wallet.cpp
                   ../utils/base64/base64.cpp
                   ../utils/easy-ecc/ecc.c
                   )

target_include_directories(wallet
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_SOURCE_DIR}/src/utils/base64"
        "${CMAKE_SOURCE_DIR}/src/utils/data"
        "${CMAKE_SOURCE_DIR}/src/utils/easy-ecc"
        "${CMAKE_SOURCE_DIR}/src/utils/json"
        "${CMAKE_SOURCE_DIR}/src/utils/sha256"
)