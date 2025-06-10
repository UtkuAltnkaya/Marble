file(GLOB ERROR_SYSTEM_SRC_FILES "${PROJECT_DIR}/ErrorSystem/*.cpp" "${PROJECT_DIR}/ErrorSystem/*/*.cpp")
file(GLOB ERROR_SYSTEM_HEADER_FILES "${PROJECT_DIR}/ErrorSystem/*.hpp" )

add_library(errorSystem STATIC ${ERROR_SYSTEM_SRC_FILES} ${ERROR_SYSTEM_HEADER_FILES})

target_include_directories(errorSystem PUBLIC ${PROJECT_DIR})