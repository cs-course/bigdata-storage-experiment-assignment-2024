from swiftclient import client

# 定义认证信息
server_url = 'http://127.0.0.1:12345/auth/v1.0' 
username = 'chris:chris1234'
key = 'testing'

# 连接Swift服务
conn = client.Connection(authurl=server_url, user=username, key=key)

# 列出所有容器和对象
def list_all():
    containers = conn.get_account()[1]
    print("buckets:")
    for container in containers:
        container_name = container['name']
        containers = conn.get_container(container_name)[1]
        print(f"{container_name}:")
        for obj in containers:
            object_name = obj['name']
            print(f"\t{object_name}")
    print()

# CRUD操作
# 创建操作：容器创建
def create_container(container_name):
    conn.put_container(container_name)
    print(f"Container '{container_name}' created.")
    list_all()
    
# 上传操作：上传对象至容器
def upload_object(container_name, object_name, local_file_path):
    with open(local_file_path, 'rb') as file_to_upload:
        conn.put_object(container_name, object_name, contents=file_to_upload.read())
    print(f"File '{local_file_path}' has been uploaded to container '{container_name}' as '{object_name}'.")
    list_all()
    
# 下载操作：下载对象至本地
def download_object(container, object_name, dest_path):
    __, contents = conn.get_object(container, object_name)
    with open(dest_path, 'wb') as downloaded_file:
        downloaded_file.write(contents)
    print(f"File '{object_name}' from container '{container}' has been downloaded to '{dest_path}'.")
    list_all()
    
# 删除操作：删除对象
def delete_object(container, object_name):
    conn.delete_object(container, object_name)
    print(f"'{object_name}' has been deleted.")
    list_all()

# 删除操作：删除容器
def delete_container(container_name):
    headers = {}
    conn.delete_container(container_name, headers=headers)
    print(f"Container '{container_name}' has been deleted.")
    list_all()


# 定义要创建的容器名称
container_name = 'test-container'
create_container(container_name)
    


# 本地定义两个文件
local_file_path = 'newfile.txt'
object_name = 'test_object1.txt'
with open(local_file_path, 'w') as file:
    file.write("This is a new file.")
    
local_file_path2 = 'newfile2.txt'
object_name2 = 'test_object2.txt'
with open(local_file_path2, 'w') as file:
    file.write("This is a new file.")
    
# 上传文件到容器
upload_object(container_name, object_name, local_file_path)
upload_object(container_name, object_name2, local_file_path2)

remote_object_name = 'test_object1.txt'
destination_file_path = './test_object1.txt'
download_object(container_name, remote_object_name, destination_file_path)

delete_object(container_name, remote_object_name)

delete_container(container_name)

conn.close()