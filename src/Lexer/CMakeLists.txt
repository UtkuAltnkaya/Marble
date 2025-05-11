file(GLOB LEXER_SRC_FILES "${PROJECT_DIR}/Lexer/*.cpp" "${PROJECT_DIR}/Lexer/Token/*.cpp")
file(GLOB LEXER_HEADER_FILES "${PROJECT_DIR}/Lexer/*.hpp" "${PROJECT_DIR}/Lexer/Token/*.hpp")

add_library(lexer STATIC ${LEXER_SRC_FILES} ${LEXER_HEADER_FILES})

target_include_directories(lexer
    PUBLIC
        ${PROJECT_DIR}
)