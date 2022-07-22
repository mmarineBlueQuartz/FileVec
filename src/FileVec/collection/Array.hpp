#pragma once

#include "FileVec/collection/IArray.hpp"
#include "FileVec/data/ChunkQueue.hpp"
#include "FileVec/data/Header.hpp"
#include "FileVec/util/Algorithm.hpp"
#include "FileVec/util/PathUtil.hpp"

#include <array>
#include <cstdint>
#include <filesystem>
#include <iterator>

namespace File
{
/**
 * @brief The Array class serves as the main class used for accessing fixed-size
 * out-of-core data. Data is stored in chunks within a target file directory based
 * on its position within N-dimensional space. If a directory is not specified, it
 * is placed in a temporary directory determined by the user's operating system. On
 * Windows, this location is ~/AppData/Local/Temp/FileCore/# where '#' corresponds
 * to the current Array index incremented when creating the target directory.
 *
 * All temporary data is cleared when the application closes.
 *
 * Array data is laid out in N-dimensional space and chunked using the same number
 * of dimensions. In additional, the order and endian values can be specified to
 * change the way the data is saved to memory. This information, in addition to the
 * data type, is saved to the corresponding header file for loading at a later time
 * using only a target path.
 *
 * Despite data being constructed using data chunks in N-dimensional space, values
 * are accessed using 1-dimensional indexing similar to a flat std::array<T, N>
 * where N is the number of values in the array.
 */
template <typename T, uint64_t MAX_BLOCK_COUNT = 6>
class Array : public IArray
{
public:
  using value_type = T;
  using index_type = uint64_t;
  using size_type = size_t;
  using chunk_type = Chunk<T>;
  using reference = typename chunk_type::reference;
  using const_reference = typename chunk_type::const_reference;

  class iterator
  {
  public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using difference_type = int64_t;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using index_type = uint64_t;

    /**
     * @brief Default iterator required for some standard library algorithm implementations.
     */
    iterator()
    : m_Array(nullptr)
    , m_Index(0)
    {
    }

    iterator(Array& arr, index_type index)
    : m_Array(&arr)
    , m_Index(index)
    {
    }

    iterator(const iterator&) = default;
    iterator(iterator&&) noexcept = default;

    iterator& operator=(const iterator&) = default;
    iterator& operator=(iterator&&) noexcept = default;

    ~iterator() noexcept = default;

    bool isValid() const
    {
      if(m_Array == nullptr)
      {
        return false;
      }
      return m_Index < m_Array->size();
    }

    iterator operator+(uint64_t offset) const
    {
      return iterator(*m_Array, m_Index + offset);
    }

    iterator operator-(index_type offset) const
    {
      return iterator(*m_Array, m_Index - offset);
    }

    iterator& operator+=(index_type offset)
    {
      m_Index += offset;
      return *this;
    }

    iterator& operator-=(index_type offset)
    {
      m_Index -= offset;
      return *this;
    }

    // prefix
    iterator& operator++()
    {
      m_Index++;
      return *this;
    }

    // postfix
    iterator operator++(int)
    {
      iterator iter = *this;
      m_Index++;
      return iter;
    }

    // prefix
    iterator& operator--()
    {
      m_Index--;
      return *this;
    }

    // postfix
    iterator operator--(int)
    {
      iterator iter = *this;
      m_Index--;
      return iter;
    }

    difference_type operator-(const iterator& rhs) const
    {
      return m_Index - rhs.m_Index;
    }

    reference operator*() const
    {
      return (*m_Array)[m_Index];
    }

    pointer operator->()
    {
      return &(*m_Array)[m_Index];
    }

    const_pointer operator->() const
    {
      return &(*m_Array)[m_Index];
    }

    bool operator==(const iterator& rhs) const
    {
      if(!isValid() && !rhs.isValid())
      {
        return true;
      }
      if(!isValid() || !rhs.isValid())
      {
        return false;
      }
      return (m_Array == rhs.m_Array) && (m_Index == rhs.m_Index);
    }

    bool operator!=(const iterator& rhs) const
    {
      return !(*this == rhs);
    }

