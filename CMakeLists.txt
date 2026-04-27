cmake_minimum_required(VERSION 3.16)
project(QtScrcpy LANGUAGES CXX)

# 对应 CONFIG += c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
    vedioForm.cpp
    vedioForm.h
    vedioForm.ui
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

set(ANDROID_SOURCES
    android/keycodes.h
    android/input.h
)

set(INPUTCONTROL_SOURCES
    inputControl/ControlEvent.h
    inputControl/ControlEvent.cpp
    inputControl/Controller.h
    inputControl/Controller.cpp
    inputControl/InputConvertBase.h
    inputControl/InputConvertBase.cpp
    inputControl/InputConvertNormal.h
    inputControl/InputConvertNormal.cpp
    inputControl/InputConvertGame.h
    inputControl/InputConvertGame.cpp
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
    ${ANDROID_SOURCES}
    ${INPUTCONTROL_SOURCES}
    theme.qrc
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
    ${CMAKE_CURRENT_SOURCE_DIR}/android
    ${CMAKE_CURRENT_SOURCE_DIR}/inputControl
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

# ********************************
# WIN平台下配置
# ********************************
if(WIN32)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set_target_properties(QtScrcpy PROPERTIES 
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output/win/debug
        )
    else()
        set_target_properties(QtScrcpy PROPERTIES 
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output/win/release
        )
    endif()

    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${FFMPEG_LIB_DIR}/avformat.lib
        ${FFMPEG_LIB_DIR}/avcodec.lib
        ${FFMPEG_LIB_DIR}/avutil.lib
        ${FFMPEG_LIB_DIR}/swscale.lib
        ${FFMPEG_LIB_DIR}/swresample.lib
        user32
    )

    # Windows: 复制 DLL 文件
    set(FFMPEG_DLL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/ffmpeg/bin")
    file(GLOB FFMPEG_DLLS "${FFMPEG_DLL_DIR}/avformat*.dll"
                          "${FFMPEG_DLL_DIR}/avcodec*.dll"
                          "${FFMPEG_DLL_DIR}/avutil*.dll"
                          "${FFMPEG_DLL_DIR}/swscale*.dll"
                          "${FFMPEG_DLL_DIR}/swresample*.dll")

    foreach(dll_file ${FFMPEG_DLLS})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${dll_file}
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMENT "Copying ${dll_file} to output directory")
    endforeach()

# ********************************
# LINUX平台下配置
# ********************************

elseif(UNIX AND NOT APPLE)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set_target_properties(QtScrcpy PROPERTIES 
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output/linux/debug
        )
    else()
        set_target_properties(QtScrcpy PROPERTIES 
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output/linux/release
        )
    endif()

    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${FFMPEG_LIB_DIR}/libavformat.a
        ${FFMPEG_LIB_DIR}/libavcodec.a
        ${FFMPEG_LIB_DIR}/libavutil.a
        ${FFMPEG_LIB_DIR}/libswscale.a
        ${FFMPEG_LIB_DIR}/libswresample.a
    )

    # Linux: 复制 .so 文件
    set(FFMPEG_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/ffmpeg/lib")
    file(GLOB FFMPEG_SO_FILES "${FFMPEG_LIB_DIR}/libavformat*.so*" 
                              "${FFMPEG_LIB_DIR}/libavcodec*.so*" 
                              "${FFMPEG_LIB_DIR}/libavutil*.so*" 
                              "${FFMPEG_LIB_DIR}/libswscale*.so*" 
                              "${FFMPEG_LIB_DIR}/libswresample*.so*")
    
    foreach(so_file ${FFMPEG_SO_FILES})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${so_file}
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMENT "Copying ${so_file} to output directory")
    endforeach()

# ********************************
# MAC平台下配置
# ********************************

elseif(APPLE)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set_target_properties(QtScrcpy PROPERTIES 
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output/mac/debug
        )
    else()
        set_target_properties(QtScrcpy PROPERTIES 
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output/mac/release
        )
    endif()

    # 复制scrcpy-server.jar到输出目录
    set(SCRCPY_SERVER_JAR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/scrcpy-server.jar")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SCRCPY_SERVER_JAR}
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "Copying scrcpy-server.jar to output directory")

    # 复制adb到输出目录
    set(ADB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/third_party/adb/mac/adb")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${ADB_PATH}
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "Copying adb to output directory")

    # 设置应用捆绑数据
    set(MACOS_BUNDLE_DATA_DIR "${CMAKE_CURRENT_BINARY_DIR}/macos_bundle_data")
    file(MAKE_DIRECTORY ${MACOS_BUNDLE_DATA_DIR})
    
    # 将adb复制到bundle数据目录
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${ADB_PATH}
        ${MACOS_BUNDLE_DATA_DIR}/adb
        COMMENT "Copying adb to bundle data directory")

    set(PLATFORM_FFMPEG_LIBS "")
    foreach(lib ${FFMPEG_LIBS})
        set(static_lib "${FFMPEG_LIB_DIR}/lib${lib}.a")
        set(shared_lib "${FFMPEG_LIB_DIR}/lib${lib}.dylib")
        if(EXISTS ${static_lib})
            list(APPEND PLATFORM_FFMPEG_LIBS ${static_lib})
        elseif(EXISTS ${shared_lib})
            list(APPEND PLATFORM_FFMPEG_LIBS ${shared_lib})
        else()
            list(APPEND PLATFORM_FFMPEG_LIBS ${lib})  # 使用系统库
        endif()
    endforeach()
    
    # 链接库文件
    target_link_libraries(${PROJECT_NAME} PRIVATE ${PLATFORM_FFMPEG_LIBS})

    # macOS: 复制 .dylib 文件
    set(FFMPEG_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/ffmpeg/lib")
    file(GLOB FFMPEG_DYLIB_FILES "${FFMPEG_LIB_DIR}/libavformat*.dylib" 
                                 "${FFMPEG_LIB_DIR}/libavcodec*.dylib" 
                                 "${FFMPEG_LIB_DIR}/libavutil*.dylib" 
                                 "${FFMPEG_LIB_DIR}/libswscale*.dylib" 
                                 "${FFMPEG_LIB_DIR}/libswresample*.dylib")
    
    foreach(dylib_file ${FFMPEG_DYLIB_FILES})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${dylib_file}
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMENT "Copying ${dylib_file} to output directory")
    endforeach()
endif()