FROM arm64v8/debian:bookworm-slim AS builder

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential cmake pkg-config \
    libgrpc++-dev libprotobuf-dev protobuf-compiler-grpc \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build
COPY . /src
RUN cmake -S /src -B /build \
    -DBUILD_SERVICES=ON -DBUILD_GUI=OFF \
    -DCMAKE_BUILD_TYPE=Release \
    && cmake --build /build --target surveillance_log_service -j$(nproc)

FROM arm64v8/debian:bookworm-slim

RUN apt-get update && apt-get install -y --no-install-recommends \
    libgrpc++1 \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /build/services/log_service/surveillance_log_service /usr/local/bin/

VOLUME ["/mnt/wdpurple"]

EXPOSE 50056

ENTRYPOINT ["surveillance_log_service"]
