FROM arm64v8/debian:bookworm-slim AS builder

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential cmake pkg-config git \
    libgrpc++-dev libprotobuf-dev protobuf-compiler-grpc \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

# Build libdatachannel from source for WebRTC
RUN git clone --depth 1 https://github.com/nicehash/libdatachannel.git /tmp/libdatachannel \
    && cmake -S /tmp/libdatachannel -B /tmp/libdatachannel/build \
        -DCMAKE_BUILD_TYPE=Release -DNO_EXAMPLES=ON -DNO_TESTS=ON \
    && cmake --build /tmp/libdatachannel/build -j$(nproc) \
    && cmake --install /tmp/libdatachannel/build

WORKDIR /build
COPY . /src
RUN cmake -S /src -B /build \
    -DBUILD_SERVICES=ON -DBUILD_GUI=OFF \
    -DCMAKE_BUILD_TYPE=Release \
    && cmake --build /build --target surveillance_stream_gateway -j$(nproc)

FROM arm64v8/debian:bookworm-slim

RUN apt-get update && apt-get install -y --no-install-recommends \
    libgrpc++1 libssl3 \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /build/services/stream_gateway/surveillance_stream_gateway /usr/local/bin/
COPY --from=builder /usr/local/lib/libdatachannel* /usr/local/lib/

EXPOSE 50055 8080 5004/udp

ENTRYPOINT ["surveillance_stream_gateway"]
