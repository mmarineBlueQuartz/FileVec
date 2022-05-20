#pragma once

#include <cstdint>
#include <filesystem>

#include "BlockQueue.hpp"
#include "Header.hpp"

#define HAS_SIZE 0

namespace File
{
template <typename T, uint64_t BLOCK_SIZE = 1024, uint64_t MAX_BLOCK_COUNT = 6>
class Vector
{
public:
  using value_type = int32_t;
  using index_type = uint64_t;
  using size_type = size_t;
  using block_type = Block<T, BLOCK_SIZE>;
  using reference = value_type&;
  using const_reference = const value_type&;

  /**
   * @brief Constructs a file-backed vector using the file at the target path.
   * If a file already exists at the target path, values are read from the target file.
   * @param path
   */
  Vector(const std::filesystem::path& path)
  : m_Path(path)
  {
  }

  /**
   * @brief Writes cached values to the target file before clearing the vector from memory.
   */
  virtual ~Vector() = default;

#if HAS_SIZE
  /**
   * @brief Returns the number of elements in the vector.
   * @return
   */
  size_type size() const
  {
    return m_Size;
  }
#endif

  /**
   * @brief Returns the path to the file backing the vector.
   * @return
   */
  [[nodiscard]] const std::filesystem::path& path() const
  {
    return m_Path;
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
      throw std::out_of_range("Target File::Vector has size: " + std::to_string(size()));
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
      throw std::out_of_range("Target File::Vector has size: " + std::to_string(size()));
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
    block_type& block = getBlockAtIndex(index);

    index_type blockIndex = (index * sizeof(value_type) - block.offset()) / sizeof(value_type);
    return block[blockIndex];
  }

  /**
   * @brief Returns a const reference to the value found at the target index.
   * @param index
   * @return const reference
   */
  const_reference operator[](index_type index) const
  {
    const block_type& block = getBlockAtIndex(index);

    index_type blockIndex = (index * sizeof(value_type) - block.offset()) / sizeof(value_type);
    return block[blockIndex];
  }

protected:
  static constexpr size_type getDataSize()
  {
    return sizeof(value_type);
  }
  // void setSize(index_type newSize);
  // void setMaxSize(index_type newSize);

  /**
   * @brief Returns a reference to the block containing the target index.
   * If no block is cached a new block is cached.
   * @param index
   * @return
   */
  block_type& getBlockAtIndex(index_type index)
  {
    uint64_t offset = index * sizeof(value_type);
    if(m_BlockQueue.contains(offset) == false)
    {
      block_type block(m_Path, offset);
      m_BlockQueue.insert(std::move(block));
    }
    return m_BlockQueue.getBlockAtOffset(offset);
  }

  /**
   * @brief Returns a const reference to the block containing the target index.
   * If no block is cached a new block is cached.
   * @param index
   * @return
   */
  const block_type& getBlockAtIndex(index_type index) const
  {
    uint64_t offset = index * sizeof(value_type);
    if(m_BlockQueue.contains(offset) == false)
    {
      block_type block(m_Path, offset);
      m_BlockQueue.insert(std::move(block));
    }
    return m_BlockQueue.getBlockAtOffset(offset);
  }

private:
  Header m_Header;
  std::filesystem::path m_Path;
  index_type m_Size = 0;
  mutable BlockQueue<T, BLOCK_SIZE, MAX_BLOCK_COUNT> m_BlockQueue;
};
} // namespace File
