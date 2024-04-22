"""Boto3 S3 client for the S3 buckets and objects operations"""
import unittest
import time

import boto3
from botocore.client import Config
from botocore.exceptions import ClientError

class Boto3S3Client:
    """Boto3 S3 client for the S3 buckets and objects operations"""
    def __init__(self):
        self.s3 = boto3.client(
            's3',
            aws_access_key_id='test:tester',
            aws_secret_access_key='testing',
            config=Config(signature_version='s3v4'),
            endpoint_url='http://43.138.127.165:12345',
            use_ssl=False
        )

    def list_buckets(self):
        """List the S3 buckets"""
        try:
            response = self.s3.list_buckets()
        except ClientError as e:
            print(e.response)
        return response

    def create_bucket(self, bucket_name):
        """Create an S3 bucket in a specified region"""
        try:
            response = self.s3.create_bucket(Bucket=bucket_name)
        except ClientError as e:
            print(e.response)
        return response

    def delete_bucket(self, bucket_name):
        """Delete an S3 bucket"""
        try:
            response = self.s3.delete_bucket(Bucket=bucket_name)
        except ClientError as e:
            print(e.response)
        return response

    def list_objects(self, bucket_name):
        """List the objects in an S3 bucket"""
        try:
            response = self.s3.list_objects(Bucket=bucket_name)
        except ClientError as e:
            print(e.response)
        return response

    def put_object(self, file_name, bucket_name, object_name):
        """Upload a file to an S3 bucket"""
        try:
            response = self.s3.upload_file(file_name, bucket_name, object_name)
        except ClientError as e:
            print(e.response)
        return response

    def get_object(self, bucket_name, object_name, file_name):
        """Download a file from an S3 bucket"""
        try:
            response = self.s3.download_file(bucket_name, object_name, file_name)
        except ClientError as e:
            print(e.response)
        return response

    def delete_object(self, bucket_name, object_name):
        """Delete an object from an S3 bucket"""
        try:
            response = self.s3.delete_object(Bucket=bucket_name, Key=object_name)
        except ClientError as e:
            print(e.response)
        return response



class TestBoto3S3Client(unittest.TestCase):
    """Test cases for the Boto3S3Client class"""
    def test_list_buckets(self):
        """Test the list_buckets method"""
        s3_client = Boto3S3Client()
        response = s3_client.list_buckets()
        self.assertEqual(response['ResponseMetadata']['HTTPStatusCode'], 200)

        print('List of S3 buckets:')
        for bucket in response['Buckets']:
            print(f'{bucket["Name"]}')

    def test_create_bucket(self):
        """Test the create_bucket method"""
        s3_client = Boto3S3Client()
        bucket_name = 'test-bucket-2'
        response = s3_client.create_bucket(bucket_name)
        self.assertEqual(response['ResponseMetadata']['HTTPStatusCode'], 200)

        print(f's3 bucket {bucket_name} created successfully')
        print(f'create_buckets response: {response}')

    def test_delete_bucket(self):
        """Test the delete_bucket method"""
        s3_client = Boto3S3Client()
        bucket_name = 'test-bucket-2'
        response = s3_client.delete_bucket(bucket_name)
        self.assertEqual(response['ResponseMetadata']['HTTPStatusCode'], 204)

        print(f's3 bucket {bucket_name} deleted successfully')
        print(f'delete_buckets response: {response}')

    def test_list_objects(self):
        """Test the list_objects method"""
        s3_client = Boto3S3Client()
        bucket_name = 'wwb'
        response = s3_client.list_objects(bucket_name)
        self.assertEqual(response['ResponseMetadata']['HTTPStatusCode'], 200)

        print(f'List of objects in s3 bucket {bucket_name}:')
        for obj in response['Contents']:
            print(f'{obj["Key"]}')

    def test_upload_file(self):
        """Test the upload_file method"""
        start_time = time.time()

        s3_client = Boto3S3Client()
        file_name = './files/bigdata-storage-experiment.pptx'
        bucket_name = 'wwb'
        object_name = 'upload-bigdata.pptx'
        response = s3_client.put_object(file_name, bucket_name, object_name)
        self.assertEqual(response, None)

        end_time = time.time()
        print(f'File {file_name} uploaded successfully to s3 bucket {bucket_name}')
        print(f'upload_file response: {response}')
        print(f'Time taken to upload the file: {end_time - start_time} seconds')

    def test_download_file(self):
        """Test the download_file method"""
        start_time = time.time()

        s3_client = Boto3S3Client()
        bucket_name = 'wwb'
        object_name = 'upload-bigdata.pptx'
        file_name = 'files/download-bigdata.pptx'
        response = s3_client.get_object(bucket_name, object_name, file_name)
        self.assertEqual(response, None)

        end_time = time.time()
        print(f'File {object_name} downloaded successfully from s3 bucket {bucket_name}')
        print(f'download_file response: {response}')
        print(f'Time taken to download the file: {end_time - start_time} seconds')

if __name__ == '__main__':
    unittest.main()
