project(surveillance_gui)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)
find_package(Qt6 COMPONENTS Multimedia MultimediaWidgets QUIET)

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
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
)

if(TARGET Qt6::Multimedia)
    target_link_libraries(surveillance_gui PRIVATE Qt6::Multimedia)
    target_compile_definitions(surveillance_gui PRIVATE HAS_QT_MULTIMEDIA=1)
endif()
if(TARGET Qt6::MultimediaWidgets)
    target_link_libraries(surveillance_gui PRIVATE Qt6::MultimediaWidgets)
endif()

# Link gRPC if available (for cluster communication)
if(TARGET surveillance_proto)
    target_link_libraries(surveillance_gui PRIVATE surveillance_proto)
    target_compile_definitions(surveillance_gui PRIVATE HAS_GRPC=1)
endif()

# Install target
install(TARGETS surveillance_gui DESTINATION bin)
