mod bench;
mod client;
mod plot;
mod s3;

use core::time;
use futures::{future::try_join_all, stream::FuturesUnordered};
use std::sync::Arc;
// use plotters::prelude::*;
use std::time::Instant;
use std::{error::Error, path::PathBuf, vec};
use tokio::task::JoinHandle;

#[derive(Clone)]
enum TestType {
    PutObject,
    GetObject,
}
async fn put_single_object(
    file_number: u32,
    object_size: u32,
) -> Result<time::Duration, Box<dyn Error + Send + Sync>> {
    let s3_client = client::S3Client::new().await;
    let bucket = "wwb";
    let object = String::from("test-object-size")
        + "-"
        + &object_size.to_string()
        + "-"
        + &file_number.to_string();

    let source =
        PathBuf::from(String::from("./files/test-object-size") + "-" + &object_size.to_string());

    let latency = s3_client.put_object(bucket, &object, &source).await?;
    Ok(latency)
}

async fn get_single_object(
    file_number: u32,
    object_size: u32,
) -> Result<time::Duration, Box<dyn Error + Send + Sync>> {
    let s3_client = client::S3Client::new().await;
    let bucket = "wwb";
    let object = String::from("test-object-size")
        + "-"
        + &object_size.to_string()
        + "-"
        + &file_number.to_string();

    let destination = PathBuf::from(
        String::from("./files/test-object-size")
            + "-"
            + &object_size.to_string()
            + "-"
            + &file_number.to_string(),
    );

    let latency = s3_client.get_object(bucket, &object, &destination).await?;
    Ok(latency)
}

async fn run_test(
    task_count: u32,
    object_size: u32,
    test_type: TestType,
    concurrency_count: u32,
) -> Result<Vec<time::Duration>, Box<dyn Error>> {
    let mut latencys = vec![];

    let handle = tokio::runtime::Handle::current();
    let tasks = FuturesUnordered::new();
    let test_type = test_type; // Move test_type outside of the loop

    let sem = Arc::new(tokio::sync::Semaphore::new(concurrency_count as usize));

    for i in 0..task_count {
        // avoid moving test_type into the closure
        let test_type_clone = test_type.clone();
        let sem_clone = Arc::clone(&sem);

        let task: JoinHandle<Result<time::Duration, Box<dyn Error + Send + Sync>>> =
            handle.spawn(async move {
                #[allow(unused_variables)]
                let premit = sem_clone.acquire().await.unwrap();

                match test_type_clone {
                    TestType::GetObject => get_single_object(i, object_size).await,
                    TestType::PutObject => put_single_object(i, object_size).await,
                }
            });
        tasks.push(task);
    }

    let results = try_join_all(tasks).await?;

    for (i, res) in results.iter().enumerate() {
        match res {
            Ok(val) => {
                //    println!("Task {} completed, {} s", i, val.as_secs());
                latencys.push(*val);
            }
            Err(e) => {
                println!("Task {} failed: {}", i, e);
            }
        }
    }

    Ok(latencys)
}

static CONCURRENCY_COUNT: [u32; 10] = [5, 10, 15, 20, 25, 30, 35, 40, 45, 50];

//static OBJECT_SIZES: [u32; 1] = [32]; // 32 KB

const TOTAL_OBJECT_SIZE: u32 = 1024 * 1024 * 4; // 4 MB

async fn test_get_objects_different_concurrency(
) -> Result<Vec<bench::TestConcurrencyResult>, Box<dyn Error>> {
    let mut res = Vec::new();

    let start = Instant::now();
    let object_size = 64 * 1024;

    // get object, task count = TOTAL_OBJECT_SIZE / object_size
    // such as TOTAL_OBJECT_SIZE = 1MB, object_size = 2KB, task count = 512
    for concurrency_count in &CONCURRENCY_COUNT {
        println!(
            "==== Test read concurrency count: {} =====",
            concurrency_count
        );

        let latencys = run_test(
            TOTAL_OBJECT_SIZE / object_size,
            object_size,
            TestType::GetObject,
            *concurrency_count,
        )
        .await?;

        let duration = start.elapsed().as_secs_f64();
        let throughput = TOTAL_OBJECT_SIZE as f64 / duration / 1024.0;
        println!("total read object size {} KB", TOTAL_OBJECT_SIZE / 1024);
        println!("read object size {} KB", object_size / 1024);
        println!("concurrenct count {}", concurrency_count);
        println!("total read duration {} s", duration);
        println!("total read throughput {} KB/s", throughput);

        let avg_latency = latencys
            .iter()
            .fold(time::Duration::new(0, 0), |acc, &x| acc + x)
            / latencys.len() as u32;
        println!("read avg latency {} s", avg_latency.as_secs_f64());

        let test_result = bench::TestConcurrencyResult {
            test_type: bench::TestType::GetObject,
            total_object_size: TOTAL_OBJECT_SIZE / 1024,
            object_size: object_size as f64 / 1024.0,
            concurrency_count: *concurrency_count,
            duration,
            throughput,
            latencys,
            avg_latency,
        };
        res.push(test_result);
    }

    Ok(res)
}

async fn test_put_objects_different_concurrency(
) -> Result<Vec<bench::TestConcurrencyResult>, Box<dyn Error>> {
    let mut res = Vec::new();

    let start = Instant::now();
    let object_size = 64 * 1024;

    // put object, task count = TOTAL_OBJECT_SIZE / object_size
    // such as TOTAL_OBJECT_SIZE = 1MB, object_size = 2KB, task count = 512
    for concurrency_count in &CONCURRENCY_COUNT {
        println!(
            "==== Test write concurrency count: {} =====",
            concurrency_count
        );

        let latencys = run_test(
            TOTAL_OBJECT_SIZE / object_size,
            object_size,
            TestType::PutObject,
            *concurrency_count,
        )
        .await?;

        let duration = start.elapsed().as_secs_f64();
        let throughput = TOTAL_OBJECT_SIZE as f64 / duration / 1024.0;
        println!("total write object size {} KB", TOTAL_OBJECT_SIZE / 1024);
        println!("write object size {} KB", object_size / 1024);
        println!("total write duration {} s", duration);
        println!("total write throughput {} KB/s", throughput);

        let avg_latency = latencys
            .iter()
            .fold(time::Duration::new(0, 0), |acc, &x| acc + x)
            / latencys.len() as u32;
        println!("write avg latency {} s", avg_latency.as_secs_f64());

        let test_result = bench::TestConcurrencyResult {
            test_type: bench::TestType::PutObject,
            total_object_size: TOTAL_OBJECT_SIZE / 1024,
            object_size: object_size as f64 / 1024.0,
            concurrency_count: *concurrency_count,
            duration,
            throughput,
            latencys,
            avg_latency,
        };
        res.push(test_result);
    }

    Ok(res)
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn Error>> {
    // test put object with different object sizes
    let res = test_put_objects_different_concurrency().await?;
    plot::draw_concurrency_average_latency_chart(&res, "Concurrency vs Average Latency").unwrap();
    plot::draw_concurrency_throught_chart(&res, "Concurrency vs Throught").unwrap();

    // test get object with different object sizes
    let res = test_get_objects_different_concurrency().await?;
    plot::draw_concurrency_average_latency_chart(&res, "Concurrency vs Average Latency").unwrap();
    plot::draw_concurrency_throught_chart(&res, "Concurrency vs Throught").unwrap();

    Ok(())
}
