add_subdirectory(symbol)
add_subdirectory(utils)

add_executable(compiler main.c ${SOURCES})

target_link_libraries(compiler Symbol)
