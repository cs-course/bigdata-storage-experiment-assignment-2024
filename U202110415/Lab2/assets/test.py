# -*- coding: UTF-8 -*-

import swiftclient


endpoint = 'http://127.0.0.1:12345/'
_user = 'test:tester'
_key = 'testing'

conn = swiftclient.Connection(authurl=endpoint + 'auth/v1.0',user=_user,key=_key)
print("Connected successfully")


container_name = 'newBucket'
conn.put_container(container_name)

print("Created a new bucket")

# 上传一个文件 (Create)
object_name = 'testObj'
with open('./file1.txt', 'rb') as file:
    conn.put_object(container_name, object_name, contents=file.read())

print("Uploaded!")

# 读取一个文件 (Read)
response_headers, file_contents = conn.get_object(container_name, object_name)
print(file_contents)

# 更新一个文件 (Update)
object_name = 'testObj'
with open('./file2.txt', 'rb') as file:
    conn.put_object(container_name, object_name, contents=file.read())

print("Updated!!")

# 删除一个文件 (Delete)
conn.delete_object(container_name, object_name)

print("Deleted a file!!")

# 删除一个 bucket
conn.delete_container(container_name)

print("Deleted a bucket!!")