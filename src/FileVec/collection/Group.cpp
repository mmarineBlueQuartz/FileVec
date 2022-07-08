#include "FileVec/collection/Group.hpp"
#include "FileVec/util/PathUtil.hpp"

namespace File
{
Group::pointer Group::Create()
{
  return Create(File::Util::createTempPath());
}

Group::pointer Group::Create(const std::filesystem::path& directory)
{
  if(!is_directory(directory))
  {
    return nullptr;
  }

  return std::make_unique<Group>(Group(directory));
}

std::unique_ptr<Group> Group::Read(const std::filesystem::path& directory)
{
  if(!exists(directory / ".zgroup"))
  {
    return nullptr;
  }

  return std::make_unique<Group>(Group(directory));
}

Group::Group()
: BaseCollection(File::Util::createTempPath())
{
}

Group::Group(std::filesystem::path directory)
: BaseCollection(std::move(directory))
{
  findChildren();
}

Group::Group(const Group& other) = default;
Group::Group(Group&& other) noexcept = default;

Group::~Group() noexcept = default;

void Group::findChildren()
{
  for(const auto& childPath : std::filesystem::directory_iterator(path()))
  {
    child_type child = BaseCollection::Read(childPath);
    if(child != nullptr)
    {
      m_Children.push_back(child);
    }
  }
}

Group& Group::operator=(const Group& rhs) = default;
Group& Group::operator=(Group&& rhs) noexcept = default;

Group::iterator Group::find(const std::string& childName)
{
  for(auto iter = begin(); iter != end(); ++iter)
  {
    if((*iter)->name() == childName)
    {
      return iter;
    }
  }
  return end();
}

Group::const_iterator Group::find(const std::string& childName) const
{
  for(auto iter = begin(); iter != end(); ++iter)
  {
    if((*iter)->name() == childName)
    {
      return iter;
    }
  }
  return end();
}

Group::iterator Group::operator[](const std::string& childName)
{
  return find(childName);
}

Group::const_iterator Group::operator[](const std::string& childName) const
{
  return find(childName);
}

Group::iterator Group::begin()
{
  return m_Children.begin();
}

Group::iterator Group::end()
{
  return m_Children.end();
}

Group::const_iterator Group::begin() const
{
  return m_Children.begin();
}

Group::const_iterator Group::end() const
{
  return m_Children.end();
}
} // namespace File
