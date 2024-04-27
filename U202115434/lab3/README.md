# 实验名称

观测分析性能-预制负载范例观察

## 实验环境

```shell
       /\         rick@ricksarchlinux
      /  \        os     Arch Linux
     /\   \       host   82JW Lenovo Legion R7000P2021
    /      \      kernel 6.8.1-arch1-1
   /   ,,   \     uptime 3h 20m
  /   |  |  -\    pkgs   1368
 /_-''    ''-_\   memory 6250M / 13830M
```

使用 docker 搭建的 openstack-swift

## 实验操作

```sh
s3bench \
-accessKey=test:tester \
-accessSecret=testing \
-bucket=tester \
-endpoint=http://127.0.0.1:18080 \
-numClients=20 \
-numSamples=1000 \
-objectNamePrefix=tester \
-objectSize=1024 \
-region=us-east-1
```

测试结果如下

```text
Test parameters
endpoint(s):      [http://127.0.0.1:18080]
bucket:           tester
objectNamePrefix: tester
objectSize:       0.0010 MB
numClients:       20
numSamples:       1000
verbose:       %!d(bool=false)


Results Summary for Write Operation(s)
Total Transferred: 0.977 MB
Total Throughput:  0.30 MB/s
Total Duration:    3.288 s
Number of Errors:  0
------------------------------------
Write times Max:       0.181 s
Write times 99th %ile: 0.146 s
Write times 90th %ile: 0.085 s
Write times 75th %ile: 0.074 s
Write times 50th %ile: 0.063 s
Write times 25th %ile: 0.053 s
Write times Min:       0.030 s


Results Summary for Read Operation(s)
Total Transferred: 0.977 MB
Total Throughput:  0.38 MB/s
Total Duration:    2.580 s
Number of Errors:  0
------------------------------------
Read times Max:       0.097 s
Read times 99th %ile: 0.090 s
Read times 90th %ile: 0.071 s
Read times 75th %ile: 0.061 s
Read times 50th %ile: 0.050 s
Read times 25th %ile: 0.040 s
Read times Min:       0.017 s


Cleaning up 1000 objects...
Deleting a batch of 1000 objects in range {0, 999}... Succeeded
Successfully deleted 1000/1000 objects in 5.092545938s
```
