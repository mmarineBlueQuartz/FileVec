#pragma once

#include <array>
#include <vector>

namespace File::util
{
#if 0
template <uint64_t DIMS>
constexpr uint64_t Flatten(const std::array<uint64_t, DIMS>& position, const std::array<uint64_t, DIMS>& shape)
{
  using index_type = uint64_t;

  index_type index = 0;
  index_type offset = 1;
  for(size_t i = 0; i < DIMS; i++)
  {
    index += position[i] * offset;
    offset *= shape[i];
  }

  return index;
}
#endif

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

#if 0
template <uint64_t DIMS>
constexpr std::array<uint64_t, DIMS> FindPosition(uint64_t index, const std::array<uint64_t, DIMS>& shape)
{
  using index_type = uint64_t;
  using shape_type = std::array<index_type, DIMS>;

  shape_type position;
  for(index_type i = 0; i < DIMS; i++)
  {
    position[i] = index % shape[i];
    index /= shape[i];
  }
  return position;
}

template <uint64_t DIMS>
constexpr std::array<uint64_t, DIMS> FindChunkId(const std::array<uint64_t, DIMS>& position, const std::array<uint64_t, DIMS>& chunk)
{
  using index_type = uint64_t;
  using shape_type = std::array<index_type, DIMS>;

  shape_type chunkIndex;
  for(index_type i = 0; i < DIMS; i++)
  {
    chunkIndex[i] = position[i] / chunk[i];
  }
  return chunkIndex;
}

template <uint64_t DIMS>
constexpr std::array<uint64_t, DIMS> FindChunkPosition(const std::array<uint64_t, DIMS>& position, const std::array<uint64_t, DIMS>& chunkIndex, const std::array<uint64_t, DIMS>& chunkShape)
{
  using index_type = uint64_t;
  using shape_type = std::array<index_type, DIMS>;

  shape_type chunkOffset;
  for(index_type i = 0; i < DIMS; i++)
  {
    const index_type chunkPos = chunkIndex[i] * chunkShape[i];
    const uint64_t value = position[i];
    if(value < chunkPos)
    {
      throw std::out_of_range("The provided position is not within the specified chunk");
    }
    chunkOffset[i] = value - chunkPos;
  }
  return chunkOffset;
}
#endif

inline std::vector<uint64_t> FindPosition(uint64_t index, const std::vector<uint64_t>& shape)
{
  using index_type = uint64_t;
  using shape_type = std::vector<index_type>;

  const size_t dimensions = shape.size();
  shape_type position(dimensions);
  for(index_type i = 0; i < dimensions; i++)
  {
    position[i] = index % shape[i];
    index /= shape[i];
  }
  return position;
}

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
    chunkIndex[i] = position[i] / chunk[i];
  }
  return chunkIndex;
}

inline std::vector<uint64_t> FindChunkPosition(const std::vector<uint64_t>& position, const std::vector<uint64_t>& chunkIndex, const std::vector<uint64_t>& chunkShape)
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
    const index_type chunkPos = chunkIndex[i] * chunkShape[i];
    const uint64_t value = position[i];
    if(value < chunkPos)
    {
      throw std::out_of_range("The provided position is not within the specified chunk");
    }
    chunkOffset[i] = value - chunkPos;
  }
  return chunkOffset;
}

inline uint64_t FindChunkIndex(uint64_t index, const std::vector<uint64_t>& shape, const std::vector<uint64_t>& chunkShape)
{
  using shape_type = std::vector<uint64_t>;

  const shape_type position = util::FindPosition(index, shape);
  const shape_type chunkIndex = util::FindChunkId(position, chunkShape);
  const shape_type chunkPosition = util::FindChunkPosition(position, chunkIndex, chunkShape);
  return util::Flatten(chunkPosition, chunkShape);
}
} // namespace File::util
