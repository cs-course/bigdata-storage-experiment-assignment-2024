# 实验名称

搭建minio服务器以及mc客户端

# 实验环境

```
OS: Windows 10
CPU: AMD Ryzen 7 5800H with Radeon Graphics            3.20 GHz
RAM:16.0 GB
```

# 实验记录

## 实验1-1: 下载安装Minio并在本地部署服务器

### 在官网下载安装minio

如实验记录LAB1\figure\CreateMinioServer.png及	downloadMinio.png所示。
下载完成后在cmd中执行minio.exe server D:\minio

### 安装minio client


如LAB1\figure\downloadMc.png及setupMc.png所示。
在cmd中执行mc.exe --help

## 实验1-2: 终端部署完成后通过浏览器初始化服务器

如MinioServer.png所示。
本次实验在命令行中执行的命令以txt文件形式记录在assets文件夹中。

# 实验小结

本次实验成功在本地部署了Minio服务器和其相关支持服务Minio Client。