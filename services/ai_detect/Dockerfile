# Based on NVIDIA L4T TensorRT container for Jetson Orin Nano
FROM nvcr.io/nvidia/l4t-tensorrt:r8.6.2-runtime AS builder

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential cmake pkg-config \
    libopencv-dev \
    libtesseract-dev leptonica-dev \
    libgrpc++-dev libprotobuf-dev protobuf-compiler-grpc \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build
COPY . /src
RUN cmake -S /src -B /build \
    -DBUILD_SERVICES=ON -DBUILD_GUI=OFF \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=/src/cmake/toolchain-jetson.cmake \
    && cmake --build /build --target surveillance_ai_detect -j$(nproc)

FROM nvcr.io/nvidia/l4t-tensorrt:r8.6.2-runtime

RUN apt-get update && apt-get install -y --no-install-recommends \
    libopencv-core4.5d libopencv-dnn4.5d libopencv-imgproc4.5d \
    libtesseract4 \
    libgrpc++1 \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /build/services/ai_detect/surveillance_ai_detect /usr/local/bin/
COPY services/ai_detect/models/ /models/

EXPOSE 50052

ENV NVIDIA_VISIBLE_DEVICES=all
ENV NVIDIA_DRIVER_CAPABILITIES=compute,utility,video

ENTRYPOINT ["surveillance_ai_detect"]
