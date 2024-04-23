mod client;
mod s3;

use clap::{Parser, Subcommand};
use env_logger::Env;
use std::error::Error;
use std::path::PathBuf;

#[derive(Parser)]
#[command(author, version, about, long_about = None)]
struct Args {
    #[command(subcommand)]
    cmd: Commands,
}

#[derive(Subcommand, Debug, Clone)]
enum Commands {
    ListBuckets,
    CreateBucket {
        #[clap(short, long, help = "Bucket name", required = true)]
        bucket: String,
    },
    ListObjects {
        #[clap(short, long, help = "Bucket name", default_value = "wwb")]
        bucket: String,
    },
    GetObject {
        #[clap(short, long, help = "Bucket name", default_value = "wwb")]
        bucket: String,
        #[clap(short, long, help = "Object name", required = true)]
        object: String,
        #[clap(short, long, help = "Destination path", required = true)]
        destination: PathBuf,
    },
    PutObject {
        #[clap(short, long, help = "Bucket name", default_value = "wwb")]
        bucket: String,
        #[clap(short, long, help = "Object name", required = true)]
        object: String,
        #[clap(short, long, help = "Source path", required = true)]
        source: PathBuf,
    },
    DeleteObject {
        #[clap(short, long, help = "Bucket name", default_value = "wwb")]
        bucket: String,
        #[clap(short, long, help = "Object name", required = true)]
        object: String,
    },
    CopyObject {
        #[clap(short, long, help = "Source bucket name", default_value = "wwb")]
        source_bucket: String,
        #[clap(short, long, help = "Source object name", required = true)]
        source_object: String,
        #[clap(short, long, help = "Destination bucket name", default_value = "wwb")]
        destination_bucket: String,
        #[clap(short, long, help = "Destination object name", required = true)]
        destination_object: String,
    },
}

#[::tokio::main]
async fn main() -> Result<(), Box<dyn Error>> {
    env_logger::Builder::from_env(Env::default().default_filter_or("info")).init();

    let s3_client = client::S3Client::new().await;

    let args = Args::parse();

    match args.cmd {
        Commands::ListBuckets => {
            s3_client.list_buckets().await?;
        }
        Commands::CreateBucket { bucket } => {
            s3_client.create_bucket(bucket.as_str()).await?;
            println!("create bucket {} success", bucket);
        }
        Commands::ListObjects { bucket } => {
            s3_client.list_objects(bucket.as_str()).await?;
        }
        Commands::GetObject {
            bucket,
            object,
            destination,
        } => {
            let byte_count = s3_client
                .get_object(bucket.as_str(), object.as_str(), &destination)
                .await?;
            println!("downloaded {} bytes", byte_count);
            println!("get object {} success", destination.display());
        }
        Commands::PutObject {
            bucket,
            object,
            source,
        } => {
            s3_client
                .put_object(bucket.as_str(), object.as_str(), &source)
                .await?;
            println!("put object {} success", source.display());
        }
        Commands::DeleteObject { bucket, object } => {
            s3_client
                .delete_object(bucket.as_str(), object.as_str())
                .await?;
            println!("delete object {} success", object);
        }
        Commands::CopyObject {
            source_bucket,
            source_object,
            destination_bucket,
            destination_object,
        } => {
            s3_client
                .copy_object(
                    source_bucket.as_str(),
                    source_object.as_str(),
                    destination_bucket.as_str(),
                    destination_object.as_str(),
                )
                .await?;
            println!(
                "copy object {} to {} success",
                source_object, destination_object
            );
        }
    }

    Ok(())
}

#[cfg(test)]
mod tests {
    use super::*;

    #[tokio::test]
    async fn test_list_buckets() -> Result<(), Box<dyn Error>> {
        let s3_client = client::S3Client::new().await;
        s3_client.list_buckets().await?;
        Ok(())
    }

    #[tokio::test]
    async fn test_create_bucket() -> Result<(), Box<dyn Error>> {
        let s3_client = client::S3Client::new().await;
        s3_client.create_bucket("test-1").await?;
        Ok(())
    }

    #[tokio::test]
    async fn test_list_objects() -> Result<(), Box<dyn Error>> {
        let s3_client = client::S3Client::new().await;
        s3_client.list_objects("wwb").await?;
        Ok(())
    }

    #[tokio::test]
    async fn test_get_object() -> Result<(), Box<dyn Error>> {
        let s3_client = client::S3Client::new().await;
        s3_client
            .get_object("test-1", "test.txt", &PathBuf::from("./files/test.txt"))
            .await?;
        Ok(())
    }

    #[tokio::test]
    async fn test_put_object() -> Result<(), Box<dyn Error>> {
        let s3_client = client::S3Client::new().await;
        s3_client
            .put_object("test-1", "test.txt", &PathBuf::from("./files/1.txt"))
            .await?;
        Ok(())
    }

    #[tokio::test]
    async fn test_delete_object() -> Result<(), Box<dyn Error>> {
        let s3_client = client::S3Client::new().await;
        s3_client.delete_object("test-1", "test.txt").await?;
        Ok(())
    }

    #[tokio::test]
    async fn test_copy_object() -> Result<(), Box<dyn Error>> {
        let s3_client = client::S3Client::new().await;
        s3_client
            .copy_object("test-1", "test.txt", "test-2", "test.txt")
            .await?;
        Ok(())
    }
}
