#pragma once

#include "FileVec/data/Header.hpp"

#include "nlohmann/json.hpp"

namespace File
{
class Header;

class FILEVEC_EXPORT ICompressor
{
public:
  virtual ~ICompressor();

  /**
   * @brief Returns a json representation of the compressor.
   * @return json
   */
  virtual nlohmann::json toJson() const = 0;

protected:
  ICompressor();

  /**
   * @brief Checks if the native endian encoding is compatible with the target.
   * @param targetEndian
   * @return Returns true if the bytes need to be swapped. Returns false otherwise.
   */
  static bool ShouldByteSwap(Endian targetEndian);
};
} // namespace File
