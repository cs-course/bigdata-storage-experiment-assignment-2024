# 实验名称 Lab1 搭建对象存储

  在本实验中通过Docker搭建了OpenStack Swift存储系统

# 实验环境 

  本机环境如下：

   处理器 AMD Ryzen 7 7840H with Radeon 780M Graphics 3.80 GHz
   机带RAM 32.0 GB (27.7 GB 可用)
   系统类型 64 位操作系统, 基于 x64 的处理器
   操作系统 Windows 11 家庭中文版

 


# 实验记录

## 安装Docker Desktop。

## 从dockerhub拉取镜像docker-swift-onlyone。

## 使用该镜像创建并且运行容器。图1证明容器成功运行。

## 在容器中安装python-swiftclient客户端，使用命令 pip install python-swiftclient。图2证明安装成功。

## 把用户credentials设置成环境变量。见图3。

## 使用python-swiftclient的CLI命令查看用户状态，正确显示用户信息，表示前后端可以协同工作。系统搭建完成，见图4。


# 实验小结

## 有关Docker
 
  第一次使用Docker平台，使用几条简单的命令语句迅速地部署好了一个完整的系统，而不需要逐一安装依赖包和确保版本匹配，体会到容器的便捷。

## 有关Swift

  服务端是Docker镜像中自带的，客户端需要自己安装，但是两者都比较简单。难点出现在用户credentials上面。因为是在容器里面操作，端口号应该取8080，而不是12345，在发现这个问题之后，成功关联上了server和client，从而搭建系统完毕。