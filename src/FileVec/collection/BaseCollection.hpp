#pragma once

#include <nlohmann/json.hpp>

#include <filesystem>
#include <memory>
#include <string>

#include "FileVec/filevec_export.hpp"

namespace File
{
class FILEVEC_EXPORT BaseCollection
{
public:
  static inline std::string k_AttributesFilename = ".zattrs";

  static std::unique_ptr<BaseCollection> Read(const std::filesystem::path& directory);

  /**
   * @brief Reads and returns the attributes for a specified directory.
   * Returns an empty json object if no attributes exist.
   * @param directory
   * @return json attributes
   */
  static nlohmann::json ReadAttributes(const std::filesystem::path& directory);

  virtual ~BaseCollection();

  /**
   * @brief Returns the path to the directory backing the collection.
   * @return Directory filepath
   */
  const std::filesystem::path& path() const;

  /**
   * @brief Returns the name of the data object based on the directory name.
   * @return std::string
   */
  std::string name() const;

  /**
   * @brief Returns a reference to the data's attributes.
   * @return json attributes
   */
  nlohmann::json& attributes();

  /**
   * @brief Returns a const reference to the data's attributes.
   * @return json attributes
   */
  const nlohmann::json& attributes() const;

  /**
   * @brief Writes attributes to file.
   * @return Returns true on success or false on failure.
   */
  bool writeAttributes() const;

protected:
  BaseCollection(std::filesystem::path directory);
  BaseCollection(const BaseCollection& other);
  BaseCollection(BaseCollection&& other) noexcept;

  BaseCollection& operator=(const BaseCollection& rhs);
  BaseCollection& operator=(BaseCollection&& rhs) noexcept;

  /**
   * @brief Reads the json attributes using the data's directory path.
   */
  void readAttributes();

private:
  std::filesystem::path m_DirectoryPath;
  nlohmann::json m_Attributes;
};
} // namespace File
