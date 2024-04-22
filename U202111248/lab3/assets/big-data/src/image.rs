use plotters::prelude::*;
use std::{
    cmp::{max, min},
    error::Error,
};
use tokio::{
    self,
    io::{AsyncBufReadExt, BufReader},
};

pub async fn draw_image(num: u32, log_file_name: &str, path: &str) -> Result<(), Box<dyn Error>> {
    // Open the log file
    let file = tokio::fs::File::open(log_file_name).await.unwrap();
    let reader = BufReader::new(file);

    // Collect data points from the log file
    let mut min_y_upload = 10000u32;
    let mut max_y_upload = 0u32;

    let mut min_y_download = 10000u32;
    let mut max_y_download = 0u32;

    let mut data_upload: Vec<(u32, u32)> = Vec::new();
    let mut data_download: Vec<(u32, u32)> = Vec::new();
    let mut lines = reader.lines();
    while let Some(line) = lines.next_line().await? {
        let parts: Vec<&str> = line.split_whitespace().collect();
        if parts.len() == 4 && parts[1] == ":" {
            let x: u32 = parts[0].parse()?;
            let y: u32 = parts[3].trim_end_matches("ms").parse()?;
            let tp: String = parts[2].parse()?;

            if tp == "upload" {
                if y > 50 {
                    min_y_upload = min(min_y_upload, y);
                    max_y_upload = max(max_y_upload, y);
                }
                data_upload.push((x, y));
            } else {
                if y > 50 {
                    min_y_download = min(min_y_download, y);
                    max_y_download = max(max_y_download, y);
                }
                data_download.push((x, y));
            }
        }
    }

    create_image(
        data_upload,
        &(path.to_owned() + "upload.png"),
        "upload",
        min_y_upload,
        max_y_upload,
        num,
    )?;
    create_image(
        data_download,
        &(path.to_owned() + "download.png"),
        "download",
        min_y_download,
        max_y_download,
        num,
    )?;

    Ok(())
}

fn create_image(
    data: Vec<(u32, u32)>,
    path: &str,
    name: &str,
    min_y: u32,
    max_y: u32,
    max_x: u32,
) -> Result<(), Box<dyn Error>> {
    let root = BitMapBackend::new(path, (640, 480)).into_drawing_area();

    root.fill(&WHITE)?;

    let mut chart = ChartBuilder::on(&root)
        .x_label_area_size(35)
        .y_label_area_size(40)
        .margin(5)
        .caption(name, ("sans-serif", 50.0))
        .build_cartesian_2d((0u32..max_x).into_segmented(), (min_y - 10)..(max_y + 1))?;

    chart
        .configure_mesh()
        .disable_x_mesh()
        .bold_line_style(WHITE.mix(0.3))
        .y_desc("time")
        .x_desc("num")
        .axis_desc_style(("sans-serif", 15))
        .draw()?;

    chart.draw_series(
        Histogram::vertical(&chart)
            .style(RED.mix(0.5).filled())
            .data(data.iter().map(|&(x_val, y_val)| (x_val, y_val))),
    )?;

    // To avoid the IO failure being ignored silently, we manually call the present function
    root.present().unwrap();
    println!("Result has been saved ");

    Ok(())
}