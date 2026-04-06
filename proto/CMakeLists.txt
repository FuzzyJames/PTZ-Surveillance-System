find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

# Get protoc and grpc_cpp_plugin executables
get_target_property(PROTOC_EXECUTABLE protobuf::protoc IMPORTED_LOCATION)
get_target_property(GRPC_CPP_PLUGIN gRPC::grpc_cpp_plugin IMPORTED_LOCATION)

set(PROTO_FILE "${CMAKE_CURRENT_SOURCE_DIR}/surveillance.proto")
set(PROTO_GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(MAKE_DIRECTORY ${PROTO_GEN_DIR})

set(PROTO_SRCS "${PROTO_GEN_DIR}/surveillance.pb.cc")
set(PROTO_HDRS "${PROTO_GEN_DIR}/surveillance.pb.h")
set(GRPC_SRCS "${PROTO_GEN_DIR}/surveillance.grpc.pb.cc")
set(GRPC_HDRS "${PROTO_GEN_DIR}/surveillance.grpc.pb.h")

add_custom_command(
    OUTPUT ${PROTO_SRCS} ${PROTO_HDRS} ${GRPC_SRCS} ${GRPC_HDRS}
    COMMAND ${PROTOC_EXECUTABLE}
        --proto_path=${CMAKE_CURRENT_SOURCE_DIR}
        --cpp_out=${PROTO_GEN_DIR}
        --grpc_out=${PROTO_GEN_DIR}
        --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
        ${PROTO_FILE}
    DEPENDS ${PROTO_FILE}
    COMMENT "Generating protobuf and gRPC sources from surveillance.proto"
)

add_library(surveillance_proto STATIC
    ${PROTO_SRCS}
    ${GRPC_SRCS}
)

target_include_directories(surveillance_proto PUBLIC
    ${PROTO_GEN_DIR}
)

target_link_libraries(surveillance_proto PUBLIC
    protobuf::libprotobuf
    gRPC::grpc++
)
