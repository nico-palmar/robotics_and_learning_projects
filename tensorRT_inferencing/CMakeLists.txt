# cmake_minimum_required(VERSION 3.10)
# set(CMAKE_CXX_STANDARD 14)
# set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
# set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}" ${CMAKE_MODULE_PATH})

# project(TensorRTSample)

# find_package(Threads)
# find_package(CUDA REQUIRED)
# find_package(OpenCV REQUIRED)
# find_package(TensorRT REQUIRED)

# # TARGETS --------------------------------------------------------------------------------------------------------------
# add_executable(trt_sample trt_sample.cpp)
# target_include_directories(trt_sample PUBLIC ${OpenCV_INCLUDE_DIRS} ${CUDA_INCLUDE_DIRS} ${TensorRT_INCLUDE_DIRS})
# target_link_libraries(trt_sample PUBLIC ${OpenCV_LIBS} ${CUDA_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${TensorRT_LIBRARIES})

cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}" ${CMAKE_MODULE_PATH})
project(TensorRTCheck)

# Specify TensorRT paths
set(TENSORRT_INCLUDE_DIR /usr/include/x86_64-linux-gnu)
set(TENSORRT_LIBRARY_DIR /usr/lib/x86_64-linux-gnu)

# Add include directories
include_directories(${TENSORRT_INCLUDE_DIR})

# Find TensorRT libraries
find_library(TENSORRT_LIBRARY nvinfer
    PATHS ${TENSORRT_LIBRARY_DIR}
    REQUIRED)

find_library(TENSORRT_PARSER_LIBRARY nvonnxparser
    PATHS ${TENSORRT_LIBRARY_DIR}
    REQUIRED)

# find_library(TENSORRT_RUNTIME_LIBRARY nvinfer_runtime
#     PATHS ${TENSORRT_LIBRARY_DIR}
#     REQUIRED)

# If any library is missing, show an error
# if(NOT TENSORRT_LIBRARY OR NOT TENSORRT_PARSER_LIBRARY OR NOT TENSORRT_RUNTIME_LIBRARY)
#     message(FATAL_ERROR "Failed to find TensorRT libraries. Ensure TensorRT is installed correctly.")
# endif()

find_package(CUDA REQUIRED)

include_directories(${CUDA_INCLUDE_DIRS})
link_directories(${CUDA_LIBRARY_DIRS})

add_executable(tensorrt_check main.cpp)
target_link_libraries(tensorrt_check ${TENSORRT_LIBRARY} ${TENSORRT_PARSER_LIBRARY} ${CUDA_LIBRARIES})

add_executable(onnx_parser onnx_parser.cpp)
target_link_libraries(onnx_parser ${TENSORRT_LIBRARY} ${TENSORRT_PARSER_LIBRARY} ${CUDA_LIBRARIES})

