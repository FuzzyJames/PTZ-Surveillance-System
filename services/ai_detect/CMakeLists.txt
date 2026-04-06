project(surveillance_ai_detect LANGUAGES CXX CUDA)

find_package(CUDA REQUIRED)
find_package(OpenCV REQUIRED)

# TensorRT
find_library(NVINFER nvinfer)
find_library(NVONNXPARSER nvonnxparser)

add_executable(surveillance_ai_detect
    src/main.cpp
)

target_include_directories(surveillance_ai_detect PRIVATE
    ${CUDA_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
    ${TENSORRT_INCLUDE}
)

target_link_libraries(surveillance_ai_detect PRIVATE
    surveillance_common
    surveillance_proto
    ${OpenCV_LIBS}
    ${CUDA_LIBRARIES}
    ${NVINFER}
    ${NVONNXPARSER}
)
