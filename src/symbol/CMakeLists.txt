add_library(Symbol symbol.c symbol_from_ast.c)

target_link_libraries(Symbol Utilities)
