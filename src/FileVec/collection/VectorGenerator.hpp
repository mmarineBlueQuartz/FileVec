#include "FileVec/collection/Vector.hpp"

namespace File
{
template <typename T>
class VectorGenerator
{
public:
  using shape_type = IVector::shape_type;

  VectorGenerator(const shape_type& shape, const shape_type& chunks, Order order = Order::ColumnMajor, Endian endian = Endian::irrelevant);
  virtual ~VectorGenerator();

  Vector<T> create() const;
};
} // namespace File