    bool operator<(const iterator& rhs) const
    {
      return m_Index < rhs.m_Index;
    }

    bool operator>(const iterator& rhs) const
    {
      return m_Index > rhs.m_Index;
    }

    bool operator<=(const iterator& rhs) const
    {
      return m_Index <= rhs.m_Index;
    }

    bool operator>=(const iterator& rhs) const
    {
      return m_Index >= rhs.m_Index;
    }

  private:
    Array* m_Array = nullptr;
    index_type m_Index = 0;
  };

  class const_iterator
  {
  public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using difference_type = int64_t;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using index_type = uint64_t;

    /**
     * @brief Default iterator required for some standard library algorithm implementations.
     */
    const_iterator()
    : m_Array(nullptr)
    , m_Index(0)
    {
    }

    const_iterator(const Array& arr, index_type index)
    : m_Array(&arr)
    , m_Index(index)
    {
    }

    const_iterator(const const_iterator&) = default;
    const_iterator(const_iterator&&) noexcept = default;

    const_iterator& operator=(const const_iterator&) = default;
    const_iterator& operator=(const_iterator&&) noexcept = default;

    ~const_iterator() noexcept = default;

    bool isValid() const
    {
      if(m_Array == nullptr)
      {
        return false;
      }
      return m_Index < m_Array->size();
    }

    const_iterator operator+(uint64_t offset) const
    {
      return const_iterator(*m_Array, m_Index + offset);
    }

    const_iterator operator-(index_type offset) const
    {
      return const_iterator(*m_Array, m_Index - offset);
    }

    const_iterator& operator+=(index_type offset)
    {
      m_Index += offset;
      return *this;
    }

    const_iterator& operator-=(index_type offset)
    {
      m_Index -= offset;
      return *this;
    }

    // prefix
    const_iterator& operator++()
    {
      m_Index++;
      return *this;
    }

    // postfix
    const_iterator operator++(int)
    {
      const_iterator iter = *this;
      m_Index++;
      return iter;
    }

    // prefix
    const_iterator& operator--()
    {
      m_Index--;
      return *this;
    }

    // postfix
    const_iterator operator--(int)
    {
      const_iterator iter = *this;
      m_Index--;
      return iter;
    }

    difference_type operator-(const const_iterator& rhs) const
    {
      return m_Index - rhs.m_Index;
    }

    reference operator*() const
    {
      return (*m_Array)[m_Index];
    }

    const_pointer operator->() const
    {
      return &(*m_Array)[m_Index];
    }

    bool operator==(const const_iterator& rhs) const
    {
      if(!isValid() && !rhs.isValid())
      {
        return true;
      }
      if(!isValid() || !rhs.isValid())
      {
        return false;
      }
      return (m_Array == rhs.m_Array) && (m_Index == rhs.m_Index);
    }

    bool operator!=(const const_iterator& rhs) const
    {
      return !(*this == rhs);
    }

    bool operator<(const const_iterator& rhs) const
    {
      return m_Index < rhs.m_Index;
    }

    bool operator>(const const_iterator& rhs) const
    {
      return m_Index > rhs.m_Index;
    }

    bool operator<=(const const_iterator& rhs) const
    {
      return m_Index <= rhs.m_Index;
    }

    bool operator>=(const const_iterator& rhs) const
    {
      return m_Index >= rhs.m_Index;
    }

  private:
    const Array* m_Array = nullptr;
    index_type m_Index = 0;
  };

  /**
   * @brief Creates and returns a temporary Array with the specified shape, chunk shape,
   * order, and endian values. Temporary arrays are cleaned up once the application closes.
   *
   * Order and Endian values are still a work in progress and have no affect upon the
   * resulting Array.
   *
   * @param shape Target shape of the data in N-dimensional space.
   * @param chunks Shape of the data chunks in memory. This must have the same number of
   * values as provided in 'shape'.
   * @param order Placeholder argument. This specifies if the data is stored in row or
   * column major order. Defaults to ColumnMajor.
   * @param fillValue Value used when data chunks cannot be found. Defaults to 0.
   * @param endian Placeholder argument. This specifies if the data is stored using big
   * or little endian or if it is unknown in which case it uses the system default.
   * @return Temporary Array
   */
  static Array Create(const shape_type& shape, const shape_type& chunks, Order order = Order::Default, std::any fillValue = 0, Endian endian = Endian::irrelevant)
  {
    std::filesystem::path path = File::Util::createTempPath();
    Header header = Header::Create(shape, chunks, File::ToDataType<T>(), order, endian, fillValue);
    return Array(header, path);
  }

