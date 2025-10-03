# GLEngine

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/0c09524c2a59475ea538d0ec615a999d)](https://app.codacy.com/gh/MySchoolEngine/GLEngine?utm_source=github.com&utm_medium=referral&utm_content=MySchoolEngine/GLEngine&utm_campaign=Badge_Grade_Dashboard)[![Build](https://github.com/MySchoolEngine/GLEngine/workflows/build/badge.svg)](https://github.com/MySchoolEngine/GLEngine/actions?workflow=build)
[![BCH compliance](https://bettercodehub.com/edge/badge/MySchoolEngine/GLEngine?branch=master)](https://bettercodehub.com/)

![Preview image of area light](https://github.com/RohacekD/GLEngine/blob/master/wikidata/AreaLight.png?raw=true)


## Setup
```
git clone <https://github.com/RohacekD/GLEngine>
cd GLEngine
git submodule init
git submodule update
premake5 vs2019 (or whatever verison are you using)
```
### Vulkan build

You can run both DirectX and OpenGL renderers side by side but in case of Vulkan you need to choose between OpenGL and Vulkan. If you would like to select Vulkan you need to set premake in this way:
```
premake5 --glfwapi=vulkan vs2019
```

## Memory sanitazer
If you want to use memory sanitazer, please use Asan configuration and add path to the ASAN DLLs to your PATH variable. E.g.:
```
C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\bin\Hostx64\x64
```

## Features

*  OpenGL rednerer
*  Entity component system (basic but working solution)
*  Shader preprocessor and loader
*  Scene, model and image loading

### Experimental features

*  Vulkan renderer - WIP, not exactly clear build instructions (build only validated or MSVC)
*  Atmospheric renderer - Radiance working ok-ish, but Irradiance part is absolutelty wrong. This means nice sky picture, but aweful ligting model.
*  Skeletal animation - I haven't tested this for ages.
*  Path tracing - most of basic functionality implemented. Never tested biasness thou.

## Documentation
Most of documentation can be found here on github/in code.

Some user/programmer documentation could be found [here](https://rohacekd.github.io/GLEngine-Documentation/).

I have written short [documentation](CodeDocumentation.md) for not so obvious cases for ease of development.

## [Contributing](CONTRIBUTING.md)
This repository is open for contribution. You can start by reading [this](CONTRIBUTING.md). If you have any question regarding code or feature feel free to contact me.

## Architecure
[![Architecture](https://github.com/RohacekD/GLEngine/blob/master/wikidata/Architecture.svg)](https://lucid.app/lucidchart/invitations/accept/d2772b03-bc43-4301-b71a-b145bfef3e73)

*  Red lines denote wrong dependencies
*  Blue lines denote opposite dependencies
*  The dashed line denotes planned dependency

## PathTracing
![CornellBox](https://github.com/RohacekD/GLEngine/blob/master/wikidata/PathTraced.png)

## Licensing
License can be found in [LICENSE](LICENSE)