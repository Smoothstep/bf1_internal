file(GLOB SRC "Source/*.cpp")
file(GLOB HDR "Include/*")

add_executable(Injector ${SRC} ${HDR})

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

include_directories(
    Include
)