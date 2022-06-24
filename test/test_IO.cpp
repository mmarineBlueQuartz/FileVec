#include <string>

#include <catch2/catch.hpp>

#include <iostream>

#include "FileVec/collection/Array.hpp"

template <typename T>
void testArrayOps(File::Array<T>& vector, T multiplier)
{
  const uint64_t size = vector.size();
  for(uint64_t index = 0; index < size; index++)
  {
    const T value = (index + 1) * multiplier;
    vector[index] = value;
  }

  for(uint64_t index = 0; index < size; index++)
  {
    const T value = (index + 1) * multiplier;
    REQUIRE(vector[index] == value);
  }
}

template <typename T>
void testArrayOpsf(File::Array<T>& vector, T multiplier)
{
  const uint64_t size = vector.size();
  for(uint64_t index = 0; index < size; index++)
  {
    const T value = index * multiplier;
    vector[index] = value;
  }

  for(uint64_t index = 0; index < size; index++)
  {
    const T value = index * multiplier;
    const T diff = std::abs(vector[index] - value);
    REQUIRE(diff < std::numeric_limits<T>::epsilon());
  }
}

template <typename T>
void testArrayOps(T multiplier)
{
  using shape_type = std::vector<uint64_t>;
  const shape_type shape = {4, 4};
  const shape_type chunks = {2, 2};
  auto vector = File::Array<T>::Create(shape, chunks);
  testArrayOps(vector, multiplier);
}

template <typename T>
void testArrayOpsf(T multiplier)
{
  using shape_type = std::vector<uint64_t>;
  const shape_type shape = {5, 5};
  const shape_type chunks = {3, 3};
  auto vector = File::Array<T>::Create(shape, chunks);
  testArrayOpsf(vector, multiplier);
}

TEST_CASE("Array Operators: numeric", "[Array]")
{
  // testArrayOps<int8_t>(-1);
  testArrayOps<int16_t>(-2);
  testArrayOps<int32_t>(-3);
  testArrayOps<int64_t>(-4);
  // testArrayOps<uint8_t>(1);
  testArrayOps<uint16_t>(2);
  testArrayOps<uint32_t>(3);
  testArrayOps<uint64_t>(4);
}

TEST_CASE("Array Operators: floating point", "[Array]")
{
  testArrayOpsf<float>(0.5f);
  testArrayOpsf<double>(0.75);
}

#if 0
TEST_CASE("Array Operators: bool", "[Array]")
{
  using shape_type = std::vector<uint64_t>;
  const shape_type shape = {5, 5};
  const shape_type chunks = {3, 3};
  auto vector = File::Array<bool>::Create(shape, chunks);
  for(uint64_t index = 0; index < 5; index++)
  {
    const bool value = index % 2 == 0;
    vector[index] = value;
    REQUIRE(vector[index] == value);
  }
}
#endif
