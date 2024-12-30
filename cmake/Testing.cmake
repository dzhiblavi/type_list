option(TYPE_LIST_BUILD_TESTS "Build tests" ON)

if(TYPE_LIST_BUILD_TESTS AND (TYPE_LIST_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR))
  enable_testing()
  add_subdirectory(tests)
endif()
