## Contributing

To start contributing into this repository simply develop your feature. The code should pass both [![Build](https://github.com/MySchoolEngine/GLEngine/workflows/build/badge.svg)](https://github.com/MySchoolEngine/GLEngine/actions?workflow=build). 

Before pull request please run clang-tidy on your code (on windows you can use clang-format.bat). And follow coding guidelines. I will review your code and then it will be merged into the codebase. If you need any help please contact me.

#### Coding rules

###### Code placement
**CP.1 - Implementation to cpp** - Headers are for reader. So try to avoid noise in headers by placing as much functions to cpp/inl files as possible.

**CP.2 - Inline, templated and constexpr** - The implementation of those functions needs to be known to the users at compile time. But in order to avoid visual noise consider placing them to separate `.inl` file and include this file to the end of header file after the end of namespace. This will force you to place code in `.inl` file to the correct namespace and this helps future reader to parse file visually quicker.

**CP.3 - Special functions** - Prefer placing all functions to teh cpp as defined in **CP.1** even defaulted special functions. Place constructors to the top of cpp and all the special functions below them before the other functions.

###### Templates

**T.1 - Just don't** Rule of the thumb - don't overdo it

**T.2 - Naming** Try to be nice to the template user. Refrein from using `class T` as universal name for everything. Do you expect enum class? Name it! `template<class Enum>`.

**T.3 - SFINAE** Be nice to the user. Rather than using `std::enable_if<>::type` use C++17 less boiler plate `std::enable_if_t<>`. And every time you can use `static_assert` instead as you can explain reason to not allow instantiation of the template with given arguments.

###### Classes

**C.1 Declaration order** - First declare `public` part of the class and place constructors and destructors as first memeber functions. Than `protected` part and `private` as last part. 
Reason: Bear in mind you want to comunicate usage of the class first, than implementation details. Programmer who will come to your header will most likely will be looking for usage of the class, not the implementation details such as member variables.

**C2. Definition** - The source file should first state reflection of the class if present. Followd by static variables definition. The first member functions defined should be constructors and after them destructor. The order of member functions is not defined.

**C2.1 Destructor definition** - Put destructor definition in source file even for defaulted destructors. This helps header separation.

**C2.2 Separators** - Each member function definition should be separated by separator. This helps readablitiy.

```
//=================================================================================
``` 

**C.3 Initialize member variables** - Uninitialized variables leads to undefined behaviour. Initialize all member variables in the constructor.

**C.3.1 Unify initialization** - Either init all member variables in initialization list or as inline initialization in header. Do not combine those as it makes **C.2** rule harder to follow.

###### Testing

This project is not fully tested. But unit test proven to be useful to keep at least basic invariants intact even during biggest changes. Rule of thumb I am trying to follow is when I encounter bug I write a unit test to fixate the invariant by it. I am trying to implement unit tests at least for the basic building block of this engine.

**TEST.1 Test structure** - Place tests to separate projects following naming convention of engine projects. Tests should be placed to source files named same way as engine sources. This could be broken by moving of files in the engine, but helps to keep track of test placemennt. Tests are executed during CI and fail of the test will block Pull requests. See: [Tests/readme](Tests/readme.md)

**TEST.2 Private functions testing** - When testing private functions of the class use fixture that is set as `friend class` of the tested class.