#include "Header.hpp"

#include <fstream>
#include <functional>
#include <numeric>

namespace File
{
template <>
DataType File::ToDataType<int8_t>()
{
  return DataType::int8;
}

template <>
DataType File::ToDataType<int16_t>()
{
  return DataType::int16;
}

template <>
DataType File::ToDataType<int32_t>()
{
  return DataType::int32;
}

template <>
DataType File::ToDataType<int64_t>()
{
  return DataType::int64;
}

template <>
DataType File::ToDataType<uint8_t>()
{
  return DataType::uint8;
}

template <>
DataType File::ToDataType<uint16_t>()
{
  return DataType::uint16;
}

template <>
DataType File::ToDataType<uint32_t>()
{
  return DataType::uint32;
}

template <>
DataType File::ToDataType<uint64_t>()
{
  return DataType::uint64;
}

template <>
DataType File::ToDataType<float>()
{
  return DataType::float32;
}

template <>
DataType File::ToDataType<double>()
{
  return DataType::float64;
}

template <>
DataType File::ToDataType<bool>()
{
  return DataType::boolean;
}

std::string Header::FileName()
{
  return ".zarray";
}

Header Header::Create(const shape_type& shape, const shape_type& chunks, DataType dataType, Order order, Endian endian)
{
  Header header;
  header.m_Shape = shape;
  header.m_Chunks = chunks;
  header.m_DataType = dataType;
  header.m_Order = order;
  header.m_Endian = endian;
  header.m_Size = std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<size_type>());
  header.m_ChunkSize = std::accumulate(chunks.begin(), chunks.end(), 1, std::multiplies<>());

  return header;
}

Header Header::Import(const std::filesystem::path& path)
{
  // const std::filesystem::path path = headerPath();
  if(exists(path) == false)
  {
    return {};
  }

  std::ifstream ifstream(path);
  const nlohmann::json json = nlohmann::json::parse(ifstream);
  return FromJson(json);
}

Header Header::FromJson(const nlohmann::json& json)
{
  const shape_type shape = json["shape"].get<shape_type>();
  const shape_type chunks = json["chunks"].get<shape_type>();
  const std::string dataTypeStr = json["dtype"].get<std::string>();
  const double fillValue = json["fill_value"].get<double>();
  const std::string orderStr = json["order"].get<std::string>();

  const DataType dataType = ParseDataTypeString(dataTypeStr);
  const Order order = ParseOrderString(orderStr);
  const Endian endian = ParseEndianString(dataTypeStr);

  return Create(shape, chunks, dataType, order, endian);
}

DataType Header::ParseDataTypeString(const std::string_view& string)
{
  std::string_view substr = string;
  // if(ParseEndianString(string) != Endian::irrelevant)
  //{
  //  // substr = string.substr(1);
  //}

  if(substr.find("i1") != std::string_view::npos)
  {
    return DataType::int8;
  }
  else if(substr.find("i2") != std::string_view::npos)
  {
    return DataType::int16;
  }
  else if(substr.find("i4") != std::string_view::npos)
  {
    return DataType::int32;
  }
  else if(substr.find("i8") != std::string_view::npos)
  {
    return DataType::int64;
  }
  else if(substr.find("u1") != std::string_view::npos)
  {
    return DataType::uint8;
  }
  else if(substr.find("u2") != std::string_view::npos)
  {
    return DataType::uint16;
  }
  else if(substr.find("u4") != std::string_view::npos)
  {
    return DataType::uint32;
  }
  else if(substr.find("u8") != std::string_view::npos)
  {
    return DataType::uint64;
  }
  else if(substr.find("f4") != std::string_view::npos)
  {
    return DataType::float32;
  }
  else if(substr.find("f8") != std::string_view::npos)
  {
    return DataType::float64;
  }
  else if(substr.find("?") != std::string_view::npos)
  {
    return DataType::boolean;
  }

  throw std::runtime_error(std::string("Cannot parse DataType from string"));
}
Order Header::ParseOrderString(const std::string_view& string)
{
  if(string == "C")
  {
    return Order::ColumnMajor;
  }
  if(string == "F")
  {
    return Order::RowMajor;
  }
  return Order::ColumnMajor;
}
Endian Header::ParseEndianString(const std::string_view& string)
{
  std::string_view substr = string.substr(0, 1);
  if(substr == "<")
  {
    return Endian::little;
  }
  if(substr == ">")
  {
    return Endian::big;
  }
  return Endian::irrelevant;
}

