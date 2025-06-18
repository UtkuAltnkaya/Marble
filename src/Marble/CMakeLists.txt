
file(GLOB MARBLE_SRC_FILES "${PROJECT_DIR}/Marble/*.cpp" )
file(GLOB MARBLE_HEADER_FILES "${PROJECT_DIR}/Marble/*.hpp")

add_executable(marble ${MARBLE_SRC_FILES} ${MARBLE_HEADER_FILES})

message(STATUS  ${LLVM_LIBRARIES})

target_link_libraries(marble PUBLIC errorSystem lexer parser symbolTable semanticAnalyzer)
# link_llvm(marble support core irreader)


target_include_directories(marble PUBLIC ${PROJECT_DIR}/Utils ${LLVM_INCLUDE_DIRS})
target_compile_definitions(marble PRIVATE ${LLVM_DEFINITIONS})

# target_compile_options(marble PRIVATE -fsanitize=address -fno-omit-frame-pointer)
# target_link_options(marble PRIVATE -fsanitize=address)