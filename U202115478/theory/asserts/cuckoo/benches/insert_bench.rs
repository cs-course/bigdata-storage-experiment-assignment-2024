// #![feature(test)]
// extern crate test;

use criterion::{criterion_group, criterion_main, Criterion};
use cuckoo::Cuckoo;
use rand::random;
use std::{iter::from_fn, time::Duration};

fn insert_benchmark(c: &mut Criterion) {
    let mut cr = c.benchmark_group("insert_group");
    cr.measurement_time(Duration::from_secs(120));
    cr.bench_function("insert", |b| {
        b.iter(|| {
            let mut kuku: Cuckoo<usize, usize> = Cuckoo::default();
            let size: usize = 100_0000usize;
            let key: Vec<usize> = from_fn(|| Some(random())).take(size).collect();
            let value: Vec<usize> = from_fn(|| Some(random())).take(size).collect();
            for (k, v) in key.iter().zip(value.iter()) {
                kuku.insert(*k, *v);
            }
        })
    });
}

criterion_group!(benches, insert_benchmark);
criterion_main!(benches);
