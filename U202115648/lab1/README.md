# 实验名称

搭建Swift服务器以及客户端环境

# 实验环境

```
Virtual Machine: VMware Workstation 17
OS: Ubuntu22.04
CPU: Ryzen R7 5800H
RAM: 8G
```

# 实验记录

## 实验1-1: 使用Docker搭建Swift服务器

### 在纯净的Ubuntu上安装Docker

运行assets/目录下的脚本即可。

```shell
./assets/docker_deploy.sh
```

### 在Docker下部署Swfit镜像

```shell
./assets/swift_deloy.sh
```

## 实验2-1: 使用swiftclient客户端连接Swift服务器

### 安装python3环境

```shell
./assets/python3_install.sh
```

### 创建虚拟环境并安装swiftclient库

```shell
./assets/create_venv.sh
```

# 实验小结

通过本次实验掌握了Docker部署对象存储服务器的方法。

由于Swift使用的并非S3协议，因此boto3客户端无法连接Swift服务器，故而使用swiftclient