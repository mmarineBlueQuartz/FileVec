#include "ICompressor.hpp"

#include "FileVec/util/Bit.hpp"

namespace File
{
ICompressor::ICompressor() = default;
ICompressor::~ICompressor() = default;

bool ICompressor::ShouldByteSwap(Endian targetEndian)
{
  if(targetEndian == Endian::irrelevant)
  {
    return false;
  }
  return static_cast<Endian>(endian::native) != targetEndian;
}
} // namespace File
