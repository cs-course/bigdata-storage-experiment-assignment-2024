import boto3
from botocore.client import Config
from botocore.exceptions import ClientError
import logging

#logging.basicConfig(level=logging.DEBUG)

s3 = boto3.client('s3',
    aws_access_key_id='test:tester',
    aws_secret_access_key='testing',
    config=Config(signature_version='s3v4'),
    endpoint_url='http://43.138.127.165:12345',
    use_ssl=False
)

try:
    response = s3.list_buckets()
    print(response)
except ClientError as e: 
    print(e.response)