
file(GLOB MARBLE_SRC_FILES "${PROJECT_DIR}/Marble/*.cpp" )
file(GLOB MARBLE_HEADER_FILES "${PROJECT_DIR}/Marble/*.hpp")

add_executable(marble ${MARBLE_SRC_FILES} ${MARBLE_HEADER_FILES})

target_link_libraries(marble PUBLIC errorSystem lexer parser symbolTable semanticAnalyzer codeGeneration)

target_include_directories(marble PUBLIC ${PROJECT_DIR}/Utils )
