import boto3
import time
from concurrent.futures import ThreadPoolExecutor, as_completed
import matplotlib.pyplot as plt
"""
    观测不同的并发数对文件获取的吞吐量和传输时间的影响
"""
access_key = 'zrEx1EHiSHgMpvauncEf'
secret_key = 'uju0b7EZXgAlTVIG9Hgw6a0i0MqR8DUlWeAkp1yI'

s3_client = boto3.client(
    's3',
    endpoint_url='http://127.0.0.1:9000',
    aws_access_key_id=access_key,
    aws_secret_access_key=secret_key
)

def get_object(bucket, key):
    # print(f"Getting object: {key}")
    start_time = time.time()
    try:
        response = s3_client.get_object(Bucket=bucket, Key=key)
        data = response['Body'].read()
        end_time = time.time()
        return end_time - start_time, len(data), 0
    except Exception as e:
        print(f"Error occurred: {e}")
        return 0, 0, 1

def benchmark(num_clients, object_key, total_transferred):
    num_samples = total_transferred // (len(object_key))
    print(f"Number of samples: {num_samples}")
    print(f"len(object_key): {len(object_key)}")
    latencies = []
    total_transferred = 0
    num_errors = 0

    with ThreadPoolExecutor(max_workers=num_clients) as executor:
        futures = [executor.submit(get_object, 'test-minio', object_key) for _ in range(num_samples)]
        for future in as_completed(futures):
            latency, transferred, errors = future.result()
            latencies.append(latency)
            total_transferred += transferred
            num_errors += errors

    total_duration = sum(latencies)
    total_throughput = total_transferred / total_duration if total_duration > 0 else 0
    print(f"Total transferred: {total_transferred} bytes")
    print(f"Total duration: {total_duration} seconds")
    print(f"Total throughput: {total_throughput} bytes/second")
    print(f"Number of errors: {num_errors}")
    return total_duration, total_throughput,num_errors


def visualize_data(data):
    plt.title("Total Duration and Throughput vs Number of Clients (Object size: 4096 bytes) GET requests")
    fig, ax1 = plt.subplots()

    color = 'tab:red'
    ax1.set_xlabel('Number of Clients')
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

total_transferred = 1024  # 1KB
object_keys = ['test.md']
num_clients = [1, 4, 8, 16, 32,64,128,256,512,1024]
import json
data_dict = {}
for key in object_keys:
    data = []
    for clients in num_clients:
        duration, throughput,numerrors = benchmark(clients, key, total_transferred)
        data.append((clients, duration, throughput,numerrors))
    print(f"Object key: {key}")
    for(clents,dur,thr,err) in data:
        data_dict[clents] = {'duration':f"{round(dur,3)} s",'throughput':f"{round(thr,3)} bytes/second",'errors':err}
    json_data = json.dumps(data_dict,indent = 2)
    print(json_data)
    visualize_data(data)