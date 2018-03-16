# NEWorld结构说明

更新时间: 16.9.2

## NEWorld(Shared) 

### Common

| 类                  | 文件                 | 作用                       | 内含                          | 备注                                    | 单例[1] |
| ------------------ | ------------------ | ------------------------ | --------------------------- | ------------------------------------- | ----- |
| AABB               | aabb.h             | 碰撞箱                      |                             |                                       |       |
| CommandExecuteStat | command.h          | 命令执行结果                   |                             |                                       |       |
| Command            | command.h          | 一条指令实例                   |                             |                                       |       |
| CommandInfo        | command.h          | 一条指令的信息                  |                             | 作者、帮助信息等                              |       |
| -                  | consolecolor.h     | Logger用的颜色               |                             |                                       |       |
| Logger             | logger.h           | 用于输出                     |                             | 每次输出都会实例化一个Logger                     |       |
| -                  | settings.cpp       | 用于加载shared的设置            |                             | 调用Settings类                           |       |
| -                  | utils.h            | 一些实用工具                   |                             |                                       |       |
| Vec3               | vec3.h             | 三维向量                     |                             |                                       |       |
| enum Identifier    | identifier.h       | 包的标识符                    |                             |                                       |       |
| NetworkStructure   | networkstructure.h | 网络包(基类)                  |                             |                                       |       |
| XXXPacket          | networkstructure.h | 各种包的定义                   |                             | 详见protocol.md                         |       |
| Packet             | packet.h           | 没有处理过的网络包                |                             |                                       |       |
| Session            | session.h          | session                  |                             |                                       |       |
| TakeDataHelper     | takedatahelper.h   | 用于帮助从raw data里提取数据       |                             |                                       |       |
| Object             | object.h           | 所有生物的基类                  |                             |                                       |       |
| PlayerObject       | playerobject.h     | 玩家                       |                             | 继承Object。被Client中的Player(被操作的那个玩家)继承。 |       |
| PluginData         | plugin.h           | 记录插件基本信息的结构。             |                             | 包括插件显示名、作者名、插件内部名。                    |       |
| Plugin             | plugin.h           | 插件类。                     |                             |                                       |       |
| -                  | pluginapi.h        | *待补充*                    |                             |                                       |       |
| PluginManager      | pluginmanager.h    | 用于管理所有插件。                |                             |                                       | Y     |
| BlockData          | blockdata.h        | 单个方块。                    |                             |                                       |       |
| BlockManager       | blockmanager.h     | 管理所有方块**种类**。            | BlockType(s)                |                                       |       |
| BlockType          | blocktype.h        | 方块类型，记录方块的属性。            |                             |                                       |       |
| Chunk              | chunk.h            | 地图区块。                    |                             |                                       |       |
| ChunkPointerArray  | chunkpointerarray  | 记录玩家周围区块的指针，用于加速查找区块的速度。 |                             | 优化用。有时可能会被缩写成CPA。和World是一对一的。         |       |
| World              | world.h            | 一个游戏世界。                  | Chunk(s), ChunkPointerArray |                                       |       |
| WorldManager       | worldmanager.h     | 管理多世界。                   | World(s)                    |                                       |       |

[1] : 在这里，类中static变量等整个程序只会**同时**存在一个实例的类都被成为单例。后表同，不重复赘述。

## NEWorld(Client)

| 类             | 文件              | 作用                   | 内含                              | 备注                            | 单例   |
| ------------- | --------------- | -------------------- | ------------------------------- | ----------------------------- | ---- |
| Application   | NEWorld.h       | 客户端程序的开始，管理整个程序。     | BlockManager, PluginManager     |                               | Y    |
| ChunkRenderer | chunkrenderer.h | 用于渲染单个chunk。         | VertexBuffer和static VertexArray | 和Chunk是一对一的。                  |      |
| Mat4          | Mat4.h          | 矩阵类。用来处理矩阵运算。        |                                 |                               |      |
| -             | opengl.h        | 用于引入OpenGL。          |                                 | 所有用到OpenGL的文件都应该#include这个文件。 |      |
| Renderer      | renderer.h      | 渲染器。底层的渲染器，用来渲染图形。   |                                 |                               | Y    |
| Texture       | texture.h       | 材质类。用于加载和储存材质。       |                                 |                               |      |
| VertexFormat  | vertexarray.h   | 用来储存VAO的格式。          |                                 |                               |      |
| VertexArray   | vertexarray.h   | 用来储存VAO。             | VertexFormat                    |                               |      |
| VertexBuffer  | vertexarray.h   | 用来储存VBO。             | VertexFormat                    |                               |      |
| WorldRenderer | worldrenderer.h | 用来渲染整个世界。            | ChunkRenderer                   |                               | Y    |
| GameScene     | gamescene.h     | 整个游戏场景。              | World, Player, WorldRenderer    |                               | Y    |
| MainMenu      | mainmenu.h      | 菜单场景。                |                                 |                               | Y    |
| BackGround    | mainmenu.h      | 菜单的背景。               |                                 |                               | Y    |
| MainWindow    | window.h        | 窗口类。                 |                                 |                               | Y    |
| Player        | player.h        | 玩家类。                 |                                 | 特指被操作的玩家。继承PlayerObject。      |      |
| -             | main.cpp        | 调用Application的run函数。 |                                 | main函数所在文件。                   |      |

## NEWorldServer
| 类                 | 文件                  | 作用              | 内含                                       | 备注                       | 单例   |
| ----------------- | ------------------- | --------------- | ---------------------------------------- | ------------------------ | ---- |
| CommandController | commandcontroller.h | 管理所有指令。         |                                          |                          | Y    |
| -                 | settings.h          | 读写服务器特有设置。      |                                          |                          |      |
| ChunkLoader       | chunkloader.h       | 用于加载单个指定区块。     |                                          | 生成或加载区块。与Chunk一对一。       |      |
| WorldLoader       | worldloader.h       | 用于加载指定世界。       |                                          | 与World一对一，会调用ChunkLoader |      |
| Server            | server.h            | 管理整个NEWorld服务器。 | io_service, Session(s), WorldManager, BlockManager, PluginManager, WorldLoader(s), CommandController |                          | Y    |
| -                 | main.cpp            | 调用Server类启动服务器。 |                                          | main函数所在文件。              |      |
