# NEWorld

全新的NEWorld!

请下载libraries并放到NEWorld项目根目录(Libraries)[http://pan.baidu.com/s/1jHz7rAe]

最新进度在0.5.0分支

Git协作协议(试行)

NEWorld Git协作基本原则
NEWorld Git Cooporation: Basic Rules

- 所有代码使用UTF-8编码
- Use UTF-8 in code files.
- 不要上传生成文件，中间文件(*.exe,*.obj)，不要擅自修改.gitignore
- Don't upload build results, temporary files (*.exe, *.obj). Don't modify .gitignore without communication.
- 每个人对自己写的代码负责
- Everyone must track his/her own code.
- 大文件(超过1MB的非源码文件)使用git lfs
- Use 'git lfs' to upload big files (binary files over 1MB)
- 所有人必须通过git测试之后才能拥有write权限，否则请使用PR
- Everyone must pass the Git test before he/she have write permission. If not, use pull requests instead.
- 确保主要开发人知晓并同意你要做的大修改（重构，加入新库等）
- Ensure that mayor developers know and agree with you before big modifications (rebuild, add new libraries and so on).
- 确保你使用的库都是可以跨平台的
- Ensure that libraries you use are portable.
- 不要擅自修改license
- Don't modify license without communication.
- 经常浏览issues，解决属于你范围的issues
- View issues frequently and solve issues that belong to you.
- 所有分支都应该可以（或计划可以）合并到master分支
- All branches should be (or plan to be) able to merge to 'master' branch.
- merge的时候应该尊重其他作者的修改；如果没有特殊情况，不要使用git push -force；如果没有必要，不要使用git revert回滚别人的代码
- While solving conflicts, please don't destroy others' contribution; no special reason, don't use 'git push -force'; don't use 'git revert' to revert others' changes if not nesserary.
