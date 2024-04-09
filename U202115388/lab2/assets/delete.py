import boto3
from botocore.client import Config

# 指定Swift的S3兼容服务端点
endpoint_url = 'http://localhost:12345'

# 配置访问密钥和秘密密钥
access_key = 'test:tester'
secret_key = 'testing'

# 创建一个S3客户端实例
s3_client = boto3.client(
    's3',
    endpoint_url=endpoint_url,
    aws_access_key_id=access_key,
    aws_secret_access_key=secret_key,
    config=Config(signature_version='s3v4'),  # 确保使用正确的签名版本
    region_name='us-east-1'  # 指定区域
)

# 指定要删除的对象和桶
bucket_name = 'testbucket'
object_name = 'testobject'

# 删除对象
s3_client.delete_object(Bucket=bucket_name, Key=object_name)
print(f'Object {object_name} in bucket {bucket_name} deleted.')

# 删除桶
s3_client.delete_bucket(Bucket=bucket_name)
print(f'Bucket {bucket_name} deleted.')
