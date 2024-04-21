use std::{fs::File, io::Write, error::Error};

use aws_sdk_s3::{Client};
use my_s3_benchmark::create_s3_client;
use criterion::async_executor::FuturesExecutor;
use criterion::{criterion_group, criterion_main, Criterion};
use tokio::runtime::Runtime;

const BUCKET_NAME: &str = "testbucket";//"wwb";//
const OBJECT_NAME: &str = "testfile";//"put_testfile0";//
const DEST_PATH: &str = "../get_output_file.txt";

#[tokio::main]
async fn get(client:&Client) -> Result<usize, Box<dyn Error>> {
    let mut file = File::create(DEST_PATH)?;

    let mut object = client
        .get_object()
        .bucket(BUCKET_NAME)
        .key(OBJECT_NAME)
        .send()
        .await?;

    let mut byte_count = 0_usize;
    while let Some(bytes) = object.body.try_next().await? {
        let bytes_len = bytes.len();
        file.write_all(&bytes)?;
        // trace!("Intermediate write of {bytes_len}");
        byte_count += bytes_len;
    }
    Ok(0)
    //Ok(byte_count)
}

fn criterion_benchmark(c: &mut Criterion) {
    let rt = Runtime::new().unwrap();
    // let client:Client = rt.block_on(create_s3_client(false));
    // c.bench_function("Async GetObject", move |b| {
    //     let cli = client.clone();
    //     b.to_async(FuturesExecutor).iter(|| async {
    //         let _ret = get(&cli);
    //     })
    // });

    let client:Client = rt.block_on(create_s3_client(false));
    c.bench_function("Async GetObject Parallel", move |b| {
        
        b.to_async(FuturesExecutor).iter(|| async {
            let futures = (0..16).map(|_| 
                async {
                    let _ret = get(&client);
                }
            );
            let _results: Vec<_> = futures_util::future::join_all(futures).await;
        })
    });
}

// 定义一个名叫 benches 的基准测试组， 其中包含 criterion_benchmark函数
criterion_group!(benches, criterion_benchmark);
// 执行 benches 组的所有基准测试
criterion_main!(benches);