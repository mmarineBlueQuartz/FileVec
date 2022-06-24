#pragma once

#include <filesystem>

#include "FileVec/filevec_export.hpp"

namespace File
{
class FILEVEC_EXPORT TempDirectory
{
public:
  TempDirectory(std::filesystem::path directoryPath);
  virtual ~TempDirectory();

  std::filesystem::path path() const;

  bool exists() const;

  void clear();

  bool removeAll();

private:
  std::filesystem::path m_Path;
};
} // namespace File
