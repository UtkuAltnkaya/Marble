file(GLOB PARSER_SRC_FILES "${PROJECT_DIR}/Parser/*.cpp" "${PROJECT_DIR}/Parser/*/*.cpp")
file(GLOB PARSER_HEADER_FILES "${PROJECT_DIR}/Parser/*.hpp" )

add_library(parser STATIC ${PARSER_SRC_FILES} ${PARSER_HEADER_FILES})

target_include_directories(parser
    PUBLIC
        ${PROJECT_DIR}
)
target_link_libraries(parser PRIVATE lexer symbolTable)