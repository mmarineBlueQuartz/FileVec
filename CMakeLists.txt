cmake_minimum_required(VERSION 3.19)

project(FileVec
  VERSION 0.1.0
  LANGUAGES CXX
)

add_executable(FileVec)

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

set(FILEVEC_SOURCE_DIR ${FileVec_SOURCE_DIR}/src/FileVec)

set(FILEVEC_HDRS
  ${FILEVEC_SOURCE_DIR}/Block.hpp
  ${FILEVEC_SOURCE_DIR}/BlockQueue.hpp
  ${FILEVEC_SOURCE_DIR}/Header.hpp
  ${FILEVEC_SOURCE_DIR}/Vector.hpp
)

set(FILEVEC_SRCS
  ${FILEVEC_SOURCE_DIR}/Header.cpp
  ${FILEVEC_SOURCE_DIR}/main.cpp
)

target_sources(FileVec
  PRIVATE
    ${FILEVEC_HDRS}
    ${FILEVEC_SRCS}
)

source_group(TREE ${FILEVEC_SOURCE_DIR} PREFIX "FileVec" FILES ${FILEVEC_HDRS})
source_group(TREE ${FILEVEC_SOURCE_DIR} PREFIX "FileVec" FILES ${FILEVEC_SRCS})
