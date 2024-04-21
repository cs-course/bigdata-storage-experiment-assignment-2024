from swiftclient import Connection

class OpenStackSwiftFileManager:
    def __init__(self, auth_url, username, password, container_name):
        self.auth_url = auth_url
        self.user = username
        self.key = password
        self.container_name = container_name
        self.conn = None
    
    def connect(self):
        """连接"""
        self.conn = Connection(authurl=self.auth_url, user=self.user, key=self.key)

    def upload_file(self, object_path, file_content):
        """上传文件到指定路径"""
        self.conn.put_object(container_name, object_path, file_content)
        print(f'File uploaded to path {object_path}.')

    def download_file(self, object_path, local_path):
        """从指定路径下载文件到本地"""
        _, file_content = self.conn.get_object(container_name, object_path)
        
        # 将内容写入本地文件
        with open(local_path, 'wb') as local_file:
            local_file.write(file_content)
        
        print(f'File from path {object_path} downloaded to local path {local_path}.')

    def update_file(self, object_path, new_file_content):
        """更新指定路径中的文件内容"""
        self.conn.put_object(container_name, object_path, new_file_content)
        print(f'File at path {object_path} updated.')

    def delete_file(self, object_path):
        """从指定路径删除文件"""
        self.conn.delete_object(container_name, object_path)
        print(f'File at path {object_path} deleted.')

if __name__ == '__main__':
    # 在这里替换为你自己的认证信息
    auth_url = 'http://127.0.0.1:12345/auth/v1.0'
    username = 'test:tester'
    password = 'testing'
    container_name = 'swift'

    # 创建 OpenStackSwiftFileManager 对象
    file_manager = OpenStackSwiftFileManager(auth_url, username, password,container_name)
    file_manager.connect()

    # 文件路径
    local_path = 'U202115667/Lab2/assets/test.txt'
    # 对象名称（文件名）
    object_path = 'test.txt'

    # 创建文件
    fp = open(local_path,'rb')
    file_content = fp.read()
    fp.close()
    file_manager.upload_file(object_path, file_content)
    print(f'File {object_path} uploaded to {container_name}.')

    # 读取文件
    new_local_path = 'U202115667/Lab2/assets/new.txt'
    file_manager.download_file(object_path, new_local_path)
    fp = open(new_local_path,'rb')
    file_content = fp.read()
    fp.close()
    print(f'File {object_path} downloaded to {new_local_path}:{file_content}.')

    # 更新并读取文件
    updated_local_path = 'U202115667/Lab2/assets/updated.txt'
    fp = open(updated_local_path,'rb')
    file_content = fp.read()
    fp.close()
    file_manager.update_file(object_path, file_content)
    print(f'File {object_path} uploaded to {container_name}:{file_content}.')
    fp = open(new_local_path,'rb')
    file_content = fp.read()
    fp.close()
    file_manager.download_file(object_path, new_local_path)
    print(f'File {object_path} downloaded to {new_local_path}:{file_content}.')

    # 删除文件
    file_manager.delete_file(object_path)

    # 断开连接
    file_manager.conn.close()