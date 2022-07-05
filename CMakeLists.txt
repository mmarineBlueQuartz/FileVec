cmake_minimum_required(VERSION 3.19)

include(${CMAKE_CURRENT_LIST_DIR}/cmake/Utility.cmake)

option(FILEVEC_BUILD_TESTS "Enable building FILEVEC tests" ON)
enable_vcpkg_manifest_feature(TEST_VAR FILEVEC_BUILD_TESTS FEATURE "tests")

project(FileVec
  VERSION 0.1.0
  LANGUAGES CXX
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(blosc CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_library(FileVec SHARED)

target_link_libraries(FileVec
  PUBLIC
  blosc
  nlohmann_json::nlohmann_json
)

target_compile_features(FileVec
  PUBLIC
  cxx_std_17
)

if(MSVC)
  target_compile_options(FileVec
    PRIVATE
    /MP
  )
endif()

set(FILEVEC_SOURCECODE_DIR ${FileVec_SOURCE_DIR}/src/FileVec)
set(FILEVEC_GENERATED_DIR ${FileVec_BINARY_DIR}/generated)
set(FILEVEC_GENERATED_HEADER_DIR ${FileVec_BINARY_DIR}/generated/FileVec)
set(FILEVEC_EXPORT_HEADER ${FILEVEC_GENERATED_HEADER_DIR}/filevec_export.hpp)

include(GenerateExportHeader)
generate_export_header(FileVec
  EXPORT_FILE_NAME ${FILEVEC_EXPORT_HEADER}

  # CUSTOM_CONTENT_FROM_VARIABLE FILEVEC_EXPORT_HEADER_EXTRA
)

set(FILEVEC_HDRS
  ${FILEVEC_SOURCECODE_DIR}/collection/IArray.hpp
  ${FILEVEC_SOURCECODE_DIR}/collection/Array.hpp
  ${FILEVEC_SOURCECODE_DIR}/data/Chunk.hpp
  ${FILEVEC_SOURCECODE_DIR}/data/ChunkQueue.hpp
  ${FILEVEC_SOURCECODE_DIR}/data/Header.hpp
  ${FILEVEC_SOURCECODE_DIR}/data/TempDirectory.hpp
  ${FILEVEC_SOURCECODE_DIR}/compressors/AbstractCompressor.hpp
  ${FILEVEC_SOURCECODE_DIR}/compressors/BloscCompressor.hpp
  ${FILEVEC_SOURCECODE_DIR}/compressors/ICompressor.hpp
  ${FILEVEC_SOURCECODE_DIR}/compressors/NullCompressor.hpp
  ${FILEVEC_SOURCECODE_DIR}/util/Algorithm.hpp
  ${FILEVEC_SOURCECODE_DIR}/util/Bit.hpp
  ${FILEVEC_SOURCECODE_DIR}/util/Encoding.hpp
  ${FILEVEC_SOURCECODE_DIR}/util/PathUtil.hpp
  ${FILEVEC_SOURCECODE_DIR}/util/Storage.hpp
)

set(FILEVEC_GENERATED_HEADERS
  ${FILEVEC_EXPORT_HEADER}
)

set(FILEVEC_SRCS
  ${FILEVEC_SOURCECODE_DIR}/collection/IArray.cpp
  ${FILEVEC_SOURCECODE_DIR}/data/Header.cpp
  ${FILEVEC_SOURCECODE_DIR}/data/TempDirectory.cpp
  ${FILEVEC_SOURCECODE_DIR}/compressors/ICompressor.cpp
  ${FILEVEC_SOURCECODE_DIR}/util/PathUtil.cpp
  ${FILEVEC_SOURCECODE_DIR}/util/Storage.cpp
)

target_sources(FileVec
  PRIVATE
  ${FILEVEC_HDRS}
  ${FILEVEC_SRCS}
  ${FILEVEC_GENERATED_HEADERS}
)

set(FILEVEC_INCLUDE_DIR ${FileVec_SOURCE_DIR}/src)

source_group(TREE ${FILEVEC_INCLUDE_DIR} PREFIX "FileVec" FILES ${FILEVEC_HDRS})
source_group(TREE ${FILEVEC_GENERATED_HEADER_DIR} PREFIX "FileVec/generated" FILES ${FILEVEC_GENERATED_HEADERS})
source_group(TREE ${FILEVEC_INCLUDE_DIR} PREFIX "FileVec" FILES ${FILEVEC_SRCS})

target_include_directories(FileVec
  PUBLIC
  $<BUILD_INTERFACE:${FILEVEC_INCLUDE_DIR}>
  $<BUILD_INTERFACE:${FILEVEC_GENERATED_DIR}>
  $<INSTALL_INTERFACE:include>
)

if(FILEVEC_BUILD_TESTS)
  include(CTest)
  add_subdirectory(test)
endif()

set(FILEVEC_PACKAGE_NAMESPACE "FileVec::")

# ------------------------------------------------------------------------------
# Install
option(FILEVEC_ENABLE_INSTALL "Enables FileVec install rules" ON)

if(FILEVEC_ENABLE_INSTALL)
  install(TARGETS FileVec
    EXPORT FileVecTargets
  )

  export(EXPORT FileVecTargets NAMESPACE ${FileVec_PACKAGE_NAMESPACE})

  include(CMakePackageConfigHelpers)

  set(FILEVEC_INSTALL_CMAKE_PREFIX "lib/cmake")

  set(FILEVEC_CONFIG_FILE ${FileVec_BINARY_DIR}/FileVecConfig.cmake)
  set(FILEVEC_VERSION_FILE ${FileVec_BINARY_DIR}/FileVecConfigVersion.cmake)

  configure_package_config_file(${FileVec_SOURCE_DIR}/cmake/FileVecConfig.cmake.in ${FILEVEC_CONFIG_FILE}
    INSTALL_DESTINATION ${FILEVEC_INSTALL_CMAKE_PREFIX}
  )

  write_basic_package_version_file(${FILEVEC_VERSION_FILE}
    COMPATIBILITY SameMajorVersion
  )

  install(FILES ${FILEVEC_CONFIG_FILE} ${FILEVEC_VERSION_FILE}
    DESTINATION ${FILEVEC_INSTALL_CMAKE_PREFIX}
    COMPONENT develop
  )

  include(GNUInstallDirs)
  install(TARGETS FileVec
    RUNTIME
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT runtime
    LIBRARY
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT runtime
    NAMELINK_COMPONENT develop
    ARCHIVE
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT develop
  )

  install_with_directory(FILES ${FILEVEC_HDRS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/FileVec
    COMPONENT develop
    BASE_DIR ${FileVec_SOURCE_DIR}
  )

  install_with_directory(FILES ${FILEVEC_GENERATED_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/FileVec
    COMPONENT develop
    BASE_DIR ${FILEVEC_GENERATED_HEADER_DIR}
  )
endif()
