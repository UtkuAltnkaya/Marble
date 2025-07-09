
file(GLOB SEMANTIC_ANALYZER_SRC_FILES "${PROJECT_DIR}/SemanticAnalyzer/*.cpp" "${PROJECT_DIR}/SemanticAnalyzer/*/*.cpp")
file(GLOB SEMANTIC_ANALYZER_HEADER_FILES "${PROJECT_DIR}/SemanticAnalyzer/*.hpp" )

add_library(semanticAnalyzer STATIC ${SEMANTIC_ANALYZER_SRC_FILES} ${SEMANTIC_ANALYZER_HEADER_FILES})

target_include_directories(semanticAnalyzer PUBLIC ${PROJECT_DIR})

target_link_libraries(semanticAnalyzer PRIVATE symbolTable errorSystem)