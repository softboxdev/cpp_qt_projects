set(AGENT_SOURCES main.cpp agent.cpp)
set(AGENT_HEADERS agent.h)

add_executable(system_agent ${AGENT_SOURCES} ${AGENT_HEADERS})

target_include_directories(system_agent
    PRIVATE
         ${CMAKE_CURRENT_SOURCE_DIR}
         ${CMAKE_SOURCE_DIR}/common
)

target_link_libraries(system_agent
    common
    Qt5::Core
    Qt5::Network
)

set_target_properties(system_agent PROPERTIES
    AUTOMOC ON
    AUTOUIC OFF
    AUTORCC OFF
)

target_compile_definitions(system_agent PRIVATE QT_CORE_LIB)
