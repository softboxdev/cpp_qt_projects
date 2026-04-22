# common/CMakeLists.txt

# Создаём статическую библиотеку из общих файлов
set(COMMON_SOURCES
    protocol.cpp
)

set(COMMON_HEADERS
    protocol.h
)

add_library(common STATIC ${COMMON_SOURCES} ${COMMON_HEADERS})

# Указываем include директории
target_include_directories(common 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Подключаем QtCore для QDataStream
target_link_libraries(common
    Qt5::Core
)

# Указываем, что библиотека common нужна для MOC
set_target_properties(common PROPERTIES
    AUTOMOC ON
    AUTOUIC OFF
    AUTORCC OFF
)

# Устанавливаем версию C++ для библиотеки
target_compile_features(common PUBLIC cxx_std_11)