  /**
   * @brief Creates and returns a temporary Array pointer with the specified shape, chunk shape,
   * order, and endian values. Temporary arrays are cleaned up once the application closes.
   *
   * Order and Endian values are still a work in progress and have no affect upon the
   * resulting Array.
   *
   * @param shape Target shape of the data in N-dimensional space.
   * @param chunks Shape of the data chunks in memory. This must have the same number of
   * values as provided in 'shape'.
   * @param order Placeholder argument. This specifies if the data is stored in row or
   * column major order. Defaults to ColumnMajor.
   * @param endian Placeholder argument. This specifies if the data is stored using big
   * or little endian or if it is unknown in which case it uses the system default.
   * @return Smart pointer to the resulting temporary Array
   */
  static pointer CreatePtr(const shape_type& shape, const shape_type& chunks, Order order = Order::ColumnMajor, Endian endian = Endian::irrelevant)
  {
    return std::make_unique<Array>(Create(shape, chunks, order, endian));
  }

  /**
   * @brief Returns a smart pointer to ann Array using the provided Header and directory path.
   * @param header Header to use for the Array.
   * @param path Directory of the data to import.
   */
  static pointer Import(const Header& header, const std::filesystem::path& path)
  {
    return std::make_unique<Array>(header, path);
  }

  /**
   * @brief Constructs a file-backed array using the directory at the specified file path.
   * If a file already exists at the target path, values are read from the target file.
   *
   * This constructor assumes that an Array already exists at the target directory.
   * @param directory
   */
  Array(const std::filesystem::path& directory)
  : IArray(directory)
  {
  }

  /**
   * @brief Constructs a file-backed array using the provided header and target directory.
   * @param header
   * @param directory
   */
  Array(const Header& header, const std::filesystem::path& directory)
  : IArray(header, directory)
  {
  }

  /**
   * @briefCopy constructor
   * @param other
   */
  Array(const Array& other) = delete;

  /**
   * @brief Move constructor
   * @param other
   */
  Array(Array&& other) noexcept
  : IArray(std::move(other))
  , m_BlockQueue(std::move(other.m_BlockQueue))
  {
  }

  /**
   * @brief Destroys the Array after writing cached chunks to memory.
   */
  virtual ~Array() = default;

  /**
   * @brief Returns a reference to the value at the target index after checking if the index is within bounds.
   * @param index
   * @return reference
   */
  reference at(index_type index)
  {
    if(index >= m_Size)
    {
      throw std::out_of_range("Target File::Array has size: " + std::to_string(size()));
    }
    return operator[](index);
  }

  /**
   * @brief Returns a const reference to the value at the target index after checking if the index is within bounds
   * @param index
   * @return const_reference
   */
  const_reference at(index_type index) const
  {
    if(index >= m_Size)
    {
      throw std::out_of_range("Target File::Array has size: " + std::to_string(size()));
    }
    return operator[](index);
  }

  /**
   * @brief Returns a reference to the value found at the target index.
   * @param index
   * @return reference
   */
  reference operator[](index_type index)
  {
    chunk_type& block = getBlockAtIndex(index);
    const index_type offsetIndex = util::FindChunkIndex(index, shape(), chunkShape(), header().order());
    return block[offsetIndex];
  }

  /**
   * @brief Returns a const reference to the value found at the target index.
   * @param index
   * @return const_reference
   */
  const_reference operator[](index_type index) const
  {
    const chunk_type& block = getBlockAtIndex(index);
    const index_type offsetIndex = util::FindChunkIndex(index, shape(), chunkShape(), header().order());
    return block[offsetIndex];
  }

  /**
   * @brief Copy operator
   * @param rhs
   * @return Array&
   */
  Array& operator=(const Array& rhs) = delete;

