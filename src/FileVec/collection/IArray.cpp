#include "FileVec/collection/Array.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

namespace File
{
std::filesystem::path IArray::HeaderPath(const std::filesystem::path& directory)
{
  return directory / Header::FileName();
}

IArray::pointer IArray::Read(const std::filesystem::path& path)
{
  const Header header = Header::Import(path / Header::FileName());
  const shape_type chunkShape = header.chunks();
  const index_type chunkSize = std::accumulate(chunkShape.begin(), chunkShape.end(), 1, std::multiplies<>());

  switch(header.dataType())
  {
  case DataType::int8:
    return Array<int8_t>::Import(header, path);
  case DataType::int16:
    return Array<int16_t>::Import(header, path);
  case DataType::int32:
    return Array<int32_t>::Import(header, path);
  case DataType::int64:
    return Array<int64_t>::Import(header, path);
  case DataType::uint8:
    return Array<uint8_t>::Import(header, path);
  case DataType::uint16:
    return Array<uint16_t>::Import(header, path);
  case DataType::uint32:
    return Array<uint32_t>::Import(header, path);
  case DataType::uint64:
    return Array<uint64_t>::Import(header, path);
  case DataType::float32:
    return Array<float>::Import(header, path);
  case DataType::float64:
    return Array<double>::Import(header, path);
  case DataType::boolean:
    return Array<bool>::Import(header, path);
  }

  return nullptr;
}

IArray::IArray(std::filesystem::path directory)
: BaseCollection(directory)
, m_Header(Header::Import(directory / Header::FileName()))
{
}

IArray::IArray(const Header& header, std::filesystem::path directory)
: BaseCollection(std::move(directory))
, m_Header(header)
{
}

IArray::IArray(const IArray& other) = default;

IArray::IArray(IArray&& other) noexcept
: BaseCollection(other)
, m_Header(std::move(other.m_Header))
{
}

IArray::~IArray() = default;

IArray& IArray::operator=(const IArray& rhs)
{
  BaseCollection::operator=(rhs);
  m_Header = rhs.m_Header;
  return *this;
}

IArray& IArray::operator=(IArray&& rhs) noexcept
{
  BaseCollection::operator=(rhs);
  m_Header = std::move(rhs.m_Header);
  return *this;
}

IArray::index_type IArray::size() const
{
  return m_Header.size();
}

IArray::shape_type IArray::shape() const
{
  return m_Header.shape();
}

IArray::shape_type IArray::chunkShape() const
{
  return m_Header.chunks();
}

IArray::index_type IArray::chunkSize() const
{
  return m_Header.chunkSize();
}

IArray::index_type IArray::dimensions() const
{
  return m_Header.shape().size();
}

const Header& IArray::header() const
{
  return m_Header;
}

std::filesystem::path IArray::headerPath() const
{
  return HeaderPath(path());
}
} // namespace File
