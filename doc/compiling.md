# 编译教程

## 在Mac OS X上编译

NEWorld使用了OpenGL、OpenAL、GLFW3、FreeType、Vorbis、Ogg这几个外部依赖库。OpenGL是Mac OS X系统中自带的，不需要额外安装。虽然OpenAL在Mac OS X中也自带，但是NEWorld使用了一些系统内置的标准OpenAL中没有的功能，需要安装第三方的OpenAL软件实现。如果您安装了Homebrew包管理系统，您可以用

```
brew install freetype openal-soft libogg libvorbis libvorbisfile glfw3
```

命令安装这几个库。NEWorld中已经包含了可供Mac OS X编译的Makefile文件。您只需要运行`make`命令就可以编译NEWorld了。您可以指定`CXXFLAGS`和`LDFLAGS`分别手动调整编译参数和链接参数。

样例：

```
CXXFLAGS='-march=broadwell -mtune=intel -ffast-math' make NOWARNING=1 -j4
```

Makefile选项| 作用
------------|---
DEBUG       | 为1时，编译调试版本(`-g`)；否则编译优化版本(`-O2`)
NOWARNING   | 为1时，不输出编译警告(`-w`)。

