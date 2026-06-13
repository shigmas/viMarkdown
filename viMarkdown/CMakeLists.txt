cmake_minimum_required(VERSION 3.16)

add_compile_definitions(BUILD_WITH_CMAKE)

project(viMarkdown VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools PrintSupport Svg)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools PrintSupport Svg)

#target_include_directories(viMarkdown PRIVATE 
#	"${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/lunasvg/include"
#	"${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/lunasvg/3rdparty/plutovg/include"
#)
add_subdirectory(3rdparty/lunasvg)
add_subdirectory(src)

if(WIN32)
    target_link_libraries(viMarkdown PRIVATE imm32)
endif()

