# 实验名称
实现CRUD操作
# 实验环境
|||
|:-:|:-:|
|对象存储服务器|MinIO Server|
|对象存储客户端|MinIO Client|
|操作系统|Windows 10 21H1|
# 实验内容
使用 MinIO client 连接到 MinIO server 并实现 CRUD 操作。
首先进行环境配置，输入以下命令，设置myminio：
```powershell
cd D:\Download
.\mc alias set myminio http://127.0.0.1:9000 admin password
```
输入以下命令，列出所有桶：
```powershell
mc ls myminio
```
得到以下输出：
```powershell
[2024-04-22 14:55:14 CST]     0B big-data-storage/
```

输入以下命令，**创建**一个桶`big-data-storage`：
```powershell
mc mb myminio/big-data-storage
```
得到以下输出：
```powershell
Bucket created successfully `myminio/big-data-storage`.
```
输入以下命令，**上传**文件到桶`big-data-storage`：
```powershell
mc cp D:\Download\miscellaneous\big-data-storage\Devilman\ myminio/big-data-storage/Devilman --recursive
```
得到以下输出：
```powershell
... (output omitted)
```
输入以下命令，**下载**文件到本地：
```powershell
mc cp myminio/big-data-storage/Devilman D:\Download\miscellaneous\big-data-storage\Devilman --recursive
```
得到以下输出：
```powershell
... (output omitted)
```
输入以下命令，**删除**文件：
```powershell
mc rm myminio/big-data-storage/Devilman --recursive
```
得到以下输出：
```powershell
... (output omitted)
```
输入以下命令，**删除**桶：
```powershell
mc rb myminio/big-data-storage --force
```
得到以下输出：
```powershell
Removing `myminio/big-data-storage`...
```
# 实验总结
通过本次实验，我学会了如何使用 MinIO client 连接到 MinIO server 并实现 CRUD 操作。在实验中，我创建了一个桶`big-data-storage`，并上传了文件到桶中。然后，我又下载了文件到本地，并删除了文件和桶。通过本次实验，我对 MinIO server 和 MinIO client 有了更深入的了解，掌握了 MinIO server 和 MinIO client 的基本操作。
