import time
from swiftclient import client
import matplotlib.pyplot as plt

# 连接到 OpenStack Swift
auth_url = 'http://127.0.0.1:12345/auth/v1.0'
user_name = 'test:tester'
password = 'testing'

conn = client.Connection(
    authurl=auth_url,
    user=user_name,
    key=password,
)

# 创建一个存储桶
def create_container(container_name):
    start_time = time.time()
    conn.put_container(container_name)
    elapsed_time = time.time() - start_time
    return elapsed_time

# 上传文件到存储桶
def upload_file(container_name, file_path, object_name):
    start_time = time.time()
    with open(file_path, 'rb') as f:
        conn.put_object(container_name, object_name, f)
    elapsed_time = time.time() - start_time
    return elapsed_time

# 下载文件
def download_file(container_name, object_name, download_path):
    start_time = time.time()
    obj = conn.get_object(container_name, object_name)
    with open(download_path, 'wb') as f:
        f.write(obj[1])
    elapsed_time = time.time() - start_time
    return elapsed_time

# 更新文件内容（重新上传相同的文件名称）
def update_file(container_name, file_path, object_name):
    return upload_file(container_name, file_path, object_name)

# 删除文件
def delete_file(container_name, object_name):
    start_time = time.time()
    conn.delete_object(container_name, object_name)
    elapsed_time = time.time() - start_time
    return elapsed_time

# 删除存储桶
def delete_container(container_name):
    start_time = time.time()
    conn.delete_container(container_name)
    elapsed_time = time.time() - start_time
    return elapsed_time

# 测试 CRUD 操作的性能并保留每次操作的耗时
def test_performance(container_name, file_path, object_name, download_path, num_runs=10, print_flag=1):
    create_times = []
    upload_times = []
    download_times = []
    update_times = []
    delete_file_times = []
    delete_container_times = []

    # 重复测试 num_runs 次
    for _ in range(num_runs):
        # 测试创建存储桶
        create_time = create_container(container_name)
        create_times.append(create_time)

        # 测试上传文件
        upload_time = upload_file(container_name, file_path, object_name)
        upload_times.append(upload_time)

        # 测试下载文件
        download_time = download_file(container_name, object_name, download_path)
        download_times.append(download_time)

        # 测试更新文件
        update_time = update_file(container_name, file_path, object_name)
        update_times.append(update_time)

        # 测试删除文件
        delete_file_time = delete_file(container_name, object_name)
        delete_file_times.append(delete_file_time)

        # 测试删除存储桶
        delete_container_time = delete_container(container_name)
        delete_container_times.append(delete_container_time)

    avg_create_time = sum(create_times) / num_runs
    avg_upload_time = sum(upload_times) / num_runs
    avg_download_time = sum(download_times) / num_runs
    avg_update_time = sum(update_times) / num_runs
    avg_delete_file_time = sum(delete_file_times) / num_runs
    avg_delete_container_time = sum(delete_container_times) / num_runs

    # 输出平均耗时
    if(print_flag == 1) :
        print(f"创建存储桶的平均耗时: {avg_create_time:.3f} 秒")
        print(f"上传文件的平均耗时: {avg_upload_time:.3f} 秒")
        print(f"下载文件的平均耗时: {avg_download_time:.3f} 秒")
        print(f"更新文件的平均耗时: {avg_update_time:.3f} 秒")
        print(f"删除文件的平均耗时: {avg_delete_file_time:.3f} 秒")
        print(f"删除存储桶的平均耗时: {avg_delete_container_time:.3f} 秒")
    
    return (avg_create_time + avg_upload_time + avg_download_time + avg_update_time + avg_delete_file_time + avg_delete_container_time) / 6

def create_txt_file(file_path: str, target_size: int, content: str = " ") -> None:
    if target_size <= 0 or not content:
        raise ValueError("目标文件大小必须大于 0 字节且内容不能为空。")
    
    content_length = len(content)
    repeat_count, remainder = divmod(target_size, content_length)
    
    with open(file_path, "w") as f:
        f.write(content * repeat_count + content[:remainder])

def plot_throughput():
    # 配置参数
    container_name = 'test'
    file_path = 'U202115667/Lab3/assets/uploadf.txt'
    object_name = 'test.txt'  # Swift 对象的名称
    download_path = 'U202115667/Lab2/assets/downloadf.txt'

    file_sizes = [10, 50, 100]  # 文件大小列表（10MB、50MB、100MB）
    num_runs = 10  # 运行测试的次数
    avg_throughputs = []

    for file_size in file_sizes:
        # 创建文件
        file_size *= 1024**2
        create_txt_file(file_path, file_size, content='hey')
        file_size /= 1024**2

        # 测试性能并计算吞吐量
        avg_time = test_performance(container_name, file_path, object_name, download_path, num_runs, print_flag=False)
        throughput = file_size / avg_time

        # 存储平均吞吐量
        avg_throughputs.append(throughput)

    # 绘制吞吐量图形
    plt.plot(file_sizes, avg_throughputs,color='blue', label='Throughput')

    # 添加标签和标题
    plt.xlabel('FileSize (MB)')
    plt.ylabel('Throughput (MB/s)')
    plt.title('Test for different size of files')
    plt.legend()

    # 显示图形
    plt.show()

# 执行测试并绘制图形
plot_throughput()