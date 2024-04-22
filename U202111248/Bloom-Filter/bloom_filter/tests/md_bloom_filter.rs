use bloom_filter::bloom::MultidimensionalBloomFilter;
use rand::{thread_rng, Rng};
use std::collections::HashSet;

#[test]
fn test_multidimensional_bloom_filter() {
    for _ in 0..100 {
        // 测试1000数组长度，100个数据

        let mut mbf = MultidimensionalBloomFilter::new(3, 1000, 100);
        let mut set = HashSet::new();

        let mut col = Vec::new();
        let mut rng = thread_rng();

        for i in 0..250 {
            let mut vec = Vec::new();
            loop {
                for _ in 0..3 {
                    let value: usize = rng.gen_range(0..10);
                    vec.push(value);
                }

                if !set.contains(&vec) {
                    break;
                }
            }

            if i < 150 {
                mbf.insert(vec.clone());
            }
            set.insert(vec.clone());

            col.push(vec);
        }

        let mut num = 0;
        for i in 150..250 {
            if mbf.contains(col[i].clone(), false) {
                num = num + 1;
            }
        }

        let mut num_1 = 0;
        for i in 150..250 {
            if mbf.contains(col[i].clone(), true) {
                num_1 = num_1 + 1;
            }
        }

        println!(
            "失误率:不采用联合bloom filter({:.2}%), 采用联合bloom filter({:.2}%)\n",
            (num as f64 / 100.0 as f64) * 100.0,
            (num_1 as f64 / 100.0 as f64) * 100.0
        );
    }
}
