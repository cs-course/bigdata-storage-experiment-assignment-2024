use std::error::Error;

use crate::bench;
use plotters::prelude::*;

pub fn draw_object_size_average_latency_chart(
    res: &Vec<bench::TestDifferentSizesResult>,
    title: &str,
) -> Result<(), Box<dyn Error>> {
    let object_size = res.iter().map(|x| x.object_size).collect::<Vec<f64>>();

    let average_latency = res
        .iter()
        .map(|x| (x.avg_latency.as_secs_f64()))
        .collect::<Vec<f64>>();

    let filename = format!("object-size-average-latency-{:?}.png", res[0].test_type);

    let root = BitMapBackend::new(&filename, (800, 600)).into_drawing_area();
    root.fill(&WHITE)?;

    let mut chart = ChartBuilder::on(&root)
        .caption(title, ("sans-serif", 40).into_font())
        .margin(5)
        .x_label_area_size(40)
        .y_label_area_size(40)
        .build_cartesian_2d(
            0.0f64..object_size.iter().cloned().fold(0. / 0., f64::max),
            0.0f64..average_latency.iter().cloned().fold(0. / 0., f64::max),
        )?;

    chart
        .configure_mesh()
        .x_desc("Object Size (KB)")
        .y_desc("Average Latency (S)")
        .draw()?;

    chart.draw_series(LineSeries::new(
        object_size
            .clone()
            .into_iter()
            .zip(average_latency.clone().into_iter()),
        &RED,
    ))?;
    chart.draw_series(PointSeries::of_element(
        object_size
            .clone()
            .into_iter()
            .zip(average_latency.into_iter()),
        5,
        &RED,
        &|c, s, st| {
            return EmptyElement::at(c) + Circle::new((0, 0), s, st.filled());
        },
    ))?;

    Ok(())
}

pub fn draw_object_size_throught_chart(
    res: &Vec<bench::TestDifferentSizesResult>,
    title: &str,
) -> Result<(), Box<dyn Error>> {
    let object_size = res.iter().map(|x| x.object_size).collect::<Vec<f64>>();

    let throught = res.iter().map(|x| (x.throughput)).collect::<Vec<f64>>();
    let filename = format!("object-size-throught-{:?}.png", res[0].test_type);

    let root = BitMapBackend::new(&filename, (800, 600)).into_drawing_area();
    root.fill(&WHITE)?;

    let mut chart = ChartBuilder::on(&root)
        .caption(title, ("sans-serif", 40).into_font())
        .margin(5)
        .x_label_area_size(40)
        .y_label_area_size(40)
        .build_cartesian_2d(
            0.0f64..object_size.iter().cloned().fold(0. / 0., f64::max),
            0.0f64..throught.iter().cloned().fold(0. / 0., f64::max),
        )?;

    chart
        .configure_mesh()
        .x_desc("Object Size (KB)")
        .y_desc("Throught (KB/S)")
        .draw()?;

    chart.draw_series(LineSeries::new(
        object_size
            .clone()
            .into_iter()
            .zip(throught.clone().into_iter()),
        &RED,
    ))?;
    chart.draw_series(PointSeries::of_element(
        object_size.clone().into_iter().zip(throught.into_iter()),
        5,
        &RED,
        &|c, s, st| {
            return EmptyElement::at(c) + Circle::new((0, 0), s, st.filled());
        },
    ))?;

    Ok(())
}

pub fn draw_concurrency_average_latency_chart(
    res: &Vec<bench::TestConcurrencyResult>,
    title: &str,
) -> Result<(), Box<dyn Error>> {
    let concurrencys = res
        .iter()
        .map(|x| x.concurrency_count as f64)
        .collect::<Vec<f64>>();

    let average_latency = res
        .iter()
        .map(|x| x.avg_latency.as_secs_f64())
        .collect::<Vec<f64>>();

    let filename = format!("concurrency-average-latency-{:?}.png", res[0].test_type);

    let root = BitMapBackend::new(&filename, (800, 600)).into_drawing_area();
    root.fill(&WHITE).unwrap();

    let mut chart = ChartBuilder::on(&root)
        .caption(title, ("sans-serif", 40).into_font())
        .margin(5)
        .x_label_area_size(40)
        .y_label_area_size(40)
        .build_cartesian_2d(
            0.0f64..concurrencys.iter().cloned().fold(0. / 0., f64::max),
            0.0f64..average_latency.iter().cloned().fold(0. / 0., f64::max),
        )
        .unwrap();

    chart
        .configure_mesh()
        .x_desc("Concurrency Count")
        .y_desc("Average Latency (S)")
        .draw()?;

    chart.draw_series(LineSeries::new(
        concurrencys
            .clone()
            .into_iter()
            .zip(average_latency.clone().into_iter()),
        &RED,
    ))?;
    chart.draw_series(PointSeries::of_element(
        concurrencys
            .clone()
            .into_iter()
            .zip(average_latency.into_iter()),
        5,
        &RED,
        &|c, s, st| {
            return EmptyElement::at(c) + Circle::new((0, 0), s, st.filled());
        },
    ))?;

    Ok(())
}

pub fn draw_concurrency_throught_chart(
    res: &Vec<bench::TestConcurrencyResult>,
    title: &str,
) -> Result<(), Box<dyn Error>> {
    let concurrencys = res
        .iter()
        .map(|x| x.concurrency_count as f64)
        .collect::<Vec<f64>>();

    let throught = res.iter().map(|x| x.throughput).collect::<Vec<f64>>();

    let filename = format!("concurrency-throught-{:?}.png", res[0].test_type);

    let root = BitMapBackend::new(&filename, (800, 600)).into_drawing_area();
    root.fill(&WHITE).unwrap();

    let mut chart = ChartBuilder::on(&root)
        .caption(title, ("sans-serif", 40).into_font())
        .margin(5)
        .x_label_area_size(40)
        .y_label_area_size(40)
        .build_cartesian_2d(
            0.0f64..concurrencys.iter().cloned().fold(0. / 0., f64::max),
            0.0f64..throught.iter().cloned().fold(0. / 0., f64::max),
        )
        .unwrap();

    chart
        .configure_mesh()
        .x_desc("Concurrency Count")
        .y_desc("Throught (KB/S)")
        .draw()?;

    chart.draw_series(LineSeries::new(
        concurrencys
            .clone()
            .into_iter()
            .zip(throught.clone().into_iter()),
        &RED,
    ))?;
    chart.draw_series(PointSeries::of_element(
        concurrencys.clone().into_iter().zip(throught.into_iter()),
        5,
        &RED,
        &|c, s, st| {
            return EmptyElement::at(c) + Circle::new((0, 0), s, st.filled());
        },
    ))?;

    Ok(())
}
