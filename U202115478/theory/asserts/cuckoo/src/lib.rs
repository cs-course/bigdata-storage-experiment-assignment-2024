pub mod cuckoo;

pub use cuckoo::*;

#[cfg(test)]
mod tests {
    use super::*;
    use rand::random;
    use std::collections::HashMap;
    use std::iter::from_fn;
    #[test]
    /// 与HashMap比较,测试操作的正确性
    fn test_consistency() {
        let mut kuku: Cuckoo<usize, usize> = Cuckoo::default();
        let mut mp: HashMap<usize, usize> = HashMap::new();
        let size: usize = 100_0000usize;
        let key: Vec<usize> = from_fn(|| Some(random())).take(size).collect();
        let value: Vec<usize> = from_fn(|| Some(random())).take(size).collect();

        for (k, v) in key.iter().zip(value.iter()) {
            kuku.insert(*k, *v);
            mp.insert(*k, *v);
        }

        kuku.clear();
        mp.clear();

        for (k, v) in key.iter().zip(value.iter()) {
            kuku.insert(*k, *v);
            mp.insert(*k, *v);
        }

        for k in key.iter() {
            assert_eq!(kuku.contains_key(k), mp.contains_key(k));
            assert_eq!(kuku.get(k), mp.get(k));
        }

        let (remain, delete): (Vec<usize>, Vec<usize>) =
            key.into_iter().partition(|k| (k & 1) == 0);

        for k in delete.iter() {
            kuku.remove(k);
            mp.remove(k);
        }

        for k in remain.iter() {
            assert_eq!(kuku.contains_key(k), mp.contains_key(k));
            assert_eq!(kuku.get(k), mp.get(k));
        }
    }

    #[test]
    /// 与HashMap比较,测试速度
    fn test_speed() {
        use cuckoo::Cuckoo;
        use rand::random;
        use std::{collections::HashMap, iter::from_fn, time::Instant};
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

        let (remain, delete): (Vec<usize>, Vec<usize>) =
            key.into_iter().partition(|k| (k & 1) == 0);

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
    #[test]
    fn test_args() {
        use cuckoo::Cuckoo;
        use rand::random;
        use std::{iter::from_fn, time::Instant};
        let size: usize = 100_0000usize;
        let key: Vec<usize> = from_fn(|| Some(random())).take(size).collect();
        let value: Vec<usize> = from_fn(|| Some(random())).take(size).collect();

        let loop_time_dec = vec![(16usize, 2usize), (12, 2), (8, 2), (4, 2)];
        let double_factor = vec![(16usize, 4usize), (12, 4), (8, 4), (4, 4)];
        let test_fn = |test_vec: &Vec<(usize, usize)>| {
            for (loop_times, factor) in test_vec {
                let mut kuku = Cuckoo::new(16, 4, *factor);
                kuku.set_max_loop_times(*loop_times);
                let timer = Instant::now();
                for (k, v) in key.iter().zip(value.iter()) {
                    kuku.insert(*k, *v);
                }
                println!(
                    "loop_times={},factor={},insert 100_0000 (usize,usize) cost {}ms",
                    loop_times,
                    factor,
                    timer.elapsed().as_millis()
                );
            }
        };
        test_fn(&loop_time_dec);
        test_fn(&double_factor);
    }
}
