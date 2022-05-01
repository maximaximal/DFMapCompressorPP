cmake_minimum_required(VERSION 3.10)
project(dfmapcompressorpp)

find_package(Boost COMPONENTS program_options)

set(LIBSRCS
    src/pixelbag.cpp
    src/layer.cpp
    src/map.cpp
    src/tile.cpp
    src/tileset.cpp
    src/stb_image.c
)

add_subdirectory(third_party)

set(APPSRCS
    src/main.cpp
)

add_library(dfmapcompressorpp SHARED ${LIBSRCS})
set_property(TARGET dfmapcompressorpp PROPERTY CXX_STANDARD 20)
target_link_libraries(dfmapcompressorpp PUBLIC xxHash)

add_executable(dfmapcompresspp ${APPSRCS})
set_property(TARGET dfmapcompresspp PROPERTY CXX_STANDARD 20)
target_link_libraries(dfmapcompresspp PUBLIC dfmapcompressorpp Boost::program_options)
