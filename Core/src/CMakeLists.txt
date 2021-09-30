cmake_minimum_required(VERSION 2.6.0)

project(libefusion)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}")

find_package(Pangolin 0.7 REQUIRED)
find_package(CUDA REQUIRED)
find_package(SuiteSparse REQUIRED)

include(GNUInstallDirs)

set(efusion_SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Shaders" CACHE PATH "Where the shaders live")

include_directories(${Pangolin_INCLUDE_DIRS})
include_directories(${CUDA_INCLUDE_DIRS})
include_directories(${EIGEN_INCLUDE_DIRS})
include_directories(${SUITESPARSE_INCLUDE_DIRS})

file(GLOB srcs *.cpp)
file(GLOB utils_srcs Utils/*.cpp)
file(GLOB shader_srcs Shaders/*.cpp)
file(GLOB cuda Cuda/*.cu)
file(GLOB containers Cuda/containers/*.cpp)

if(WIN32)
  file(GLOB hdrs *.h)
  file(GLOB utils_hdrs Utils/*.h)
  file(GLOB shader_hdrs Shaders/*.h)
  file(GLOB cuda_hdrs Cuda/*.cuh)
  file(GLOB containers_hdrs Cuda/containers/*.hpp)
endif()

set(CUDA_ARCH_BIN "35 50 52 61" CACHE STRING "Specify 'real' GPU arch to build binaries for, BIN(PTX) format is supported. Example: 1.3 2.1(1.3) or 13 21(13)")
set(CUDA_ARCH_PTX "" CACHE STRING "Specify 'virtual' PTX arch to build PTX intermediate code for. Example: 1.0 1.2 or 10 12")              

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})
include(CudaComputeTargetFlags.cmake)                  
APPEND_TARGET_ARCH_FLAGS()

set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS}  "-Xcompiler;-fPIC;")           
set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} "--ftz=true;--prec-div=false;--prec-sqrt=false") 

CUDA_COMPILE(cuda_objs ${cuda})

if(WIN32)
  set(ADDITIONAL_CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
endif()

set(CMAKE_CXX_FLAGS ${ADDITIONAL_CMAKE_CXX_FLAGS} "-O3 -msse2 -msse3 -Wall -std=c++11 -DSHADER_DIR=${efusion_SHADER_DIR}")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -std=c++11 -DSHADER_DIR=${efusion_SHADER_DIR}")
  
if(WIN32)
  add_definitions(-DWIN32_LEAN_AND_MEAN)
  add_definitions(-DNOMINMAX)
  set (EXTRA_WINDOWS_LIBS ${EXTRA_WINDOWS_LIBS} ws2_32)
endif()

add_library(efusion SHARED 
            ${srcs}
            ${utils_srcs}
            ${shader_srcs}
            ${cuda} 
            ${cuda_objs} 
            ${containers}
            ${hdrs}
            ${utils_hdrs}
            ${shader_hdrs}
            ${cuda_hdrs} 
            ${containers_hdrs}
)

target_link_libraries(efusion
                      ${Eigen_LIBRARIES}
                      ${Pangolin_LIBRARIES}
                      ${CUDA_LIBRARIES}
                      ${SUITESPARSE_LIBRARIES}
					  ${EXTRA_WINDOWS_LIBS}
)

set_property(TARGET efusion PROPERTY CXX_STANDARD 17)

target_include_directories(efusion PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>"
    "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>/${PROJECT_NAME};${Pangolin_INCLUDE_DIRS}")

INSTALL(TARGETS efusion EXPORT efusion
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib
)

set(PUBLIC_HEADERS
  "Ferns.h"
  "GPUTexture.h"
  "PoseMatch.h"
  "Utils/CholeskyDecomp.h"
  "Utils/OrderedJacobianRow.h"
  "Utils/Stopwatch.h"
  "Utils/GPUConfig.h"
  "Utils/Jacobian.h"
  "Utils/GraphNode.h"
  "Utils/Img.h"
  "Utils/DeformationGraph.h"
  "Utils/RGBDOdometry.h"
  "Utils/Resolution.h"
  "Utils/WindowsExtras.h"
  "Utils/Parse.h"
  "Utils/OdometryProvider.h"
  "Utils/Intrinsics.h"
  "Deformation.h"
  "ElasticFusion.h"
  "Defines.h"
  "GlobalModel.h"
  "IndexMap.h"
  "Shaders/FillIn.h"
  "Shaders/Vertex.h"
  "Shaders/Uniform.h"
  "Shaders/ComputePack.h"
  "Shaders/Resize.h"
  "Shaders/FeedbackBuffer.h"
  "Shaders/Shaders.h"
)

foreach(HEADER ${PUBLIC_HEADERS})
    string(REGEX MATCH "(.*)[/\\]" DIR ${HEADER})
    install(FILES ${HEADER} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/${DIR})
endforeach()

INSTALL(EXPORT efusion
    DESTINATION share/${PROJECT_NAME}/cmake
    FILE ${PROJECT_NAME}Config.cmake
)
