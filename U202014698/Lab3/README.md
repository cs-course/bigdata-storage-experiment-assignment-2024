# 实验名称

观测分析性能

# 实验环境

Windows 11 下 VirtualBox 运行 Ubuntu 22.04 LTS，运行s3proxy作为对象存储系统。

s3proxy 使用 filesystem 作为 backend 来存取数据。

s3proxy 使用默认的设置，无需进行验证。

# 实验记录

我首先尝试使用 [s3-bench-rs](https://github.com/SKTT1Ryze/s3-bench-rs) 对 s3proxy 进行性能分析，事实上 s3-bench-rs 在当前的 rust 版本和 rusty-s3 上存在数个编译错误。我对 rust 的了解不足，以至于我无法修正错误，因此实验无法在 s3-bench-rs 上继续进行。

我们最终尝试使用 s3bench 来对 s3proxy 进行性能分析。

## 实验 3-1: 安装 s3bench

```bash
go install github.com/igneous-systems/s3bench@latest
```

这会将 s3bench 安装在 `$GOPATH/bin/s3bench` 下。
可以通过 `go env` 查看 `$GOPATH` 路径。

## 实验 3-2: 性能分析与尾延迟观测

为方便进行测试，我们撰写一个 bash 脚本来运行 s3bench。

```bash
~/go/bin/s3bench \
    -accessKey=x \
    -accessSecret=x \
    -bucket=abucket \
    -endpoint=http://localhost:80 \
    -numClients=10 -numSamples=10 \
    -objectNamePrefix=loadgen -objectSize=1024 \
    > benchmark_result.txt
```

`numClients` 代表同步进行请求的客户端个数。
`numSamples` 代表创建的 Object 样本个数。
`objectSize` 代表 Object 大小（字节数）。
其中 bucket 需要事先在 s3proxy 中创建，因此我们同样撰写一个 setup 脚本来做前置准备工作。

```python
session = boto3.Session(
    aws_access_key_id='x', 
    aws_secret_access_key='x'
)

s3 = session.client('s3', endpoint_url='http://127.0.0.1:8070')

bucket_name = 'abucket'

# Create a bucket.
try:
    response = s3.create_bucket(Bucket=bucket_name)
    print(response)
except Exception as e:
    print(e)
```

尝试运行脚本以查看 benchmark 是否能够运行。
输出结果被放置在 benchmark_result.txt 内，得到如下结果：
```
Test parameters
endpoint(s):      [http://localhost:80]
bucket:           abucket
objectNamePrefix: loadgen
objectSize:       0.0010 MB
numClients:       10
numSamples:       10
verbose:       %!d(bool=false)


Generating in-memory sample data... Done (15.075µs)

Running Write test...

Running Read test...

Test parameters
endpoint(s):      [http://localhost:80]
bucket:           abucket
objectNamePrefix: loadgen
objectSize:       0.0010 MB
numClients:       10
numSamples:       10
verbose:       %!d(bool=false)


Results Summary for Write Operation(s)
Total Transferred: 0.010 MB
Total Throughput:  2.20 MB/s
Total Duration:    0.004 s
Number of Errors:  0
------------------------------------
Write times Max:       0.004 s
Write times 99th %ile: 0.004 s
Write times 90th %ile: 0.004 s
Write times 75th %ile: 0.004 s
Write times 50th %ile: 0.003 s
Write times 25th %ile: 0.003 s
Write times Min:       0.003 s


Results Summary for Read Operation(s)
Total Transferred: 0.010 MB
Total Throughput:  5.70 MB/s
Total Duration:    0.002 s
Number of Errors:  0
------------------------------------
Read times Max:       0.002 s
Read times 99th %ile: 0.002 s
Read times 90th %ile: 0.002 s
Read times 75th %ile: 0.001 s
Read times 50th %ile: 0.001 s
Read times 25th %ile: 0.001 s
Read times Min:       0.001 s


Cleaning up 10 objects...
Deleting a batch of 10 objects in range {0, 9}... Succeeded
Successfully deleted 10/10 objects in 729.572µs
```

我们可以开始调整并发客户端数量以观察其对尾延迟的影响。s3proxy的原理实际上是将s3 API的请求代理（改写）至其它的存储系统后端，而在本地运行测试的 s3proxy 默认使用本地文件系统作为数据库后端，因此硬盘 I/O 的波动对测试的结果影响较大，事实上我也进行了数次实验得到的结果波动相差在 1s 左右。考虑可以使用的资源，我们将样本数量固定为足够大的 `100000`，将每个样本的大小固定为 `1024B` 。

测试结果如下。

#### 写操作延迟 (单位: 秒)

| Clients | 75th %ile | 90th %ile | 99th %ile |
|-----------|-------------|-------------|-------------|
| 10        | 0.001       | 0.004       | 0.007       |
| 100       | 0.019       | 0.036       | 0.060       |
| 1000      | 0.167       | 0.468       | 0.826       |

#### 读操作延迟 (单位: 秒)

| Clients | 75th %ile | 90th %ile | 99th %ile |
|-----------|-------------|-------------|-------------|
| 10        | 0.001       | 0.003       | 0.007       |
| 100       | 0.021       | 0.034       | 0.086       |
| 1000      | 0.226       | 0.630       | 2.383       |

可以观察到随着 clients 数的增长，尾延迟成显著的非线性增长趋势。在100到1000客户端90百分位-99百分位的延迟增长十分明显，大大拖慢了系统的请求速度。

原始的 log 数据可以在 assets 文件夹中查看。

为了考虑减少尾延迟，我们可以部署集群来进行对冲请求以尝试缓解尾延迟。然而 s3proxy 的方案实现比较简单，我没有找到相关的资料或文档供我部署 s3proxy 的集群方案使我来实现对冲请求方案。

我随后尝试了部署三台无关联的 s3proxy 的方案，s3bench 将会在三个 endpoint 上等分地写入 1/3 总量的 Object，并在三个 endpoint 上随机地读取所有的 Object。最终测试的结果会有约 2/3 的读请求发生错误，因此虽然写操作延迟得到了显著降低，但是读操作延迟的测试数据没有实际的参考意义。因此本报告略过该实验数据与对对冲请求方案的分析。

# 实验小结

本实验我们使用 s3bench 分析了 s3proxy+filesystem 的性能，并调整并发客户端数以测试该系统的尾延迟。
我们在客户端数目从 100 至 1000 的过程中观察到了显著的系统尾延迟的增加，由于 s3proxy 相关的资料不足，我们没能部署 s3proxy 集群或者找到相关的测试方案来测试对冲请求方案对尾延迟的影响。