
file(GLOB TEST_SRC_FILES "${CMAKE_SOURCE_DIR}/tests/*.cpp" "${CMAKE_SOURCE_DIR}/tests/*/*.cpp")
file(GLOB TEST_HEADER_FILES "${CMAKE_SOURCE_DIR}/tests/*.hpp" "${CMAKE_SOURCE_DIR}/tests/*/*.hpp")


add_executable(MarbleTests
    ${TEST_SRC_FILES}
    ${TEST_HEADER_FILES}
)

target_link_libraries(MarbleTests
    gtest_main
    lexer
    parser
    symbolTable
    semanticAnalyzer
    codeGeneration
    errorSystem
)

target_include_directories(MarbleTests PRIVATE
    ${CMAKE_SOURCE_DIR}/tests
    ${CMAKE_SOURCE_DIR}/src
)

include(GoogleTest)
gtest_discover_tests(MarbleTests)
