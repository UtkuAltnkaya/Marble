file(GLOB CODEGEN_SRC_FILES "${PROJECT_DIR}/Codegen/*.cpp" "${PROJECT_DIR}/Codegen/*/*.cpp")
file(GLOB CODEGEN_HEADER_FILES "${PROJECT_DIR}/Codegen/*.hpp" )


add_library(codegen STATIC ${CODEGEN_SRC_FILES} ${CODEGEN_HEADER_FILES})

target_link_libraries(codegen PRIVATE symbolTable errorSystem)
link_llvm(codegen Core Support)

target_include_directories(codegen PUBLIC ${PROJECT_DIR} ${LLVM_INCLUDE_DIRS})
target_compile_definitions(codegen PRIVATE ${LLVM_DEFINITIONS})
