#include "FileVec/collection/BaseCollection.hpp"
#include "FileVec/collection/Group.hpp"
#include "FileVec/collection/IArray.hpp"

namespace File
{
std::unique_ptr<BaseCollection> BaseCollection::Read(const std::filesystem::path& directory)
{
  if(!is_directory(directory))
  {
    return nullptr;
  }
  if(exists(directory / ".zgroup"))
  {
    return Group::Read(directory);
  }
  if(exists(directory / ".zarray"))
  {
    return IArray::Read(directory);
  }
  return nullptr;
}

BaseCollection::BaseCollection(std::filesystem::path directory)
: m_DirectoryPath(std::move(directory))
{
}

BaseCollection::BaseCollection(const BaseCollection& other) = default;

BaseCollection::BaseCollection(BaseCollection&& other) noexcept = default;

BaseCollection& BaseCollection::operator=(const BaseCollection& rhs) = default;

BaseCollection& BaseCollection::operator=(BaseCollection&& rhs) noexcept = default;

BaseCollection::~BaseCollection() = default;

const std::filesystem::path& BaseCollection::path() const
{
  return m_DirectoryPath;
}

std::string BaseCollection::name() const
{
  if(m_DirectoryPath.empty())
  {
    return {};
  }
  if(!m_DirectoryPath.has_parent_path())
  {
    return m_DirectoryPath.string();
  }

  std::string pathString = m_DirectoryPath.string();
  const std::string parentPathString = m_DirectoryPath.parent_path().string();
  pathString.erase(0, parentPathString.size() + 1);
  return pathString;
}
} // namespace File
