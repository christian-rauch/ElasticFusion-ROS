cmake_minimum_required(VERSION 2.6.0)

project(GPUTest)

find_package(Pangolin 0.1 REQUIRED)
find_package(CUDA REQUIRED)

find_package(libefusion REQUIRED)

include_directories(/usr/include/suitesparse)
include_directories(${CUDA_INCLUDE_DIRS})
include_directories(${EIGEN_INCLUDE_DIRS})
include_directories(${Pangolin_INCLUDE_DIRS})
include_directories(${libefusion_INCLUDE_DIR})

file(GLOB srcs *.cpp)

set(CMAKE_CXX_FLAGS "-O3 -msse2 -msse3 -Wall -std=c++11")
#set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} " -g -Wall")

add_executable(GPUTest 
               ${srcs}
)

target_link_libraries(GPUTest
                      ${libefusion_LIBRARY}
                      ${Eigen_LIBRARIES}
                      ${Pangolin_LIBRARIES}
                      ${CUDA_LIBRARIES}
                      ${SUITESPARSE_LIBRARIES}
)

