# NW工程指导委员会 (NEWorld Engineering Steering Committe) 通告

目前，NEWorld正在执行希修斯工程。请不要加入新功能。详细情况请查阅`theseus.md`。

# NEWorld

NEWorld是一个开放源代码的模仿Minecraft游戏规则的游戏。

## 编译

### Visual Studio用户

**请先下载libraries并放到NEWorld项目根目录[百度云/1jHz7rAe](http://pan.baidu.com/s/1jHz7rAe)**

如果你没有Visual Studio Platform Tools v140 for Windows XP，你可以使用默认的平台工具。

### UNIX用户

放心好了，不会有人给你如何在类UNIX操作系统中编译NEWorld的任何提示的。

## Git协作协议(试行)

NEWorld Git协作基本原则

- 所有代码使用UTF-8编码。

- 不要上传生成文件，中间文件(*.exe,*.obj)，不要擅自修改.gitignore。

- 每个人对自己写的代码负责。

- 大文件(超过1MB的非源码文件)使用`git lfs`。

- 所有人必须通过git测试之后才能拥有write权限，否则请使用PR。

- 确保主要开发人知晓并同意你要做的大修改（重构，加入新库等）。

- 确保你使用的库都是可以跨平台的。

- 不要擅自修改license。

- 经常浏览issues，解决属于你范围的issues。

- 所有分支都应该可以（或计划可以）合并到master分支。

- merge的时候应该尊重其他作者的修改；如果没有特殊情况，不要使用`git push -force`；如果没有必要，不要使用`git revert`回滚别人的代码。

- GitHub是一个存储源代码的地方，最好不要把二进制文件放上来。
