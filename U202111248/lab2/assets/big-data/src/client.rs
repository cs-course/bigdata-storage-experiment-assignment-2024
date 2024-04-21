use rusoto_core::{ByteStream, HttpClient, Region, RusotoError};
use rusoto_credential::{ProfileProvider, StaticProvider};
use rusoto_s3::{
    CreateBucketRequest, DeleteBucketRequest, DeleteObjectRequest, GetObjectRequest,
    PutObjectRequest, S3Client, S3,
};
use tokio::{
    fs::{remove_file, File},
    io::{AsyncReadExt, AsyncWriteExt},
};

pub async fn create_file_with_contents(file_path: &str) {
    let mut file = File::create(file_path)
        .await
        .expect("unable to create file");

    for i in 1..10 {
        file.write_all(format!("it is number {}\n", i).as_bytes())
            .await
            .expect("write into file error");
    }
}

pub async fn delete_file(file_path: &str) {
    match remove_file(file_path).await {
        Ok(_) => {}
        Err(e) => {
            eprintln!("Error deleting file '{}': {}", file_path, e);
        }
    }
}

pub async fn write_into_file(file_path: &str, byte_stream: ByteStream) {
    let mut file = File::create(file_path)
        .await
        .expect("write_into_file: create file error");

    let mut byte_stream = byte_stream.into_async_read();
    let mut buf = Vec::new();

    byte_stream
        .read_to_end(&mut buf)
        .await
        .expect("get from byte_stream error");

    file.write_all(&buf.as_slice())
        .await
        .expect("write into file error")
}

pub async fn byte_stream_to_string(byte_stream: ByteStream) -> Result<String, RusotoError<()>> {
    let mut buf = Vec::new();
    let mut byte_stream = byte_stream.into_async_read();

    // 读取字节流数据并写入缓冲区
    byte_stream.read_to_end(&mut buf).await?;

    // 将字节数据转换为字符串
    let string_data = String::from_utf8_lossy(&buf);

    Ok(string_data.to_string())
}

pub struct Client {
    cli: S3Client,
}

impl Client {
    pub fn new_with_params(access_key: String, secret_key: String, region: Region) -> Self {
        let provider = StaticProvider::new_minimal(access_key, secret_key);
        let http_client = HttpClient::new().expect("create http client error");
        Client {
            cli: S3Client::new_with(http_client, provider, region),
        }
    }

    pub fn new_with_region(region: Region) -> Self {
        Client {
            cli: S3Client::new_with(
                HttpClient::new().unwrap(),
                ProfileProvider::new().unwrap(),
                region,
            ),
        }
    }
}

impl Client {
    // 创建存储桶
    pub async fn create_bucket(
        &self,
        name: String,
    ) -> Result<rusoto_s3::CreateBucketOutput, rusoto_core::RusotoError<rusoto_s3::CreateBucketError>>
    {
        let create_bucket_req = CreateBucketRequest {
            bucket: name,
            ..Default::default()
        };

        self.cli.create_bucket(create_bucket_req).await
    }

    // 列出存储桶
    pub async fn list_buckets(
        &self,
    ) -> Result<rusoto_s3::ListBucketsOutput, rusoto_core::RusotoError<rusoto_s3::ListBucketsError>>
    {
        self.cli.list_buckets().await
    }

    // 插入对象
    pub async fn insert_object(
        &self,
        bucket_name: String,
        key: &str,
        content: Option<ByteStream>,
    ) -> Result<rusoto_s3::PutObjectOutput, rusoto_core::RusotoError<rusoto_s3::PutObjectError>>
    {
        let insert_object_req = PutObjectRequest {
            bucket: bucket_name,
            key: key.to_string(),
            body: content,
            ..Default::default()
        };

        self.cli.put_object(insert_object_req).await
    }

    // 更新对象
    pub async fn update_object(
        &self,
        bucket_name: String,
        key: &str,
        content: &str,
    ) -> Result<rusoto_s3::PutObjectOutput, rusoto_core::RusotoError<rusoto_s3::PutObjectError>>
    {
        let insert_object_req = PutObjectRequest {
            bucket: bucket_name,
            key: key.to_string(),
            body: Some(content.as_bytes().to_vec().into()),
            ..Default::default()
        };

        self.cli.put_object(insert_object_req).await
    }

    // 查询对象
    pub async fn get_object(
        &self,
        bucket_name: String,
        key: &str,
    ) -> Result<rusoto_s3::GetObjectOutput, rusoto_core::RusotoError<rusoto_s3::GetObjectError>>
    {
        let get_object_req = GetObjectRequest {
            bucket: bucket_name,
            key: key.into(),
            ..Default::default()
        };

        self.cli.get_object(get_object_req).await
    }

    // 删除对象
    pub async fn delete_object(
        &self,
        bucket_name: String,
        key: &str,
    ) -> Result<rusoto_s3::DeleteObjectOutput, rusoto_core::RusotoError<rusoto_s3::DeleteObjectError>>
    {
        let delete_object_req = DeleteObjectRequest {
            bucket: bucket_name,
            key: key.into(),
            ..Default::default()
        };

        self.cli.delete_object(delete_object_req).await
    }

    // 删除存储桶
    pub async fn delete_bucket(
        &self,
        bucket_name: String,
    ) -> Result<(), rusoto_core::RusotoError<rusoto_s3::DeleteBucketError>> {
        let delete_bucket_req = DeleteBucketRequest {
            bucket: bucket_name,
            ..Default::default()
        };

        self.cli.delete_bucket(delete_bucket_req).await
    }
}
