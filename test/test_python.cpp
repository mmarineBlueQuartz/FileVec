#include <catch2/catch.hpp>

#include "FileVec/collection/Array.hpp"

#include <filesystem>
#include <iostream>

#include "FileVec/TestGenConstants.hpp"

#define PRINTSTREAM 0

TEST_CASE("Read Python-Generated Array", "[Python]")
{
  const std::filesystem::path path(FileVec::constants::TestDataDir / "python1.zarr");
  REQUIRE(std::filesystem::exists(path));
  std::cout << "Data Path: " << path.string() << std::endl;

  auto iArrayPtr = File::IArray::Read(path);
  REQUIRE(iArrayPtr != nullptr);

  File::Array<int32_t>* arrayPtr = dynamic_cast<File::Array<int32_t>*>(iArrayPtr.get());
  REQUIRE(arrayPtr != nullptr);
  File::Array<int32_t>& array = *arrayPtr;

  uint64_t maxSize = array.size();
  uint64_t chunkSize = array.chunkSize();
  REQUIRE(array.size() == 36);
  REQUIRE(array.chunkSize() == 4);

  REQUIRE(array.chunkShape() == std::vector<uint64_t>{2, 2});
  std::cout << "Size: " << maxSize << std::endl;
  std::cout << "Chunk Size: " << array.chunkSize() << std::endl;

#if PRINTSTREAM
  for(int64_t i = 0; i < array.size(); i++)
  {
    std::cout << std::to_string(i) << ": " << std::to_string(array[i]) << std::endl;
  }

  auto chunkPath = path / "0.0";

  File::Chunk<int32_t> chunk(chunkPath, 16);
  for(int64_t i = 0; i < chunk.size(); i++)
  {
    std::cout << std::to_string(i) << ": " << std::to_string(chunk[i]) << std::endl;
  }
#endif

  REQUIRE(array[0] == 2);
  REQUIRE(array[1] == 5);
  REQUIRE(array[3] == 6);
}
