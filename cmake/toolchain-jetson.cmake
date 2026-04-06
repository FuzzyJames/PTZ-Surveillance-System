set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# On Jetson, build natively or in NVIDIA L4T container
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

# CUDA configuration for Jetson Orin Nano
set(CMAKE_CUDA_COMPILER /usr/local/cuda/bin/nvcc)
set(CMAKE_CUDA_ARCHITECTURES 87)  # Orin Nano compute capability

# TensorRT paths (JetPack default locations)
set(TENSORRT_ROOT /usr/lib/aarch64-linux-gnu)
set(TENSORRT_INCLUDE /usr/include/aarch64-linux-gnu)

# OpenCV with CUDA support (JetPack default)
set(OpenCV_DIR /usr/lib/aarch64-linux-gnu/cmake/opencv4)

# Note: This toolchain is for building inside the Jetson Docker container.
# The Dockerfile uses nvidia/l4t-tensorrt as base image which has all
# CUDA/TensorRT/OpenCV pre-installed.
