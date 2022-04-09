# 文件hash计算器
一个用 wxWidgets 写的，支持同时计算多个文件的 CRC32 和 MD5 校验值的小应用。

![Snipaste_2022-04-07_22-20-39](https://user-images.githubusercontent.com/17548735/162221164-e456f4f3-1547-4b20-af3f-468a70a19352.png)

# 使用方式
直接从文件管理器拖入文件，或者点击"文件-->打开"，选中需要计算的文件，再点打开。

# 依赖
1. wxWidgets  UI框架
2. openssl  md5计算
3. [crc32](https://gist.github.com/timepp/1f678e200d9e0f2a043a9ec6b3690635)
4. [threadpool](https://github.com/lzpong/threadpool) 用以支持同时计算多个文件，同时计算的文件个数是CPU虚拟核心数。

# 更新记录
 - 2022.04.07 初版提交
