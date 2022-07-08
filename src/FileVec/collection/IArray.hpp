#pragma once

#include <filesystem>
#include <vector>

#include "FileVec/collection/BaseCollection.hpp"
#include "FileVec/data/Header.hpp"

namespace File
{
/**
 * @brief the IArray class serves as a generic interface for out-of-core Arrays
 * without exposing access to the typed data. The Header and its path are the
 * only information made available.
 *
 * A static Read method allows an Array to be loaded into memory using only a
 * target filepath. No information about the underlying data type is required
 * to load the Array.
 */
class FILEVEC_EXPORT IArray : public BaseCollection
{
public:
  using index_type = uint64_t;
  using size_type = size_t;
  using shape_type = std::vector<index_type>;
  using pointer = std::unique_ptr<IArray>;

  /**
   * @brief Attempts to read an Array at the target filepath into memory and
   * returns the resulting smart pointer. No other information is required.
   * If the process fails, this method returns nullptr.
   *
   * This method currently only supports integer, floating point, and bool types.
   * @param path
   * @return std::unique_ptr<IArray>
   */
  static pointer Read(const std::filesystem::path& path);

  /**
   * @brief Returns the filepath for the corresponding Header based on a target
   * IArray directory.
   * @param directory
   * @return std::filesystem::path
   */
  static std::filesystem::path HeaderPath(const std::filesystem::path& directory);

  /**
   * @brief Copy constructor
   * @param other
   */
  IArray(const IArray& other);

  /**
   * @brief Move constructor
   * @param other
   */
  IArray(IArray&& other) noexcept;

  virtual ~IArray() noexcept;

  /**
   * @brief Returns the number of items in the IArray.
   * @return size_t
   */
  index_type size() const;

  /**
   * @brief Returns the shape of the array along N-dimensions.
   * @return shape_type
   */
  shape_type shape() const;

  /**
   * @brief Returns the shape of the array chunks along N-dimensions.
   * @return shape_type
   */
  shape_type chunkShape() const;

  /**
   * @brief Returns the number of items in a standard chunk.
   * @return index_type
   */
  index_type chunkSize() const;

  /**
   * @brief Returns the number of dimensions in the array.
   * @return index_type
   */
  index_type dimensions() const;

  /**
   * @brief Returns a const reference to the Header.
   * @return const Header&
   *
   */
  const Header& header() const;

  /**
   * @brief Returns the filepath to the corresponding Header file.
   * @return std::filesystem::path
   */
  std::filesystem::path headerPath() const;

  IArray& operator=(const IArray& rhs);
  IArray& operator=(IArray&& rhs) noexcept;

protected:
  /**
   * @brief Constructs an IArray at the target directory.
   * The directory is assumed to be valid.
   * @param directory
   */
  IArray(std::filesystem::path directory);

  /**
   * @brief Constructs an IArray at the target directory using the provided Header.
   * The directory is assumed to be valid.
   * @param header
   * @param directory
   */
  IArray(const Header& header, std::filesystem::path directory);

private:
  Header m_Header;
};
} // namespace File
