cmake_minimum_required(VERSION 3.12)

project(app
        VERSION 0.1.0
        DESCRIPTION "Application"
        LANGUAGES C CXX
)

add_library(file-api SHARED
        file-api/cache/cache_list/cache_list.c
        file-api/cache/cache_list/cache_list.c
        file-api/cache/cache.c
        file-api/cache/cache.h
        file-api/file-api.c
        file-api/file-api.h
)

add_executable(io-lat-write-2
        io-lat-write/str-tools/str-tools.c
        io-lat-write/str-tools/str-tools.h
        io-lat-write/io-lat-write.c
        io-lat-write/io-lat-write.h
        io-lat-write/main.c
)

target_include_directories(io-lat-write-2 PRIVATE file-api)
target_link_libraries(io-lat-write-2 PUBLIC file-api)
