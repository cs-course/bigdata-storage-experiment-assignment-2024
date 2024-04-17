# 实验名称

# 实验环境
| 操作系统 | Windows 11 专业版                                          |
| -------- | ---------------------------------------------------------- |
| 处理器   | AMD Ryzen 5 4600U with Radeon Graphics            2.10 GHz |
| 内存     | 16G                                                        |
| 服务器端 | MinIO Server                                               |
| 系统类型 | 64 位操作系统, 基于 x64 的处理器                           |
| 客户端   | Boto3                                                      |

# 实验记录

## 实验lab3

##### puts_吞吐量，传输时间：



| Object_size | total_transferred | numClients | duration | numErrors | throughput             |
| ----------- | ----------------- | ---------- | -------- | --------- | ---------------------- |
| 512         | 1048576 bytes     | 1          | 17.248 s | 0         | 60793.273bytes/second  |
| 1024        | 1048576 bytes     | 1          | 8.472 s  | 0         | 123774.113bytes/second |
| 2048        | 1048576 bytes     | 1          | 4.243 s  | 0         | 247127.053bytes/second |
| 4096        | 1048576 bytes     | 1          | 2.07 s   | 0         | 506522.299bytes/second |
| 8192        | 1048576 bytes     | 1          | 1.05s    | 0         | 999100.526bytes/second |

![1712102529866.png](./figure/1712102529866.png)

##### puts——延迟:

![1712063087559.png](./figure/1712063087559.png)

![1712063121488.png](./figure/1712063121488.png)

![1712063149762.png](./figure/1712063149762.png)

![1712063178830.png](./figure/1712063178830.png)

![1712063223300.png](./figure/1712063223300.png)


##### gets——吞吐量，传输时间：



| numClients | total_transferred | duration | throughput               | errors |
| ---------- | ----------------- | -------- | ------------------------ | ------ |
| 1          | 1196032 bytes     | 0.677 s  | 1767279.491 bytes/second | 0      |
| 4          | 1196032 bytes     | 1.915 s  | 624689.453 bytes/second  | 0      |
| 8          | 1196032 bytes     | 3.595 s  | 332734.977 bytes/second  | 0      |
| 16         | 1196032 bytes     | 5.538 s  | 215958.779 bytes/second  | 0      |
| 32         | 1196032 bytes     | 5.771 s  | 207263.482 bytes/second  | 0      |
| 64         | 1196032 bytes     | 5.194 s  | 230254.36 bytes/second   | 0      |
| 128        | 1196032 bytes     | 5.087 s  | 235113.876 bytes/second  | 0      |
| 256        | 1196032 bytes     | 5.745 s  | 208180.769 bytes/second  | 0      |
| 512        | 1196032 bytes     | 5.665 s  | 211123.831 bytes/second  | 0      |
| 1024       | 1196032 bytes     | 4.608 s  | 259570.931 bytes/second  | 0      |

![1712104539156.png](./figure/1712104539156.png)

##### gets——延迟:

![1712063255444.png](./figure/1712063255444.png)

# 实验小结
