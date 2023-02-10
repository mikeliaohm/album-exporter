cmake_minimum_required(VERSION 3.5)

project(album-exporter VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools Sql)

set(TS_FILES album-exporter_zh_TW.ts)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.ui
        dialog.ui
        ProjectSettings.cpp ProjectSettings.h
        Task.cpp Task.h
        mainwindow.cpp mainwindow.h
        DbManager.cpp DbManager.h
        KeyPressEater.cpp KeyPressEater.h
        CheckableSqlQueryModel.cpp CheckableSqlQueryModel.h
        TaskExe.cpp TaskExe.h
        ${TS_FILES}
)

if(APPLE)
#! [appicon_macOS]
    set(MACOSX_BUNDLE_ICON_FILE album-exporter.icns)
#    # And the following tells CMake where to find and install the file itself.
    set(app_icon_macos "${CMAKE_CURRENT_SOURCE_DIR}/resources/album-exporter.icns")
    set_source_files_properties(${app_icon_macos} PROPERTIES
           MACOSX_PACKAGE_LOCATION "Resources")
#! [appicon_macOS]
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(album-exporter
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${app_icon_macos}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET album-exporter APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(album-exporter SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(album-exporter
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(album-exporter PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql)

set_target_properties(album-exporter PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER mikeliao.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
    WIN32_EXECUTABLE TRUE
)

install(TARGETS album-exporter
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(album-exporter)
endif()
