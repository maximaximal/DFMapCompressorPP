set(XXHASH_PATH "xxHash-0.8.1")

set(XXHASHSRCS ${XXHASH_PATH}/xxhash.c)

add_library(xxHash STATIC ${XXHASHSRCS})
target_include_directories(xxHash PUBLIC ${XXHASH_PATH})

set_property(TARGET xxHash PROPERTY POSITION_INDEPENDENT_CODE ON)
