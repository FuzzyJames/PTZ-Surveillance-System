FROM arm64v8/debian:bookworm-slim AS builder

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential cmake pkg-config \
    libavformat-dev libavcodec-dev libavutil-dev libswscale-dev \
    libgrpc++-dev libprotobuf-dev protobuf-compiler-grpc \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build
COPY . /src
RUN cmake -S /src -B /build \
    -DBUILD_SERVICES=ON -DBUILD_GUI=OFF \
    -DCMAKE_BUILD_TYPE=Release \
    && cmake --build /build --target surveillance_storage -j$(nproc)

FROM arm64v8/debian:bookworm-slim

RUN apt-get update && apt-get install -y --no-install-recommends \
    libavformat59 libavcodec59 libavutil57 libswscale6 \
    libgrpc++1 ffmpeg \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /build/services/storage/surveillance_storage /usr/local/bin/

# WD Purple mount point
VOLUME ["/mnt/wdpurple"]

EXPOSE 50054

ENTRYPOINT ["surveillance_storage"]
