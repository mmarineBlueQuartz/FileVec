#include "catch.hpp"

#include "FileVec/collection/Group.hpp"
#include "FileVec/util/PathUtil.hpp"

#include <string>

template <typename T>
void testWriteAttribute(File::BaseCollection& data, T value)
{
  const std::string name = "__name__";
  auto& attr = data.attributes();
  attr[name] = value;
  REQUIRE(!attr[name].empty());
  REQUIRE(attr[name].get<T>() == value);
}

TEST_CASE("Create Attributes", "Header")
{
  File::Group group;
  testWriteAttribute<int8_t>(group, 1);
  testWriteAttribute<int16_t>(group, 2);
  testWriteAttribute<int32_t>(group, 3);
  testWriteAttribute<int64_t>(group, 4);
  testWriteAttribute<uint8_t>(group, 5);
  testWriteAttribute<uint16_t>(group, 6);
  testWriteAttribute<uint32_t>(group, 7);
  testWriteAttribute<uint64_t>(group, 8);
  testWriteAttribute<float>(group, 9.5f);
  testWriteAttribute<double>(group, 10.25);
  testWriteAttribute<bool>(group, true);
  testWriteAttribute<bool>(group, false);
  // testWriteAttribute<std::string>(group, "foobar");
}

TEST_CASE("Attribute IO", "Header")
{
  const std::filesystem::path path = File::Util::createTempPath();
  const std::string name1 = "foo";
  const std::string name2 = "bar";
  const std::string name3 = "bazz";

  const nlohmann::json value1 = "Testing";
  const nlohmann::json value2 = 1;
  const nlohmann::json value3 = true;

  {
    auto group = File::Group::Create(path);
    group->attributes()[name1] = value1;
    group->attributes()[name2] = value2;
    group->attributes()[name3] = value3;
  }

  {
    auto group = File::Group::Create(path);
    REQUIRE(group->attributes()[name1] == value1);
    REQUIRE(group->attributes()[name2] == value2);
    REQUIRE(group->attributes()[name3] == value3);
  }
}
