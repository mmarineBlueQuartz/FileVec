#pragma once

#include "FileVec/compressors/BloscCompressor.hpp"
#include "FileVec/compressors/NullCompressor.hpp"
#include "FileVec/data/Header.hpp"

namespace File
{
template <typename T>
std::unique_ptr<ICompressor> createEncoder(const nlohmann::json& json)
{
  if(json.empty())
  {
    return std::make_unique<BloscCompressor<T>>();
  }

  if(json.is_null())
  {
    return std::make_unique<NullCompressor<T>>();
  }

  const std::string id = json["id"].get<std::string>();
  if(id == "blosc")
  {
    return BloscCompressor<T>::FromJson(json);
  }
  else
  {
    throw std::runtime_error("Unknown compressor");
  }
}
} // namespace File
