cmake_minimum_required(VERSION 2.8.4)
project(ChickenShooter)

find_file(SDL2_INCLUDE_DIR NAME SDL.h HINTS SDL2)
find_library(SDL2_LIBRARY NAME SDL2)
add_executable(ChickenShooter main.cpp)
target_include_directories(ChickenShooter ${SDL2_INCLUDE_DIR})
target_link_libraries(ChickenShooter ${SDL2_LIBRARY})    


set(SDL2_INCLUDE_DIR C:/SDL/SDL2-2.0.3/include)
set(SDL2_LIBRARY C:/SDL/SDL2-2.0.3/lib/x64)

include_directories(${SDL2_INCLUDE_DIR})
set(SOURCE_FILES main.cpp)

add_executable(ChickenShooter ${SOURCE_FILES})
target_link_libraries(ChickenShooter ${SDL2_LIBRARY})
