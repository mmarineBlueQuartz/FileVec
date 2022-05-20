#pragma once

#include <array>
#include <filesystem>
#include <fstream>

namespace File
{
template <typename TYPE, uint64_t SIZE>
class Block
{
public:
  /**
   * @brief Creates a default and invalid Block.
   */
  Block()
  : m_Path()
  , m_DataSize(0)
  {
  }

  /**
   * @brief Creates a new Block using a target file and byte offset.
   * @param filepath
   * @param offset Byte offset from the start of the file.
   */
  Block(const std::filesystem::path& filepath, uint64_t offset)
  : m_Path(filepath)
  , m_DataSize(sizeof(TYPE) * SIZE)
  {
    read();
  }

  /**
   * @brief Copy constructor deleted. Objects of this class cannot be copied.
   */
  Block(const Block& other)
  : m_Path(other.m_Path)
  , m_Offset(other.m_Offset)
  , m_Data(other.m_Data)
  , m_DataSize(other.m_DataSize)
  {
  }

  /**
   * @brief Move constructor.
   * @param other
   */
  Block(Block&& other) noexcept
  : m_Path(std::move(other.m_Path))
  , m_Offset(other.m_Offset)
  , m_Data(std::move(other.m_Data))
  , m_DataSize(other.m_DataSize)
  {
  }

  /**
   * @brief Flushes the Block before deleting the stored data.
   */
  virtual ~Block() noexcept
  {
    flush();
  }

  /**
   * @brief
   * @return
   */
  bool isValid() const
  {
    return m_DataSize > 0 && std::filesystem::exists(m_Path);
  }

  /**
   * @brief Returns the number of values available for the Block to access.
   */
  static constexpr uint64_t size()
  {
    return SIZE;
  }

  /**
   * @brief
   * @param index
   * @return
   */
  TYPE& operator[](uint64_t index)
  {
    return m_Data[index];
  }

  /**
   * @brief
   * @param index
   * @return
   */
  const TYPE& operator[](uint64_t index) const
  {
    return m_Data[index];
  }

  /**
   * @brief
   */
  void flush()
  {
    if(!isValid())
    {
      return;
    }

    std::ofstream ostream(m_Path, std::fstream::out | std::fstream::binary);
    ostream.seekp(m_Offset);
    ostream.write(reinterpret_cast<char*>(m_Data.data()), m_DataSize);
    ostream.flush();
  }

  /**
   * @brief
   */
  void read()
  {
    if(!isValid())
    {
      return;
    }

    std::ifstream istream(m_Path, std::fstream::in | std::fstream::binary);
    istream.seekg(m_Offset);
    istream.read(reinterpret_cast<char*>(m_Data.data()), m_DataSize);
  }

  /**
   * @brief Returns the offset at the beginning of the Block.
   * @return
   */
  uint64_t offset() const
  {
    return m_Offset;
  }

  /**
   * @brief Returns an offset just past the end of the Block.
   * @return
   */
  uint64_t endOffset() const
  {
    return m_Offset + SIZE * sizeof(TYPE);
  }

  /**
   * @brief Copy constructor deleted. Objects of this class cannot be copied.
   */
  Block& operator=(const Block& rhs)
  {
    flush();

    m_Path = rhs.m_Path;
    m_Offset = rhs.m_Offset;
    m_Data = rhs.m_Data;
    m_DataSize = rhs.m_DataSize;

    return *this;
  }

  /**
   * @brief
   * @param rhs Right-hand side block to move.
   * @return Returns a reference to the Block being updated.
   */
  Block& operator=(Block&& rhs) noexcept
  {
    flush();

    m_Path = std::move(rhs.m_Path);
    m_Offset = rhs.m_Offset;
    m_Data = std::move(rhs.m_Data);
    m_DataSize = rhs.m_DataSize;

    return *this;
  }

  /**
   * @brief Checks if the file offset and data are equal.
   * @param rhs
   * @return Returns true if values are equal. Returns false otherwise.
   */
  bool operator==(const Block& rhs) const
  {
    return (m_Offset == rhs.m_Offset) && (m_Data == rhs.m_Data);
  }

  /**
   * @brief Returns the target file offset from a given header size and array index.
   * @param headerSize
   * @param index
   * @return Target file offset
   */
  static uint64_t GetTargetOffset(uint64_t headerSize, uint64_t index)
  {
    return headerSize + index * sizeof(TYPE);
  }

private:
  std::filesystem::path m_Path;
  uint64_t m_Offset = 0;
  std::array<TYPE, SIZE> m_Data;
  uint64_t m_DataSize;
};
} // namespace File
