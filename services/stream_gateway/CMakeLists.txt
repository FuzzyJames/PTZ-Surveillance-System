project(surveillance_stream_gateway)

add_executable(surveillance_stream_gateway
    src/main.cpp
)

target_link_libraries(surveillance_stream_gateway PRIVATE
    surveillance_common
    surveillance_proto
)

# libdatachannel for WebRTC - linked at build time in Docker
# find_package(LibDataChannel REQUIRED)
