from swiftclient import client
import os
import time
from concurrent.futures import ThreadPoolExecutor, as_completed
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# Swift服务器的连接参数
auth_url = 'http://127.0.0.1:12345/auth/v1.0'
user = 'test:tester'
key = 'testing'

# 创建连接
conn = client.Connection(
    authurl=auth_url,
    user=user,
    key=key,
    insecure=True
)

# 创建容器
container_name = 'bucket'
try:
    conn.put_container(container_name)
    print(f"Container '{container_name}' created successfully.")
except Exception as e:
    print(f"Error creating container: {e}")

# 定义不同的对象大小和并发级别
object_sizes = [1024, 10*1024, 100*1024, 300*1024, 500*1024, 800*1024, 1*1024*1024]
concurrency_levels = [1, 5, 10, 20, 30]  # 不同的并发级别


def generate_random_data(size):
    """生成给定大小的随机数据"""
    return os.urandom(size)

def upload_object(object_name, data):
    """上传随机数据为对象"""
    try:
        start_time = time.perf_counter()
        conn.put_object(container_name, object_name, contents=data, content_type='application/octet-stream')
        end_time = time.perf_counter()
        return end_time - start_time
    except Exception as e:
        return float('inf')

def download_object(object_name):
    """下载对象"""
    try:
        start_time = time.perf_counter()
        _, contents = conn.get_object(container_name, object_name)
        end_time = time.perf_counter()
        return end_time - start_time
    except Exception as e:
        return float('inf')

def test_performance(size, concurrency):
    """测试给定大小和并发级别的上传和下载性能"""
    executor = ThreadPoolExecutor(max_workers=concurrency)
    object_names = [f"object_{size}_bytes_{i}" for i in range(concurrency)]
    upload_latencies = []
    download_latencies = []

    # 上传对象
    upload_futures = {executor.submit(upload_object, name, generate_random_data(size)): name for name in object_names}
    for future in as_completed(upload_futures):
        latency = future.result()
        upload_latencies.append(latency)

    # 下载对象
    download_futures = {executor.submit(download_object, name): name for name in object_names}
    for future in as_completed(download_futures):
        latency = future.result()
        download_latencies.append(latency)

    upload_tail_latency = np.percentile(upload_latencies, 95)
    download_tail_latency = np.percentile(download_latencies, 95)

    return upload_tail_latency, download_tail_latency

# 执行测试并收集数据
upload_data = []
download_data = []

for size in object_sizes:
    for concurrency in concurrency_levels:
        upload_tail_latency, download_tail_latency = test_performance(size, concurrency)
        upload_data.append({'Object Size': size / 1024, 'Concurrency': concurrency, 'Latency': upload_tail_latency})
        download_data.append({'Object Size': size / 1024, 'Concurrency': concurrency, 'Latency': download_tail_latency})
        print(f"95th percentile upload latency for size {size} bytes and concurrency {concurrency}: {upload_tail_latency:.4f} seconds")
        print(f"95th percentile download latency for size {size} bytes and concurrency {concurrency}: {download_tail_latency:.4f} seconds")

# 转换数据为 DataFrame
upload_df = pd.DataFrame(upload_data)
download_df = pd.DataFrame(download_data)

# 保存数据到 CSV 文件
upload_df.to_csv('upload_data.csv', index=False)
download_df.to_csv('download_data.csv', index=False)

# 绘制图表
plt.figure(figsize=(12, 6))
sns.lineplot(data=upload_df, x='Object Size', y='Latency', hue='Concurrency')
plt.title('Upload Latency vs Object Size and Concurrency')
plt.xlabel('Object Size (KB)')
plt.ylabel('Latency (seconds)')
plt.legend(title='Concurrency Level')
plt.grid(True)
plt.show()

plt.figure(figsize=(12, 6))
sns.lineplot(data=download_df, x='Object Size', y='Latency', hue='Concurrency')
plt.title('Download Latency vs Object Size and Concurrency')
plt.xlabel('Object Size (KB)')
plt.ylabel('Latency (seconds)')
plt.legend(title='Concurrency Level')
plt.grid(True)
plt.show()

# 计算上传延迟的平均值
upload_avg_latency = upload_df.groupby('Concurrency')['Latency'].mean().reset_index()
# 计算下载延迟的平均值
download_avg_latency = download_df.groupby('Concurrency')['Latency'].mean().reset_index()
plt.figure(figsize=(12, 6))
sns.lineplot(data=upload_avg_latency, x='Concurrency', y='Latency', marker='o', color='blue', label='Upload')
sns.lineplot(data=download_avg_latency, x='Concurrency', y='Latency', marker='o', color='red', label='Download')
plt.title('Average Latency vs Concurrency')
plt.xlabel('Concurrency Level')
plt.ylabel('Average 95th Percentile Latency (seconds)')
plt.legend()
plt.grid(True)
plt.show()
