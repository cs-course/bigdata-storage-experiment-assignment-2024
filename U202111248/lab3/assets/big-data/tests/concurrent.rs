// 测试并发上传文件，下载文件

use std::sync::Arc;

use big_data::{
    client::{delete_bucket_all, Client},
    image::draw_image,
};
use rusoto_core::Region;
use tokio::{
    fs::{create_dir, remove_dir_all, File},
    io::{AsyncReadExt, AsyncWriteExt},
    time::Instant,
};

async fn upload_file(cli: Arc<Client>, bucket: String, file_num: i32, worker_id: i32) {
    for i in 1..(file_num + 1) {
        let path = format!("./upload/file{}_{}.txt", worker_id, i);
        let mut file = File::open(path.clone()).await.unwrap();
        let mut buf = Vec::new();

        file.read_to_end(&mut buf).await.unwrap();

        let start_time = Instant::now();

        cli.insert_object(bucket.clone(), path.as_str(), Some(buf.into()))
            .await
            .unwrap();

        let upload_time = start_time.elapsed().as_millis();
        println!("{} : upload {}ms", worker_id * file_num + i, upload_time)
    }
}

// async fn upload_file_with_hedging(cli: Client, bucket: &str, file_num: i32, worker_id: i32) {
//     // 对冲请求，推迟发送第二个请求，直到第一个请求到达 95 分位数还没有返回
// }

async fn download_file(cli: Arc<Client>, bucket: String, file_num: i32, worker_id: i32) {
    for i in 1..(file_num + 1) {
        let path = format!("./download/file{}_{}.txt", worker_id, i);
        let key = format!("./upload/file{}_{}.txt", worker_id, i);
        let mut file = File::create(path.clone()).await.unwrap();

        let start_time = Instant::now();

        let resp = cli.get_object(bucket.clone(), key.as_str()).await.unwrap();

        let mut byte_stream = resp.body.unwrap().into_async_read();
        let mut buf = Vec::new();

        byte_stream
            .read_to_end(&mut buf)
            .await
            .expect("get from byte_stream error");

        file.write_all(&buf.as_slice())
            .await
            .expect("write into file error");

        let download_time = start_time.elapsed().as_millis();
        println!(
            "{} : download {}ms",
            worker_id * file_num + i,
            download_time
        );
    }
}

// async fn download_file_with_hedging() {}

async fn concurrent_upload(
    cli: Arc<Client>,
    bucket: String,
    file_size: i32,
    concurrency: i32,
    total_size: i32,
) {
    let file_count = total_size / file_size;
    let per_count = file_count / concurrency;
    let (concurrency, per_count) = if file_count < concurrency {
        (file_count, 1)
    } else {
        (concurrency, per_count)
    };

    let last_count = file_count - per_count * (concurrency - 1);

    let mut handles = Vec::new();

    for i in 0..concurrency {
        if i != concurrency - 1 {
            let cli = Arc::clone(&cli);
            handles.push(tokio::spawn(upload_file(cli, bucket.clone(), per_count, i)));
        } else {
            let cli = Arc::clone(&cli);
            handles.push(tokio::spawn(upload_file(
                cli,
                bucket.clone(),
                last_count,
                i,
            )));
        }
    }

    for handle in handles {
        let _ = handle.await;
    }
}

async fn concurrent_download(
    cli: Arc<Client>,
    bucket: String,
    file_size: i32,
    concurrency: i32,
    total_size: i32,
) {
    let file_count = total_size / file_size;
    let per_count = file_count / concurrency;
    let (concurrency, per_count) = if file_count < concurrency {
        (file_count, 1)
    } else {
        (concurrency, per_count)
    };

    let last_count = file_count - per_count * (concurrency - 1);

    let mut handles = Vec::new();

    for i in 0..concurrency {
        if i != concurrency - 1 {
            let cli = Arc::clone(&cli);
            handles.push(tokio::spawn(download_file(
                cli,
                bucket.clone(),
                per_count,
                i,
            )));
        } else {
            let cli = Arc::clone(&cli);
            handles.push(tokio::spawn(download_file(
                cli,
                bucket.clone(),
                last_count,
                i,
            )));
        }
    }

    for handle in handles {
        let _ = handle.await;
    }
}

async fn create_upload_files(file_size: i32, concurrency: i32, total_size: i32) {
    create_dir("./upload").await.unwrap();

    let file_count = total_size / file_size;
    let per_count = file_count / concurrency;
    let (concurrency, per_count) = if file_count < concurrency {
        (file_count, 1)
    } else {
        (concurrency, per_count)
    };

    let last_count = file_count - per_count * (concurrency - 1);

    for i in 0..(concurrency - 1) {
        for j in 1..(per_count + 1) {
            let mut file = File::create(format!("./upload/file{}_{}.txt", i, j))
                .await
                .unwrap();

            let size: usize = file_size.try_into().unwrap();
            let buf = vec![1; size];

            file.write_all(&buf).await.unwrap();
        }
    }

    for i in 1..(last_count + 1) {
        let mut file = File::create(format!("./upload/file{}_{}.txt", concurrency - 1, i))
            .await
            .unwrap();

        let size: usize = file_size.try_into().unwrap();
        let buf = vec![1; size];

        file.write_all(&buf).await.unwrap();
    }
}

