# NEWorld编译指南

### Windows(Old)

1. 请下载所有所需[资源](./Notfinishedyet)，并解压到/res文件夹
2. 请自行下载以下所有依赖库（或从[这里](./Notfinishedyet)直接下载Windows版本的整合包），并将头文件复制或链接到/dependencies/include文件夹，将库文件复制或链接到/dependencies/lib文件夹：

    - OpenAL 1.1 [官网](http://www.openal.org/)
    - boost 1.61.0 [官网](http://www.boost.org/)
    - freetype [官网](https://www.freetype.org/)
    - OpenGL [官网](https://www.opengl.org/)
    - libogg 1.3.2 [官网](https://www.xiph.org/)
    - libvorbis 1.3.5 [官网](https://www.xiph.org/)

### Linux

Compile NEWorld in Linux

1. You need have the dependences installed.
   dependences: git,gcc/clang,cmake,boost(1.61.0+),libgl,glew,sdl2,sdl_image,freetype2,alsa
2. git clone https://github.com/Infinideastudio/NEWorld.git
3. mkdir /build/cmake/
4. cd /build/cmake/
5. cmake ../../
6. make
