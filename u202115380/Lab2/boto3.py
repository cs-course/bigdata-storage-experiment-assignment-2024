import boto3
# 配置 OpenStack Swift 服务的参数
endpoint_url = 'http://localhost:12345'  # Swift 服务的 endpoint URL
aws_access_key_id = 'test:tester'  # Swift 访问密钥 ID
aws_secret_access_key = 'testing'  # Swift 密钥


# 创建 S3 客户端
s3_client = boto3.client(
    's3',
    endpoint_url=endpoint_url,  # 指定 OpenStack Swift 服务的 endpoint URL
    aws_access_key_id=aws_access_key_id,  # 指定您的 Swift 访问密钥 ID
    aws_secret_access_key=aws_secret_access_key,  # 指定Swift 密钥
)

# 存储桶名称
bucket_name='test-bucket'  # 替换为您的存储桶名称


#  读取存储桶
def list_bucket_contents(bucket_name):
    print(f"Listing contents of bucket: {bucket_name}")
    # 列出存储桶中的所有对象
    response = s3_client.list_objects_v2(Bucket=bucket_name)

    if 'Contents' in response:
        for obj in response['Contents']:
            print(f"Object key: {obj['Key']}, Size: {obj['Size']}")
    else:
        print("Bucket is empty.")


#  更新存储桶（示例中将上传一个文件作为更新操作）
def upload_object_to_bucket(bucket_name, object_key, file_path):
    print(f"Uploading file {file_path} as object {object_key} to bucket {bucket_name}")
    # 上传一个文件到存储桶
    with open(file_path, 'rb') as file_data:
        s3_client.put_object(Bucket=bucket_name, Key=object_key, Body=file_data)
    print(f"File uploaded successfully.")


# 3. 删除存储桶
def delete_bucket(bucket_name):
    print(f"Deleting bucket: {bucket_name}")
    # 首先删除存储桶中的所有对象
    response = s3_client.list_objects_v2(Bucket=bucket_name)
    if 'Contents' in response:
        for obj in response['Contents']:
            s3_client.delete_object(Bucket=bucket_name, Key=obj['Key'])
    # 删除存储桶
    s3_client.delete_bucket(Bucket=bucket_name)
    print("Bucket deleted successfully.")


# 示例操作：
if __name__ == '__main__':
    s3_client.create_bucket(Bucket=bucket_name)
    # 列出存储桶中的内容
    list_bucket_contents(bucket_name)
    # 上传一个文件到存储桶
    object_key = '3.mp4'  # 上传对象的键名
    file_path = 'C:\\Users\\Administrator\\Desktop\\wyp\\3.mp4'  # 本地文件路径
    upload_object_to_bucket(bucket_name, object_key, file_path)

    # 再次列出存储桶中的内容，查看上传结果
    list_bucket_contents(bucket_name)

    # 删除存储桶
    delete_bucket(bucket_name)
