#include <iostream>

#include "Vector.hpp"

namespace V1
{
template <typename T>
void read(const File::Vector<T>& vector)
{
  std::cout << "Read existing values:" << std::endl;
  for(uint64_t i = 0; i < 5; i++)
  {
    std::cout << "Value: " << vector.at(i) << std::endl;
  }
  std::cout << std::endl;
}

template <typename T>
void write(File::Vector<T>& vector)
{
  std::cout << "Write values:" << std::endl;
  for(uint64_t index = 0; index < 5; index++)
  {
    vector[index] = index * 2;
    std::cout << "Value: " << vector.at(index) << std::endl;
  }
  std::cout << std::endl;
}
} // namespace V1

namespace V2
{
template <typename T>
void read(const File::Vector<T>& vector)
{
  std::cout << "Read existing values:" << std::endl;
  for(uint64_t i = 0; i < 5; i++)
  {
    std::cout << "Value: " << vector[i] << std::endl;
  }
  std::cout << std::endl;
}

template <typename T>
void write(File::Vector<T>& vector)
{
  std::cout << "Write values:" << std::endl;
  for(uint64_t index = 0; index < 5; index++)
  {
    vector[index] = index * 3;
    std::cout << "Value: " << vector[index] << std::endl;
  }
  std::cout << std::endl;
}
} // namespace V2

int32_t main(int32_t argc, char** argv)
{
  {
    File::Vector<int32_t> vector("test.data");

    std::cout << "V1" << std::endl;
    V1::write(vector);

    std::cout << "V2" << std::endl;
    V2::read(vector);
    V2::write(vector);
    V2::read(vector);
  }

  std::cout << "Flushed Vector" << std::endl;

  {
    File::Vector<int32_t> vector("test.data");

    std::cout << "V1" << std::endl;
    V1::read(vector);
    std::cout << "V2" << std::endl;
    V2::read(vector);
  }

#if 0
  std::cout << "Read existing values:\n";
  for(uint64_t i = 0; i < 5; i++)
  {
    std::cout << "Value: " << vector.getValue(i) << std::endl;
  }

  std::cout << "\nWrite values:\n";
  for(uint64_t index = 0; index < 5; index++)
  {
    vector.setValue(index, index * 2);
    // vector[index] = index * 2;
    std::cout << "Value: " << vector.getValue(index) << std::endl;
  }
#endif

  return 0;
}
