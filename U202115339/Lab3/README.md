# 实验名称
Lab3观测性能分析
# 实验环境
```
Microsoft Window 10 
Git-2.44.0-64-bit.exe
Visual Studio Code1.88.1.0
Server	minIO Server
Client	boto3 & mc
Enviroment	python 3.12 & s3bench
```
# 实验记录
## Lab 3-1 : 测试
### 第一步：环境安装

在官网下载 GO 语言

使用go install github.com/igneous-systems/s3bench@latest 安装 s3bench

### 第二步：执行测试

使用以下 s3bench 指令对 minIO 中的 loadgen 桶进行测试
```
s3bench -accessKey=PjImVufTmBqriPgzp0Zo -accessSecret=dCL3V2KvN7bAnAZzsqbGJJYObSjlah11BIvelapP -endpoint=http://10.11.163.247:55750 -bucket=loadgen -objectNamePrefix=loadgen\ -numClients=10 -numSamples=100 -objectSize=1024
```
在指令结尾加上 `>filepath\file.filetype` 可保存执行结果到 filepath\file.filetype
执行结果
## Lab 3-2 : 观测对象大小对性能的影响
### 测试目的
对象大小对存储性能的影响

### 测试分析
当观测对象大小对性能影响时，需要关注读写延迟、吞吐量、CPU使用率、内存使用、磁盘I/O、网络指标、系统负载、错误与超时率以及缓存命中率这些数据和指标。通过收集和分析上述数据和指标，我们可以更全面地了解对象大小对系统性能的影响，并找出潜在的性能瓶颈或优化机会。

本次实验仅以读写延迟及吞吐率为反应性变数，对性能影响进行观察与分析。

### 测试脚本：
```
工具	       数据生成                 数据图生成                 文件路径
s3bench	       tesh.sh	                myplot.py	          s3-latency
python-boto3   latency_test_MinIO.py	latency-plot.ipynb	  py-latency
```
# 实验小结
...
