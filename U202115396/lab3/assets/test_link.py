import boto3

"""
列出所有mock_s3中的存储桶
"""

# 任意指定Mock_s的access_key和secret_key
access_key = '00000000-0000-0000-0000-000000000000'
secret_key = '00000000-0000-0000-0000-000000000000'

# mock_s3的 endpoint URL
endpoint_url = 'http://127.0.0.1:9000'
# 初始化客户端
s3_client = boto3.client(
    's3',
    endpoint_url=endpoint_url,
    aws_access_key_id=access_key,
    aws_secret_access_key=secret_key
)

# 列出所有的存储桶
response = s3_client.list_buckets()
print(response)   