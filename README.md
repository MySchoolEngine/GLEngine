# GLEngine

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/0c09524c2a59475ea538d0ec615a999d)](https://app.codacy.com/gh/MySchoolEngine/GLEngine?utm_source=github.com&utm_medium=referral&utm_content=MySchoolEngine/GLEngine&utm_campaign=Badge_Grade_Dashboard)[![Build](https://github.com/MySchoolEngine/GLEngine/workflows/build/badge.svg)](https://github.com/MySchoolEngine/GLEngine/actions?workflow=build)

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
premake5 --glfwapi=vulkan --vulkanPath=%VULKAN_SDK% vs2019
```

## Features

*  OpenGL rednerer
*  Entity component system (basic but working solution)
*  Shader preprocessor and loader
*  Scene, model and image loading

### Experimental features

*  Vulkan renderer - WIP, no build validation, not exactly clear build instructions
*  Atmospheric renderer - Radiance working ok-ish, but Irradiance part is absolutelty wrong. This means nice sky picture, but aweful ligting model.
*  Skeletal animation - I haven't tested this for ages.
*  Path tracing - most of basic functionality implemented. Never tested biasness thou.

## Documentation
Most of documentation can be found here on github/in code.

Some user/programmer documentation could be found [here](https://rohacekd.github.io/GLEngine-Documentation/).

## [Contributing](CONTRIBUTING.md)
This repository is open for contribution. You can start by reading [this](CONTRIBUTING.md). If you have any question regarding code or feature feel free to contact me.

## Architecure
[![Architecture](https://github.com/RohacekD/GLEngine/blob/master/wikidata/Architecture.svg)](https://lucid.app/lucidchart/invitations/accept/d2772b03-bc43-4301-b71a-b145bfef3e73)

## PathTracing
![CornellBox](https://github.com/RohacekD/GLEngine/blob/master/wikidata/PathTraced.png)

*  Red lines denote wrong dependencies
*  Blue lines denote opposite dependencies
*  The dashed line denotes planned dependency

## Licensing
License can be found in [LICENSE](LICENSE)
