cmake_minimum_required(VERSION 3.16)

file(GLOB LUNASVG_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/plutovg/source/*.c"
)

add_library(lunasvg STATIC
    ${LUNASVG_SOURCES}
)
target_include_directories(lunasvg PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/plutovg/include
)

#target_compile_definitions(lunasvg PUBLIC
#    LUNASVG_BUILD_STATIC
#    PLUTOVG_BUILD_STATIC
#)
