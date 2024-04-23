# 大数据存储系统与管理2024 lab2

## 实验名称

对象存储技术实践

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

- #### MinIO Client基本功能实践

通过ls指令，我们可以看到在上一次实验中创建的test-bucket

通过mb指令，我们可以另外新增一个addtest桶

此时我们再利用ls指令，可以看到目前的两个bucket

我们再通过rb指令，删除addtest这个bucket

最后利用ls指令，可以看到addtest已被成功删除，目前只有test-bucket这个桶

![](.\figures\image-20240420144717266.png)

下面我们新建一个uploadtest来进行上传测试

我们在本地新建一个txt文件，利用cp指令上传至新建立的bucket

![](.\figures\image-20240423180334056.png)

在UI界面中，我们可以看到已经上传的文件

![image-20240423180541641](.\figures\image-20240423180541641.png)

再次利用cp指令，将test文档拷贝至test-bucket中

![image-20240423180811492](.\figures\image-20240423180811492.png)

查看UI界面，文档已成功拷贝

![image-20240423180908567](.\figures\image-20240423180908567.png)

至此，我们完成了在MinIO Server中进行增删查改等基本功能的实践。
