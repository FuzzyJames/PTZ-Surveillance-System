FROM arm64v8/debian:bookworm-slim AS builder

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential cmake pkg-config \
    libgrpc++-dev libprotobuf-dev protobuf-compiler-grpc \
    libgsoap-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build
COPY . /src
RUN cmake -S /src -B /build \
    -DBUILD_SERVICES=ON -DBUILD_GUI=OFF \
    -DCMAKE_BUILD_TYPE=Release \
    && cmake --build /build --target surveillance_ptz_control -j$(nproc)

FROM arm64v8/debian:bookworm-slim

RUN apt-get update && apt-get install -y --no-install-recommends \
    libgrpc++1 libgsoap-2.8.117 \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /build/services/ptz_control/surveillance_ptz_control /usr/local/bin/

EXPOSE 50053

ENTRYPOINT ["surveillance_ptz_control"]
