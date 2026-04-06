cmake_minimum_required(VERSION 3.20)
project(SurveillanceSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options to control which components to build
option(BUILD_GUI "Build the Windows GUI application" ON)
option(BUILD_SERVICES "Build cluster microservices" OFF)
option(BUILD_TESTS "Build unit tests" OFF)

# ---- vcpkg integration ----
# Auto-detect vcpkg: check VCPKG_ROOT env var, then known install locations
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(DEFINED ENV{VCPKG_ROOT})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "Vcpkg toolchain file")
    elseif(EXISTS "C:/Users/matta/source/vcpkg/vcpkg-master/scripts/buildsystems/vcpkg.cmake")
        set(CMAKE_TOOLCHAIN_FILE "C:/Users/matta/source/vcpkg/vcpkg-master/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "Vcpkg toolchain file")
    endif()
endif()

# ---- Find common dependencies ----
find_package(Protobuf CONFIG QUIET)
find_package(gRPC CONFIG QUIET)

# ---- Protobuf / gRPC code generation ----
if(Protobuf_FOUND AND gRPC_FOUND)
    add_subdirectory(proto)
endif()

# ---- Common library (shared utilities) ----
add_subdirectory(common)

# ---- Cluster services (Linux ARM64 builds) ----
if(BUILD_SERVICES)
    add_subdirectory(services/ingest)
    add_subdirectory(services/ai_detect)
    add_subdirectory(services/ptz_control)
    add_subdirectory(services/storage)
    add_subdirectory(services/stream_gateway)
    add_subdirectory(services/log_service)
endif()

# ---- Windows GUI ----
if(BUILD_GUI)
    add_subdirectory(gui)
endif()

# ---- Test executables (always available) ----
add_executable(test_onvif tests/test_onvif.cpp)
target_link_libraries(test_onvif PRIVATE surveillance_common)

message(STATUS "============================================")
message(STATUS "  Surveillance System v${PROJECT_VERSION}")
message(STATUS "  BUILD_GUI:      ${BUILD_GUI}")
message(STATUS "  BUILD_SERVICES: ${BUILD_SERVICES}")
message(STATUS "  BUILD_TESTS:    ${BUILD_TESTS}")
message(STATUS "============================================")
