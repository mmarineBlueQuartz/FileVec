#pragma once

#include <filesystem>
#include <memory>

#include "FileVec/filevec_export.hpp"

namespace File
{
class FILEVEC_EXPORT BaseCollection
{
public:
  static std::unique_ptr<BaseCollection> Read(const std::filesystem::path& directory);

  virtual ~BaseCollection();

  /**
   * @brief Returns the path to the directory backing the collection.
   * @return Directory filepath
   */
  const std::filesystem::path& path() const;

  std::string name() const;

protected:
  BaseCollection(std::filesystem::path directory);
  BaseCollection(const BaseCollection& other);
  BaseCollection(BaseCollection&& other) noexcept;

  BaseCollection& operator=(const BaseCollection& rhs);
  BaseCollection& operator=(BaseCollection&& rhs) noexcept;

private:
  std::filesystem::path m_DirectoryPath;
};
} // namespace File