  /**
   * @brief Move operator
   * @param rhs
   * @return Array&
   */
  Array& operator=(Array&& rhs) noexcept
  {
    IArray::operator=(std::move(rhs));
    m_BlockQueue = std::move(rhs.m_BlockQueue);
    return *this;
  }

  /**
   * @brief Fills the Array with the specified value.
   * @param value
   */
  void fill(value_type value)
  {
    std::fill(begin(), end(), value);
  }

  /**
   * @brief Returns an iterator to the beginning of the Array.
   * @return iterator
   */
  iterator begin()
  {
    return iterator(*this, 0);
  }

  /**
   * @brief Returns an iterator to the end of the Array.
   * @return iterator
   */
  iterator end()
  {
    return iterator();
  }

  /**
   * @brief Returns a const_iterator to the beginning of the Array.
   * @return const_iterator
   */
  const_iterator begin() const
  {
    return const_iterator(*this, 0);
  }

  /**
   * @brief Returns a const_iterator to the end of the Array.
   * @return const_iterator
   */
  const_iterator end() const
  {
    return const_iterator();
  }

protected:
  /**
   * @brief Returns the total size of the Array in bytes.
   * @return size_type
   */
  static constexpr size_type getDataSize()
  {
    return sizeof(value_type);
  }

  /**
   * @brief Returns the filepath for the data chunk at the specified position.
   * For the sake of calculations, the position is assumed to be the chunk index
   * in N-dimensional space.
   *
   * No bounds checking is used to ensure that the path exists or is within the Array.
   * @param position Vector representing the chunk index in N-dimensional space.
   * @return std::filesystem::path to the target chunk.
   */
  std::filesystem::path getChunkPath(const shape_type& position) const
  {
    std::string positionStr;
    const size_t dimensions = position.size();
    for(size_t i = 0; i < dimensions; i++)
    {
      if(i != 0)
      {
        positionStr += ".";
      }
      positionStr += std::to_string(position[i]);
    }

    return path() / positionStr;
  }

  /**
   * @brief Returns a reference to the block containing the target value index.
   * If no block is cached a new block is loaded into memory.
   * @param index
   * @return chunk_type&
   */
  chunk_type& getBlockAtIndex(index_type index)
  {
    const shape_type chunkIndex = util::FindChunkId(index, shape(), chunkShape(), header().order());
    return getBlockAtChunkIndex(chunkIndex);
  }

  /**
   * @brief Returns a const reference to the block containing the target value index.
   * If no block is cached a new block is loaded into memory.
   * @param index
   * @return const chunk_type&
   */
  const chunk_type& getBlockAtIndex(index_type index) const
  {
    const shape_type chunkIndex = util::FindChunkId(index, shape(), chunkShape());
    return getBlockAtChunkIndex(chunkIndex);
  }

  /**
   * @brief Returns a reference to the block at the target chunk index in
   * N-dimensional space. If no block is cached a new block is loaded into memory.
   * @param chunkIndex
   * @return chunk_type&
   */
  chunk_type& getBlockAtChunkIndex(const shape_type& chunkIndex)
  {
    if(m_BlockQueue.contains(chunkIndex) == false)
    {
      chunk_type chunk(Util::chunkPath(path(), chunkIndex), chunkSize(), header());
      m_BlockQueue.insert(std::move(chunk));
    }
    return m_BlockQueue.getChunkAtOffset(chunkIndex);
  }

  /**
   * @brief Returns a reference to the block at the target chunk index in
   * N-dimensional space. If no block is cached a new block is loaded into memory.
   * @param chunkIndex
   * @return chunk_type&
   */
  const chunk_type& getBlockAtChunkIndex(const shape_type& chunkIndex) const
  {
    if(m_BlockQueue.contains(chunkIndex) == false)
    {
      chunk_type block(Util::chunkPath(path(), chunkIndex), chunkSize(), header());
      m_BlockQueue.insert(std::move(block));
    }
    return m_BlockQueue.getChunkAtOffset(chunkIndex);
  }

private:
  mutable ChunkQueue<T, MAX_BLOCK_COUNT> m_BlockQueue;
};
} // namespace File
