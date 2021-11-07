set(CMAKE_CXX_FLAGS_RELEASE "/MT")
set(CMAKE_CXX_FLAGS_DEBUG "/MDd")
file(GLOB SRC "Source/*.cpp")

add_executable(Browser ${SRC} Browser.manifest)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

target_compile_definitions(Browser PRIVATE "$<$<CONFIG:DEBUG>:DEBUG>")
target_include_directories(Browser PRIVATE "${LIBCEF_INCLUDE}")
target_link_directories(Browser PRIVATE "${LIBCEF_LIB}")

include_directories(
    Include
)

target_link_libraries(Browser PRIVATE
	optimized ${LIBCEF_LIB}/libcef.lib
	optimized ${LIBCEF_LIB}/libcef_dll_wrapper.lib
	debug ${LIBCEF_LIB_DEBUG}/libcef.lib
	debug ${LIBCEF_LIB_DEBUG}/libcef_dll_wrapper.lib
)

add_custom_command(
        TARGET Browser POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/Configs/
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/Configs)
