#pragma once

#include <iostream>
#include <vector>

namespace File::Parsing
{
template <typename T>
inline void readData(std::istream& inputStream, std::vector<T>& data)
{
  const uint64_t dataSize = data.size();
  // inputStream >> std::hex >> std::setw(sizeof(T));
  for(uint64_t i = 0; i < dataSize; ++i)
  {
    inputStream >> std::setw(sizeof(T)) >> std::hex >> data[i];
  }
}

template <>
inline void readData<float>(std::istream& inputStream, std::vector<float>& data)
{
  const uint64_t dataSize = data.size();
  // inputStream >> std::hexfloat >> std::setw(sizeof(float));
  for(uint64_t i = 0; i < dataSize; ++i)
  {
    inputStream >> std::setw(sizeof(float)) >> data[i];
  }
}

template <>
inline void readData<double>(std::istream& inputStream, std::vector<double>& data)
{
  const uint64_t dataSize = data.size();
  data.resize(dataSize);
  // inputStream >> std::hexfloat >> std::setw(sizeof(double));
  for(uint64_t i = 0; i < dataSize; ++i)
  {
    inputStream >> std::hexfloat >> std::setw(sizeof(double)) >> data[i];
  }
}

template <typename T>
inline void writeData(std::ostream& outputStream, const std::vector<T>& data)
{
  const uint64_t dataSize = data.size();
  // outputStream << std::hex << std::setw(sizeof(T));
  for(const T& value : data)
  {
    outputStream << std::setw(sizeof(T)) << std::hex << value;
  }
  outputStream.flush();
}

template <>
inline void writeData<float>(std::ostream& outputStream, const std::vector<float>& data)
{
  const uint64_t dataSize = data.size();
  // outputStream << std::hexfloat << std::setw(sizeof(float));
  for(const float& value : data)
  {
    outputStream << std::hexfloat << std::setw(sizeof(float)) << value;
  }
  outputStream.flush();
}

template <>
inline void writeData<double>(std::ostream& outputStream, const std::vector<double>& data)
{
  outputStream << std::hexfloat << std::setw(sizeof(double));
  for(const double& value : data)
  {
    outputStream << std::hexfloat << std::setw(sizeof(double)) << value;
  }
  outputStream.flush();
}
} // namespace File::Parsing
