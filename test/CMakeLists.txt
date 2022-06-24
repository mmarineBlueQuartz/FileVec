find_package(Catch2 CONFIG REQUIRED)

include(Catch)

set(FileVec_TestDataDir "${FileVec_SOURCE_DIR}/test/data")

set(configured_filepath ${FileVec_BINARY_DIR}/generated/FileVec/TestGenConstants.hpp)
configure_file(${CMAKE_CURRENT_LIST_DIR}/cmake/TestGenConstants.hpp.in
  ${FileVec_BINARY_DIR}/generated/FileVec/TestGenConstants.hpp
)

add_executable(FileVec_test
  filevec_test_main.cpp
  test_headers.cpp
  test_IO.cpp
  test_python.cpp
  test_indexMapping.cpp
  test_storage.cpp
  ${configured_filepath}
)

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

target_include_directories(FileVec_test PRIVATE ${FileVec_GENERATED_DIR})

catch_discover_tests(FileVec_test)