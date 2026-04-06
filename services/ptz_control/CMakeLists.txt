project(surveillance_ptz_control)

add_executable(surveillance_ptz_control
    src/main.cpp
)

target_link_libraries(surveillance_ptz_control PRIVATE
    surveillance_common
    surveillance_proto
)

# gSOAP for ONVIF PTZ control - linked at build time in Docker
# find_library(GSOAP_LIB gsoap++)
