add_subdirectory(utils)
add_subdirectory(wallet)

add_executable(app main.cpp)

target_link_libraries(app wallet)


