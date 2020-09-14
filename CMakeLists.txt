cmake_minimum_required(VERSION 3.8)
project(TextureProcessor)

if (NOT TARGET clean-core)
    message(FATAL_ERROR "[texture-processor] clean-core must be available")
endif()

# =========================================
# global options


# =========================================
# define library

file(GLOB_RECURSE SOURCES "src/*.cc")
file(GLOB_RECURSE HEADERS "src/*.hh")

arcana_add_library(TP texture-processor SOURCES HEADERS)

target_include_directories(texture-processor PUBLIC src/)

target_link_libraries(texture-processor PUBLIC
    clean-core
    typed-geometry
    babel-serializer # TODO: make optional
)


# =========================================
# set up compile flags
