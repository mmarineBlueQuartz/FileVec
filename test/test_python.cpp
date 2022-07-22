#include "catch.hpp"

#include "FileVec/collection/Array.hpp"
#include "FileVec/collection/Group.hpp"

#include <filesystem>
#include <iostream>

#include "FileVec/TestGenConstants.hpp"

//#define PRINTSTREAM

namespace debugging
{
template <typename T>
void printArray(const File::Array<T>& arr)
{
  std::cout << "Size: " << arr.size() << std::endl;
  for(int64_t i = 0; i < arr.size(); i++)
  {
    std::cout << std::to_string(i) << ": " << std::to_string(arr[i]) << std::endl;
  }
}

template <typename T>
void printChunk(const File::Chunk<T>& chunk)
{
  std::cout << "Chunk Size: " << chunk.size() << std::endl;
  for(int64_t i = 0; i < chunk.size(); i++)
  {
    std::cout << std::to_string(i) << ": " << std::to_string(chunk[i]) << std::endl;
  }
}
} // namespace debugging

void testArrayAtPath(const std::filesystem::path& arrayPath)
{
  REQUIRE(std::filesystem::exists(arrayPath));
#ifdef PRINTSTREAM
  std::cout << "Data Path: " << arrayPath.string() << std::endl;
#endif

  auto iArrayPtr = File::IArray::Read(arrayPath);
  REQUIRE(iArrayPtr != nullptr);

  File::Array<int32_t>* arrayPtr = dynamic_cast<File::Array<int32_t>*>(iArrayPtr.get());
  REQUIRE(arrayPtr != nullptr);
  File::Array<int32_t>& array = *arrayPtr;

  REQUIRE(array.size() == 36);
  REQUIRE(array.chunkSize() == 4);

  REQUIRE(array.chunkShape() == std::vector<uint64_t>{2, 2});

  File::Chunk<int32_t> chunk(arrayPath / "0.0", 4, array.header());

#ifdef PRINTSTREAM
  debugging::printArray(array);
  std::cout << std::endl;
  debugging::printChunk(chunk);
  std::cout << std::endl;
#endif

  REQUIRE(array[0] == 2);
  REQUIRE(array[1] == 5);
  REQUIRE(array[6] == 6);

  if(array.header().order() == File::Order::ColumnMajor)
  {
    REQUIRE(chunk[0] == 2);
    REQUIRE(chunk[1] == 5);
    REQUIRE(chunk[2] == 6);
  }
  else
  {
    REQUIRE(chunk[0] == 2);
    REQUIRE(chunk[1] == 6);
    REQUIRE(chunk[2] == 5);
  }
}

TEST_CASE("Read Array w/ Compression", "[Python]")
{
  testArrayAtPath(FileVec::constants::TestDataDir / "group" / "compressionless.zarr");
  testArrayAtPath(FileVec::constants::TestDataDir / "group" / "blosc.zarr");
}

TEST_CASE("Read Python - Generated Group", "[Python]")
{
  auto groupPtr = File::Group::Read(FileVec::constants::TestDataDir / "group");
  REQUIRE(groupPtr != nullptr);
  REQUIRE(groupPtr->find("compressionless.zarr") != groupPtr->end());
  REQUIRE(groupPtr->find("blosc.zarr") != groupPtr->end());

  testArrayAtPath(groupPtr->find("compressionless.zarr")->get()->path());
  testArrayAtPath(groupPtr->find("blosc.zarr")->get()->path());
}

TEST_CASE("Read Row-Major Order Array", "[Python]")
{
  testArrayAtPath(FileVec::constants::TestDataDir / "group" / "order-F.zarr");
}
