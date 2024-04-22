import boto3

# 修改为您的Access Key和Secret Key
access_key = 'xmLt6Uruw0DvkSiedNVL'
secret_key = 'tgw0XGubjYVSDtId14TdwZHqY7VvTqHAZno9d5RO'

# 修改为Minio Server正在运行的网址
endpoint_url = 'http://10.21.207.28:58063/api/v1/service-account-credentials'

# 创建Boto3客户端
s3 = boto3.client('s3', endpoint_url=endpoint_url, aws_access_key_id=access_key, aws_secret_access_key=secret_key)

# 获取所有存储桶
response = s3.list_buckets()

# 打印所有存储桶
print("Existing buckets:")
for bucket in response['Buckets']:
    print(f"  {bucket['Name']}")