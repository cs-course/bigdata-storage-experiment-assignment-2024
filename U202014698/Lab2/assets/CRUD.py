import boto3
import boto3.session

def list_all_buckets(s3):
    response = s3.list_buckets()
    for buc in response['Buckets']:
        print(buc['Name'])

def upload_file(s3, bucket_name, file_name, object_name=None):
    if object_name is None:
        object_name = file_name
    try:
        s3.upload_file(file_name, bucket_name, object_name)
        print("File uploaded.")
    except Exception as e:
        print(e)

def list_files_in_bucket(s3, bucket_name):
    try:
        response = s3.list_objects(Bucket=bucket_name)
        print("Objects in bucket", bucket_name, ":")
        for obj in response.get('Contents', []):
            print(obj['Key'])
    except Exception as e:
        print(e)

def download_file(s3, bucket_name, object_name, file_name):
    try:
        s3.download_file(bucket_name, object_name, file_name)
        print("File downloaded.")
    except Exception as e:
        print(e)

def delete_file(s3, bucket_name, object_name):
    try:
        response = s3.delete_object(Bucket=bucket_name, Key=object_name)
        print("File deleted:", response)
    except Exception as e:
        print(e)

def delete_bucket(s3, bucket_name):
    try:
        response = s3.delete_bucket(Bucket=bucket_name)
        print("Bucket deleted:", response)
    except Exception as e:
        print(e)

session = boto3.Session(
    aws_access_key_id='x', 
    aws_secret_access_key='x'
)

s3 = session.client('s3', endpoint_url='http://127.0.0.1:80')

bucket_name = 'abucket'

# Create a bucket.
try:
    response = s3.create_bucket(Bucket=bucket_name)
    print(response)
except Exception as e:
    print(e)

# List all buckets.
list_all_buckets(s3)

# Upload a file to the bucket.
file_name = 'example.txt'
upload_file(s3, bucket_name, file_name)

# List all files in the bucket.
list_files_in_bucket(s3, bucket_name)

# Download the file from the bucket.
download_file(s3, bucket_name, 'example.txt', 'downloaded_example.txt')

# Delete the file from the bucket.
delete_file(s3, bucket_name, 'example.txt')
list_files_in_bucket(s3, bucket_name)

# Delete the bucket.
delete_bucket(s3, bucket_name)
list_all_buckets(s3)