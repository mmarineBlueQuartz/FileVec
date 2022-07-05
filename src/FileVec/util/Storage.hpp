#pragma once

#include <cstdint>
#include <filesystem>

#include <FileVec/filevec_export.hpp>

namespace File::Storage
{
struct FILEVEC_EXPORT data
{
  uint64_t total = 0;
  uint64_t free = 0;
};

data FILEVEC_EXPORT checkStorage();
} // namespace File::Storage
