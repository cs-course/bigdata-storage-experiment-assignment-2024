# 实验名称

实践对象存储基本功能

# 实验环境

```
Virtual Machine: VMware Workstation 17
OS: Ubuntu22.04
CPU: Ryzen R7 5800H
RAM: 8G
```

# 实验记录

在lab1中，使用Docker部署了Swift服务器。启动服务器后，激活虚拟环境，即可运行python程序，实现对象存储的CRUD功能。

我们将./assets/source/目录下的文件作为写入的源文件，将./assets/taret/目录作为从对象存储服务器中读取的文件存放的位置。完成写入和读出后，检查文件的diff即可得知读写是否成功。

运行脚本可以一步进行测试：

```shell
./assets/test_store_retrieve.sh
```

# 实验小结

经过测试，完成了对象存储的CRUD基本功能。