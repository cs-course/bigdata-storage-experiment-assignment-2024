"""
# 读取对象
"""
import boto3
# 指定 Minio Server 的access_key和secret_key
access_key = '0iMGl80WigBHPYXtZvZu'
secret_key = 'AY9DMoxHivmGY7KQBE5WPxuEixNcMX4ceNIOBLQ7'
# 初始化客户端
s3_client = boto3.client(
    's3',
    endpoint_url='http://10.22.26.77:9000',
    aws_access_key_id=access_key,
    aws_secret_access_key=secret_key
)

# # 列出所有存储桶
# buckets = s3_client.list_buckets()
# print('Buckets:')
# for bucket in buckets['Buckets']:
#     print(bucket['Name'])

# 获取特定存储桶中的对象列表
bucket_name = 'my-new-bucket'
response = s3_client.list_objects_v2(Bucket=bucket_name)
objects = response.get('Contents', [])
print(f'Objects in bucket {bucket_name}:')
for obj in objects:
    print(obj['Key'])