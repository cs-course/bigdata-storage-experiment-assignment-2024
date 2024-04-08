use std::error::Error;
use std::fs::File;
use std::io::Write;
use std::path::PathBuf;
use aws_sdk_s3::primitives::ByteStream;
use crate::client::S3Client;

impl S3Client {
    pub async fn list_buckets(&self) -> Result<(), Box<dyn Error>> {
        let resp = self.s3_client.list_buckets().send().await?;
        let buckets = resp.buckets();
        let num_buckets = buckets.len();

        for bucket in buckets {
            println!("{}", bucket.name().unwrap_or_default());
        }

        println!();
        println!("Found {} buckets in all regions.", num_buckets);

        Ok(())
    }

    pub async fn create_bucket(&self, bucket: &str) -> Result<(), Box<dyn Error>> {
        let resp = self.s3_client
            .create_bucket()
            .bucket(bucket)
            .send()
            .await?;

        log::info!("CreateBucketOutput {:?}", resp);
        Ok(())
    }

    pub async fn get_object(&self, bucket: &str, object: &str, destination: &PathBuf) -> Result<usize, Box<dyn Error>> {
        log::info!("bucket:      {}", bucket);
        log::info!("object:      {}", object);
        log::info!("destination: {}", destination.display());

        let mut file = File::create(&destination)?;

        let mut object = self.s3_client
            .get_object()
            .bucket(bucket)
            .key(object)
            .send()
            .await?;

        let mut byte_count = 0usize;

        while let Some(bytes) = object.body.try_next().await? {
            let bytes_len = bytes.len();
            file.write_all(&bytes)?;
            log::info!("Intermediate write of {bytes_len}");
            byte_count += bytes_len;
        }

        Ok(byte_count)
    }

    pub async fn list_objects(&self, bucket: &str) -> Result<(), Box<dyn Error>> {
        let resp = self.s3_client
            .list_objects_v2()
            .bucket(bucket)
            .send()
            .await?;

        let objects = resp.contents();
        let num_objects = objects.len();

        for object in objects {
            println!("{}", object.key().unwrap_or_default());
        }

        println!("Found {} objects in bucket: {}", num_objects, bucket);

        Ok(())
    }

    pub async fn put_object(&self, bucket: &str, object: &str, source: &PathBuf) -> Result<(), Box<dyn Error>> {
        log::info!("bucket: {}", bucket);
        log::info!("object: {}", object);
        log::info!("source: {}", source.display());

        let body = ByteStream::read_from()
            .path(source.clone())
            .buffer_size(2048)
            .build()
            .await?;

        let request = self.s3_client
            .put_object()
            .bucket(bucket)
            .key(object)
            .body(body);


        let out = request.send().await?;

        log::info!("PutObjectOutput {:?}", out);
        Ok(())
    }

    pub async fn delete_object(&self, bucket: &str, object: &str) -> Result<(), Box<dyn Error>> {
        log::info!("bucket: {}", bucket);
        log::info!("object: {}", object);

        let resp = self.s3_client
            .delete_object()
            .bucket(bucket)
            .key(object)
            .send()
            .await?;

        log::info!("DeleteObjectOutput {:?}", resp);
        Ok(())
    }

    pub async fn copy_object(&self, source_bucket: &str, source_object: &str, destination_bucket: &str, destination_object: &str) -> Result<(), Box<dyn Error>> {
        log::info!("source_bucket: {}", source_bucket);
        log::info!("source_object: {}", source_object);
        log::info!("destination_bucket: {}", destination_bucket);
        log::info!("destination_object: {}", destination_object);

        let resp = self.s3_client
            .copy_object()
            .copy_source(format!("{}/{}", source_bucket, source_object))
            .bucket(destination_bucket)
            .key(destination_object)
            .send()
            .await?;

        log::info!("CopyObjectOutput {:?}", resp);
        Ok(())
    }
}