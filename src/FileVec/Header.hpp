#pragma once

#include <iostream>

namespace File
{
enum class DataType : uint8_t
{
  int8 = 0,
  int16,
  int32,
  int64,
  uint8,
  uint16,
  uint32,
  uint64,
  float32,
  float64,
  boolean
};

enum class Endian : uint8_t
{
  big = 0,
  little,
  irrelevant
};

enum class Order : uint8_t
{
  RowMajor = 0,
  ColumnMajor
};

struct Header
{
  static constexpr uint64_t DEFAULT_SIZE = 136;
  uint64_t header_size = DEFAULT_SIZE;
  uint64_t item_count = 0;
  uint64_t max_count = 0;
  DataType data_type = DataType::int32;
};

Header readHeaderFromStream(std::istream& stream);
void writeHeaderToStream(std::ostream& stream, const Header& header);
} // namespace File