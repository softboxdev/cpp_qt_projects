# gui/CMakeLists.txt

set(GUI_SOURCES
    main.cpp
    gui.cpp
)

set(GUI_HEADERS
    gui.h
)

add_executable(system_gui ${GUI_SOURCES} ${GUI_HEADERS})

# Подключаем общую библиотеку и все необходимые Qt модули
target_link_libraries(system_gui
    common                           # Наша общая библиотека
    Qt5::Core                        # Ядро
    Qt5::Widgets                     # GUI элементы
    Qt5::Network                     # QTcpServer, QTcpSocket
    Qt5::Charts                      # QtCharts для графиков
)

# Указываем include директории
target_include_directories(system_gui
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/common
)

# Включаем AUTOMOC для обработки Q_OBJECT макросов
set_target_properties(system_gui PROPERTIES
    AUTOMOC ON
    AUTOUIC ON          # Если будете использовать .ui файлы
    AUTORCC ON          # Если будете использовать ресурсы (qrc)
)

# Устанавливаем имя выходного файла
set_target_properties(system_gui PROPERTIES
    OUTPUT_NAME "system_gui"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Включаем поддержку OpenGL (иногда нужно для Charts)
find_package(OpenGL REQUIRED)
target_link_libraries(system_gui ${OPENGL_LIBRARIES})

# Для отладки: показываем расположение файлов
message(STATUS "GUI sources: ${GUI_SOURCES}")
message(STATUS "GUI headers: ${GUI_HEADERS}")
