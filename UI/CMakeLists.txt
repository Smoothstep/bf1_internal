add_custom_target (UI ALL
	cd ${CMAKE_CURRENT_SOURCE_DIR} COMMAND npm install && 
	cd ${CMAKE_CURRENT_SOURCE_DIR} COMMAND npm run build
)
add_custom_command(
        TARGET UI POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/dist/
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/dist)