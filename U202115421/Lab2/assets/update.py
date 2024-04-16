"""
  更新存储桶中的对象
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


# 替换存储桶中的对象
bucket_name = 'my-new-bucket'
object_key = 'fortest.txt'
new_file_path = "E:\\大数据存储系统实验\\相关文件\\fortest.txt"
s3_client.upload_file(new_file_path, bucket_name, object_key)
print(f'Object {object_key} in {bucket_name} updated.')