add_subdirectory(wallet)
add_subdirectory(server)

add_executable(wallet_app wallet_app.cpp)
add_executable(server_app server_app.cpp)

target_link_libraries(wallet_app wallet)

target_link_libraries(server_app server)


