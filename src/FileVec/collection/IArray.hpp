#pragma once

#include <filesystem>
#include <vector>

#include "FileVec/data/Header.hpp"

namespace File
{
class FILEVEC_EXPORT IArray
{
public:
  using index_type = uint64_t;
  using size_type = size_t;
  using shape_type = std::vector<index_type>;
  using pointer = std::unique_ptr<IArray>;

  static pointer Read(const std::filesystem::path& path);
  static std::filesystem::path HeaderPath(const std::filesystem::path& directory);

  IArray(const IArray& other);
  IArray(IArray&& other) noexcept;

  virtual ~IArray() noexcept;

  index_type size() const;

  shape_type shape() const;

  shape_type chunkShape() const;

  index_type chunkSize() const;

  index_type dimensions() const;

  const Header& header() const;

  std::filesystem::path headerPath() const;

  /**
   * @brief Returns the path to the directory backing the vector.
   * @return Directory filepath
   */
  const std::filesystem::path& path() const;

  /**
   * @brief
   * @param shape
   */
  void reshapeTuples(const shape_type& shape);

  IArray& operator=(const IArray& rhs);
  IArray& operator=(IArray&& rhs) noexcept;

protected:
  IArray(std::filesystem::path directory);

  IArray(const Header& header, std::filesystem::path directory);

private:
  Header m_Header;
  std::filesystem::path m_DirectoryPath;
};
} // namespace File
