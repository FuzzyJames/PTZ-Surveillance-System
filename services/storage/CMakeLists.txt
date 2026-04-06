project(surveillance_storage)

find_package(PkgConfig REQUIRED)
pkg_check_modules(FFMPEG REQUIRED libavformat libavcodec libavutil libswscale)

add_executable(surveillance_storage
    src/main.cpp
)

target_include_directories(surveillance_storage PRIVATE
    ${FFMPEG_INCLUDE_DIRS}
)

target_link_libraries(surveillance_storage PRIVATE
    surveillance_common
    surveillance_proto
    ${FFMPEG_LIBRARIES}
)
