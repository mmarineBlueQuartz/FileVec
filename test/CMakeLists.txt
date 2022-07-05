find_package(Catch2 CONFIG REQUIRED)

include(Catch)

add_executable(FileVec_test)

target_link_libraries(FileVec_test
  PRIVATE
  FileVec
  Catch2::Catch2
)

set_target_properties(FileVec_test
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:FileVec>
)

target_compile_definitions(FileVec_test
  PRIVATE
  FILEVEC_BUILD_DIR="$<TARGET_FILE_DIR:FileVec_test>"
)

target_compile_options(FileVec_test
  PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/MP>
)

set(TEST_SOURCE_DIR "${FileVec_SOURCE_DIR}/test")
set(FileVec_TestDataDir "${FileVec_BINARY_DIR}/test")

set(configured_filepath ${FileVec_BINARY_DIR}/generated/FileVec/TestGenConstants.hpp)
configure_file(${CMAKE_CURRENT_LIST_DIR}/cmake/TestGenConstants.hpp.in
  ${FileVec_BINARY_DIR}/generated/FileVec/TestGenConstants.hpp
)

target_sources(FileVec_test
  PRIVATE
  ${TEST_SOURCE_DIR}/filevec_test_main.cpp
  ${TEST_SOURCE_DIR}/test_headers.cpp
  ${TEST_SOURCE_DIR}/test_IO.cpp
  ${TEST_SOURCE_DIR}/test_python.cpp
  ${TEST_SOURCE_DIR}/test_indexMapping.cpp
  ${TEST_SOURCE_DIR}/test_storage.cpp

  ${configured_filepath}
)

add_custom_command(TARGET FileVec_test PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory

  ${CMAKE_SOURCE_DIR}/test/data "${FileVec_TestDataDir}")

# ${CMAKE_SOURCE_DIR}/test/data $<TARGET_FILE_DIR:FileVec_test>)
target_include_directories(FileVec_test PRIVATE ${FileVec_GENERATED_DIR})

catch_discover_tests(FileVec_test)