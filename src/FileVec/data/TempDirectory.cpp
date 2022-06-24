#include "TempDirectory.hpp"

namespace File
{
TempDirectory::TempDirectory(std::filesystem::path directoryPath)
: m_Path(std::move(directoryPath))
{
  std::filesystem::create_directory(m_Path);
}

TempDirectory::~TempDirectory()
{
  removeAll();
}

std::filesystem::path TempDirectory::path() const
{
  return m_Path;
}

bool TempDirectory::exists() const
{
  return std::filesystem::exists(m_Path);
}

void TempDirectory::clear()
{
  for(const auto& entry : std::filesystem::directory_iterator(m_Path))
  {
    std::filesystem::remove_all(entry.path());
  }
}

bool TempDirectory::removeAll()
{
  std::filesystem::remove_all(m_Path);

  return !std::filesystem::exists(m_Path);
}
} // namespace File
