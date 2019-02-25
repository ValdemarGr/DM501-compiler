find_package(BISON)
find_package(FLEX)

BISON_TARGET(Parser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.c)
FLEX_TARGET(Scanner scanner.l ${CMAKE_CURRENT_BINARY_DIR}/scanner.c)
ADD_FLEX_BISON_DEPENDENCY(Scanner Parser)

SET(SOURCES ${BISON_Parser_OUTPUTS} ${FLEX_Scanner_OUTPUTS})

include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_executable(PrettyPrint scan_parse.c pretty.c ${SOURCES})

# Ugly, but otherwise it cannot find tree.h when compiling our parser
target_include_directories(PrettyPrint PUBLIC ${PROJECT_SOURCE_DIR}/src/scan_parse)

target_link_libraries(PrettyPrint AbstractSyntaxTree Weeder Symbol)