async fn concurrent_test(
    cli: Arc<Client>,
    bucket: String,
    file_size: i32,
    concurrency: i32,
    total_size: i32,
) {
    // 创建初始upload文件
    create_upload_files(file_size, concurrency, total_size).await;

    let cli_1 = Arc::clone(&cli);
    concurrent_upload(cli_1, bucket.clone(), file_size, concurrency, total_size).await;

    // 创建download文件夹
    create_dir("./download").await.unwrap();

    let cli_2 = Arc::clone(&cli);
    concurrent_download(cli_2, bucket.clone(), file_size, concurrency, total_size).await;
}

// test_1: test file upload/download
// total_size: 10 mb
// concurrency: 10
// file_size: 5 kb
#[tokio::test]
async fn test_1() {
    let s3_endpoint = match env::var("END_POINT") {
        Ok(res) => res,
        Err(_) => panic!("can not get end_point"),
    };

    let region = Region::Custom {
        name: "us-east-1".to_owned(),
        endpoint: s3_endpoint.to_owned(),
    };
    let cli = Client::new_with_region(region);
    let cli = Arc::new(cli);

    let bucket_name = "bucket-4".to_string();

    // delete bucket
    delete_bucket_all(Arc::clone(&cli), bucket_name.clone()).await;

    let resp = cli.create_bucket(bucket_name.clone()).await;
    match resp {
        Ok(output) => {
            println!("create bucket ok : {:?}", output.location)
        }
        Err(e) => {
            println!("create bucket err : {:?}", e);
            return;
        }
    }

    let concurrency = 10;
    let total_size = 10 * 1024 * 1024;
    let single_file_size = 5 * 1024;

    concurrent_test(
        Arc::clone(&cli),
        bucket_name,
        single_file_size,
        concurrency,
        total_size,
    )
    .await;

    let file_sum: u32 = ((total_size / single_file_size) + 100).try_into().unwrap();

    draw_image(file_sum, "./test_1.log", "../figure/test_1/")
        .await
        .unwrap();

    remove_dir_all("./upload").await.unwrap();
    remove_dir_all("./download").await.unwrap();
}

// test_2: test file upload/download
// total_size: 10 mb
// concurrency: 10
// file_size: 10 kb
#[tokio::test]
async fn test_2() {
    let s3_endpoint = match env::var("END_POINT") {
        Ok(res) => res,
        Err(_) => panic!("can not get end_point"),
    };

    let region = Region::Custom {
        name: "us-east-1".to_owned(),
        endpoint: s3_endpoint.to_owned(),
    };
    let cli = Client::new_with_region(region);
    let cli = Arc::new(cli);

    let bucket_name = "bucket-5".to_string();

    // delete bucket
    delete_bucket_all(Arc::clone(&cli), bucket_name.clone()).await;

    let resp = cli.create_bucket(bucket_name.clone()).await;
    match resp {
        Ok(output) => {
            println!("create bucket ok : {:?}", output.location)
        }
        Err(e) => {
            println!("create bucket err : {:?}", e);
            return;
        }
    }

    let concurrency = 10;
    let total_size = 10 * 1024 * 1024;
    let single_file_size = 10 * 1024;

    concurrent_test(
        Arc::clone(&cli),
        bucket_name,
        single_file_size,
        concurrency,
        total_size,
    )
    .await;

    let file_sum: u32 = ((total_size / single_file_size) + 100).try_into().unwrap();

    draw_image(file_sum, "./test_2.log", "../figure/test_2/")
        .await
        .unwrap();

    remove_dir_all("./upload").await.unwrap();
    remove_dir_all("./download").await.unwrap();
}

// test_3: test file upload/download
// total_size: 10 mb
// concurrency: 20
// file_size: 5 kb
#[tokio::test]
async fn test_3() {
    let s3_endpoint = match env::var("END_POINT") {
        Ok(res) => res,
        Err(_) => panic!("can not get end_point"),
    };

    let region = Region::Custom {
        name: "us-east-1".to_owned(),
        endpoint: s3_endpoint.to_owned(),
    };
    let cli = Client::new_with_region(region);
    let cli = Arc::new(cli);

    let bucket_name = "bucket-6".to_string();

    // delete bucket
    delete_bucket_all(Arc::clone(&cli), bucket_name.clone()).await;

    let resp = cli.create_bucket(bucket_name.clone()).await;
    match resp {
        Ok(output) => {
            println!("create bucket ok : {:?}", output.location)
        }
        Err(e) => {
            println!("create bucket err : {:?}", e);
            return;
        }
    }

    let concurrency = 20;
    let total_size = 10 * 1024 * 1024;
    let single_file_size = 5 * 1024;

    concurrent_test(
        Arc::clone(&cli),
        bucket_name,
        single_file_size,
        concurrency,
        total_size,
    )
    .await;

    let file_sum: u32 = ((total_size / single_file_size) + 100).try_into().unwrap();

    draw_image(file_sum, "./test_3.log", "../figure/test_3/")
        .await
        .unwrap();

    remove_dir_all("./upload").await.unwrap();
    remove_dir_all("./download").await.unwrap();
}
