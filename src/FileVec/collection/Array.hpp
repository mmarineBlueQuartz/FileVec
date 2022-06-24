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

#define HAS_SIZE 0

namespace File
{
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

  static Array Create(const shape_type& shape, const shape_type& chunks, Order order = Order::ColumnMajor, Endian endian = Endian::irrelevant)
  {
    std::filesystem::path path = File::Util::getTempDir();
    Header header = Header::Create(shape, chunks, File::ToDataType<T>(), order, endian);
    return Array(header, path);
  }

  static std::unique_ptr<Array> CreatePtr(const shape_type& shape, const shape_type& chunks, Order order = Order::ColumnMajor, Endian endian = Endian::irrelevant)
  {
    return std::make_unique<Array>(Create(shape, chunks, order, endian));
  }

  static pointer Import(const Header& header, const std::filesystem::path& path)
  {
    return std::make_unique<Array>(header, path);
  }

  /**
   * @brief Constructs a file-backed vector using the file at the target path.
   * If a file already exists at the target path, values are read from the target file.
   * @param directory
   */
  Array(const std::filesystem::path& directory)
  : IArray(directory)
  {
  }

  Array(const Header& header, const std::filesystem::path& directory)
  : IArray(header, directory)
  {
    Util::createDataChunks(directory, header.shape(), header.chunks());
  }

  Array(const Array& other) = delete;

  Array(Array&& other) noexcept
  : IArray(std::move(other))
  , m_BlockQueue(std::move(other.m_BlockQueue))
  {
  }

  /**
   * @brief Writes cached values to the target file before clearing the vector from memory.
   */
  virtual ~Array() = default;

  shape_type index2position(index_type index) const
  {
    return util::FindPosition(index, header().shape());
  }

  shape_type position2chunkindex(const shape_type& position) const
  {
    const Header& hdr = header();
    return util::FindChunkId(position, hdr.chunks());
  }

  shape_type findChunkPosition(const shape_type& position, const shape_type& chunkIndex) const
  {
    const Header& hdr = header();
    return util::FindChunkPosition(position, chunkIndex, hdr.chunks());
  }

  /**
   * @brief Returns a reference to the value at the target index.
   * @param index
   * @return reference
   */
  reference at(index_type index)
  {
#if HAS_SIZE
    if(index >= m_Size)
    {
      throw std::out_of_range("Target File::Array has size: " + std::to_string(size()));
    }
#endif
    return operator[](index);
  }

  /**
   * @brief Returns a const reference to the value at the target index.
   * @param index
   * @return const reference
   */
  const_reference at(index_type index) const
  {
#if HAS_SIZE
    if(index >= m_Size)
    {
      throw std::out_of_range("Target File::Array has size: " + std::to_string(size()));
    }
#endif
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

    // const shape_type position = index2position(index);
    // const shape_type chunkIndex = position2chunkindex(position);
    // const shape_type chunkPosition = findChunkPosition(position, chunkIndex);
    // const index_type offsetIndex = util::Flatten(chunkPosition, header().chunks());
    const index_type offsetIndex = util::FindChunkIndex(index, shape(), chunkShape());
    return block[offsetIndex];
  }

  /**
   * @brief Returns a const reference to the value found at the target index.
   * @param index
   * @return const reference
   */
  const_reference operator[](index_type index) const
  {
    const chunk_type& block = getBlockAtIndex(index);

    const shape_type position = index2position(index);
    const shape_type chunkIndex = position2chunkindex(position);
    const shape_type offset = findChunkPosition(position, chunkIndex);
    const index_type offsetIndex = util::Flatten(offset, header().chunks());
    return block[offsetIndex];
  }

  Array& operator=(const Array& rhs) = delete;

  Array& operator=(Array&& rhs) noexcept
  {
    IArray::operator=(std::move(rhs));
    m_BlockQueue = std::move(rhs.m_BlockQueue);
    return *this;
  }

  void fill(T value)
  {
    const index_type count = size();
    for(index_type i = 0; i < count; i++)
    {
      operator[](i) = value;
    }
  }

  iterator begin()
  {
    return iterator(*this, 0);
  }

  iterator end()
  {
    return iterator();
  }

  const_iterator begin() const
  {
    return const_iterator(*this, 0);
  }

  const_iterator end() const
  {
    return const_iterator();
  }

protected:
  static constexpr size_type getDataSize()
  {
    return sizeof(value_type);
  }

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

  // void setSize(index_type newSize);
  // void setMaxSize(index_type newSize);

  /**
   * @brief Returns a reference to the block containing the target index.
   * If no block is cached a new block is cached.
   * @param index
   * @return
   */
  chunk_type& getBlockAtIndex(index_type index)
  {
    const shape_type chunkIndex = index2position(index);
    return getBlockAtChunkIndex(chunkIndex);
  }

  /**
   * @brief Returns a const reference to the block containing the target index.
   * If no block is cached a new block is cached.
   * @param index
   * @return
   */
  const chunk_type& getBlockAtIndex(index_type index) const
  {
    const shape_type chunkIndex = index2position(index);
    return getBlockAtChunkIndex(chunkIndex);
  }

  /**
   * @brief
   * @param chunkIndex
   * @return
   */
  chunk_type& getBlockAtChunkIndex(const shape_type& chunkIndex)
  {
    if(m_BlockQueue.contains(chunkIndex) == false)
    {
      chunk_type chunk(Util::chunkPath(path(), chunkIndex), chunkSize());
      m_BlockQueue.insert(std::move(chunk));
    }
    return m_BlockQueue.getBlockAtOffset(chunkIndex);
  }

  /**
   * @brief
   * @param chunkIndex
   * @return
   */
  const chunk_type& getBlockAtChunkIndex(const shape_type& chunkIndex) const
  {
    if(m_BlockQueue.contains(chunkIndex) == false)
    {
      chunk_type block(Util::chunkPath(path(), chunkIndex));
      m_BlockQueue.insert(std::move(block));
    }
    return m_BlockQueue.getBlockAtOffset(chunkIndex);
  }

private:
  mutable ChunkQueue<T, MAX_BLOCK_COUNT> m_BlockQueue;
};
} // namespace File
