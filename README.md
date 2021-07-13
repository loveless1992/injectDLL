# injectDLL
DLL注入，主要逆向微信PC版。

现在处于开发期，后续会添加各种模块，当前注入工具及DLL均兼容3.2.x版本微信

如需支持其他版本，需要自行测量偏移。

2210713版本更新:

需要将AutoReplayMessage\lib\Pre-built.2\lib下两个文件夹内的DLL至C:\Windows\SysWOW64下，可以运行，否则pthread包会报错
