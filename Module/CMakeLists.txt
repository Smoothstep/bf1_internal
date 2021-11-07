set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI /MDd")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /MANIFEST:NO /INCREMENTAL")

file(GLOB SRC "Source/*.cpp")
file(GLOB ASM "Source/Assembly/*.asm")
file(GLOB HDR "Include/*")

enable_language(ASM_MASM)

add_library(BF1_INF SHARED ${SRC} ${ASM} ${HDR})

include_directories(
    Include
    Source
    Lib
)

target_compile_definitions(BF1_INF PRIVATE "$<$<CONFIG:DEBUG>:DEBUG>")
target_include_directories(BF1_INF PRIVATE "Include/")
target_include_directories(BF1_INF PRIVATE "${LIBCEF_INCLUDE}")

target_link_libraries(BF1_INF PRIVATE
	d3d11.lib
	d3dcompiler.lib
	dxgi.lib
	Shlwapi.lib
	dwrite.lib
	d2d1.lib
	optimized ${LIBCEF_LIB}/libcef.lib
	optimized ${LIBCEF_LIB}/libcef_dll_wrapper.lib
	debug ${LIBCEF_LIB_DEBUG}/libcef.lib
	debug ${LIBCEF_LIB_DEBUG}/libcef_dll_wrapper.lib
)

add_custom_command(
        TARGET BF1_INF POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/Shaders/
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/Shaders)

add_custom_command(
        TARGET BF1_INF POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/Inject.bat
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/Inject.bat)