# NEWorld

![Screenshot](https://raw.githubusercontent.com/Infinideastudio/NEWorld/refactor/Docs/old.png)

NEWorld is a 3D sandbox game inspired by Minecraft, licensed under [LGPL v3](http://www.gnu.org/licenses/lgpl.html).
It features an engine specifically made for this game and hand-written with C++ for maximized optimization.
We welcome all contributions.  

NEWorld 是一个类似于 Minecraft 的 3D 沙盒游戏，采用[LGPLv3许可证](http://www.gnu.org/licenses/lgpl.html)发布并受其保护。
我们使用 C++ 专为 NEWorld 其自制了游戏引擎，而没有使用任何现成的解决方案，以最大化性能，欢迎感兴趣的人加入到我们的开发中。

There were attempts to [refactor](https://github.com/Infinideastudio/NEWorld/tree/refactor) or
[rewrite](https://github.com/Infinideastudio/NEWorld/tree/renew) the game since the codebase was old and somewhat messy.
What you see here is the latest attempt to incrementally clean the codebase while keeping most of its features.

## Features

1. C++20
2. Optimized for performance
3. Should be cross-platform (although the development is currently focused in Windows)
4. Depends on OpenGL 4.5, glew, glfw, Noesis GUI.
5. Re-invented wheels
6. Written for fun!

## Compilation

1. Clone this repo
2. Install vcpkg and install dependencies
3. Download [Noesis](https://www.noesisengine.com/), and put it into `External/Noesis`
4. Draft the license key into `External/Noesis/Include/NoesisLicense.h`
5. Compile and run!