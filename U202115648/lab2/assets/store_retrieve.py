from swiftclient import client
import os

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

# 容器名称
container_name = 'bucket'

# 尝试创建容器
try:
    conn.put_container(container_name)
    print(f"Container '{container_name}' created successfully.")
except Exception as e:
    print(f"Error creating container: {e}")

# 文件列表
files_to_upload = ['example1.txt', 'example2.txt']
source_dir = './source/'
target_dir = './target/'

# 确保目标目录存在
os.makedirs(target_dir, exist_ok=True)

# 上传文件
for file_name in files_to_upload:
    file_path = os.path.join(source_dir, file_name)
    try:
        with open(file_path, 'r') as f:
            conn.put_object(container_name, file_name, contents=f.read(), content_type='text/plain')
        print(f"File '{file_name}' uploaded successfully.")
    except Exception as e:
        print(f"Error uploading file {file_name}: {e}")

# 下载文件
for file_name in files_to_upload:
    try:
        resp_headers, file_contents = conn.get_object(container_name, file_name)
        target_file_path = os.path.join(target_dir, file_name)
        with open(target_file_path, 'w') as f:
            f.write(file_contents.decode('utf-8'))
        print(f"File '{file_name}' downloaded successfully to {target_file_path}.")
    except Exception as e:
        print(f"Error downloading file {file_name}: {e}")

