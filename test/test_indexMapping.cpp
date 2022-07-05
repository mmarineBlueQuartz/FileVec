#include <catch2/catch.hpp>

#include "FileVec/util/Algorithm.hpp"

using index_type = uint64_t;
using shape_type = std::vector<index_type>;

TEST_CASE("Flatten Position", "[Algorithms]")
{
  const shape_type shape = {5, 5};
  shape_type position = {0, 0};

  REQUIRE(File::util::Flatten(position, shape) == 0);

  position = {1, 0};
  REQUIRE(File::util::Flatten(position, shape) == 1);

  position = {4, 0};
  REQUIRE(File::util::Flatten(position, shape) == 4);

  position = {0, 1};
  REQUIRE(File::util::Flatten(position, shape) == 5);

  position = {1, 1};
  REQUIRE(File::util::Flatten(position, shape) == 6);

  position = {4, 4};
  REQUIRE(File::util::Flatten(position, shape) == 24);
}

TEST_CASE("Find Position", "[Algorithms]")
{
  const shape_type shape = {5, 5};

  index_type index = 0;
  shape_type targetValue = {0, 0};
  REQUIRE(File::util::FindPosition(index, shape) == targetValue);

  index = 1;
  targetValue = {1, 0};
  REQUIRE(File::util::FindPosition(index, shape) == targetValue);

  index = 5;
  targetValue = {0, 1};
  REQUIRE(File::util::FindPosition(index, shape) == targetValue);

  index = 6;
  targetValue = {1, 1};
  REQUIRE(File::util::FindPosition(index, shape) == targetValue);
}

TEST_CASE("Find Chunk ID", "[Algorithms]")
{
  const shape_type chunk = {2, 2};

  shape_type position = {0, 0};
  shape_type targetPosition = {0, 0};
  REQUIRE(File::util::FindChunkId(position, chunk) == targetPosition);

  position = {1, 0};
  targetPosition = {0, 0};
  REQUIRE(File::util::FindChunkId(position, chunk) == targetPosition);

  position = {2, 2};
  targetPosition = {1, 1};
  REQUIRE(File::util::FindChunkId(position, chunk) == targetPosition);

  position = {3, 3};
  targetPosition = {1, 1};
  REQUIRE(File::util::FindChunkId(position, chunk) == targetPosition);

  position = {4, 3};
  targetPosition = {2, 1};
  REQUIRE(File::util::FindChunkId(position, chunk) == targetPosition);
}

TEST_CASE("Find Chunk ID From index", "[Algorithms]")
{
  const shape_type shape = {4, 4};
  const shape_type chunk = {2, 2};

  index_type index = 0;
  shape_type targetPosition = {0, 0};
  REQUIRE(File::util::FindChunkId(index, shape, chunk) == targetPosition);

  index = 1;
  targetPosition = {0, 0};
  REQUIRE(File::util::FindChunkId(index, shape, chunk) == targetPosition);

  index = 2;
  targetPosition = {1, 0};
  REQUIRE(File::util::FindChunkId(index, shape, chunk) == targetPosition);

  index = 3;
  targetPosition = {1, 0};
  REQUIRE(File::util::FindChunkId(index, shape, chunk) == targetPosition);

  index = 4;
  targetPosition = {0, 0};
  REQUIRE(File::util::FindChunkId(index, shape, chunk) == targetPosition);

  index = 5;
  targetPosition = {0, 0};
  REQUIRE(File::util::FindChunkId(index, shape, chunk) == targetPosition);

  index = 6;
  targetPosition = {1, 0};
  REQUIRE(File::util::FindChunkId(index, shape, chunk) == targetPosition);

  index = 7;
  targetPosition = {1, 0};
  REQUIRE(File::util::FindChunkId(index, shape, chunk) == targetPosition);

  index = 8;
  targetPosition = {0, 1};
  REQUIRE(File::util::FindChunkId(index, shape, chunk) == targetPosition);

  index = 9;
  targetPosition = {0, 1};
  REQUIRE(File::util::FindChunkId(index, shape, chunk) == targetPosition);

  index = 10;
  targetPosition = {1, 1};
  REQUIRE(File::util::FindChunkId(index, shape, chunk) == targetPosition);

  index = 11;
  targetPosition = {1, 1};
  REQUIRE(File::util::FindChunkId(index, shape, chunk) == targetPosition);

  index = 12;
  targetPosition = {0, 1};
  REQUIRE(File::util::FindChunkId(index, shape, chunk) == targetPosition);

  index = 13;
  targetPosition = {0, 1};
  REQUIRE(File::util::FindChunkId(index, shape, chunk) == targetPosition);

  index = 14;
  targetPosition = {1, 1};
  REQUIRE(File::util::FindChunkId(index, shape, chunk) == targetPosition);

  index = 15;
  targetPosition = {1, 1};
  REQUIRE(File::util::FindChunkId(index, shape, chunk) == targetPosition);
}

