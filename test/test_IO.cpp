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
    const T value = index * multiplier;
    vector[index] = value;
  }

  for(uint64_t index = 0; index < size; index++)
  {
    const T value = index * multiplier;
    const T storedValue = vector[index];
    REQUIRE(storedValue == value);
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
    const T storedValue = vector[index];
    const T diff = std::abs(storedValue - value);
    REQUIRE(diff <= std::numeric_limits<T>::epsilon());
  }
}

template <typename T>
void testArrayOps(T multiplier)
{
  static constexpr size_t SDIM = 80;
  static constexpr size_t CDIM = 20;

  using shape_type = std::vector<uint64_t>;
  const shape_type shape = {SDIM, SDIM};
  const shape_type chunks = {CDIM, CDIM};
  auto vector = File::Array<T>::Create(shape, chunks);
  testArrayOps(vector, multiplier);
}

template <typename T>
void testArrayOpsf(T multiplier)
{
  static constexpr size_t SDIM = 80;
  static constexpr size_t CDIM = 20;

  using shape_type = std::vector<uint64_t>;
  const shape_type shape = {SDIM, SDIM};
  const shape_type chunks = {CDIM, CDIM};
  auto vector = File::Array<T>::Create(shape, chunks);
  testArrayOpsf(vector, multiplier);
}

TEST_CASE("Array Operators: numeric", "[Array]")
{
  testArrayOps<int8_t>(-1);
  testArrayOps<int16_t>(-2);
  testArrayOps<int32_t>(-3);
  testArrayOps<int64_t>(-4);
  testArrayOps<uint8_t>(1);
  testArrayOps<uint16_t>(2);
  testArrayOps<uint32_t>(3);
  testArrayOps<uint64_t>(4);
}

TEST_CASE("Array Operators: floating point", "[Array]")
{
  testArrayOpsf<float>(0.5f);
  testArrayOpsf<double>(0.75);
}

TEST_CASE("Array Operators: bool", "[Array]")
{
  static constexpr size_t SDIM = 128;
  static constexpr size_t CDIM = 32;

  using shape_type = std::vector<uint64_t>;
  const shape_type shape = {SDIM, SDIM};
  const shape_type chunks = {CDIM, CDIM};
  auto vector = File::Array<bool>::Create(shape, chunks);
  for(uint64_t index = 0; index < 5; index++)
  {
    const bool value = index % 2 == 0;
    vector[index] = value;
    REQUIRE(vector[index] == value);
  }
}
