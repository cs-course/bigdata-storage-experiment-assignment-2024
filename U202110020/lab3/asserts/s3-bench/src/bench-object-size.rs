mod bench;
mod client;
mod plot;
mod s3;

use bench::TestDifferentSizesResult;
use core::time;
use futures::{future::try_join_all, stream::FuturesUnordered};
use std::{error::Error, path::PathBuf, vec};
use std::{path::Path, time::Instant};
use tokio::io::AsyncWriteExt;
use tokio::{fs::File, task::JoinHandle};

async fn put_single_object(
    object_size: u32,
) -> Result<time::Duration, Box<dyn Error + Send + Sync>> {
    let s3_client = client::S3Client::new().await;
    let bucket = "wwb";
    let object = String::from("test-object-size") + "-" + &object_size.to_string();
    let source =
        PathBuf::from(String::from("./files/test-object-size") + "-" + &object_size.to_string());

    let latency = s3_client.put_object(bucket, &object, &source).await?;
    Ok(latency)
}

async fn get_single_object(
    object_size: u32,
) -> Result<time::Duration, Box<dyn Error + Send + Sync>> {
    let s3_client = client::S3Client::new().await;
    let bucket = "wwb";
    let object = String::from("test-object-size") + "-" + &object_size.to_string();
    let destination =
        PathBuf::from(String::from("./files/test-object-size") + "-" + &object_size.to_string());

    let latency = s3_client.get_object(bucket, &object, &destination).await?;
    Ok(latency)
}

async fn run_test(
    task_count: u32,
    object_size: u32,
    test_type: bench::TestType,
) -> Result<Vec<time::Duration>, Box<dyn Error>> {
    let mut latencys = vec![];
    let handle = tokio::runtime::Handle::current();
    let tasks = FuturesUnordered::new();
    let test_type = test_type; // Move test_type outside of the loop

    for _i in 0..task_count {
        // avoid moving test_type into the closure
        let test_type_clone = test_type.clone();

        let task: JoinHandle<Result<time::Duration, Box<dyn Error + Send + Sync>>> =
            handle.spawn(async move {
                match test_type_clone {
                    bench::TestType::GetObject => get_single_object(object_size).await,
                    bench::TestType::PutObject => put_single_object(object_size).await,
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

static OBJECT_SIZES: [u32; 9] = [8, 16, 32, 64, 128, 256, 512, 1024, 2048]; // KB

//static OBJECT_SIZES: [u32; 1] = [32]; // 32 KB

const TOTAL_OBJECT_SIZE: u32 = 1024 * 1024 * 2; // 2 MB

async fn test_get_objects_different_sizes() -> Result<Vec<TestDifferentSizesResult>, Box<dyn Error>>
{
    // test get object with different object sizes
    let mut res = vec![];
    for object_size in &OBJECT_SIZES {
        let start = Instant::now();
        let object_size = object_size * 1024;
        println!("==== Test read object size: {} =====", object_size);

        // get object, task count = TOTAL_OBJECT_SIZE / object_size
        // such as TOTAL_OBJECT_SIZE = 1MB, object_size = 2KB, task count = 512
        let latencys = run_test(
            TOTAL_OBJECT_SIZE / object_size,
            object_size,
            bench::TestType::GetObject,
        )
        .await?;

        let duration = start.elapsed().as_secs_f64();
        let throughput = TOTAL_OBJECT_SIZE as f64 / duration / 1024.0;
        println!("total read object size {} KB", TOTAL_OBJECT_SIZE / 1024);
        println!("read object size {} KB", object_size / 1024);
        println!("total read duration {} s", duration);
        println!("total read throughput {} KB/s", throughput);

        let avg_latency = latencys
            .iter()
            .fold(time::Duration::new(0, 0), |acc, &x| acc + x)
            / latencys.len() as u32;
        println!("read avg latency {:?} s", avg_latency);

        let test_result = TestDifferentSizesResult {
            test_type: bench::TestType::GetObject,
            total_object_size: TOTAL_OBJECT_SIZE / 1024,
            object_size: object_size as f64 / 1024.0,
            duration,
            throughput,
            latencys,
            avg_latency,
        };
        res.push(test_result);
    }
    Ok(res)
}

async fn test_put_objects_different_sizes() -> Result<Vec<TestDifferentSizesResult>, Box<dyn Error>>
{
    let mut res = vec![];
    // test put object with different object sizes
    for object_size in &OBJECT_SIZES {
        let object_size = object_size * 1024;

        // write file of object_size bytes
        let source = PathBuf::from(
            String::from("./files/test-object-size") + "-" + &object_size.to_string(),
        );
        if Path::new(&source).exists() {
            std::fs::remove_file(&source)?;
        }
        let mut file = File::create(&source).await?;
        let bytes = vec![0u8; object_size as usize];
        file.write_all(&bytes).await?;

        println!("==== Test write object size: {} ====", object_size);
        let start = Instant::now();
        // put object, task count = TOTAL_OBJECT_SIZE / object_size
        // such as TOTAL_OBJECT_SIZE = 1MB, object_size = 2KB, task count = 512
        let latencys = run_test(
            TOTAL_OBJECT_SIZE / object_size,
            object_size,
            bench::TestType::PutObject,
        )
        .await?;

        let duration = start.elapsed().as_secs_f64();
        let throughput = TOTAL_OBJECT_SIZE as f64 / duration / 1024.0;
        println!("total write object size {} KB", TOTAL_OBJECT_SIZE / 1024);
        println!("write object size {} KB", object_size / 1024);
        println!("write total duration {} s", duration);
        println!("total write throughput {} KB/s", throughput);

        let avg_latency = latencys
            .iter()
            .fold(time::Duration::new(0, 0), |acc, &x| acc + x)
            / latencys.len() as u32;
        println!("write avg latency {} s", avg_latency.as_secs_f64());

        let test_result = TestDifferentSizesResult {
            test_type: bench::TestType::PutObject,
            total_object_size: TOTAL_OBJECT_SIZE / 1024,
            object_size: object_size as f64 / 1024.0,
            duration,
            throughput,
            avg_latency,
            latencys,
        };
        res.push(test_result);
    }

    Ok(res)
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn Error>> {
    // test put object with different object sizes
    let res = test_put_objects_different_sizes().await?;
    plot::draw_object_size_average_latency_chart(&res, "Object Size vs Average Latency")?;
    plot::draw_object_size_throught_chart(&res, "Object Size vs Throughput")?;

    // test get object with different object sizes
    let res = test_get_objects_different_sizes().await?;
    plot::draw_object_size_average_latency_chart(&res, "Object Size vs Average Latency")?;
    plot::draw_object_size_throught_chart(&res, "Object Size vs Throughput")?;

    Ok(())
}
