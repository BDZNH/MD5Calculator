# 文件hash计算器
一个用 wxWidgets 写的，支持同时计算多个文件的 CRC32 和 MD5 校验值的小应用。

![Snipaste_2024-05-11_21-31-17](https://github.com/BDZNH/MD5Calculator/assets/17548735/9dbdff72-a7d3-4877-8ee7-ccaa7c4c65e9)

# 使用方式
直接从文件管理器拖入文件，或者点击"文件-->打开"，选中需要计算的文件，再点打开。

# 依赖
1. wxWidgets  UI框架
2. openssl  md5计算
3. [crc32](https://rosettacode.org/wiki/CRC-32#C)
4. [threadpool](https://github.com/lzpong/threadpool) 用以支持同时计算多个文件，同时计算的文件个数是CPU虚拟核心数。
5. [libqrencode](https://github.com/fukuchi/libqrencode) 用以支持生成二维码
6. [svpng](http://github.com/miloyip/svpng)

# 编译
通过 Visual Studio 编译，下面是大致步骤
1. 安装 vcpkg
2. 通过 vcpkg 安装 wxWidgets,openssl
3. 下载 https://github.com/fukuchi/libqrencode 至 FileHashCalculator/thirdparty/libqrencode
4. 拷贝 FileHashCalculator/config.h 至 FileHashCalculator/thirdparty/libqrencode
5. 编译
