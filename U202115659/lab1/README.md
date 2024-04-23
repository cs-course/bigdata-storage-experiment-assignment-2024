# 大数据存储系统与管理2024 lab1

## 实验名称

搭建对象存储

## 实验环境

|                |                                                   |
| -------------- | ------------------------------------------------- |
| 处理器         | AMD Ryzen 7 5800U with Radeon Graphics   1.90 GHz |
| 内存           | 32.0GB                                            |
| 操作系统       | Windows 10 家庭中文版                             |
| 对象存储服务器 | MinIO Server                                      |
| 对象存储客户端 | MinIO Client                                      |
| 评测工具       | s3bench                                           |

## 实验记录

* #### 对象存储服务器MinIO Server的安装与运行

  直接在官网下载Windows环境下的MinIO Server，此时我们会得到一个exe文件minio.exe

  ![](.\figures\屏幕截图 2024-04-23 002948.png)

在minio.exe所在文件夹下运行命令行，通过如下指令使服务器开始运行。

```
.\minio.exe server D:\minio --console-address :9090
```

![](.\figures\image-20240420140929421.png)

然后，我们任选一个WebUI中的网址，在浏览器中进行访问，使用默认账户名与密码minioadmin登录，这时我们就会看到默认首页的buckets为空。

![](.\figures\image-20240420141708009.png)

我们可以直接点击Create a bucket来创建一个bucket。

![](.\figures\image-20240420141859251.png)

- #### 对象存储客户端MinIO Client的安装与运行

同样在minio官网上，我们下载MinIO Client客户端，此时我们也会得到一个可执行文件mc.exe。

在mc.exe所在的文件夹中打开命令行，进行客户端的初始化运行。

最开始我遇到过如下的问题：

![](.\figures\image-20240420143452087.png)

后来发现，问题在于我在命令中所给出的url链接是WebUI中的，而此处应该使用API的url路径，端口要设置正确。

改正端口号，成功运行客户端的结果如下所示

![](E:\大三下\大数据存储系统与管理\bigdata-storage-experiment-assignment-2024\U202115659\lab1\figures\image-20240420144454443.png)

## 实验记录

在lab1中，我们借助MinIO官网的服务器与用户端完成了部署，初步搭建了存储系统。
