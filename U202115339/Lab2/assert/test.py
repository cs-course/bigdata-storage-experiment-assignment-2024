import boto3

# 修改为您的Access Key和Secret Key
access_key = 'Q7IJN4uJKI51Ww6EgnXi'
secret_key = '2DfGuyJUy5cHNUJfI7rGL7jCJc8x5uriI19AUgTN'

# 修改为Minio Server正在运行的网址
endpoint_url = 'http://10.11.163.247:64264'

# 创建Boto3客户端
s3 = boto3.client('s3', endpoint_url=endpoint_url, aws_access_key_id=access_key, aws_secret_access_key=secret_key)

# 获取所有存储桶
response = s3.list_buckets()

# 打印所有存储桶
print("Existing buckets:")
for bucket in response['Buckets']:
    print(f"  {bucket['Name']}")