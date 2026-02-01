# GLEngine Tests

<!--TOC-->
  - [Directory Structure](#directory-structure)
  - [Building Tests](#building-tests)
  - [Running Tests](#running-tests)
    - [All Tests](#all-tests)
  - [Adding New Tests](#adding-new-tests)
    - [Create Test File](#create-test-file)
  - [Test Naming Conventions](#test-naming-conventions)
    - [Test Suites](#test-suites)
    - [Test Names](#test-names)
    - [Test Fixtures](#test-fixtures)
  - [Available Test Fixtures](#available-test-fixtures)
    - [XMLSerializeFixture](#xmlserializefixture)
    - [XMLDeserializeFixture](#xmldeserializefixture)
  - [RTTR Registration for Reflection Testing](#rttr-registration-for-reflection-testing)
<!--/TOC-->

This directory contains unit tests for GLEngine components using Google Test (gtest).

## Directory Structure

```
Tests/
├── CommonTestUtils/            # Shared test utilities and fixtures
│   ├── GlmPrinters.h           # GLM type printers for test output
│   └── ...
├── Module/                     # Moduel library tests
│   ├── ModuleTest/             # Test source files
│   │   └── ...
│   ├── ModuleTestStdafx.h      # Precompiled header
│   ├── ModuleTestStdafx.cpp
│   ├── TestMain.cpp          # Test entry point included 
│   └── premake5.lua          # Build configuration
├── AnotherModule/            # Another module library tests
└── readme.md                 # This file
```

## Building Tests

Tests are built as part of the standard GLEngine build process using Premake5.

## Running Tests

### All Tests

```bash
# Windows
bin/Debug-windows-x86_64/CoreTest/CoreTest.exe

# Linux
./bin/Debug-linux-x86_64/CoreTest/CoreTest
```

## Adding New Tests

### Create Test File

Create a new `$OriginalFileName$Test.cpp` file in the appropriate subdirectory:

```cpp
// Tests/Core/CoreTest/MyFeature/MyFeatureTest.cpp

#include <CoreTestStdafx.h>

#include <Core/MyFeature/MyClass.h>

namespace GLEngine::Core {

TEST(MyClass, BasicConstruction)
{
    MyClass instance;
    EXPECT_TRUE(instance.IsValid());
}

TEST(MyClass, MethodBehavior)
{
    MyClass instance;
    instance.DoSomething();
    EXPECT_EQ(instance.GetResult(), 42);
}

} // namespace GLEngine::Core
```

## Test Naming Conventions

### Test Suites

Name test suites after the class or feature being tested:
- `EventTest` - Tests for I_Event and related types
- `EventDispatcherTest` - Tests for C_EventDispatcher
- `LayerStackTest` - Tests for C_LayerStack

### Test Names

Use descriptive names that explain what is being tested:
- `ConstructWithName` - Tests constructor with name parameter
- `GetTypeReturnsCorrectValue` - Tests return value of GetType()
- `EventPropagationStopsWhenHandled` - Tests event propagation behavior

### Test Fixtures

Use the `Fixture` suffix for fixture classes:
- `ResourceHandleFixture`
- `LayerStackTest` (also serves as fixture - not preferred)

## Available Test Fixtures

### XMLSerializeFixture

For testing XML serialization:

```cpp
TEST_F(XMLSerializeFixture, MyTypeSerializes)
{
    MyType obj;
    obj.value = 42;

    const auto xmlDoc = serializer.Serialize(obj);

    auto node = xmlDoc.child("MyType");
    ASSERT_TRUE(node);
    EXPECT_EQ(node.attribute("value").as_int(), 42);
}
```

### XMLDeserializeFixture

For testing XML deserialization:

```cpp
TEST_F(XMLDeserializeFixture, MyTypeDeserializes)
{
    const pugi::xml_document doc = ConstructDocument(R"x(
<?xml version="1.0"?>
<MyType value="42" />)x");

    const auto result = deserializer.Deserialize<MyType>(doc);

    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->value, 42);
}
```

## RTTR Registration for Reflection Testing

For types that need reflection testing, register them with RTTR:

```cpp
// In your test file, outside the namespace
struct TestStruct {
    int value;
    std::string name;
};

RTTR_REGISTRATION
{
    rttr::registration::class_<TestStruct>("TestStruct")
        .constructor<>()
        .property("value", &TestStruct::value)
        .property("name", &TestStruct::name);
}

// Then use in tests
namespace GLEngine {
TEST(Reflection, TestStructRegistered)
{
    auto type = rttr::type::get<TestStruct>();
    EXPECT_TRUE(type.is_valid());
    EXPECT_EQ(type.get_properties().size(), 2);
}
}
```