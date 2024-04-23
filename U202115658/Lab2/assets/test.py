import boto3

# Minio
minio_client = boto3.client('s3',
                            endpoint_url='http://192.168.137.1:9000',
                            aws_access_key_id='minioadmin',
                            aws_secret_access_key='minioadmin')

#创建桶
minio_client.create_bucket(Bucket='test1')
minio_client.create_bucket(Bucket='test2')

# 删除桶
minio_client.delete_bucket(Bucket='test2')

# 上传对象
with open('file1.txt', 'rb') as f:
    minio_client.put_object(Bucket='test1', Key='file1.txt', Body=f)
with open('file2.txt', 'rb') as f:
    minio_client.put_object(Bucket='test1', Key='file2.txt', Body=f)
with open('file3.txt', 'rb') as f:
    minio_client.put_object(Bucket='test1', Key='file3.txt', Body=f)

# 删除对象
minio_client.delete_object(Bucket='test1', Key='file2.txt')

# 列举对象
objects = minio_client.list_objects(Bucket='test1')
for obj in objects['Contents']:
    print(obj['Key'])
