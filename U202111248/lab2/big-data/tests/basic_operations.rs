use std::env;

use big_data::client::{byte_stream_to_string, create_file_with_contents, write_into_file, Client};
use rusoto_core::Region;
use tokio::{fs::File, io::AsyncReadExt};

#[tokio::test]
async fn basic_operations_for_str() {
    let s3_endpoint = match env::var("END_POINT") {
        Ok(res) => res,
        Err(_) => panic!("can not get end_point"),
    };

    let region = Region::Custom {
        name: "us-east-1".to_owned(),
        endpoint: s3_endpoint.to_owned(),
    };
    let cli = Client::new_with_region(region);

    let bucket_name = "bucket";
    let key = "my key";
    let content = "12345678910";

    // create bucket
    let resp = cli.create_bucket(bucket_name.into()).await;
    match resp {
        Ok(output) => {
            println!("create bucket ok: {:?}", output.location)
        }
        Err(e) => {
            println!("create bucket err: {:?}", e);
            return;
        }
    }

    // insert obj
    let resp = cli
        .insert_object(
            bucket_name.into(),
            key,
            Some(content.as_bytes().to_vec().into()),
        )
        .await
        .unwrap();
    println!("insert object version_id: {:?}", resp.version_id);

    // select obj
    let resp = cli.get_object(bucket_name.into(), key).await.unwrap();
    println!(
        "select object: {:?}",
        byte_stream_to_string(resp.body.unwrap()).await.unwrap()
    );

    // update obj
    let _ = cli
        .update_object(bucket_name.into(), key, "10987654321")
        .await
        .unwrap();
    println!("update object: {:?}", key);

    // select obj again
    let resp = cli.get_object(bucket_name.into(), key).await.unwrap();
    println!(
        "select object: {:?}",
        byte_stream_to_string(resp.body.unwrap()).await.unwrap()
    );

    let resp = cli.delete_object(bucket_name.to_string(), key).await;
    match resp {
        Ok(_) => {
            println!("delete object: ok!")
        }
        Err(e) => {
            println!("delete object err: {:?}", e);
            return;
        }
    }

    // select obj again
    let resp = cli.get_object(bucket_name.into(), key).await;
    match resp {
        Ok(output) => {
            println!(
                "select object: {:?}",
                byte_stream_to_string(output.body.unwrap()).await.unwrap()
            );
        }
        Err(e) => {
            println!("select object err: {:?}", e);
        }
    }
}

#[tokio::test]
async fn basic_operations_for_files() {
    let s3_endpoint = match env::var("END_POINT") {
        Ok(res) => res,
        Err(_) => panic!("can not get end_point"),
    };

    let region = Region::Custom {
        name: "us-east-1".to_owned(),
        endpoint: s3_endpoint.to_owned(),
    };
    let cli = Client::new_with_region(region);

    let bucket_name = "file_bucket";
    let key = "./unload.txt";
    create_file_with_contents(key).await;

    // create bucket
    let resp = cli.create_bucket(bucket_name.into()).await;
    match resp {
        Ok(output) => {
            println!("create bucket ok: {:?}", output.location)
        }
        Err(e) => {
            println!("create bucket err: {:?}", e);
            return;
        }
    }

    // insert obj
    let mut buf = Vec::new();
    let mut file = File::open(key).await.unwrap();

    file.read_to_end(&mut buf).await.unwrap();

    let _resp = cli
        .insert_object(bucket_name.into(), key, Some(buf.into()))
        .await
        .unwrap();
    println!("upload file successfully!");

    // select obj
    let resp = cli.get_object(bucket_name.into(), key).await.unwrap();
    // write into file
    write_into_file("./download.txt", resp.body.unwrap()).await;
    println!("download file successfully!")
}
