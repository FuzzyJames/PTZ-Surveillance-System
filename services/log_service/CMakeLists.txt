project(surveillance_log_service)

add_executable(surveillance_log_service
    src/main.cpp
)

target_link_libraries(surveillance_log_service PRIVATE
    surveillance_common
    surveillance_proto
)
