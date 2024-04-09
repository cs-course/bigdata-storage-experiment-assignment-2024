//! Put 请求测试

use criterion::async_executor::FuturesExecutor;
use criterion::{criterion_group, criterion_main, Criterion};
use reqwest::Url;
use s3_bench_rs::{PutTaskBuilder, StdError, Task, TaskBuiler};

const ENDPOINT: &str = "http://127.0.0.1:12345/auth/v1.0";
const KEY: &str = "chris:chris1234";
const SECRET: &str = "testing";
const BUCKET: &str = "user_uploads";
const OBJECT: &str = "test.txt";

#[tokio::main]
async fn put() -> Result<String, Box<StdError>> {
    let tasks: [(String, String); 2] = [
        ("bucket1".into(), "test0.txt".into()),
        ("bucket1".into(), "test1.txt".into()),
    ];
    let put_task_builder = PutTaskBuilder::new(
        ENDPOINT.parse::<Url>().unwrap(),
        KEY,
        SECRET,
        "minio",
        tasks,
    );
    let task = put_task_builder.spawn(BUCKET, OBJECT);
    let resp = task.run().await?;
    Ok(resp)
}

fn criterion_benchmark(c: &mut Criterion) {
    let mut c = c.benchmark_group("Async PutObject");
    c.measurement_time(std::time::Duration::new(10, 0));
    c.bench_function("Async PutObject", move |b| {
        b.to_async(FuturesExecutor).iter(|| async {
            let _ret = put();
        })
    });
}

criterion_group!(benches, criterion_benchmark);
criterion_main!(benches);
