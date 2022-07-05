#include <catch2/catch.hpp>

#include "FileVec/collection/Array.hpp"
#include "FileVec/util/Storage.hpp"

#include <iostream>

TEST_CASE("Check Storage", "[Storage]")
{
  const auto data = File::Storage::checkStorage();
  std::printf("Total Memory: %u\n", data.total);
  std::printf("Free Memory:  %u\n", data.free);
  // std::printf("Available Memory:  %u\n", data.available);
}

#if 0
TEST_CASE("Large Data", "[Storage]")
{
  constexpr uint64_t size = 500000;

  using shape_type = std::vector<uint64_t>;
  const shape_type shape = {size};
  const shape_type chunks = {1028};
  auto vector = File::Array<uint64_t>::Create(shape, chunks);
  std::cout << "Large Data path: " << vector.path().string();

  for(uint64_t i = 0; i < size; ++i)
  {
    vector[i] = i;
  }
  for(uint64_t i = 0; i < size; ++i)
  {
    REQUIRE(vector[i] == i);
  }
}
#endif
