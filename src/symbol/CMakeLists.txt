add_library(Symbol symbol.c decorate_ast.c)

target_link_libraries(Symbol Utilities)
