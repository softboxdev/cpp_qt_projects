# agent/CMakeLists.txt

set(AGENT_SOURCES
    main.cpp
    agent.cpp
)

set(AGENT_HEADERS
    agent.h
)

add_executable(system_agent ${AGENT_SOURCES} ${AGENT_HEADERS})

# Подключаем общую библиотеку и Qt
target_link_libraries(system_agent
    common                           # Наша общая библиотека
    Qt5::Core                        # QCoreApplication, QTimer
    Qt5::Network                     # QLocalServer, QLocalSocket
)

# Указываем include директории
target_include_directories(system_agent
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/common   # Чтобы видел protocol.h
)

# Включаем AUTOMOC для обработки Q_OBJECT макросов
set_target_properties(system_agent PROPERTIES
    AUTOMOC ON
    AUTOUIC OFF
    AUTORCC OFF
)

# Устанавливаем имя выходного файла (опционально)
set_target_properties(system_agent PROPERTIES
    OUTPUT_NAME "system_agent"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Указываем, что это консольное приложение (без GUI)
target_compile_definitions(system_agent PRIVATE QT_CORE_LIB)
