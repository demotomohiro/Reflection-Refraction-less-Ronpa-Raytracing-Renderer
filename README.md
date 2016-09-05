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
* Boost(program_options wave)
* GLEW
* libpng

## How to build
```console
git clone https://github.com/demotomohiro/Reflection-Refraction-less-Ronpa-Raytracing-Renderer.git
mkdir build
cd build
cmake ../Reflection-Refraction-less-Ronpa-Raytracing-Renderer -G "Visual Studio 14 2015 Win64"
```

Open build/batchRIshader.sln with Visual Studio and build it. 


This software is released under the MIT License, see LICENSE.
