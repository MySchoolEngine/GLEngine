## Contributing

To start contributing into this repository simply develop your feature. The code should pass both [![Build](https://github.com/MySchoolEngine/GLEngine/workflows/build/badge.svg)](https://github.com/MySchoolEngine/GLEngine/actions?workflow=build). 

Before pull request please run clang-tidy on your code (on windows you can use clang-format.bat). And follow coding guidelines. I will review your code and then it will be merged into the codebase. If you need any help please contact me.

#### Coding rules

###### Templates

**T.1 - Just don't** Rule of the thumb - don't overdo it

**T.2 - Naming** Try to be nice to the template user. Refrein from using `class T` as universal name for everything. Do you expect enum class? Name it! `template<class Enum>`.

**T.3 - SFINAE** Be nice to the user. Rather than using `std::enable_if<>::type` use C++17 less boiler plate `std::enable_if_t<>`. And every time you can use `static_asser` instead as you can explain reason to not allow instantiation of the template with given arguments.
