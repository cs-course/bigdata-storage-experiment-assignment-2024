use std::{path::Path};

use my_s3_benchmark::create_s3_client;
use criterion::async_executor::FuturesExecutor;
use criterion::{criterion_group, criterion_main, Criterion};
use aws_sdk_s3::{error::SdkError, operation::put_object::{PutObjectError, PutObjectOutput}, primitives::ByteStream, Client};
use tokio::runtime::Runtime;

const BUCKET_NAME: &str = "testbucket";//"wwb";//
const OBJECT_NAME: &str = "put_testfile";
const SOURCE_PATH: &str = "../put_testfile.txt";

#[tokio::main]
async fn put(client:&Client, id:i32) -> Result<PutObjectOutput, SdkError<PutObjectError>> {
    let body = ByteStream::from_path(Path::new(SOURCE_PATH)).await;
    client
        .put_object()
        .bucket(BUCKET_NAME)
        .key(format!("{}{}",OBJECT_NAME,id))
        .body(body.unwrap())
        .send().await
}

fn criterion_benchmark(c: &mut Criterion) {
    let rt = Runtime::new().unwrap();
    // let client:Client = rt.block_on(create_s3_client(false));
    // c.bench_function("Async PutObject", move |b| {
    //     let cli = client.clone();
    //     b.to_async(FuturesExecutor).iter(|| async {
    //         let _ret = put(&cli, 0);
    //     })
    // });

    // let client:Client = rt.block_on(create_s3_client(false));
    // c.bench_function("Async PutObject Parallel", move |b| {
        
    //     b.to_async(FuturesExecutor).iter(|| async {
    //         let futures = (0..16).map(|i| {
    //             let cli = client.clone();
    //                 async move {
    //                     let id = i;
    //                     let _ret = put(&cli, id);
    //                 }
    //             }
    //         );
    //         let _results: Vec<_> = futures_util::future::join_all(futures).await;
    //     })
    // });
}

// 定义一个名叫 benches 的基准测试组， 其中包含 criterion_benchmark函数
criterion_group!(benches, criterion_benchmark);
// 执行 benches 组的所有基准测试
criterion_main!(benches);