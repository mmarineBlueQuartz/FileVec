#pragma once

#include "FileVec/collection/BaseCollection.hpp"

#include "FileVec/filevec_export.hpp"

namespace File
{
class FILEVEC_EXPORT Group : public BaseCollection
{
public:
  using child_type = std::shared_ptr<BaseCollection>;
  using collection_type = std::vector<child_type>;
  using iterator = collection_type::iterator;
  using const_iterator = collection_type::const_iterator;
  using pointer = std::unique_ptr<Group>;

  static pointer Create();
  static pointer Create(const std::filesystem::path& directory);
  static pointer Read(const std::filesystem::path& directory);

  Group();
  Group(const Group& other);
  Group(Group&& other) noexcept;
  virtual ~Group() noexcept;

  Group& operator=(const Group& rhs);
  Group& operator=(Group&& rhs) noexcept;

  iterator find(const std::string& childName);
  const_iterator find(const std::string& childName) const;
  iterator operator[](const std::string& childName);
  const_iterator operator[](const std::string& childName) const;

  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;

protected:
  Group(std::filesystem::path directory);

private:
  void findChildren();

  collection_type m_Children;
};
} // namespace File
