cmake_minimum_required(VERSION 3.10)
project(dfmapcompressorpp)

set(LIBSRCS
    src/pixelbag.cpp
    src/map.cpp
    src/stb_image.c
)

set(APPSRCS
    src/main.cpp
)

add_library(dfmapcompressorpp SHARED ${LIBSRCS})
set_property(TARGET dfmapcompressorpp PROPERTY CXX_STANDARD 20)

add_executable(dfmapcompresspp ${APPSRCS})
set_property(TARGET dfmapcompresspp PROPERTY CXX_STANDARD 20)
target_link_libraries(dfmapcompresspp PUBLIC dfmapcompressorpp)
