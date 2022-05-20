#pragma once

#include <array>
#include <optional>
#include <stdexcept>

#include "Block.hpp"

namespace File
{
template <typename TYPE, uint64_t SIZE, uint32_t MAX_BLOCK_COUNT>
class BlockQueue
{
public:
  using block_type = Block<TYPE, SIZE>;

  /**
   * @brief Constructs an empty BlockQueue.
   */
  BlockQueue()
  {
  }

  /**
   * @brief Copy constructor deleted.
   */
  BlockQueue(const BlockQueue& other) = delete;

  /**
   * @brief Move constructor
   */
  BlockQueue(BlockQueue&& other) noexcept
  {
    m_Blocks = std::move(other.m_Blocks);
  }

  virtual ~BlockQueue() = default;

  /**
   * @brief Returns the number of items in the queue.
   * @return
   */
  uint32_t size() const
  {
    if(m_End == MAX_BLOCK_COUNT)
    {
      return 0;
    }
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
  static constexpr uint32_t maxSize()
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
  void insert(block_type&& block)
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
  BlockQueue& operator=(const BlockQueue& rhs) = delete;

  /**
   * @brief Move operator
   * @param rhs
   * @return
   */
  BlockQueue& operator=(BlockQueue&& rhs) noexcept
  {
    m_Blocks = std::move(rhs.m_Blocks);
    return *this;
  }

  /**
   * @brief
   * @param targetBlock
   * @return
   */
  bool contains(const block_type& targetBlock) const
  {
    for(const block_type& block : m_Blocks)
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
    for(const block_type& block : m_Blocks)
    {
      if(offset >= block.offset() && offset < block.endOffset())
      {
        return true;
      }
    }
    return false;
  }

  block_type& getBlockAtOffset(uint64_t offset)
  {
    for(block_type& block : m_Blocks)
    {
      if(offset >= block.offset() && offset < block.endOffset())
      {
        return block;
      }
    }
    throw std::runtime_error("");
  }

  const block_type& getBlockAtOffset(uint64_t offset) const
  {
    for(const block_type& block : m_Blocks)
    {
      if(offset >= block.offset() && offset < block.endOffset())
      {
        return block;
      }
    }
    throw std::runtime_error("");
  }

private:
  std::array<block_type, MAX_BLOCK_COUNT> m_Blocks;
  uint64_t m_Begin = 0;
  uint64_t m_End = MAX_BLOCK_COUNT;
};
} // namespace File
