#pragma once

#include <array>
#include <vector>

#include "FileVec/data/Header.hpp"

namespace File::util
{
/**
 *
 */
inline uint64_t Flatten(const std::vector<uint64_t>& position, const std::vector<uint64_t>& shape)
{
  using index_type = uint64_t;
  const size_t dimensions = position.size();

  if(shape.size() != dimensions)
  {
    throw std::runtime_error("Could not flatten position due to mismatched dimensions");
  }

  index_type index = 0;
  index_type offset = 1;
  for(size_t i = 0; i < dimensions; i++)
  {
    index += position[i] * offset;
    offset *= shape[i];
  }

  return index;
}

/**
 *
 */
inline std::vector<uint64_t> FindPosition(uint64_t index, const std::vector<uint64_t>& shape, Order order = Order::Default)
{
  using index_type = uint64_t;
  using shape_type = std::vector<index_type>;

  const size_t dimensions = shape.size();
  shape_type position(dimensions);
  for(index_type i = 0; i < dimensions; i++)
  {
    const index_type offset = (order == Order::ColumnMajor) ? i : dimensions - i - 1;
    position[offset] = index % shape[offset];
    index /= shape[offset];
  }
  return position;
}

/**
 *
 */
inline std::vector<uint64_t> FindChunkId(const std::vector<uint64_t>& position, const std::vector<uint64_t>& chunk)
{
  using index_type = uint64_t;
  using shape_type = std::vector<index_type>;

  const size_t dimensions = position.size();
  if(dimensions != chunk.size())
  {
    throw std::runtime_error("Could not find chunk ID due to mismatched dimensions");
  }

  shape_type chunkIndex(dimensions);
  for(index_type i = 0; i < dimensions; i++)
  {
    const index_type offset = i;
    chunkIndex[i] = position[offset] / chunk[offset];
  }
  return chunkIndex;
}

/**
 *
 */
inline std::vector<uint64_t> FindChunkId(uint64_t index, const std::vector<uint64_t>& shape, const std::vector<uint64_t>& chunkShape, Order order = Order::Default)
{
  const auto position = FindPosition(index, shape, order);
  return FindChunkId(position, chunkShape);
}

/**
 *
 */
inline std::vector<uint64_t> FindChunkPosition(const std::vector<uint64_t>& position, const std::vector<uint64_t>& chunkIndex, const std::vector<uint64_t>& chunkShape, Order order = Order::Default)
{
  using index_type = uint64_t;
  using shape_type = std::vector<index_type>;

  const size_t dimensions = position.size();
  if(dimensions != chunkIndex.size() || dimensions != chunkShape.size())
  {
    throw std::runtime_error("Could not find chunk position due to mismatched dimensions");
  }

  shape_type chunkOffset(dimensions);
  for(index_type i = 0; i < dimensions; i++)
  {
    const index_type offset = (order == Order::ColumnMajor) ? i : dimensions - i - 1;

    const index_type chunkPos = chunkIndex[offset] * chunkShape[offset];
    const uint64_t value = position[offset];
    if(value < chunkPos)
    {
      throw std::out_of_range("The provided position is not within the specified chunk");
    }
    chunkOffset[offset] = value - chunkPos;
  }
  return chunkOffset;
}

inline uint64_t FindChunkIndex(uint64_t index, const std::vector<uint64_t>& shape, const std::vector<uint64_t>& chunkShape, Order order = Order::Default)
{
  using shape_type = std::vector<uint64_t>;

  const shape_type position = util::FindPosition(index, shape, order);
  const shape_type chunkIndex = util::FindChunkId(position, chunkShape);
  const shape_type chunkPosition = util::FindChunkPosition(position, chunkIndex, chunkShape, order);
  return util::Flatten(chunkPosition, chunkShape);
}
} // namespace File::util
