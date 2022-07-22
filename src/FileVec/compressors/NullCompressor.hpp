#pragma once

#include "FileVec/compressors/AbstractCompressor.hpp"

namespace File
{
template <typename T>
class NullCompressor : public AbstractCompressor<T>
{
public:
  NullCompressor() = default;
  virtual ~NullCompressor() = default;

  /**
   * @brief Returns a json representation of the compressor.
   * @return json
   */
  nlohmann::json toJson() const override
  {
    nlohmann::json json(nullptr);
    return json;
  }

  /**
   * @brief Passes a vector of byte data, passes it through the compression algorithm, and returns a vector of the requested type.
   * @param bytes
   * @param header
   * @return decompressed data
   */
  data_vector decompressData(const byte_vector& bytes, const Header& header) const override
  {
    return AbstractCompressor<T>::FromByteVector(bytes, header);
  }

  /**
   * @brief Passes a collection of values through the compression algorithm after setting the appropriate endian encoding.
   * @param data
   * @param header
   * @return compressed byte data
   */
  byte_vector compressData(const data_vector& data, const Header& header) const override
  {
    return ToByteVector(data, header);
  }
};
} // namespace File
