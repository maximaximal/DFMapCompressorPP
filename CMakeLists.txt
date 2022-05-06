cmake_minimum_required(VERSION 3.10)
project(dfmapcompressorpp)

if(WIN32)
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    set(CMAKE_C_FLAGS_RELEASE "-O3")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--stack,10000000")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto -fno-use-linker-plugin -fsanitize=address -fsanitize=undefined")
    set(CMAKE_C_FLAGS_RELEASE "-O3 -flto -fno-use-linker-plugin -fsanitize=address -fsanitize=undefined")
    set(CMAKE_CXX_FLAGS_DEBUG "-fsanitize=address -fsanitize=undefined")
    set(CMAKE_C_FLAGS_DEBUG "-fsanitize=address -fsanitize=undefined")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-flto")
endif()

set(LIBSRCS
    src/pixelbag.cpp
    src/layer.cpp
    src/map.cpp
    src/tile.cpp
    src/fdf-map.cpp
    src/tileset.cpp
    src/stb_image.c
)

add_subdirectory(third_party)

set(APPSRCS
    src/main.cpp
)

add_library(dfmapcompressorpp STATIC ${LIBSRCS})
set_property(TARGET dfmapcompressorpp PROPERTY CXX_STANDARD 20)
target_link_libraries(dfmapcompressorpp PUBLIC xxHash zlibstatic)

add_executable(dfmapcompresspp ${APPSRCS})
set_property(TARGET dfmapcompresspp PROPERTY CXX_STANDARD 20)
target_link_libraries(dfmapcompresspp PUBLIC dfmapcompressorpp)
