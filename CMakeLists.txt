cmake_minimum_required(VERSION 3.16)
project(QtScrcpy LANGUAGES CXX)

# 对应 CONFIG += c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 开启 Qt 自动处理（自动运行 moc/uic/rcc，无需手动配置）
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 智能兼容 Qt5 和 Qt6
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets Network)

# 主工程文件
set(MAIN_SOURCES
    main.cpp
    dialog.cpp
    dialog.h
    dialog.ui
)

# 对应 adb/adb.pri 的内容
set(ADB_SOURCES
    adb/adbprocess.cpp
    adb/adbprocess.h
)

# 对应 server/server.pri 的内容
set(SERVER_SOURCES
    server/Server.cpp
    server/Server.h
)

file(GLOB ENUMS_SOURCES "enums/*.h")

# 生成可执行文件（所有 .cpp/.h/.ui 统一放入）
add_executable(${PROJECT_NAME}
    ${MAIN_SOURCES}
    ${ADB_SOURCES}
    ${SERVER_SOURCES}
    ${ENUMS_SOURCES}
)

# 对应 INCLUDEPATH
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/adb
    ${CMAKE_CURRENT_SOURCE_DIR}/server
    ${CMAKE_CURRENT_SOURCE_DIR}/enums
)

# 对应 QT += core gui widgets
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
)