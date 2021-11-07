project(BF1_Internal)

cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(LIBS ${LIBS} ${LIBCEF_LIBRARIES})
set(LIBCEF_LIB "" CACHE FILEPATH "Path to libcef libraries")
set(LIBCEF_LIB_DEBUG "" CACHE FILEPATH "Path to libcef debug libraries")
set(LIBCEF_INCLUDE "" CACHE FILEPATH "Path to libcef headers")

add_subdirectory(Module)
add_subdirectory(Injector)
add_subdirectory(Browser)
add_subdirectory(UI)

#include_directories(${LIBCEF_INCLUDE})