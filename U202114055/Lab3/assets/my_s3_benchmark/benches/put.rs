use std::{error::Error, fs::File, io::{self, Write}, path::Path};

use my_s3_benchmark::create_s3_client;
use criterion::async_executor::FuturesExecutor;
use criterion::{criterion_group, criterion_main, Criterion, black_box};
use aws_sdk_s3::{error::SdkError, operation::put_object::{PutObjectError, PutObjectOutput}, primitives::ByteStream, Client};
use tokio::runtime::Runtime;

const BUCKET_NAME: &str = "testbucket";
const OBJECT_NAME: &str = "put_testfile";
const SOURCE_PATH: &str = "../put_testfile.txt";

#[tokio::main]
async fn put(client:&Client) -> Result<PutObjectOutput, SdkError<PutObjectError>> {
    let body = ByteStream::from_path(Path::new(SOURCE_PATH)).await;
    client
        .put_object()
        .bucket(BUCKET_NAME)
        .key(OBJECT_NAME)
        .body(body.unwrap())
        .send().await
}

fn criterion_benchmark(c: &mut Criterion) {
    let rt = Runtime::new().unwrap();
    let client:Client = rt.block_on(create_s3_client(false));
    c.bench_function("Async PutObject", move |b| {
        let cli = client.clone();
        b.to_async(FuturesExecutor).iter(|| async {
            let _ret = put(black_box(&cli));
        })
    });
}

// 定义一个名叫 benches 的基准测试组， 其中包含 criterion_benchmark函数
criterion_group!(benches, criterion_benchmark);
// 执行 benches 组的所有基准测试
criterion_main!(benches);