# NEWorld
NEWorld is an open-source game with similar game rules to Minecraft.  
--------------------------------------------------  
There is a Visual Studio 2015 solution. If you have installed VS2015 on your computer, you can use the solution.  
If you don't have VS Platform Tools v140 for Windows XP, you can use your default platform tools.  
You can also compile it with any C++ compiler that supports the ISO C++11 standards. But you need to set some directories.  
  
For client:  
- Library headers are in the directory 'include'.  
- Libraries are in 'libraries'. (These are MSVC static link libraries '.lib' . if you used G++, you must look for '.a' files.)  
- Source code is in 'source'. (Maybe there are some encoding problems...)  
  
For server:  
Just compile it and run!
--------------------------------------------------  
Welcome to develop with us!  

全新的NEWorld!

请下载libraries并放到NEWorld项目根目录(Libraries)[http://pan.baidu.com/s/1jHz7rAe]

最新进度在0.5.0分支

Git协作协议(试行)

NEWorld Git协作基本原则

- 所有代码使用UTF-8编码
- 不要上传生成文件，中间文件(*.exe,*.obj)，不要擅自修改.gitignore
- 每个人对自己写的代码负责
- 大文件(超过1MB的非源码文件)使用git lfs
- 所有人必须通过git测试之后才能拥有write权限，否则请使用PR
- 确保主要开发人知晓并同意你要做的大修改（重构，加入新库等）
- 确保你使用的库都是可以跨平台的
- 不要擅自修改license
- 经常浏览issues，解决属于你范围的issues
- 所有分支都应该可以（或计划可以）合并到master分支
- merge的时候应该尊重其他作者的修改；如果没有特殊情况，不要使用git push -force；如果没有必要，不要使用git revert回滚别人的代码
