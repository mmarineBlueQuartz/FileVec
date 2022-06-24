#pragma once

#include <array>
#include <optional>
#include <stdexcept>

#include "FileVec/data/Chunk.hpp"

namespace File
{
template <typename TYPE, uint32_t MAX_BLOCK_COUNT>
class ChunkQueue
{
public:
  using chunk_type = Chunk<TYPE>;
  using shape_type = typename chunk_type::shape_type;

  /**
   * @brief Constructs an empty BlockQueue.
   */
  ChunkQueue()
  : m_Blocks()
  {
  }

  /**
   * @brief Copy constructor deleted.
   */
  ChunkQueue(const ChunkQueue& other) = delete;

  /**
   * @brief Move constructor
   */
  ChunkQueue(ChunkQueue&& other) noexcept
  {
    m_Blocks = std::move(other.m_Blocks);
  }

  virtual ~ChunkQueue() = default;

  /**
   * @brief Returns the number of items in the queue.
   * @return
   */
  uint32_t size() const
  {
    if(m_Begin <= m_End)
    {
      return m_End - m_Begin;
    }
    return m_End + m_Begin - MAX_BLOCK_COUNT;
  }

  /**
   * @brief Returns the maximum number of Blocks in the queue.
   * @return MAX_BLOCK_SIZE
   */
  static uint32_t maxSize()
  {
    return MAX_BLOCK_COUNT;
  }

  /**
   * @brief Checks if the queue is empty.
   * @return Returns true if the queue is empty. Otherwise, this method returns false.
   */
  bool empty() const
  {
    return size() == 0;
  }

  /**
   * @brief
   * @return
   */
  bool hasCapacity() const
  {
    return (size() + 1) < MAX_BLOCK_COUNT;
  }

  /**
   * @brief
   * @param block
   */
  void insert(chunk_type&& block)
  {
    if(hasCapacity() == false)
    {
      m_Begin = (m_Begin + 1) % MAX_BLOCK_COUNT;
    }
    if(empty())
    {
      m_Blocks[m_Begin] = std::move(block);
      m_End++;
      return;
    }
    uint64_t index = (m_End + 1) % MAX_BLOCK_COUNT;
    m_Blocks[index] = std::move(block);
  }

  /**
   * @brief Copy operator deleted.
   * @param rhs
   * @return
   */
  ChunkQueue& operator=(const ChunkQueue& rhs) = delete;

  /**
   * @brief Move operator
   * @param rhs
   * @return
   */
  ChunkQueue& operator=(ChunkQueue&& rhs) noexcept
  {
    m_Blocks = std::move(rhs.m_Blocks);
    return *this;
  }

  /**
   * @brief
   * @param targetBlock
   * @return
   */
  bool contains(const chunk_type& targetBlock) const
  {
    for(const chunk_type& block : m_Blocks)
    {
      if(block == targetBlock)
      {
        return true;
      }
    }
    return false;
  }

  /**
   * @brief
   * @param offset
   * @return
   */
  bool contains(uint64_t offset) const
  {
    for(const chunk_type& block : m_Blocks)
    {
      if(offset >= block.offset() && offset < block.endOffset())
      {
        return true;
      }
    }
    return false;
  }

  /**
   * @brief
   * @param offset
   * @return
   */
  bool contains(const shape_type& index) const
  {
    for(const chunk_type& block : m_Blocks)
    {
      if(block.index() == index)
      {
        return true;
      }
    }
    return false;
  }

  chunk_type& getBlockAtOffset(uint64_t offset)
  {
    for(chunk_type& block : m_Blocks)
    {
      if(offset >= block.offset() && offset < block.endOffset())
      {
        return block;
      }
    }
    throw std::runtime_error("");
  }

  const chunk_type& getBlockAtOffset(uint64_t offset) const
  {
    for(const chunk_type& block : m_Blocks)
    {
      if(offset >= block.offset() && offset < block.endOffset())
      {
        return block;
      }
    }
    throw std::runtime_error("");
  }

  chunk_type& getBlockAtOffset(const shape_type& index)
  {
    for(chunk_type& block : m_Blocks)
    {
      // if(block.index() == index && block.isValid())
      if(block.index() == index)
      {
        return block;
      }
    }
    throw std::runtime_error("Chunk not found");
  }

  const chunk_type& getBlockAtOffset(const shape_type& index) const
  {
    for(const chunk_type& block : m_Blocks)
    {
      if(block.position() == index)
      {
        return block;
      }
    }
    throw std::runtime_error("Chunk not found");
  }

private:
  std::array<chunk_type, MAX_BLOCK_COUNT> m_Blocks;
  uint64_t m_Begin = 0;
  uint64_t m_End = 0;
};
} // namespace File
