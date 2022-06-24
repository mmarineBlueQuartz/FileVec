#pragma once

#include <filesystem>

#include "FileVec/filevec_export.hpp"

namespace File
{
namespace Util
{
/**
 * @brief Returns the path to the desired temp directory.
 * @return Temp directory path
 */
std::filesystem::path FILEVEC_EXPORT getTempDir();

/**
 * @brief Returns a path for use by data with an unspecified data path.
 * @return Data directory path
 */
std::filesystem::path FILEVEC_EXPORT createTempPath();

std::filesystem::path FILEVEC_EXPORT chunkPath(const std::filesystem::path& directory, const std::vector<uint64_t>& position);

bool FILEVEC_EXPORT createDataChunks(const std::filesystem::path& directory, const std::vector<uint64_t>& shape, const std::vector<uint64_t>& chunks);
} // namespace Util
} // namespace File
