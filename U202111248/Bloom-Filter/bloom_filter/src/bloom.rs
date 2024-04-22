use std::time::{SystemTime, UNIX_EPOCH};

use rand::{thread_rng, Rng};
use sha2::{Digest, Sha256};

// 创建哈希函数生成器
fn create_hash_function(seed: usize) -> Box<dyn Fn(usize) -> usize> {
    Box::new(move |value: usize| -> usize {
        let mut hasher = Sha256::new();
        hasher.update(&[seed as u8]);
        hasher.update(&value.to_ne_bytes());
        let result = hasher.finalize();
        let hash_bytes = result.as_slice();
        let mut hash: usize = 0;
        for &byte in hash_bytes.iter() {
            hash = (hash << 8) | byte as usize;
        }
        hash
    })
}

pub struct BloomFilter {
    bitset: Vec<bool>,
    hash_functions: Vec<Box<dyn Fn(usize) -> usize>>,
}

impl BloomFilter {
    // size代表位数组大小，hash_count代表hash函数数量
    pub fn new(size: usize, hash_count: usize) -> Self {
        let mut hash_functions = Vec::with_capacity(hash_count);
        for i in 0..hash_count {
            // 获取当前时间作为种子
            let now = SystemTime::now();
            let since_the_epoch = now.duration_since(UNIX_EPOCH).expect("Time went backwards");
            let seed = since_the_epoch.as_millis() as usize;

            let mut rng = thread_rng();

            let up: usize = rng.gen_range(1111..9999);

            hash_functions.push(create_hash_function(seed + up * i));
        }
        BloomFilter {
            bitset: vec![false; size],
            hash_functions,
        }
    }

    pub fn insert(&mut self, item: usize) {
        for hasher in &mut self.hash_functions {
            let hash = hasher(item);
            let index = hash % self.bitset.len();
            self.bitset[index] = true;
        }
    }

    pub fn contains(&mut self, item: usize) -> bool {
        for hasher in &mut self.hash_functions {
            let hash = hasher(item);
            let index = hash % self.bitset.len();
            if !self.bitset[index] {
                return false;
            }
        }
        true
    }

    pub fn judge(&self, index: usize) -> bool {
        self.bitset[index]
    }

    // 返回第x个hash函数的值
    pub fn get_x_hash_res(&mut self, x: usize, item: usize) -> usize {
        let hasher = &mut self.hash_functions[x];
        let hash = hasher(item);
        hash % self.bitset.len()
    }
}

pub struct MultidimensionalBloomFilter {
    bloom_filters: Vec<BloomFilter>,
    union_bloom_filter: Vec<bool>,
    hash_num: usize,
}

impl MultidimensionalBloomFilter {
    // dimension代表维度，m代表单个布隆过滤器数组长度，n代表预估的数据量
    pub fn new(dimension: usize, m: usize, n: usize) -> MultidimensionalBloomFilter {
        let k = ((m as f64 / n as f64) * 0.693) as usize;

        let mut bloom_filers = Vec::with_capacity(dimension);
        for _ in 0..dimension {
            bloom_filers.push(BloomFilter::new(m, k + 1));
        }

        MultidimensionalBloomFilter {
            bloom_filters: bloom_filers,
            union_bloom_filter: vec![false; m],
            hash_num: k + 1,
        }
    }

    pub fn insert(&mut self, items: Vec<usize>) -> bool {
        if items.len() != self.bloom_filters.len() {
            return false;
        }

        for i in 0..self.bloom_filters.len() {
            self.bloom_filters[i].insert(items[i]);
        }

        // 联合布隆过滤器
        for i in 0..self.hash_num {
            let mut res: usize = 0;

            for (index, bt) in self.bloom_filters.iter_mut().enumerate() {
                res = res ^ (bt.get_x_hash_res(i, items[index]));
            }

            let len = self.union_bloom_filter.len();
            self.union_bloom_filter[res % len] = true;
        }

        true
    }

    // flag表示是否查找联合布隆过滤器
    pub fn contains(&mut self, items: Vec<usize>, flag: bool) -> bool {
        if items.len() != self.bloom_filters.len() {
            return false;
        }

        for (index, bf) in self.bloom_filters.iter_mut().enumerate() {
            if !bf.contains(items[index]) {
                return false;
            }
        }

        if flag {
            for i in 0..self.hash_num {
                let mut res: usize = 0;

                for (index, bt) in self.bloom_filters.iter_mut().enumerate() {
                    res = res ^ (bt.get_x_hash_res(i, items[index]));
                }

                if !self.union_bloom_filter[res % self.union_bloom_filter.len()] {
                    return false;
                }
            }
        }

        true
    }
}
