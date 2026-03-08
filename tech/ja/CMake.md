# CMake

## QtCreator で新規プロジェクト作成した場合

```keisen
TestCMake/
├─ .qtcreator/                       # Qt Creator が生成するIDE用設定ディレクトリ（Git管理しないことが多い）
│   ├─ qtc-cmake-presets-RBvjAwKZ/  # Qt Creator が内部で使う CMake Presets（ビルド設定など）
│   └─ CMakeLists.txt.user          # Qt Creator のユーザ固有設定（Kit、ビルド設定、実行設定など）
├─ build/                            # ビルド生成物を格納するディレクトリ（out-of-source build）
│   └─ Desktop_Qt_6_xxx-Debug       # 使用Kit＋ビルドタイプごとのビルドディレクトリ
├─ CMakeLists.txt                    # CMake のメイン設定ファイル（プロジェクト構成・依存ライブラリ定義）
├─ main.cpp                          # アプリケーションのエントリポイント（QApplication生成、MainWindow起動）
├─ mainwindow.cpp                    # MainWindow クラスの実装
├─ mainwindow.h                      # MainWindow クラスの宣言（Qtのシグナル・スロット定義など）
└─ mainwindow.ui                     # Qt Designer で作成されたUIレイアウト（XML形式）
```

```cmake
cmake_minimum_required(VERSION 3.16)

project(TestCMake VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(TestCMake
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET TestCMake APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(TestCMake SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(TestCMake
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(TestCMake PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.TestCMake)
endif()
set_target_properties(TestCMake PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS TestCMake
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(TestCMake)
endif()
```

