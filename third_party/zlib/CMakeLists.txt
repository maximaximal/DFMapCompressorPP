add_subdirectory(zlib-1.2.12)

target_include_directories(zlibstatic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(zlibstatic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/zlib-1.2.12)
target_include_directories(zlibstatic PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/zlib-1.2.12)
