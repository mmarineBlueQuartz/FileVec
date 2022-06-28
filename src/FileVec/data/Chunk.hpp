#pragma once

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace File
{
template <typename TYPE>
class Chunk
{
  using store_type = std::vector<TYPE>;

public:
  using index_type = uint64_t;
  using data_type = TYPE;
  using reference = typename store_type::reference;
  using const_reference = typename store_type::const_reference;
  using iterator = typename store_type::iterator;
  using const_iterator = typename store_type::const_iterator;
  using shape_type = std::vector<uint64_t>;

  Chunk()
  : m_Path()
  , m_Index()
  , m_DataSize(0)
  , m_FillValue(0)
  {
  }

  /**
   * @brief Creates a default and invalid Block.
   */
  Chunk(const shape_type& index, index_type size, data_type fill = 0)
  : m_Path()
  , m_Size(size)
  , m_Index(index)
  , m_Data(size)
  , m_FillValue(fill)
  , m_DataSize(sizeof(data_type) * size)
  {
    std::fill(begin(), end(), fill);
  }

  /**
   * @brief Creates a new Block using a target filepath.
   * @param filepath
   */
  Chunk(const std::filesystem::path& filepath, index_type size, data_type fill = 0)
  : m_Path(filepath)
  , m_Size(size)
  , m_Data(size)
  , m_FillValue(fill)
  , m_DataSize(sizeof(data_type) * size)
  {
    std::fill(begin(), end(), fill);
    parseIndexFromPath();
    read();
  }

  /**
   * @brief Copy constructor deleted. Objects of this class cannot be copied.
   */
  Chunk(const Chunk& other)
  : m_Path(other.m_Path)
  , m_Size(other.m_Size)
  , m_Data(other.m_Data)
  , m_FillValue(other.m_FillValue)
  , m_Index(other.m_Index)
  , m_DataSize(other.m_DataSize)
  {
  }

  /**
   * @brief Move constructor.
   * @param other
   */
  Chunk(Chunk&& other) noexcept
  : m_Path(std::move(other.m_Path))
  , m_Size(other.m_Size)
  , m_Data(std::move(other.m_Data))
  , m_FillValue(other.m_FillValue)
  , m_Index(std::move(other.m_Index))
  , m_DataSize(other.m_DataSize)
  {
  }

  /**
   * @brief Flushes the Block before deleting the stored data.
   */
  virtual ~Chunk() noexcept
  {
    flush();
  }

  /**
   * @brief
   * @return
   */
  bool isValid() const
  {
    return m_DataSize > 0 && m_Index.size() > 0;
  }

  /**
   * @brief Returns the number of values available for the Block to access.
   */
  constexpr uint64_t size()
  {
    return m_Size;
  }

  /**
   * @brief
   * @param index
   * @return
   */
  reference operator[](uint64_t index)
  {
    if(index >= m_Size)
    {
      std::stringstream ss;
      ss << "Chunk Index (" << index << ") is out of range";
      throw std::out_of_range(ss.str());
    }

    return m_Data[index];
  }

  /**
   * @brief
   * @param index
   * @return
   */
  const_reference operator[](uint64_t index) const
  {
    if(index >= m_Size)
    {
      std::stringstream ss;
      ss << "Chunk Index (" << index << ") is out of range";
      throw std::out_of_range(ss.str());
    }

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

    using stream_iterator = std::istreambuf_iterator<data_type>;
    std::basic_ofstream<data_type> ostream(m_Path, std::ios::out | std::ios::binary | std::ios::trunc);
    ostream.seekp(0);
    ostream.write(m_Data.data(), m_Data.size());
    ostream.flush();
    ostream.close();
  }

  /**
   *
   */
  void parseIndexFromPath()
  {
    std::string filename = m_Path.filename().string();
    std::string tmp;
    std::stringstream ss(filename);
    m_Index.clear();
    while(getline(ss, tmp, '.'))
    {
      uint64_t value = std::stoull(tmp);
      m_Index.push_back(value);
    }
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
    if(!std::filesystem::exists(m_Path))
    {
      m_Data.resize(m_Size);
      std::fill(m_Data.begin(), m_Data.end(), m_FillValue);
      return;
    }

    using stream_iterator = std::istreambuf_iterator<data_type>;

    std::basic_ifstream<data_type> stream(m_Path, std::ios::in | std::ios::binary);
    stream.seekg(0, std::ios::beg);
    stream.read(m_Data.data(), m_Size);
    stream.close();
  }

  const shape_type& index() const
  {
    return m_Index;
  }

  /**
   * @brief Copy constructor deleted. Objects of this class cannot be copied.
   */
  Chunk& operator=(const Chunk& rhs)
  {
    flush();

    m_Path = rhs.m_Path;
    m_Data = rhs.m_Data;
    m_FillValue = rhs.m_FillValue;
    m_Index = rhs.m_Index;
    m_DataSize = rhs.m_DataSize;
    m_Size = rhs.m_Size;

    return *this;
  }

  /**
   * @brief
   * @param rhs Right-hand side block to move.
   * @return Returns a reference to the Block being updated.
   */
  Chunk& operator=(Chunk&& rhs) noexcept
  {
    flush();

    m_Path = std::move(rhs.m_Path);
    m_Data = std::move(rhs.m_Data);
    m_FillValue = rhs.m_FillValue;
    m_Index = std::move(rhs.m_Index);
    m_DataSize = rhs.m_DataSize;
    m_Size = rhs.m_Size;

    return *this;
  }

  /**
   * @brief Checks if the file and data are equal.
   * @param rhs
   * @return Returns true if values are equal. Returns false otherwise.
   */
  bool operator==(const Chunk& rhs) const
  {
    return (m_Path == rhs.m_Path) && (m_Data == rhs.m_Data);
  }

  iterator begin()
  {
    return m_Data.begin();
  }

  iterator end()
  {
    return m_Data.end();
  }

  const_iterator begin() const
  {
    return m_Data.begin();
  }

  const_iterator end() const
  {
    return m_Data.end();
  }

private:
  std::filesystem::path m_Path;
  index_type m_Size = 0;
  shape_type m_Index;
  store_type m_Data;
  data_type m_FillValue;
  uint64_t m_DataSize;
};

template <>
class Chunk<bool>
{
  struct data
  {
    data(bool value = false)
    : m_Value(value)
    {
    }

    operator bool&()
    {
      return m_Value;
    }

    operator const bool&() const
    {
      return m_Value;
    }

    bool operator==(bool value) const
    {
      return m_Value == value;
    }

    bool operator!=(bool value) const
    {
      return m_Value != value;
    }

    data& operator=(bool value)
    {
      m_Value = value;
      return *this;
    }

    bool m_Value;
  };
  using store_type = std::vector<data>;

public:
  using index_type = uint64_t;
  using data_type = bool;
  using reference = bool&;
  using const_reference = const bool&;
  using iterator = typename store_type::iterator;
  using const_iterator = typename store_type::const_iterator;
  using shape_type = std::vector<uint64_t>;

  Chunk()
  : m_Path()
  , m_Index()
  , m_DataSize(0)
  {
  }

  /**
   * @brief Creates a default and invalid Block.
   */
  Chunk(const shape_type& index, index_type size, bool fill = 0)
  : m_Path()
  , m_Size(size)
  , m_Index(index)
  , m_Data(size)
  , m_DataSize(sizeof(data_type) * size)
  {
    std::fill(begin(), end(), fill);
  }

  /**
   * @brief Creates a new Block using a target filepath.
   * @param filepath
   */
  Chunk(const std::filesystem::path& filepath, index_type size, data_type fill = 0)
  : m_Path(filepath)
  , m_Size(size)
  , m_Data(size)
  , m_DataSize(sizeof(data_type) * size)
  {
    std::fill(begin(), end(), fill);
    parseIndexFromPath();
    read();
  }

  /**
   * @brief Copy constructor deleted. Objects of this class cannot be copied.
   */
  Chunk(const Chunk& other)
  : m_Path(other.m_Path)
  , m_Size(other.m_Size)
  , m_Data(other.m_Data)
  , m_Index(other.m_Index)
  , m_DataSize(other.m_DataSize)
  {
  }

  /**
   * @brief Move constructor.
   * @param other
   */
  Chunk(Chunk&& other) noexcept
  : m_Path(std::move(other.m_Path))
  , m_Size(other.m_Size)
  , m_Data(std::move(other.m_Data))
  , m_Index(std::move(other.m_Index))
  , m_DataSize(other.m_DataSize)
  {
  }

  /**
   * @brief Flushes the Block before deleting the stored data.
   */
  virtual ~Chunk() noexcept
  {
    flush();
  }

  /**
   * @brief
   * @return
   */
  bool isValid() const
  {
    return m_DataSize > 0 && m_Index.size() > 0;
  }

  /**
   * @brief Returns the number of values available for the Block to access.
   */
  constexpr uint64_t size()
  {
    return m_Size;
  }

  /**
   * @brief
   * @param index
   * @return
   */
  reference operator[](uint64_t index)
  {
    return m_Data[index];
  }

  /**
   * @brief
   * @param index
   * @return
   */
  const_reference operator[](uint64_t index) const
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

    std::vector<char> data(m_Data.begin(), m_Data.end());

    std::ofstream ostream(m_Path, std::fstream::out | std::fstream::trunc | std::fstream::binary);
    ostream.seekp(0);
    ostream.write(reinterpret_cast<char*>(data.data()), m_DataSize);
    ostream.flush();
    ostream.close();
  }

  void parseIndexFromPath()
  {
    std::string filename = m_Path.filename().string();
    std::string tmp;
    std::stringstream ss(filename);
    m_Index.clear();
    while(getline(ss, tmp, '.'))
    {
      uint64_t value = std::stoull(tmp);
      m_Index.push_back(value);
    }
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

    using stream_iterator = std::istreambuf_iterator<data_type>;

    m_Data.clear();
    std::basic_ifstream<data_type> stream(m_Path, std::ios::in | std::ios::binary | std::ios::app);
    std::vector<bool> data((stream_iterator(stream)), stream_iterator());
    for(bool value : data)
    {
      m_Data.emplace_back(value);
    }
    stream.close();
  }

  const shape_type& index() const
  {
    return m_Index;
  }

  /**
   * @brief Copy constructor deleted. Objects of this class cannot be copied.
   */
  Chunk& operator=(const Chunk& rhs)
  {
    flush();

    m_Path = rhs.m_Path;
    m_Data = rhs.m_Data;
    m_Index = rhs.m_Index;
    m_DataSize = rhs.m_DataSize;

    return *this;
  }

  /**
   * @brief
   * @param rhs Right-hand side block to move.
   * @return Returns a reference to the Block being updated.
   */
  Chunk& operator=(Chunk&& rhs) noexcept
  {
    flush();

    m_Path = std::move(rhs.m_Path);
    m_Data = std::move(rhs.m_Data);
    m_Index = std::move(rhs.m_Index);
    m_DataSize = rhs.m_DataSize;

    return *this;
  }

  /**
   * @brief Checks if the file and data are equal.
   * @param rhs
   * @return Returns true if values are equal. Returns false otherwise.
   */
  bool operator==(const Chunk& rhs) const
  {
    return (m_Path == rhs.m_Path) && (m_Data == rhs.m_Data);
  }

  iterator begin()
  {
    return m_Data.begin();
  }

  iterator end()
  {
    return m_Data.end();
  }

  const_iterator begin() const
  {
    return m_Data.begin();
  }

  const_iterator end() const
  {
    return m_Data.end();
  }

private:
  std::filesystem::path m_Path;
  index_type m_Size = 0;
  shape_type m_Index;
  store_type m_Data;
  uint64_t m_DataSize;
};

#if 0
template <>
void Chunk<bool>::flush()
{
  if(!isValid())
  {
    return;
  }

  std::vector<char> data(m_Data.begin(), m_Data.end());

  std::ofstream ostream(m_Path, std::fstream::out | std::fstream::trunc | std::fstream::binary);
  ostream.seekp(0);
  ostream.write(reinterpret_cast<char*>(data.data()), m_DataSize);
  ostream.flush();
  ostream.close();
}
#endif
} // namespace File
