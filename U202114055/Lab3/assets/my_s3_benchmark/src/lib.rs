use std::{fs::File, io::{self, Write}, path::{Path, PathBuf}, str::FromStr};

use aws_sdk_s3::{error::SdkError, primitives::ByteStream, Client, Error};
use aws_credential_types::{provider::SharedCredentialsProvider, Credentials};
use aws_sdk_s3::operation::{create_bucket::CreateBucketOutput, create_bucket::CreateBucketError, 
                            put_object::PutObjectOutput, put_object::PutObjectError};

const ACCESS_KEY_ID: &str = "swift123:swift123";
// const ACCESS_KEY_ID: &str = "test:tester"; // remote server
const SECRET_ACCESS_KEY: &str = "swift_key";
// const SECRET_ACCESS_KEY: &str = "testing";// remote server

pub async fn create_s3_client(show_config:bool) -> Client {
//     let access_key_id = "swift123:swift123";
//     let secret_access_key = "swift_key";
    
    let config = aws_config::from_env()
        //.endpoint_url("http://43.138.127.165:12345".to_string()) // wwb remote server
        .endpoint_url("http://127.0.0.1:12345".to_string())
        .credentials_provider(SharedCredentialsProvider::new(Credentials::from_keys(
            ACCESS_KEY_ID.to_string(),
            SECRET_ACCESS_KEY.to_string(),
            None,
        )))
        .load()
        .await;
    let s3_local_config = aws_sdk_s3::config::Builder::from(&config).build();
    if show_config {
        println!("{:#?}", s3_local_config);
    }
    let client = Client::from_conf(s3_local_config);
    return client;
}