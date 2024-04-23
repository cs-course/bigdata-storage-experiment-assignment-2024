# 实验名称

观测分析性能

# 实验环境

```
OS: Windows 10
CPU: AMD Ryzen 7 5800H with Radeon Graphics            3.20 GHz
RAM:16.0 GB
GoVersion：1.22.2
```

# 实验记录

## 实验3-1: 本地安装并配置Go语言环境

### 下载Go语言环境

在官网进行下载并按照提示进行安装。
###  配置Go环境
环境配置如figure中configGo.png 所示

## 实验3-2: 安装并配置S3bench
在github上进行下载s3bench,按照其readme文件进行依赖项的下载，去掉版本号后使用go build指令进行安装，生成了s3bench可执行文件,同样去掉其版本号
编写s3bench的配置文件以确认测试地址的accesskey和secretkey，出于未知原因，s3bench --config指令未能正常执行，flag provided but not defined: -config，
因此使用s3bench -endpoint http://127.0.0.1:63339 -bucket bucket0进行测试，测试结果已保存在figure文件夹中

# 实验小结

本次实验成功在本地实现了对minio存储服务器的压力测试。