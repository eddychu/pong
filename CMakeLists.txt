cmake_minimum_required(VERSION 3.20)

project(pong)

add_executable(${PROJECT_NAME} main.c)

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main)

find_package(sdl2_image CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2_image)

find_package(SDL2_ttf CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2_ttf)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})