cmake_minimum_required(VERSION 2.8)
project(Renderer)


include_directories(${CMAKE_CURRENT_LIST_DIR}/)
set(SRCS ${CMAKE_CURRENT_LIST_DIR}/tgaimage.cpp ${CMAKE_CURRENT_LIST_DIR}/model.cpp ${CMAKE_CURRENT_LIST_DIR}/a_gl.cpp)
set(MAIN ${CMAKE_CURRENT_LIST_DIR}/main.cpp)


add_executable(renderer ${MAIN} ${SRCS})
#set(CMAKE_CXX_FLAGS "-O0 -fsanitize=address -fno-omit-frame-pointer  -g3")


# For normal
#set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra")
# For profiling
#set(CMAKE_CXX_FLAGS "-pg -O0")
#SET(CMAKE_EXE_LINKER_FLAGS " -pg")
#SET(CMAKE_SHARED_LINKER_FLAGS " -pg")
target_compile_features(renderer PRIVATE cxx_std_17)
