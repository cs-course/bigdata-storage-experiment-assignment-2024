import boto3
import boto3.session

session = boto3.Session(
  aws_access_key_id = 'x', 
  aws_secret_access_key = 'x'
)

s3 = session.resource('s3', endpoint_url='http://127.0.0.1:80')

for bucket in s3.buckets.all():
    print(bucket.name)