TEST_CASE("Find Chunk Position From Position", "[Algorithms]")
{
  const shape_type chunk = {2, 2};

  shape_type position = {0, 0};
  shape_type chunkId = {0, 0};
  shape_type targetIndex = {0, 0};
  REQUIRE(File::util::FindChunkPosition(position, chunkId, chunk) == targetIndex);

  position = {1, 0};
  chunkId = {0, 0};
  targetIndex = {1, 0};
  REQUIRE(File::util::FindChunkPosition(position, chunkId, chunk) == targetIndex);

  position = {2, 0};
  chunkId = {1, 0};
  targetIndex = {0, 0};
  REQUIRE(File::util::FindChunkPosition(position, chunkId, chunk) == targetIndex);

  position = {3, 0};
  chunkId = {1, 0};
  targetIndex = {1, 0};
  REQUIRE(File::util::FindChunkPosition(position, chunkId, chunk) == targetIndex);

  position = {3, 1};
  chunkId = {1, 0};
  targetIndex = {1, 1};
  REQUIRE(File::util::FindChunkPosition(position, chunkId, chunk) == targetIndex);

  position = {1, 3};
  chunkId = {0, 1};
  targetIndex = {1, 1};
  REQUIRE(File::util::FindChunkPosition(position, chunkId, chunk) == targetIndex);

  position = {2, 3};
  chunkId = {1, 1};
  targetIndex = {0, 1};
  REQUIRE(File::util::FindChunkPosition(position, chunkId, chunk) == targetIndex);
}

TEST_CASE("Find Chunk Index From Index", "[Algorithms]")
{
  const shape_type shape = {4, 4};
  const shape_type chunk = {2, 2};

  index_type index = 0;
  REQUIRE(File::util::FindChunkIndex(index, shape, chunk) == 0);

  index = 1;
  REQUIRE(File::util::FindChunkIndex(index, shape, chunk) == 1);

  index = 2;
  REQUIRE(File::util::FindChunkIndex(index, shape, chunk) == 0);

  index = 3;
  REQUIRE(File::util::FindChunkIndex(index, shape, chunk) == 1);

  index = 4;
  REQUIRE(File::util::FindChunkIndex(index, shape, chunk) == 2);

  index = 5;
  REQUIRE(File::util::FindChunkIndex(index, shape, chunk) == 3);

  index = 6;
  REQUIRE(File::util::FindChunkIndex(index, shape, chunk) == 2);

  index = 7;
  REQUIRE(File::util::FindChunkIndex(index, shape, chunk) == 3);

  index = 8;
  REQUIRE(File::util::FindChunkIndex(index, shape, chunk) == 0);

  index = 9;
  REQUIRE(File::util::FindChunkIndex(index, shape, chunk) == 1);

  index = 10;
  REQUIRE(File::util::FindChunkIndex(index, shape, chunk) == 0);

  index = 11;
  REQUIRE(File::util::FindChunkIndex(index, shape, chunk) == 1);

  index = 12;
  REQUIRE(File::util::FindChunkIndex(index, shape, chunk) == 2);

  index = 13;
  REQUIRE(File::util::FindChunkIndex(index, shape, chunk) == 3);

  index = 14;
  REQUIRE(File::util::FindChunkIndex(index, shape, chunk) == 2);

  index = 15;
  REQUIRE(File::util::FindChunkIndex(index, shape, chunk) == 3);
}
