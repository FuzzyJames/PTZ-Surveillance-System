FROM arm64v8/debian:bookworm-slim AS builder

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential cmake pkg-config \
    libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
    libgstreamer-plugins-bad1.0-dev \
    gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly \
    gstreamer1.0-libav gstreamer1.0-rtsp \
    libgrpc++-dev libprotobuf-dev protobuf-compiler-grpc \
    libgsoap-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build
COPY . /src
RUN cmake -S /src -B /build \
    -DBUILD_SERVICES=ON -DBUILD_GUI=OFF \
    -DCMAKE_BUILD_TYPE=Release \
    && cmake --build /build --target surveillance_ingest -j$(nproc)

FROM arm64v8/debian:bookworm-slim

RUN apt-get update && apt-get install -y --no-install-recommends \
    gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly \
    gstreamer1.0-libav gstreamer1.0-rtsp \
    libgstreamer1.0-0 libgstreamer-plugins-base1.0-0 \
    libgrpc++1 \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /build/services/ingest/surveillance_ingest /usr/local/bin/

EXPOSE 50051 5004/udp 5006/udp

ENTRYPOINT ["surveillance_ingest"]
