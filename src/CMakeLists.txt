add_subdirectory(ast)
add_subdirectory(utils)
add_subdirectory(symbol)
add_subdirectory(scan_parse)
add_subdirectory(error)
add_subdirectory(weeder)
add_subdirectory(type_checker)
add_subdirectory(pretty_print)
add_subdirectory(asm_code_gen)
add_subdirectory(ast_to_ir)

add_executable(compiler main.c ${SOURCES})

target_link_libraries(compiler PrettyPrint ScanParsePrint AbstractSyntaxTree Weeder Symbol TypeChecker AsmCodeGen AstToIR)
