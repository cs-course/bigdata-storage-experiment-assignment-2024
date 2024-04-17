use cuckoo::Cuckoo;
use rand::random;
use std::{collections::HashMap, iter::from_fn, time::Instant};

fn main() {
    let mut kuku: Cuckoo<usize, usize> = Cuckoo::default();
    let mut mp: HashMap<usize, usize> = HashMap::new();
    let size: usize = 100_0000usize;
    let key: Vec<usize> = from_fn(|| Some(random())).take(size).collect();
    let value: Vec<usize> = from_fn(|| Some(random())).take(size).collect();

    let timer = Instant::now();
    for (k, v) in key.iter().zip(value.iter()) {
        kuku.insert(*k, *v);
    }
    println!(
        "Cuckoo insert {} (k,v) cost {}ms",
        size,
        timer.elapsed().as_millis()
    );

    let timer = Instant::now();
    for (k, v) in key.iter().zip(value.iter()) {
        mp.insert(*k, *v);
    }
    println!(
        "HashMap insert {} (k,v) cost {}ms",
        size,
        timer.elapsed().as_millis()
    );

    println!(
        "Cuckoo Space utilization rate = {:.4}",
        kuku.utilization_rate()
    );

    for k in key.iter() {
        assert_eq!(kuku.contains_key(k), mp.contains_key(k));
    }

    let (remain, delete): (Vec<usize>, Vec<usize>) = key.into_iter().partition(|k| (k & 1) == 0);

    let timer = Instant::now();
    for k in delete.iter() {
        kuku.remove(k);
    }
    println!(
        "Cuckoo remove {} (k,v) cost {}ms",
        delete.len(),
        timer.elapsed().as_millis()
    );

    let timer = Instant::now();
    for k in delete.iter() {
        mp.remove(k);
    }
    println!(
        "HashMap remove {} (k,v) cost {}ms",
        delete.len(),
        timer.elapsed().as_millis()
    );

    println!(
        "Cuckoo Space utilization rate = {:.4}",
        kuku.utilization_rate()
    );

    let timer = Instant::now();
    for k in remain.iter() {
        kuku.contains_key(k);
    }
    println!(
        "Cuckoo query {} (k,v) cost {}ms",
        remain.len(),
        timer.elapsed().as_millis()
    );

    let timer = Instant::now();
    for k in remain.iter() {
        mp.contains_key(k);
    }
    println!(
        "HashMap query {} (k,v) cost {}ms",
        remain.len(),
        timer.elapsed().as_millis()
    );

    let timer = Instant::now();
    for k in remain.iter() {
        kuku.get(k);
    }
    println!(
        "Cuckoo get {} (k,v) cost {}ms",
        remain.len(),
        timer.elapsed().as_millis()
    );

    let timer = Instant::now();
    for k in remain.iter() {
        mp.get(k);
    }
    println!(
        "HashMap get {} (k,v) cost {}ms",
        remain.len(),
        timer.elapsed().as_millis()
    );
}