Header::Header()
{
}

Header::Header(const Header& other)
: m_Size(other.m_Size)
, m_ChunkSize(other.m_ChunkSize)
, m_Shape(other.m_Shape)
, m_Chunks(other.m_Chunks)
, m_Endian(other.m_Endian)
, m_Order(other.m_Order)
, m_DataType(other.m_DataType)
{
}
Header::Header(Header&& other) noexcept
: m_Size(other.m_Size)
, m_ChunkSize(other.m_ChunkSize)
, m_Shape(std::move(other.m_Shape))
, m_Chunks(std::move(other.m_Chunks))
, m_Endian(other.m_Endian)
, m_Order(other.m_Order)
, m_DataType(other.m_DataType)
{
}

Header::~Header() = default;

Header& Header::operator=(const Header& rhs)
{
  m_Size = rhs.m_Size;
  m_ChunkSize = rhs.m_ChunkSize;
  m_Shape = rhs.m_Shape;
  m_Chunks = rhs.m_Chunks;
  m_Endian = rhs.m_Endian;
  m_Order = rhs.m_Order;
  m_DataType = rhs.m_DataType;

  return *this;
}

Header& Header::operator=(Header&& rhs) noexcept
{
  m_Size = rhs.m_Size;
  m_ChunkSize = rhs.m_ChunkSize;
  m_Shape = std::move(rhs.m_Shape);
  m_Chunks = std::move(rhs.m_Chunks);
  m_Endian = rhs.m_Endian;
  m_Order = rhs.m_Order;
  m_DataType = rhs.m_DataType;

  return *this;
}

Header::size_type Header::size() const
{
  return m_Size;
}

Header::size_type Header::chunkSize() const
{
  return m_ChunkSize;
}

Header::index_type Header::dimensions() const
{
  return m_Shape.size();
}

Header::shape_type Header::shape() const
{
  return m_Shape;
}

Header::shape_type Header::chunks() const
{
  return m_Chunks;
}

DataType Header::dataType() const
{
  return m_DataType;
}

Order Header::order() const
{
  return m_Order;
}

Endian Header::endian() const
{
  return m_Endian;
}

void Header::read(const nlohmann::json& json)
{
  // m_Shape =
  // m_Chunks =
  // m_Endian =
  // m_Order =
  // m_DataType =
  std::accumulate(m_Shape.begin(), m_Shape.end(), 1, std::multiplies<size_type>());
}

nlohmann::json Header::toJson() const
{
  nlohmann::json json;

  return json;
}

uint64_t Header::requiredMemory() const
{
  switch(m_DataType)
  {
  case DataType::int8:
    return size() * sizeof(int8_t);
  case DataType::int16:
    return size() * sizeof(int16_t);
  case DataType::int32:
    return size() * sizeof(int32_t);
  case DataType::int64:
    return size() * sizeof(int64_t);
  case DataType::uint8:
    return size() * sizeof(uint8_t);
  case DataType::uint16:
    return size() * sizeof(uint16_t);
  case DataType::uint32:
    return size() * sizeof(uint32_t);
  case DataType::uint64:
    return size() * sizeof(uint64_t);
  case DataType::boolean:
    return size() * sizeof(uint8_t);
  case DataType::float32:
    return size() * sizeof(float);
  case DataType::float64:
    return size() * sizeof(double);
  }
}
} // namespace File
