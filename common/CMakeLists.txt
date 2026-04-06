add_library(surveillance_common STATIC
    src/config.cpp
    src/logging.cpp
    src/onvif_client.cpp
)

target_include_directories(surveillance_common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

find_package(spdlog CONFIG QUIET)
find_package(nlohmann_json CONFIG QUIET)

if(spdlog_FOUND)
    target_link_libraries(surveillance_common PUBLIC spdlog::spdlog)
endif()

if(nlohmann_json_FOUND)
    target_link_libraries(surveillance_common PUBLIC nlohmann_json::nlohmann_json)
endif()

# Platform libraries for ONVIF HTTP client
if(WIN32)
    target_link_libraries(surveillance_common PUBLIC winhttp)
endif()
