find_package(LLVM REQUIRED CONFIG)

if(NOT LLVM_FOUND)
  message(FATAL_ERROR "LLVM not found!")
endif()

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION} in ${LLVM_DIR}")

include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

set(LLVM_USE_CRT_RELEASE_MT 1)  

function(link_llvm TARGET_NAME)
  set(LLVM_COMPONENTS ${ARGN})
  message(STATUS "Resolved LLVM libs for ${TARGET_NAME}: ${LLVM_LIBS}")
  llvm_map_components_to_libnames(LLVM_LIBS ${LLVM_COMPONENTS})
  target_link_libraries(${TARGET_NAME} PRIVATE ${LLVM_LIBS})
endfunction()
