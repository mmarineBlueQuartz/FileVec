#pragma once

#include "FileVec/compressors/ICompressor.hpp"
#include "FileVec/util/Bit.hpp"

namespace File
{
/**
 * @brief Wrapper around boolean data to bypass std::vector<bool> specialization.
 * The struct is designed such that a std::vector<data> operates the same way
 * std::vector<bool> would without the specialization.
 */
template <typename T>
struct datawrapper
{
  using value_type = T;
  using reference = T&;
  using const_reference = const T&;

  datawrapper(value_type value = 0)
  : m_Value(value)
  {
  }

  /**
   * @brief Allow default casting the struct to a reference.
   * @return reference
   */
  operator reference()
  {
    return m_Value;
  }

  /**
   * @brief Allow default casting the struct to a const reference.
   * @return const_reference
   */
  operator const_reference() const
  {
    return m_Value;
  }

  /**
   * @brief Checks equality against a specified value.
   * @param value
   * @return equality
   */
  bool operator==(value_type value) const
  {
    return m_Value == value;
  }

  /**
   * @brief Checks inequality against a specified value.
   * @param value
   * @return inequality
   */
  bool operator!=(value_type value) const
  {
    return m_Value != value;
  }

  /**
   * @brief Assigns a target value to the struct.
   * @param value
   * @return self reference
   */
  datawrapper& operator=(value_type value)
  {
    m_Value = value;
    return *this;
  }

private:
  value_type m_Value;
};

template <typename T>
class AbstractCompressor : public ICompressor
{
public:
  using data_vector = std::vector<datawrapper<T>>;
  using byte_vector = std::vector<char>;

  virtual ~AbstractCompressor() = default;

  /**
   * @brief Passes a vector of byte data, passes it through the compression algorithm, and returns a vector of the requested type.
   * @param bytes
   * @param header
   * @return decompressed data
   */
  virtual data_vector decompressData(const byte_vector& bytes, const Header& header) const = 0;

  /**
   * @brief Passes a collection of values through the compression algorithm after setting the appropriate endian encoding.
   * @param data
   * @param header
   * @return compressed byte data
   */
  virtual byte_vector compressData(const data_vector& data, const Header& header) const = 0;

protected:
  /**
   * @brief Converts raw byte data into a vector of the requested type.
   * Converts from the endian encoding to the native format.
   * @param bytes
   * @param header
   * @return data vector
   */
  static data_vector FromByteVector(const byte_vector& bytes, const Header& header)
  {
    const bool shouldSwap = ShouldByteSwap(header.endian());
    const size_t count = bytes.size() / sizeof(T);
    const T* byteData = reinterpret_cast<const T*>(bytes.data());
    data_vector output(count);
    for(size_t i = 0; i < count; ++i)
    {
      output[i] = shouldSwap ? byteswap(byteData[i]) : byteData[i];
    }
    return output;
  }

  /**
   * @brief Converts data into a vector of raw bytes.
   * Converts from native endian encoding to the requested format.
   * @param data
   * @param header
   * @return raw byte vector
   */
  static byte_vector ToByteVector(const data_vector& data, const Header& header)
  {
    const bool shouldSwap = ShouldByteSwap(header.endian());
    byte_vector output;
    for(const T& value : data)
    {
      T valueOut = shouldSwap ? byteswap(value) : value;
      std::copy(static_cast<const char*>(static_cast<const void*>(&valueOut)), static_cast<const char*>(static_cast<const void*>(&valueOut)) + sizeof valueOut, std::back_inserter(output));
    }
    return output;
  }
};
} // namespace File
