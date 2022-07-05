#include "PathUtil.hpp"

#include "FileVec/data/TempDirectory.hpp"

#include <fstream>
#include <string>

std::filesystem::path File::Util::getTempDir()
{
  static TempDirectory tempDirectory(defaultTempPath());
  return tempDirectory.path();
}

std::filesystem::path File::Util::defaultTempPath()
{
  std::filesystem::path targetPath = std::filesystem::temp_directory_path() / "FileCore";

  if(std::filesystem::exists(targetPath) || std::filesystem::create_directory(targetPath))
  {
    return targetPath;
  }
  targetPath = std::filesystem::current_path() / "temp";
  if(std::filesystem::exists(targetPath) || std::filesystem::create_directory(targetPath))
  {
    return targetPath;
  }
  return {};
}

std::filesystem::path File::Util::createTempPath()
{
  static uint16_t dataIndex = 0;

  const std::string dataName = std::to_string(dataIndex++);
  std::filesystem::path targetPath = getTempDir() / dataName;
  std::filesystem::create_directory(targetPath);

  return targetPath;
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
