Reflection/Refraction-less Ronpa Raytracing Renderer
===========
OpenGL offline renderer.
レイトレ合宿3!!!で公開したレンダラ.

[レンダリング結果画像(サイズ:15360x8640)](https://drive.google.com/file/d/0B7G5goy1SEP2S2RwSG51SVlYZ0U/view?usp=sharing)

[3840x2160に縮小した画像](http://demotomohiro.github.io/pic/render2015_4k.png)

by Tomohiro

This is offline OpenGL renderer designed to execute heavy shader which might take more then 1 second to complete.
It is also designed to render large size image like 30720x17280 pixels by dividing a image into multiple tiles and render to each tile.
It supports fragment shader which is executed for every pixels on a image and vertex/fragment shaders to render particles.

## Sample images:
Random 268435456 particles: [30720x17280 741MB](https://drive.google.com/file/d/0B7G5goy1SEP2T0U4dFFYVmRNTGs/view?usp=sharing), [3840x2160](http://demotomohiro.github.io/pic/render2016_4k.png)

## Requirement
* OpenGL 4.3

## Required libraries:
* Boost(program_options, wave)
* GLEW
* libpng
* [GLUtil](https://github.com/demotomohiro/GLUtil)

## Required tools:
CMake
### On Linux
gcc
### On Windows
Visual Studio 2015

## How to build
```console
git clone https://github.com/demotomohiro/Reflection-Refraction-less-Ronpa-Raytracing-Renderer.git
mkdir build
cd build
```
On Windows
```console
cmake -G "Visual Studio 14 2015 Win64" ../Reflection-Refraction-less-Ronpa-Raytracing-Renderer
msbuild batchRIshader.vcxproj
```
You can also open batchRIshader.sln with Visual Studio and build it. 

On Linux
```console
cmake ../Reflection-Refraction-less-Ronpa-Raytracing-Renderer
make
```
### cmake options:
* -CMAKE_BUILD_TYPE=[Debug|Release|RelWithDebInfo|MinSizeRel]
Specifies the build type. This options is ignored when used with Visual Studio.
* -DCMAKE_PREFIX_PATH="/path/to/library;/other/path"
List of directories used to search libraries.


This software is released under the MIT License, see LICENSE.
