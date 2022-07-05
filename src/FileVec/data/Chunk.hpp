#pragma once

#include "FileVec/data/Header.hpp"
#include "FileVec/util/Encoding.hpp"

#include <filesystem>
#include <fstream>
#include <functional>
#include <sstream>
#include <string>
#include <vector>

namespace File
{
/**
 * @brief the Chunk class handles loading, accessing, and saving a chunk of Array
 * data in memory. Chunks should not need to be manually accessed by the user but
 * instead handled indirectly through the corresponding Array.
 */
template <typename TYPE>
class Chunk
{
  using store_type = std::vector<datawrapper<TYPE>>;

public:
  using index_type = uint64_t;
  using data_type = TYPE;
  using reference = typename store_type::reference;
  using const_reference = typename store_type::const_reference;
  using iterator = typename store_type::iterator;
  using const_iterator = typename store_type::const_iterator;
  using shape_type = std::vector<uint64_t>;

  /**
   * @brief Constructs an empty Chunk with no filepath or index.
   */
  Chunk()
  : m_Path()
  , m_Index()
  , m_DataSize(0)
  , m_Header(nullptr)
  {
  }

  /**
   * @brief Creates a chunk with the target index, size, and value but no filepath.
   * @param index
   * @param size
   * @param header
   */
  Chunk(const shape_type& index, index_type size, const Header& header)
  : m_Path()
  , m_Size(size)
  , m_Index(index)
  , m_Data(size)
  , m_Header(&header)
  , m_DataSize(sizeof(data_type) * size)
  {
    read();
  }

  /**
   * @brief Constructs a Chunk using the specified file path, number of values, and
   * default fill value. Values are read into memory once the chunk index has been
   * determined based on the target filepath.
   * @param filepath
   * @param size
   * @param header
   */
  Chunk(const std::filesystem::path& filepath, index_type size, const Header& header)
  : m_Path(filepath)
  , m_Size(size)
  , m_Data(size)
  , m_Header(&header)
  , m_DataSize(sizeof(data_type) * size)
  {
    parseIndexFromPath();
    read();
  }

  /**
   * @brief Copy constructor
   * @param other
   */
  Chunk(const Chunk& other)
  : m_Path(other.m_Path)
  , m_Size(other.m_Size)
  , m_Data(other.m_Data)
  , m_Header(other.m_Header)
  , m_Index(other.m_Index)
  , m_DataSize(other.m_DataSize)
  {
  }

  /**
   * @brief Move constructor
   * @param other
   */
  Chunk(Chunk&& other) noexcept
  : m_Path(std::move(other.m_Path))
  , m_Size(other.m_Size)
  , m_Data(std::move(other.m_Data))
  , m_Header(std::move(other.m_Header))
  , m_Index(std::move(other.m_Index))
  , m_DataSize(other.m_DataSize)
  {
  }

  /**
   * @brief Writes the Chunk to the specified file before the stored data is
   * deleted from memory.
   */
  virtual ~Chunk() noexcept
  {
    flush();
  }

  /**
   * @brief Checks if the Chunk is valid. Chunks are determined to be valid
   * if the data size in bytes is greater than 0 and there is a known chunk
   * index.
   * @return bool
   */
  bool isValid() const
  {
    return m_DataSize > 0 && m_Index.size() > 0 && m_Header != nullptr;
  }

  /**
   * @brief Checks if the data chunk has a default fill value.
   * @return Returns true if a fill value exists. Otherwise, this method returns false.
   */
  bool hasFillValue() const
  {
    const std::any fillValue = m_Header->fillValue();
    return fillValue.has_value() && fillValue.type() == typeid(TYPE);
  }

  /**
   * @brief Returns the number of values available for the Chunk to access.
   * @return size
   */
  constexpr uint64_t size() const
  {
    return m_Size;
  }

  /**
   * @brief Returns a reference to the value at the target offset.
   * @param index
   * @return reference
   */
  reference operator[](uint64_t index)
  {
    if(index >= m_Size)
    {
      std::stringstream ss;
      ss << "Chunk Index (" << index << ") is out of range";
      throw std::out_of_range(ss.str());
    }

    return m_Data[index];
  }

  /**
   * @brief Returns a const reference to the value at the target offset.
   * @param index
   * @return reference
   */
  const_reference operator[](uint64_t index) const
  {
    if(index >= m_Size)
    {
      std::stringstream ss;
      ss << "Chunk Index (" << index << ") is out of range";
      throw std::out_of_range(ss.str());
    }

    return m_Data[index];
  }

