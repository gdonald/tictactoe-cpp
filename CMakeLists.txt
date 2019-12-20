cmake_minimum_required(VERSION 3.10.2)
project(tictactoe)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

if(NOT APPLE AND NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

include(FindPackageHandleStandardArgs)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_gfx REQUIRED)
find_package(SDL2_ttf REQUIRED)

include_directories(include
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        ${SDL2_GFX_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS})

add_executable(tictactoe
        src/Move.cpp
        src/Board.cpp
        src/Game.cpp
        src/main.cpp)

target_link_libraries(tictactoe
        ${SDL2_LIBRARIES}
        ${SDL2_IMAGE_LIBRARIES}
        ${SDL2_GFX_LIBRARIES}
        ${SDL2_TTF_LIBRARIES})

add_custom_command(TARGET tictactoe
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:tictactoe> ..)
