cmake_minimum_required(VERSION 3.16)

project(viMarkdown LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)
set(QT_VERSION_MAJOR 6)

# GLOBを使ってディレクトリ内の全ファイルを自動取得
file(GLOB_RECURSE SOURCE_FILES "viMarkdown/*.cpp")
file(GLOB_RECURSE HEADER_FILES "viMarkdown/*.h")
file(GLOB_RECURSE UI_FILES     "viMarkdown/*.ui")
file(GLOB_RECURSE QRC_FILES    "viMarkdown/*.qrc")

# 実行ファイルの定義
add_executable(viMarkdown
    ${SOURCE_FILES}
    ${HEADER_FILES}
    ${UI_FILES}
    ${QRC_FILES}
)

target_link_libraries(viMarkdown PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# インクルードパスの設定 (ソースファイルがあるディレクトリを通す)
target_include_directories(viMarkdown PRIVATE viMarkdown)
