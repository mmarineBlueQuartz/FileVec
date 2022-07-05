#pragma once

#include <array>
#include <optional>
#include <stdexcept>

#include "FileVec/data/Chunk.hpp"

namespace File
{
/**
 * @brief The ChunkQueue class handles loading, saving, and accessing Chunk
 * objects within an Array using N-dimensional indexing.
 */
template <typename TYPE, size_t MAX_CHUNK_COUNT>
class ChunkQueue
{
public:
  using chunk_type = Chunk<TYPE>;
  using shape_type = typename chunk_type::shape_type;

  /**
   * @brief Constructs an empty ChunkQueue.
   */
  ChunkQueue()
  : m_Chunks()
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
    m_Chunks = std::move(other.m_Chunks);
  }

  /**
   * Deletes all Chunks. Deleted Chunks have their memory written to file
   * before their values go out of scope.
   */
  virtual ~ChunkQueue() = default;

  /**
   * @brief Returns the number of items currently in the queue.
   * @return size_t
   */
  size_t size() const
  {
    if(m_Begin <= m_End)
    {
      return m_End - m_Begin;
    }
    return m_End + m_Begin - MAX_CHUNK_COUNT;
  }

  /**
   * @brief Returns the maximum number of Chunks in the queue.
   * @return MAX_BLOCK_SIZE
   */
  static size_t maxSize()
  {
    return MAX_CHUNK_COUNT;
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
   * @brief Checks if the queue has extra capacity remaining.
   * @return bool
   */
  bool hasCapacity() const
  {
    return (size() + 1) < MAX_CHUNK_COUNT;
  }

  /**
   * @brief Attempts to insert the Chunk into the queue.
   *
   * This method does nothing if the chunk is already contained
   * within the queue.
   * @param chunk
   */
  void insert(chunk_type&& chunk)
  {
    if(contains(chunk))
    {
      return;
    }
    if(hasCapacity() == false)
    {
      m_Begin = (m_Begin + 1) % MAX_CHUNK_COUNT;
    }
    if(empty())
    {
      m_Chunks[m_Begin] = std::move(chunk);
      m_End++;
    }
    else
    {
      uint64_t index = (m_End + 1) % MAX_CHUNK_COUNT;
      m_Chunks[index] = std::move(chunk);
    }
  }

  /**
   * @brief Copy operator deleted.
   * @param rhs
   * @return ChunkQueue&
   */
  ChunkQueue& operator=(const ChunkQueue& rhs) = delete;

  /**
   * @brief Move operator
   * @param rhs
   * @return ChunkQueue&
   */
  ChunkQueue& operator=(ChunkQueue&& rhs) noexcept
  {
    m_Chunks = std::move(rhs.m_Chunks);
    return *this;
  }

  /**
   * @brief Checks if the queue contains the target Chunk.
   * @param targetChunk
   * @return Returns true if the chunk is contained within the queue. Returns false otherwise.
   */
  bool contains(const chunk_type& targetChunk) const
  {
    for(const chunk_type& chunk : m_Chunks)
    {
      if(chunk == targetChunk)
      {
        return true;
      }
    }
    return false;
  }

  /**
   * @brief Checks if the queue contains a Chunk with the target N-dimensional index.
   * @param index
   * @return Returns true if the Chunk is in queue's memory. Returns false otherwise.
   */
  bool contains(const shape_type& index) const
  {
    for(const chunk_type& chunk : m_Chunks)
    {
      if(chunk.index() == index)
      {
        return true;
      }
    }
    return false;
  }

  /**
   * @brief Returns a reference to the chunk with the target index.
   *
   * This method throws if the target Chunk could not be found.
   * @param index
   * @throws std::runtime_error
   * @return chunk_type&
   */
  chunk_type& getChunkAtOffset(const shape_type& index)
  {
    for(chunk_type& chunk : m_Chunks)
    {
      if(chunk.index() == index)
      {
        return chunk;
      }
    }
    throw std::runtime_error("Chunk not found");
  }

  /**
   * @brief Returns a const reference to the chunk with the target index.
   *
   * This method throws if the target Chunk could not be found.
   * @param index
   * @throws std::runtime_error
   * @return const chunk_type&
   */
  const chunk_type& getChunkAtOffset(const shape_type& index) const
  {
    for(const chunk_type& chunk : m_Chunks)
    {
      if(chunk.position() == index)
      {
        return chunk;
      }
    }
    throw std::runtime_error("Chunk not found");
  }

private:
  std::array<chunk_type, MAX_CHUNK_COUNT> m_Chunks;
  uint64_t m_Begin = 0;
  uint64_t m_End = 0;
};
} // namespace File
