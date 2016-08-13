Reflection/Refraction-less Ronpa Raytracing Renderer
===========
OpenGL offline renderer.
レイトレ合宿3!!!で公開したレンダラ.

[レンダリング結果画像(サイズ:15360x8640)](https://drive.google.com/file/d/0B7G5goy1SEP2S2RwSG51SVlYZ0U/view?usp=sharing)

[3840x2160に縮小した画像](http://demotomohiro.github.io/pic/render2015_4k.png)


作者: Tomohiro

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