  /**
   * @brief Writes the data to file. If the Chunk is not valid, this method does nothing.
   */
  void flush()
  {
    if(!isValid())
    {
      return;
    }

    const auto encoder = std::dynamic_pointer_cast<AbstractCompressor<TYPE>>(m_Header->encoder());
    std::vector<char> data = encoder->compressData(m_Data, *m_Header);

    std::ofstream ostream(m_Path, std::ios::binary | std::ios::trunc);
    ostream.seekp(0);
    ostream.write(data.data(), data.size());
    ostream.flush();
    ostream.close();
  }

  /**
   * @brief Sets the Chunk index based on the current filepath.
   */
  void parseIndexFromPath()
  {
    std::string filename = m_Path.filename().string();
    std::string tmp;
    std::stringstream ss(filename);
    m_Index.clear();
    while(getline(ss, tmp, '.'))
    {
      uint64_t value = std::stoull(tmp);
      m_Index.push_back(value);
    }
  }

  /**
   * @brief Loads data from the target file. If the file does not exist, the fill
   * value is used for all target values.
   */
  void read()
  {
    if(!isValid())
    {
      return;
    }
    if(!std::filesystem::exists(m_Path))
    {
      m_Data.resize(m_Size);
      if(hasFillValue())
      {
        const TYPE fillValue = std::any_cast<TYPE>(m_Header->fillValue());
        std::fill(m_Data.begin(), m_Data.end(), fillValue);
      }

      return;
    }

    std::ifstream stream(m_Path, std::ios::binary);

    // Compressed byte data is of variable size
    std::streampos fileSize = stream.tellg();
    stream.seekg(0, std::ios::end);
    fileSize = stream.tellg() - fileSize;

    std::vector<char> data(fileSize);

    stream.seekg(0, std::ios::beg);
    stream.read(data.data(), data.size());
    stream.close();

    const auto encoder = std::dynamic_pointer_cast<AbstractCompressor<TYPE>>(m_Header->encoder());
    m_Data = encoder->decompressData(data, *m_Header);
  }

  /**
   * @brief Returns the Chunk's index in N-dimensional space.
   * @return const shape_type&
   */
  const shape_type& index() const
  {
    return m_Index;
  }

  /**
   * @brief Copy constructor. Existing data is flushed before overriding values.
   * @param rhs
   * @return Returns a reference to the Chunk being updated.
   */
  Chunk& operator=(const Chunk& rhs)
  {
    flush();

    m_Path = rhs.m_Path;
    m_Data = rhs.m_Data;
    m_Header = rhs.m_Header;
    m_Index = rhs.m_Index;
    m_DataSize = rhs.m_DataSize;
    m_Size = rhs.m_Size;

    return *this;
  }

  /**
   * @brief Move operator. Flushes data before overwriting values.
   * @param rhs
   * @return Returns a reference to the Chunk being updated.
   */
  Chunk& operator=(Chunk&& rhs) noexcept
  {
    flush();

    m_Path = std::move(rhs.m_Path);
    m_Data = std::move(rhs.m_Data);
    m_Header = std::move(rhs.m_Header);
    m_Index = std::move(rhs.m_Index);
    m_DataSize = rhs.m_DataSize;
    m_Size = rhs.m_Size;

    return *this;
  }

  /**
   * @brief Checks equality with another Chunk. Equality is determined by the filepath and data being identical.
   * @param rhs
   * @return Returns true if values are equal. Returns false otherwise.
   */
  bool operator==(const Chunk& rhs) const
  {
    return (m_Path == rhs.m_Path) && (m_Data == rhs.m_Data);
  }

  /**
   * @brief Creates and returns an iterator to the beginning of the chunk.
   * @return iterator
   */
  iterator begin()
  {
    return m_Data.begin();
  }

  /**
   * @brief Creates and returns an iterator to the end of the chunk.
   * @return iterator
   */
  iterator end()
  {
    return m_Data.end();
  }

  /**
   * @brief Creates and returns a const iterator to the beginning of the chunk.
   * @return const_iterator
   */
  const_iterator begin() const
  {
    return m_Data.begin();
  }

  /**
   * @brief Creates and returns a const iterator to the end of the chunk.
   * @return const_iterator
   */
  const_iterator end() const
  {
    return m_Data.end();
  }

private:
  std::filesystem::path m_Path;
  index_type m_Size = 0;
  shape_type m_Index;
  store_type m_Data;
  const Header* m_Header;
  uint64_t m_DataSize;
};
} // namespace File
