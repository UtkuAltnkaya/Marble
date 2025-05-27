file(GLOB SYMBOL_TABLE_SRC_FILES "${PROJECT_DIR}/SymbolTable/*.cpp")
file(GLOB SYMBOL_TABLE_HEADER_FILES "${PROJECT_DIR}/SymbolTable/*.hpp" )

add_library(symbolTable STATIC ${SYMBOL_TABLE_SRC_FILES} ${SYMBOL_TABLE_HEADER_FILES})

target_include_directories(symbolTable PUBLIC ${PROJECT_DIR})