cmake_minimum_required(VERSION 3.16)
project(QtScrcpy LANGUAGES CXX)

# 对应 CONFIG += c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置运行时库（Windows）
if(WIN32)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

# 开启 Qt 自动处理
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 智能兼容 Qt5 和 Qt6
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets Network OpenGL OpenGLWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets Network OpenGL OpenGLWidgets)

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
    server/DeviceSocket.cpp
    server/DeviceSocket.h
    server/TcpServer.h
    server/TcpServer.cpp
)

set(DECODER_SOURCES
    decoder/Decoder.cpp
    decoder/Decoder.h
    decoder/Frames.cpp
    decoder/Frames.h
)

set(COMMON_SOURCES
    common/QScrcpyEvent.h
)

set(RENDER_SOURCES
    render/QYUVOpenGLWidget.cpp
    render/QYUVOpenGLWidget.h
)

set(SCOPE_GUARD_SOURCES
    scopeGuard/ScopeGuard.h
)

file(GLOB ENUMS_SOURCES "enums/*.h")

# 生成可执行文件
add_executable(${PROJECT_NAME}
    ${MAIN_SOURCES}
    ${ADB_SOURCES}
    ${SERVER_SOURCES}
    ${ENUMS_SOURCES}
    ${DECODER_SOURCES}
    ${COMMON_SOURCES}
    ${RENDER_SOURCES}
    ${SCOPE_GUARD_SOURCES}
)

# 包含目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/adb
    ${CMAKE_CURRENT_SOURCE_DIR}/server
    ${CMAKE_CURRENT_SOURCE_DIR}/enums
    ${CMAKE_CURRENT_SOURCE_DIR}/decoder
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/ffmpeg/include
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/scopeGuard
)

# Qt 库链接
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::OpenGL
    Qt${QT_VERSION_MAJOR}::OpenGLWidgets
)

# FFMPEG 库路径和链接
set(FFMPEG_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/ffmpeg/lib")
target_link_directories(${PROJECT_NAME} PRIVATE ${FFMPEG_LIB_DIR})

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        avformat
        avcodec
        avutil
        swscale
        swresample
        ws2_32
        secur32
        shlwapi
        ole32
    )
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE
        avformat
        avcodec
        avutil
        swscale
        swresample
    )
endif()

if(WIN32)
    # 定义FFmpeg DLL目录
    set(FFMPEG_DLL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/ffmpeg/bin")
    
    # 获取所有DLL文件
    file(GLOB FFMPEG_DLLS "${FFMPEG_DLL_DIR}/*.dll")
    
    # 复制DLL到输出目录
    foreach(dll_file ${FFMPEG_DLLS})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${dll_file}
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMENT "Copying ${dll_file} to output directory")
    endforeach()
endif()