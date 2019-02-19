add_subdirectory(${PROJECT_SOURCE_DIR}/tests/symbol)
add_subdirectory(${PROJECT_SOURCE_DIR}/tests/utils)

add_executable(tests main.c)

target_link_libraries(tests Symbol)
target_link_libraries(tests Utilities)
target_link_libraries(tests TestsSymbol)