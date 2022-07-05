#pragma once

#include "FileVec/compressors/AbstractCompressor.hpp"

#include <blosc.h>

namespace File
{
template <typename T>
class BloscCompressor : public AbstractCompressor<T>
{
public:
  using data_vector = typename AbstractCompressor<T>::data_vector;
  using byte_vector = typename AbstractCompressor<T>::byte_vector;

  static inline const std::string k_CLevel_Id = "clevel";
  static inline const std::string k_Shuffle_Id = "shuffle";
  static inline const std::string k_CName_Id = "cname";
  static inline const std::string k_BlockSize_Id = "blocksize";

  BloscCompressor() = default;
  virtual ~BloscCompressor() = default;

  static std::unique_ptr<BloscCompressor> FromJson(const nlohmann::json& json)
  {
    auto encoder = std::make_unique<BloscCompressor>();
    if(json.contains(k_CLevel_Id))
    {
      auto clevel = json[k_CLevel_Id].get<int>();
      encoder->setCompressionLevel(clevel);
    }
    if(json.contains(k_Shuffle_Id))
    {
      auto shuffle = json[k_Shuffle_Id].get<int>();
      encoder->setShuffle(shuffle);
    }
    if(json.contains(k_CName_Id))
    {
      auto cname = json[k_CName_Id].get<std::string>();
      encoder->setCName(cname);
    }
    if(json.contains(k_BlockSize_Id))
    {
      auto blockSize = json[k_BlockSize_Id].get<int>();
      encoder->setBlockSize(blockSize);
    }

    return encoder;
  }

  /**
   * @brief Returns a json representation of the compressor.
   * @return json
   */
  nlohmann::json toJson() const override
  {
    nlohmann::json json;
    json[k_CLevel_Id] = m_CLevel;
    json[k_Shuffle_Id] = m_Shuffle;
    json[k_CName_Id] = m_CName;
    json[k_BlockSize_Id] = m_BlockSize;
    return json;
  }

  data_vector decompressData(const byte_vector& bytes, const Header& header) const override
  {
    byte_vector decompressed;

    blosc_init();
    size_t outSize;
    if(blosc_cbuffer_validate(bytes.data(), bytes.size(), &outSize) == -1)
    {
      // Bytes are not valid blosc compression
      decompressed = bytes;
    }
    else
    {
      decompressed.resize(outSize);
      blosc_decompress(bytes.data(), decompressed.data(), outSize);
    }
    blosc_destroy();

    return FromByteVector(decompressed, header);
  }

  byte_vector compressData(const data_vector& data, const Header& header) const override
  {
    byte_vector bytes = ToByteVector(data, header);
    byte_vector compressed(bytes.size());

    blosc_init();
    blosc_set_compressor(m_CName.c_str());
    blosc_set_blocksize(m_BlockSize);
    auto compressedSize = blosc_compress(m_CLevel, m_Shuffle, sizeof(char), bytes.size(), bytes.data(), compressed.data(), compressed.size());
    blosc_destroy();

    // Failed to compress bytes
    if(compressedSize <= 0)
    {
      return bytes;
    }

    compressed.resize(compressedSize);
    return compressed;
  }

  int clevel() const
  {
    return m_CLevel;
  }

  void setCompressionLevel(int clevel)
  {
    m_CLevel = clevel;
  }

  int shuffle() const
  {
    return m_Shuffle;
  }

  void setShuffle(int shuffle)
  {
    m_Shuffle = shuffle;
  }

  const std::string& cname() const
  {
    return m_CName;
  }

  void setCName(const std::string& cname)
  {
    m_CName = cname;
  }

  size_t blockSize() const
  {
    return m_BlockSize;
  }

  void setBlockSize(size_t blockSize)
  {
    m_BlockSize = blockSize;
  }

private:
  int m_CLevel = 5;
  int m_Shuffle = BLOSC_SHUFFLE;
  std::string m_CName = "lz4";
  size_t m_BlockSize = 0;
};
} // namespace File
