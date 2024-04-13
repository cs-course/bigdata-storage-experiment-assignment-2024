# 实验名称

LAB 3 - 观测性能分析

# 实验环境

| | |
| :----- | :----- |
| Server | minIO Server |
| Client | boto3 & mc |
| Enviroment | python 3.12 & s3bench |

# 实验记录

## LAB 2-1 : 测试

**第一步：环境安装**

- 在官网下载 GO 语言

- 使用`go install github.com/igneous-systems/s3bench@latest` 安装 s3bench

**第二步：执行测试**

使用以下 s3bench 指令对 minIO 中的 loadgen 桶进行测试

    s3bench -accessKey=<accessKey> -accessSecret=<accessSecret> -endpoint=<endpoint> -bucket=loadgen -objectNamePrefix=loadgen\ -numClients=10 -numSamples=100 -objectSize=1024

- 在指令结尾加上 '>filepath\file.filetype' 可保存执行结果到 filepath\file.filetype
- [执行结果](figure/Lab3-s3-test.png)

## LAB 2-2 : 观测对象大小对性能的影响

### **测试目的**

对象大小对存储性能的影响

### **测试分析**

当观测对象大小对性能影响时，需要关注读写延迟、吞吐量、CPU使用率、内存使用、磁盘I/O、网络指标、系统负载、错误与超时率以及缓存命中率这些数据和指标。通过收集和分析上述数据和指标，我们可以更全面地了解对象大小对系统性能的影响，并找出潜在的性能瓶颈或优化机会。

本次实验仅以读写延迟及吞吐率为反应性变数，对性能影响进行观察与分析。

### **测试脚本：**

|工具|数据生成|数据图生成|文件路径|
|:---|:---|:---|:---|
|s3bench|[tesh.sh](assets/s3-latency/test.sh)|[myplot.py](assets/s3-latency/myplot.py)|[s3-latency](assets/s3-latency)
|python-boto3|[latency_test_MinIO.py](assets/py-latency/latency_test_MinIO.py) |[latency-plot.ipynb](assets/py-latency/latency-plot.ipynb)|[py-latency](assets/py-latency)

下列所展示的测试结果与分析均为 s3bench 工具下生成的结果并对其进行分析。

### **测试结果**

<div style="display: flex;">
    <img src="figure\Lab3_1-ObjectSize.JPG" alt="4Kgraph" style="flex: 50%; padding: 5px;">
</div>


### **结果分析**

1. 对象尺寸与吞吐率的关系：

     对于小到中等大小的对象（1MB至8MB），系统的`写吞吐率`有所提高，但在处理1024MB的大对象时，写吞吐率略有下降。这可能是受磁盘I/O或存储系统等其他因素影响所导致的现象。

    对于小对象（1MB至2MB），`读吞吐率`显著下降，但当对象大小增加到8MB时，读吞吐率大幅提高，尤其在1024MB下。这可能表示系统在处理较小对象时效率较低，但对于较大对象，读取效率明显提高。

2. 对象大小与延迟的关系：

    1MB到1024MB的对象大小，`写响应时间`显著增长，从6.153MB/s到951.926MB/s。这表示随着对象大小的增加，`写操作的延迟`也大幅增加。处理大对象时的写延迟远高于处理小对象的延迟，这可能是系统在处理大对象写入时的性能瓶颈。

   在1MB到2MB的对象大小范围内，`读延迟`有轻微的增长。但是，当对象大小从2MB增加到1024MB时，读延迟显著增加，从0.566MB/s增加到132.394MB/s。读取小到中等大小的对象的延迟相对较低，而读取大对象时的延迟则显著增加。对于小到中等大小的对象，读延迟相对较低。但当处理大对象时，读延迟显著增加，表明系统在读取大对象时的效率较低。

### **测试结论**

吞吐率和延迟是性能的两个关键指标。一般而言，吞吐率高的系统性能好，而延迟低则表示响应更快。但在我们的实验中，对象大小对吞吐率和延迟的影响并不总是一致的。例如，1024MB大小的对象在读吞吐率上有所提高，但读延迟也显著增加。

根据实验结果，针对不同的应用场景和性能需求，可能需要采取不同的`优化`策略。例如，对于需要高吞吐率的应用，可以考虑使用小到中等大小的对象；而对于需要低延迟的应用，可能需要优化大对象的处理机制。

# 实验小结

在本次实验中，我学会了数据分析的重要性。同时，我也通过实验观测到了对象大小对性能影响。除此之外，能影响性能的因素远不止吞吐率和延时，不同的环境与设备都会对性能有不一样的影响。因此，在获得数据的各个环节以及得到数据后的后续分析都是非常需要耐心与细心的。