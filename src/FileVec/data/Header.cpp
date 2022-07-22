#include "Header.hpp"

#include "FileVec/compressors/BloscCompressor.hpp"
#include "FileVec/compressors/NullCompressor.hpp"

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

std::unique_ptr<ICompressor> createDefaultCompressor(DataType dataType)
{
  switch(dataType)
  {
  case DataType::int8:
    return std::make_unique<BloscCompressor<int8_t>>();
  case DataType::int16:
    return std::make_unique<BloscCompressor<int16_t>>();
  case DataType::int32:
    return std::make_unique<BloscCompressor<int32_t>>();
  case DataType::int64:
    return std::make_unique<BloscCompressor<int64_t>>();
  case DataType::uint8:
    return std::make_unique<BloscCompressor<uint8_t>>();
  case DataType::uint16:
    return std::make_unique<BloscCompressor<uint16_t>>();
  case DataType::uint32:
    return std::make_unique<BloscCompressor<uint32_t>>();
  case DataType::uint64:
    return std::make_unique<BloscCompressor<uint64_t>>();
  case DataType::float32:
    return std::make_unique<BloscCompressor<float>>();
  case DataType::float64:
    return std::make_unique<BloscCompressor<double>>();
  case DataType::boolean:
    return std::make_unique<BloscCompressor<bool>>();
  }

  throw std::runtime_error("Cannot resolve DataType during compressor creation");
}

std::unique_ptr<ICompressor> createBloscCompressor(DataType dataType, const nlohmann::json& json)
{
  switch(dataType)
  {
  case DataType::int8:
    return BloscCompressor<int8_t>::FromJson(json);
  case DataType::int16:
    return BloscCompressor<int16_t>::FromJson(json);
  case DataType::int32:
    return BloscCompressor<int32_t>::FromJson(json);
  case DataType::int64:
    return BloscCompressor<int64_t>::FromJson(json);
  case DataType::uint8:
    return BloscCompressor<uint8_t>::FromJson(json);
  case DataType::uint16:
    return BloscCompressor<uint16_t>::FromJson(json);
  case DataType::uint32:
    return BloscCompressor<uint32_t>::FromJson(json);
  case DataType::uint64:
    return BloscCompressor<uint64_t>::FromJson(json);
  case DataType::float32:
    return BloscCompressor<float>::FromJson(json);
  case DataType::float64:
    return BloscCompressor<double>::FromJson(json);
  case DataType::boolean:
    return BloscCompressor<bool>::FromJson(json);
  }

  throw std::runtime_error("Cannot resolve DataType during compressor creation");
}

std::unique_ptr<ICompressor> createNullCompressor(DataType dataType)
{
  switch(dataType)
  {
  case DataType::int8:
    return std::make_unique<NullCompressor<int8_t>>();
  case DataType::int16:
    return std::make_unique<NullCompressor<int16_t>>();
  case DataType::int32:
    return std::make_unique<NullCompressor<int32_t>>();
  case DataType::int64:
    return std::make_unique<NullCompressor<int64_t>>();
  case DataType::uint8:
    return std::make_unique<NullCompressor<uint8_t>>();
  case DataType::uint16:
    return std::make_unique<NullCompressor<uint16_t>>();
  case DataType::uint32:
    return std::make_unique<NullCompressor<uint32_t>>();
  case DataType::uint64:
    return std::make_unique<NullCompressor<uint64_t>>();
  case DataType::float32:
    return std::make_unique<NullCompressor<float>>();
  case DataType::float64:
    return std::make_unique<NullCompressor<double>>();
  case DataType::boolean:
    return std::make_unique<NullCompressor<bool>>();
  }

  throw std::runtime_error("Cannot resolve DataType during compressor creation");
}

