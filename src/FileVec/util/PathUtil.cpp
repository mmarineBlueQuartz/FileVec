#include "PathUtil.hpp"

#include "FileVec/data/TempDirectory.hpp"

#include <fstream>
#include <string>

std::filesystem::path File::Util::getTempDir()
{
  static TempDirectory tempDirectory(createTempPath());
  return tempDirectory.path();
}

std::filesystem::path File::Util::createTempPath()
{
  static uint16_t dataIndex = 0;
  const std::string dataName = std::to_string(dataIndex++);
  std::filesystem::path targetPath = std::filesystem::temp_directory_path() / dataName;

  if(std::filesystem::exists(targetPath) || std::filesystem::create_directory(targetPath))
  {
    return targetPath;
  }
  targetPath = std::filesystem::current_path() / "temp";
  if(std::filesystem::exists(targetPath) || std::filesystem::create_directory(targetPath))
  {
    targetPath /= dataName;
    if(std::filesystem::exists(targetPath) || std::filesystem::create_directory(targetPath))
    {
      return targetPath;
    }
  }
  return {};
}

std::filesystem::path File::Util::chunkPath(const std::filesystem::path& directory, const std::vector<uint64_t>& position)
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

  return directory / positionStr;
}

bool createDataChunksRecursive(const std::filesystem::path& directory, std::vector<uint64_t> numChunks, uint64_t index)
{
  if(index >= numChunks.size())
  {
    return true;
  }

  const uint64_t maxValue = numChunks[index];
  for(uint64_t i = 0; i <= maxValue; i++)
  {
    numChunks[index] = i;
    const std::filesystem::path path = File::Util::chunkPath(directory, numChunks);
    if(!std::ofstream(path, std::ofstream::out | std::ofstream::app))
    {
      return false;
    }
    if(!createDataChunksRecursive(directory, numChunks, index + 1))
    {
      return false;
    }
  }
  return true;
}

bool File::Util::createDataChunks(const std::filesystem::path& directory, const std::vector<uint64_t>& shape, const std::vector<uint64_t>& chunks)
{
  const uint64_t dimensions = shape.size();
  std::vector<uint64_t> numChunks(dimensions);
  for(uint64_t i = 0; i < dimensions; i++)
  {
    numChunks[i] = shape[i] / chunks[i];
  }

  return createDataChunksRecursive(directory, numChunks, 0);
}
