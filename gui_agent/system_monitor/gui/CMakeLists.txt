set(GUI_SOURCES
    main.cpp
    gui.cpp
)

set(GUI_HEADERS
    gui.h
)

add_executable(system_gui ${GUI_SOURCES} ${GUI_HEADERS})

target_link_libraries(system_gui
    common
    Qt5::Core
    Qt5::Widgets
    Qt5::Network
)

target_include_directories(system_gui
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/common
)

set_target_properties(system_gui PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
    OUTPUT_NAME "system_gui"
)

message(STATUS "GUI configured (text mode, without Charts)")
