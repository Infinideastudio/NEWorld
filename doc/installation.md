# NEWorld编译指南

0. 请下载所有所需[资源](./Notfinishedyet)，并解压到/res文件夹  
0. 请自行下载以下所有依赖库（或从[这里](./Notfinishedyet)直接下载Windows版本的整合包），并将头文件复制或链接到/dependencies/include文件夹，将库文件复制或链接到/dependencies/lib文件夹：

    - OpenAL 1.1 [官网](http://www.openal.org/)  
    - boost 1.61.0 [官网](http://www.boost.org/)  
    - freetype [官网](https://www.freetype.org/)  
    - OpenGL [官网](https://www.opengl.org/)  
    - libogg 1.3.2 [官网](https://www.xiph.org/)  
    - libvorbis 1.3.5 [官网](https://www.xiph.org/)  

# 开发者注意事项

0. 在Visual Studio中配置缩进为4个空格。
0. 安装AStyle插件，设置`--style=allman --indent=spaces=4 --indent-namespaces --mode=c`选项。
0. 在Git中配置行尾，在本地以CRLF(Windows)行尾保存文件，而远程以LF(UNIX)行尾提交文件。
