import boto3
import time
from concurrent.futures import ThreadPoolExecutor, as_completed
import matplotlib.pyplot as plt

"""
    观测不同的对象尺寸对文件上传的吞吐量和传输时间的影响

"""
access_key = 'zrEx1EHiSHgMpvauncEf'
secret_key = 'uju0b7EZXgAlTVIG9Hgw6a0i0MqR8DUlWeAkp1yI'

s3_client = boto3.client(
    's3',
    endpoint_url='http://127.0.0.1:9000',
    aws_access_key_id=access_key,
    aws_secret_access_key=secret_key
)

def put_object(bucket, key, size):
    """
    put_object函数用于将指定大小的对象上传到S3服务,并返回上传的时间、对象大小和是否发生错误。
    """
    start_time = time.time()
    try:
        s3_client.put_object(Bucket=bucket, Key=key, Body=b'0' * size)
        end_time = time.time()
        return end_time - start_time, size, 0
    except Exception as e:
        print(f"Error occurred: {e}")
        return 0, 0, 1

def benchmark(num_clients, object_size, total_transferred):
    """
    benchmark函数用于并发上传多个对象,并计算总的上传时间、吞吐量和错误数量。
    """
    num_samples = total_transferred // object_size
    latencies = []
    total_transferred = 0
    num_errors = 0

    with ThreadPoolExecutor(max_workers=num_clients) as executor:
        futures = [executor.submit(put_object, 'test-minio', 'test.md', object_size) for _ in range(num_samples)]
        for future in as_completed(futures):
            latency, transferred, errors = future.result()
            latencies.append(latency)
            total_transferred += transferred
            num_errors += errors

    total_duration = sum(latencies)
    total_throughput = total_transferred / total_duration if total_duration > 0 else 0
    print("object size: ", object_size)
    print("=====================================")
    print(f"Total transferred: {total_transferred} bytes")
    print(f"Total duration: {total_duration} seconds")
    print(f"Total throughput: {total_throughput} bytes/second")
    print(f"Number of errors: {num_errors}")
    return total_duration, total_throughput, num_errors

def visualize_data(data):
    """
    visualize_data函数用于将上传时间和吞吐量可视化
    """
    fig, ax1 = plt.subplots()

    color = 'tab:red'
    ax1.set_xlabel('Size of Object')
    ax1.set_ylabel('Total Duration (seconds)', color=color)
    ax1.plot([x[0] for x in data], [x[1] for x in data], color=color)
    ax1.tick_params(axis='y', labelcolor=color)

    ax2 = ax1.twinx()
    color = 'tab:blue'
    ax2.set_ylabel('Total Throughput (bytes/sec)', color=color)
    ax2.plot([x[0] for x in data], [x[2] for x in data], color=color)
    ax2.tick_params(axis='y', labelcolor=color)

    fig.tight_layout()
    plt.show()

total_transferred = 1024 # 1MB
object_sizes = [16,64,128,256,512]
# num_clients = [1, 4, 8, 16, 32,64,128,256,512,1024]
# for size in object_sizes:
    # data = []
    # for clients in num_clients:
    #     duration, throughput = benchmark(clients, size, total_transferred)
    #     data.append((clients, duration, throughput))
    # print(f"Object size: {size}")
    # visualize_data(data)
data = []
# size = 4096
clients = 1
for size in object_sizes:
        duration, throughput,numErrors = benchmark(clients, size, total_transferred)
        data.append((size, duration, throughput,numErrors))
print(f"num of clients: {clients}")
# plt.plot([1, 2, 3], [4, 5, 6]);
# plt.show()
print()
data_list ={}
for d in data:
     for size,duration,throughput,numErrors in data:
         data_list[size] = {'duration':round(duration,3),'throughput':round(throughput,3),'numErrors':numErrors}
# 将字典转换为JSON格式的字符串
import json
json_str = json.dumps(data_list, indent=2, sort_keys=True)
print(json_str)

visualize_data(data)


# benchmark(10, 1024, 100)