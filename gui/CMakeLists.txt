project(surveillance_gui)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia MultimediaWidgets Network)

set(GUI_SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/VideoWidget.cpp
    src/PtzController.cpp
    src/CalendarPanel.cpp
    src/DetectionLog.cpp
    src/PlaybackController.cpp
    src/ClipExporter.cpp
)

set(GUI_HEADERS
    src/MainWindow.h
    src/VideoWidget.h
    src/PtzController.h
    src/CalendarPanel.h
    src/DetectionLog.h
    src/PlaybackController.h
    src/ClipExporter.h
)

add_executable(surveillance_gui WIN32
    ${GUI_SOURCES}
    ${GUI_HEADERS}
)

target_link_libraries(surveillance_gui PRIVATE
    surveillance_common
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::MultimediaWidgets
    Qt6::Network
)

# Link gRPC if available (for cluster communication)
if(TARGET surveillance_proto)
    target_link_libraries(surveillance_gui PRIVATE surveillance_proto)
    target_compile_definitions(surveillance_gui PRIVATE HAS_GRPC=1)
endif()

# Install target
install(TARGETS surveillance_gui DESTINATION bin)
