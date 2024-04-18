import boto3
import time
import numpy as np
from concurrent.futures import ThreadPoolExecutor, as_completed
import matplotlib.pyplot as plt

"""
    观测不同的对象尺寸和并发数对文件获取的延迟分布的影响
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
    num_samples = total_transferred // len(object_key)
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
    # 平均延迟
    average_latency = total_duration / num_samples if num_samples > 0 else 0
    # 最大延迟
    max_latency = max(latencies) if latencies else 0
    
    tail_latency = np.percentile(latencies, 99)  # 计算99th百分位的尾延迟

    total_throughput = total_transferred / total_duration if total_duration > 0 else 0
    print("object key: ", object_key)
    print("=====================================")
    print(f"Total transferred: {total_transferred} bytes")
    print(f"Total duration: {total_duration} seconds")
    print(f"Total throughput: {total_throughput} bytes/second")
    print(f"Number of errors: {num_errors}")
    return average_latency, max_latency, tail_latency, total_throughput, num_errors

# def visualize_data(performance_data):
#     fig, axes = plt.subplots(len(performance_data), 1, figsize=(10, 12))  # 创建子图的数量与对象大小的数量相同
#     if len(performance_data) == 1:
#         axes = [axes]  # 确保axes是一个列表，即使只有一个子图

#     for ax, (object_size, size_data) in zip(axes, performance_data.items()):
#         # 绘制平均延迟
#         ax.plot(size_data['average_latency'], label=f'Average Latency (Object Size: {object_size} bytes)', marker='o')
#         # 绘制最大延迟
#         ax.scatter(size_data['max_latency'], [object_size] * len(size_data['max_latency']), color='r', label=f'Max Latency (Object Size: {object_size} bytes)')
#         # 绘制百分位延迟
#         for percentile in size_data['percentiles']:
#             ax.plot([0, len(size_data['average_latency'])], [percentile, percentile], label=f'{int(percentile*100)}th Percentile', linestyle='--', color='purple')
#         # 设置子图的标题和标签
#         ax.set_title(f'Performance Metrics for Object Size: {object_size} bytes')
#         ax.set_xlabel('Number of Clients')
#         ax.set_ylabel('Latency (seconds)')
#         ax.legend()

#     # 调整子图布局
#     plt.tight_layout()

#     plt.show()
from pyecharts.charts import Bar
from pyecharts import options as opts
from pyecharts.charts import Page

def visualize_data(performance_data):
    page = Page()  # 创建一个Page对象

    for object_key, size_data in performance_data.items():
        # 创建柱状图
        bar = Bar()
        bar.add_xaxis(num_clients)
        bar.add_yaxis(f'Average Latency (Object Size: {object_key} bytes)', [round(num, 5) for num in size_data['average_latency']])
        bar.add_yaxis(f'Max Latency (Object Size: {object_key} bytes)', [round(num, 5) for num in size_data['max_latency']])
        bar.add_yaxis(f'99th Percentile Latency (Object Size: {object_key} bytes)',  [round(num, 5) for num in size_data['percentiles']])
        
        # 设置全局配置项
        bar.set_global_opts(
            title_opts=opts.TitleOpts(title=f'Performance Metrics for Object Key: {object_key} bytes'),
            tooltip_opts=opts.TooltipOpts(trigger='axis', axis_pointer_type='cross'),
            xaxis_opts=opts.AxisOpts(axislabel_opts=opts.LabelOpts(rotate=45)),
            yaxis_opts=opts.AxisOpts(name='Latency (seconds)'),
            legend_opts=opts.LegendOpts(orient='vertical', pos_top="10px", pos_left="100%") 
        )
        
        # 将柱状图添加到Page对象中
        page.add(bar)

    # 渲染到一个HTML文件中
    page.render('gets_charts.html')

total_transferred = 1024  # 1KB
num_clients = [1, 4, 16, 32,128]
# for size in object_sizes:
    # data = []
    # for clients in num_clients:
    #     duration, throughput = benchmark(clients, size, total_transferred)
    #     data.append((clients, duration, throughput))
    # print(f"Object size: {size}")
    # visualize_data(data)
performance_data = {}  # 用于存储性能数据的字典
clients = 1
object_keys = ['test.md']
for key in object_keys:
    size_data = {'average_latency': [], 'max_latency': [], 'percentiles': [], 'throughput': []}
    for clients in num_clients:
        average_latency, max_latency, percentiles, throughput, _ = benchmark(clients, key, total_transferred)
        size_data['average_latency'].append(average_latency)
        size_data['max_latency'].append(max_latency)
        size_data['percentiles'].append(percentiles)
        size_data['throughput'].append(throughput)
        print(f"Object key: {key}, Clients: {clients}")
        print()
    performance_data[key] = size_data

print(performance_data)
# 可视化数据
visualize_data(performance_data) 
# benchmark(10, 1024, 100)