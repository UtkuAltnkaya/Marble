file(GLOB CODEGEN_SRC_FILES "${PROJECT_DIR}/Codegen/*.cpp" "${PROJECT_DIR}/Codegen/*/*.cpp")
file(GLOB CODEGEN_HEADER_FILES "${PROJECT_DIR}/Codegen/*.hpp" )


add_library(codeGeneration STATIC ${CODEGEN_SRC_FILES} ${CODEGEN_HEADER_FILES})

target_link_libraries(codeGeneration PRIVATE symbolTable errorSystem)
link_llvm(codeGeneration Core Support)

target_include_directories(codeGeneration PUBLIC ${PROJECT_DIR} ${LLVM_INCLUDE_DIRS})
target_compile_definitions(codeGeneration PRIVATE ${LLVM_DEFINITIONS})
