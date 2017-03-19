# NEWorld编译指南

### Windows

#### 如果您已经安装有Visual Studio的话

Visual Studio 配置方法(32位)：

1. Clone NEWorld

2. 下载[dependencies.exe](https://raw.githubusercontent.com/Infinideastudio/NEWorld-binaries/master/dependencies.exe)，解压到./

3. 安装[Python](https://www.python.org/downloads/)(Python3或Python2)

4. 运行./scripts/bootstrap.bat

5. 打开./build/visualstudio_v14/NEWorld.sln，设置调试设置

   RunClient调试设置：

-  命令 NEWorldLauncher.exe

-  命令参数 client

-  工作目录 $(SolutionDir)../../release/

    RunServer调试设置：

-  命令 NEWorldLauncher.exe

-  命令参数 server

-  工作目录 $(SolutionDir)../../release/

7. 下载boost, 复制（或软连接）boost/boost到VC/include或dependencies/include（无需编译boost）

8. 编译运行！


VS配置方法(64位)：

配置方法和32位类似，只是将相关文件改为64位版本。

#### 如果您没有安装Visual Studio 2015的话

您也可以使用CMake来编译NEWorld，但是我们推荐您安装[Visual Studio 2015](https://go.microsoft.com/fwlink/?LinkId=691978&clcid=0x407) (您也可以通过其它渠道获取VS2015)

### Linux
To compile NEWorld on Linux

1. You need to have the dependencies installed./您需要安装依赖。
   dependencies/依赖: git,gcc/clang,cmake(3.1+),boost(1.61.0+),libgl,glew,sdl2,sdl_image,alsa,raknet,flatbuffers,gtest(optional)
2. git clone https://github.com/Infinideastudio/NEWorld.git
3. mkdir /build/cmake/
4. cd /build/cmake/
5. cmake ../../
6. make
