cmake_minimum_required(VERSION 3.10)
project(dfmapcompressorpp)

if(CMAKE_CXX_COMPILER MATCHES "/em\\+\\+(-[a-zA-Z0-9.])?$")
    set(CMAKE_CXX_COMPILER_ID "Emscripten")
endif()

if(WIN32 AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "Emscripten")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto")
    set(CMAKE_C_FLAGS_RELEASE "-O3 -flto")

    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    set(CMAKE_EXE_LINKER_FLAGS "-static")
elseif(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Emscripten")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto -fno-use-linker-plugin")
    set(CMAKE_C_FLAGS_RELEASE "-O3 -flto -fno-use-linker-plugin")
    set(CMAKE_CXX_FLAGS_DEBUG "-fsanitize=address -fsanitize=undefined")
    set(CMAKE_C_FLAGS_DEBUG "-fsanitize=address -fsanitize=undefined")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-flto")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Emscripten")
    set(CMAKE_CXX_FLAGS_RELEASE "-Os")
    set(CMAKE_C_FLAGS_RELEASE "-Os")
endif()

set(BUILD_SHARED_LIBS OFF)

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

if(CMAKE_CXX_COMPILER_ID STREQUAL "Emscripten")
    add_executable(dfmapcompressorpp ${LIBSRCS} src/emscripten.cpp)
else()
    add_library(dfmapcompressorpp STATIC ${LIBSRCS})
endif()

set_property(TARGET dfmapcompressorpp PROPERTY CXX_STANDARD 20)
target_link_libraries(dfmapcompressorpp PUBLIC xxHash zlibstatic)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Emscripten")
    target_link_libraries(dfmapcompressorpp PUBLIC embind)
    set_target_properties(dfmapcompressorpp PROPERTIES LINK_FLAGS "-lworkerfs.js -s ENVIRONMENT=worker -s MODULARIZE=1 -s 'EXPORTED_RUNTIME_METHODS=[\"FS\", \"WORKERFS\"]' -s ALLOW_MEMORY_GROWTH=1")
    target_compile_definitions(dfmapcompressorpp PUBLIC "__MUSL__")
endif()

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Emscripten")
    add_executable(dfmapcompresspp ${APPSRCS})
    set_property(TARGET dfmapcompresspp PROPERTY CXX_STANDARD 20)
    target_link_libraries(dfmapcompresspp PUBLIC dfmapcompressorpp)
endif()
