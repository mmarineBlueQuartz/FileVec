#pragma once

#include "nlohmann/json.hpp"

#include <any>
#include <filesystem>
#include <memory>
#include <vector>

#include "FileVec/filevec_export.hpp"

namespace File
{
enum class FILEVEC_EXPORT DataType : uint8_t
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

enum class FILEVEC_EXPORT Endian : uint8_t
{
  little = 0,
  big,
  irrelevant
};

enum class FILEVEC_EXPORT Order : uint8_t
{
  RowMajor = 0,
  ColumnMajor,
  Default = ColumnMajor
};

class ICompressor;

class FILEVEC_EXPORT Header
{
public:
  using size_type = uint64_t;
  using index_type = uint64_t;
  using shape_type = std::vector<index_type>;

  static Header Create(const shape_type& shape, const shape_type& chunks, DataType dataType, Order order, Endian endian, std::any fillValue, std::shared_ptr<ICompressor> encoder = nullptr);
  static Header Import(const std::filesystem::path& path);
  static Header FromJson(const nlohmann::json& json);

  static std::string FileName();

  Header();
  Header(const Header& other);
  Header(Header&& other) noexcept;
  virtual ~Header();

  size_type size() const;
  size_type chunkSize() const;
  index_type dimensions() const;

  shape_type shape() const;
  shape_type chunks() const;
  DataType dataType() const;
  std::any fillValue() const;
  Order order() const;
  Endian endian() const;
  std::shared_ptr<ICompressor> encoder() const;

  nlohmann::json toJson() const;

  uint64_t requiredMemory() const;

  Header& operator=(const Header& rhs);
  Header& operator=(Header&& rhs) noexcept;

protected:
  static DataType ParseDataTypeString(const std::string_view& string);
  static Order ParseOrderString(const std::string_view& string);
  static Endian ParseEndianString(const std::string_view& string);
  static std::unique_ptr<ICompressor> ParseEncoderJson(DataType dataType, const nlohmann::json& json);

private:
  size_type m_Size = 0;
  size_type m_ChunkSize = 0;
  shape_type m_Shape;
  shape_type m_Chunks;
  std::any m_FillValue;
  Endian m_Endian = Endian::big;
  Order m_Order = Order::Default;
  DataType m_DataType = DataType::int32;
  std::shared_ptr<ICompressor> m_Compressor = nullptr;
};

template <typename T>
DataType ToDataType()
{
  throw std::runtime_error("Unsupported DataType conversion");
}

template <>
DataType FILEVEC_EXPORT ToDataType<int8_t>();
template <>
DataType FILEVEC_EXPORT ToDataType<int16_t>();
template <>
extern DataType FILEVEC_EXPORT ToDataType<int32_t>();
template <>
extern DataType FILEVEC_EXPORT ToDataType<int64_t>();
template <>
extern DataType FILEVEC_EXPORT ToDataType<uint8_t>();
template <>
extern DataType FILEVEC_EXPORT ToDataType<uint16_t>();
template <>
extern DataType FILEVEC_EXPORT ToDataType<uint32_t>();
template <>
extern DataType FILEVEC_EXPORT ToDataType<uint64_t>();
template <>
extern DataType FILEVEC_EXPORT ToDataType<float>();
template <>
extern DataType FILEVEC_EXPORT ToDataType<double>();
template <>
extern DataType FILEVEC_EXPORT ToDataType<bool>();
} // namespace File