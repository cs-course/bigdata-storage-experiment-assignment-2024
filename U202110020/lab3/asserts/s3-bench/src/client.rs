use aws_config::meta::region::RegionProviderChain;
use aws_sdk_s3::config::{Credentials, SharedCredentialsProvider};
use aws_sdk_s3::Client;

struct S3Config {
    region: &'static str,
    endpoint: String,
    access_key: String,
    secret_key: String,
    provider: &'static str,
}

impl S3Config {
    pub fn new() -> Self {
        S3Config {
            region: "us-east-1",
            endpoint: "http://43.138.127.165:12345".to_string(),
            access_key: "test:tester".to_string(),
            secret_key: "testing".to_string(),
            provider: "Static",
        }
    }
}

pub struct S3Client {
    pub(crate) s3_client: Client,
}

impl S3Client {
    pub async fn new() -> Self {
        let s3_config = S3Config::new();

        let region_provider = RegionProviderChain::default_provider().or_else(s3_config.region);

        let cli_config = aws_config::from_env()
            .credentials_provider(SharedCredentialsProvider::new(Credentials::new(
                s3_config.access_key,
                s3_config.secret_key,
                None,
                None,
                s3_config.provider,
            )))
            .endpoint_url(s3_config.endpoint)
            .region(region_provider)
            .load()
            .await;

        let client = Client::new(&cli_config);

        S3Client { s3_client: client }
    }
}
