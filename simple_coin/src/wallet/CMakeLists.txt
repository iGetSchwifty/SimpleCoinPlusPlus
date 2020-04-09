add_library(wallet wallet.cpp wallet.hpp)

target_include_directories(wallet PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})