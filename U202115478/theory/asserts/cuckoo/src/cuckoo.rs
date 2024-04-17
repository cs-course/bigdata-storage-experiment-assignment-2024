use fnv::FnvHasher;
use rand::random;
use std::{
    borrow::Borrow,
    collections::hash_map::DefaultHasher,
    hash::{Hash, Hasher},
    mem::swap,
};
type HashImplOne = DefaultHasher;
type HashImplTwo = FnvHasher;
/// 双桶四槽Cuckoo Hash,API命名与标准库HashMap一致
pub struct Cuckoo<K, V> {
    bucket_size: usize,
    slot_size: usize,
    factor: usize,
    used: usize,
    bucket_one: Vec<Vec<Option<(K, V)>>>,
    bucket_two: Vec<Vec<Option<(K, V)>>>,
}

impl<K, V> Default for Cuckoo<K, V>
where
    K: Clone,
    V: Clone,
{
    fn default() -> Self {
        let bucket_size = Cuckoo::<K, V>::BUCKET_SIZE;
        let slot_size = Cuckoo::<K, V>::SLOT_SIZE;
        let factor = Cuckoo::<K, V>::FACTOR;
        Cuckoo {
            bucket_size,
            slot_size,
            factor,
            used: 0,
            bucket_one: vec![vec![None; slot_size]; bucket_size],
            bucket_two: vec![vec![None; slot_size]; bucket_size],
        }
    }
}

impl<K, V> Cuckoo<K, V> {
    const BUCKET_SIZE: usize = 16;
    const MAX_LOOP_TIMES: usize = 4;
    const SLOT_SIZE: usize = 4;
    const FACTOR: usize = 4;
    ///
    pub fn new(bucket_size: usize, slot_size: usize, factor: usize) -> Self
    where
        K: Clone,
        V: Clone,
    {
        Cuckoo {
            bucket_size,
            slot_size,
            factor,
            used: 0,
            bucket_one: vec![vec![None; slot_size]; bucket_size],
            bucket_two: vec![vec![None; slot_size]; bucket_size],
        }
    }
    ///
    pub fn insert(&mut self, k: K, v: V) -> bool
    where
        K: Eq + Hash + Clone,
        V: Clone,
    {
        if self.contains_key(&k) {
            return true;
        }
        self.used += 1;
        if let Some(pair) = self.__insert(k, v) {
            // rehash
            let mut new_kuku =
                Cuckoo::new(self.bucket_size * self.factor, self.slot_size, self.factor);
            new_kuku.insert(pair.0, pair.1);
            for p in self
                .bucket_one
                .iter_mut()
                .chain(self.bucket_two.iter_mut())
                .flatten()
            {
                if let Some(t) = p.take() {
                    new_kuku.insert(t.0, t.1);
                }
            }
            self.bucket_size = new_kuku.bucket_size;
            self.bucket_one = new_kuku.bucket_one;
            self.bucket_two = new_kuku.bucket_two;
        }
        true
    }
    ///
    fn __insert(&mut self, k: K, v: V) -> Option<(K, V)>
    where
        K: Eq + Hash,
    {
        let mut pair = (k, v);
        for _ in 0usize..Cuckoo::<K, V>::MAX_LOOP_TIMES {
            let p1 = self.h1(&pair.0);
            if let Some((idx, _)) = self.bucket_one[p1]
                .iter()
                .enumerate()
                .find(|(_, op)| op.is_none())
            {
                self.bucket_one[p1][idx] = Some(pair);
                return None;
            }
            let mut new_pair = Some(pair);
            swap(
                &mut new_pair,
                &mut self.bucket_one[p1][random::<usize>() % Cuckoo::<K, V>::SLOT_SIZE],
            );
            pair = new_pair.unwrap();

            let p2 = self.h2(&pair.0);
            if let Some((idx, _)) = self.bucket_two[p2]
                .iter()
                .enumerate()
                .find(|(_, op)| op.is_none())
            {
                self.bucket_two[p2][idx] = Some(pair);
                return None;
            }
            let mut new_pair = Some(pair);
            swap(
                &mut new_pair,
                &mut self.bucket_two[p2][random::<usize>() % Cuckoo::<K, V>::SLOT_SIZE],
            );
            pair = new_pair.unwrap();
        }
        Some(pair)
    }
    ///
    pub fn contains_key<Q>(&self, k: &Q) -> bool
    where
        K: Borrow<Q> + Eq,
        Q: Hash + Eq + ?Sized,
    {
        let p1 = self.h1(k);
        let p2 = self.h2(k);
        self.bucket_one[p1]
            .iter()
            .zip(self.bucket_two[p2].iter())
            .any(|(o1, o2)| {
                (match o1 {
                    Some((ref key, _)) => key.borrow() == k,
                    _ => false,
                }) || (match o2 {
                    Some((ref key, _)) => key.borrow() == k,
                    _ => false,
                })
            })
    }
    ///
    pub fn remove<Q>(&mut self, key: &Q) -> bool
    where
        K: Borrow<Q> + Eq,
        Q: Hash + Eq + ?Sized,
    {
        let p1 = self.h1(key);
        let p2 = self.h2(key);

        if let Some(idx) = self.bucket_one[p1]
            .iter()
            .position(|op| op.is_some() && op.as_ref().unwrap().0.borrow() == key)
        {
            self.bucket_one[p1][idx].take();
            self.used -= 1;
            return true;
        }
        if let Some(idx) = self.bucket_one[p2]
            .iter()
            .position(|op| op.is_some() && op.as_ref().unwrap().0.borrow() == key)
        {
            self.bucket_one[p2][idx].take();
            self.used -= 1;
            return true;
        }
        false
    }
    pub fn get<Q>(&self, k: &Q) -> Option<&V>
    where
        K: Borrow<Q>,
        Q: Hash + Eq + ?Sized,
    {
        let p1 = self.h1(k);
        let p2 = self.h2(k);
        if let Some(idx) = self.bucket_one[p1]
            .iter()
            .position(|op| op.is_some() && op.as_ref().unwrap().0.borrow() == k)
        {
            return Some(&self.bucket_one[p1][idx].as_ref().unwrap().1);
        }
        if let Some(idx) = self.bucket_two[p2]
            .iter()
            .position(|op| op.is_some() && op.as_ref().unwrap().0.borrow() == k)
        {
            return Some(&self.bucket_two[p2][idx].as_ref().unwrap().1);
        }
        None
    }
    pub fn utilization_rate(&self) -> f64 {
        self.used as f64 / (self.bucket_size * self.slot_size) as f64
    }
    /// 默认使用std::DefaultHasher
    fn h1<Q>(&self, k: &Q) -> usize
    where
        K: Borrow<Q>,
        Q: Hash + Eq + ?Sized,
    {
        let mut state = HashImplOne::default();
        k.hash(&mut state);
        (state.finish() % self.bucket_size as u64) as usize
    }
    /// 默认使用fnv::FnvHasher
    fn h2<Q>(&self, k: &Q) -> usize
    where
        K: Borrow<Q>,
        Q: Hash + Eq + ?Sized,
    {
        let mut state = HashImplTwo::default();
        k.hash(&mut state);
        (state.finish() % self.bucket_size as u64) as usize
    }
}
