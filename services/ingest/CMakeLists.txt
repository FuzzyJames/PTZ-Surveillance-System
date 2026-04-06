project(surveillance_ingest)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0 gstreamer-rtsp-1.0 gstreamer-app-1.0)

add_executable(surveillance_ingest
    src/main.cpp
)

target_include_directories(surveillance_ingest PRIVATE
    ${GSTREAMER_INCLUDE_DIRS}
)

target_link_libraries(surveillance_ingest PRIVATE
    surveillance_common
    surveillance_proto
    ${GSTREAMER_LIBRARIES}
)