Header Header::Create(const shape_type& shape, const shape_type& chunks, DataType dataType, Order order, Endian endian, std::any fillValue, std::shared_ptr<ICompressor> compressor)
{
  Header header;
  header.m_Shape = shape;
  header.m_Chunks = chunks;
  header.m_DataType = dataType;
  if(compressor == nullptr)
  {
    // header.m_Compressor = createDefaultCompressor(dataType);
    header.m_Compressor = createNullCompressor(dataType);
  }
  else
  {
    header.m_Compressor = std::move(compressor);
  }

  header.m_FillValue = std::move(fillValue);
  header.m_Order = order;
  header.m_Endian = endian;
  header.m_Size = std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<size_type>());
  header.m_ChunkSize = std::accumulate(chunks.begin(), chunks.end(), 1, std::multiplies<>());

  return header;
}

Header Header::Import(const std::filesystem::path& path)
{
  if(exists(path) == false)
  {
    return {};
  }

  std::ifstream ifstream(path);
  return FromJson(nlohmann::json::parse(ifstream));
}

Header Header::FromJson(const nlohmann::json& json)
{
  const shape_type shape = json["shape"].get<shape_type>();
  const shape_type chunks = json["chunks"].get<shape_type>();
  const std::string dataTypeStr = json["dtype"].get<std::string>();
  const std::any fillValue = json["fill_value"].get<double>();
  const std::string orderStr = json["order"].get<std::string>();

  const DataType dataType = ParseDataTypeString(dataTypeStr);
  const Order order = ParseOrderString(orderStr);
  const Endian endian = ParseEndianString(dataTypeStr);

  std::shared_ptr<ICompressor> encoder = createDefaultCompressor(dataType);
  if(json.contains("compressor"))
  {
    const nlohmann::json encoderJson = json["compressor"];
    encoder = ParseEncoderJson(dataType, encoderJson);
  }

  return Create(shape, chunks, dataType, order, endian, fillValue, encoder);
}

DataType Header::ParseDataTypeString(const std::string_view& string)
{
  const std::string_view substr = string;

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

std::unique_ptr<ICompressor> Header::ParseEncoderJson(DataType dataType, const nlohmann::json& json)
{
  if(json.is_null())
  {
    return createNullCompressor(dataType);
  }

  auto idJson = json["id"];
  if(!json.contains("id"))
  {
    return createDefaultCompressor(dataType);
  }
  std::string id = json["id"].get<std::string>();
  if(id == "blosc")
  {
    return createBloscCompressor(dataType, json);
  }

  return nullptr;
}

Header::Header() = default;

Header::Header(const Header& other) = default;

Header::Header(Header&& other) noexcept
: m_Size(other.m_Size)
, m_ChunkSize(other.m_ChunkSize)
, m_Shape(std::move(other.m_Shape))
, m_Chunks(std::move(other.m_Chunks))
, m_FillValue(std::move(other.m_FillValue))
, m_Endian(other.m_Endian)
, m_Order(other.m_Order)
, m_DataType(other.m_DataType)
, m_Compressor(std::move(other.m_Compressor))
{
}

Header::~Header() = default;

Header& Header::operator=(const Header& rhs)
{
  if(&rhs == this)
  {
    return *this;
  }

  m_Size = rhs.m_Size;
  m_ChunkSize = rhs.m_ChunkSize;
  m_Shape = rhs.m_Shape;
  m_Chunks = rhs.m_Chunks;
  m_FillValue = rhs.m_FillValue;
  m_Endian = rhs.m_Endian;
  m_Order = rhs.m_Order;
  m_DataType = rhs.m_DataType;
  m_Compressor = rhs.m_Compressor;

  return *this;
}

Header& Header::operator=(Header&& rhs) noexcept
{
  if(&rhs == this)
  {
    return *this;
  }

  m_Size = rhs.m_Size;
  m_ChunkSize = rhs.m_ChunkSize;
  m_Shape = std::move(rhs.m_Shape);
  m_Chunks = std::move(rhs.m_Chunks);
  m_FillValue = rhs.m_FillValue;
  m_Endian = rhs.m_Endian;
  m_Order = rhs.m_Order;
  m_DataType = rhs.m_DataType;
  m_Compressor = std::move(rhs.m_Compressor);

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

std::any Header::fillValue() const
{
  return m_FillValue;
}

Order Header::order() const
{
  return m_Order;
}

Endian Header::endian() const
{
  return m_Endian;
}

std::shared_ptr<ICompressor> Header::encoder() const
{
  return m_Compressor;
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
