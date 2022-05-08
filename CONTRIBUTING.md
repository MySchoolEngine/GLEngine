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

**T.3 - SFINAE** Be nice to the user. Rather than using `std::enable_if<>::type` use C++17 less boiler plate `std::enable_if_t<>`. And every time you can use `static_asser` instead as you can explain reason to not allow instantiation of the template with given arguments.
