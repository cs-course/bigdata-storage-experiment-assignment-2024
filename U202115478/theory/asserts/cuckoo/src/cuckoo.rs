use fnv::FnvHasher;
use rand::random;
use std::{
    borrow::Borrow,
    collections::hash_map::DefaultHasher,
    hash::{Hash, Hasher},
    mem::swap,
    ops::{Index, IndexMut},
};
type HashImplOne = DefaultHasher;
type HashImplTwo = FnvHasher;
/// 双桶四槽Cuckoo Hash,API命名与标准库HashMap一致
pub struct Cuckoo<K, V> {
    bucket_size: usize,
    slot_size: usize,
    factor: usize,
    used: usize,
    max_loop_times: usize,
    bucket_one: Vec<Vec<Option<(K, V)>>>,
    bucket_two: Vec<Vec<Option<(K, V)>>>,
}

impl<K, V> Cuckoo<K, V> {
    const BUCKET_SIZE: usize = 16;
    const MAX_LOOP_TIMES: usize = 4;
    const SLOT_SIZE: usize = 4;
    const FACTOR: usize = 4;
    /// bucket_size: 桶的大小
    /// slot_size: 桶每个槽的槽位
    /// factor: 扩容因子, new_cap = old_cap * factor
    pub fn new(bucket_size: usize, slot_size: usize, factor: usize) -> Self
    where
        K: Clone,
        V: Clone,
    {
        Cuckoo {
            bucket_size,
            slot_size,
            factor,
            max_loop_times: Cuckoo::<K, V>::MAX_LOOP_TIMES,
            used: 0,
            bucket_one: vec![vec![None; slot_size]; bucket_size],
            bucket_two: vec![vec![None; slot_size]; bucket_size],
        }
    }
    /// 如果k存在,使用新v覆盖旧v
    /// 如果不存在,插入，
    /// Kick 超过最大循环次数就扩容
    pub fn insert(&mut self, k: K, v: V) -> bool
    where
        K: Eq + Hash + Clone,
        V: Clone,
    {
        match self.index_of_key(&k) {
            (0, p, idx) => {
                self.bucket_one[p][idx] = Some((k, v));
                return true;
            }
            (1, p, idx) => {
                self.bucket_two[p][idx] = Some((k, v));
                return true;
            }
            _ => {}
        }
        self.used += 1;
        if let Some(pair) = self.try_insert(k, v) {
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
    /// 单次插入,成功返回None，否则返回需要插入的(k,v)
    fn try_insert(&mut self, k: K, v: V) -> Option<(K, V)>
    where
        K: Eq + Hash,
    {
        let mut pair = (k, v);
        for _ in 0usize..self.max_loop_times {
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
    // 查找指定key对应的 桶索引和槽索引
    fn index_of_key<Q>(&self, k: &Q) -> (isize, usize, usize)
    where
        K: Borrow<Q> + Eq,
        Q: Hash + Eq + ?Sized,
    {
        let p1 = self.h1(k);

        if let Some(idx) = self.bucket_one[p1].iter().position(|op| match op {
            Some((ref key, _)) => key.borrow() == k,
            _ => false,
        }) {
            return (0, p1, idx);
        }

        let p2 = self.h2(k);
        if let Some(idx) = self.bucket_two[p2].iter().position(|op| match op {
            Some((ref key, _)) => key.borrow() == k,
            _ => false,
        }) {
            return (1, p2, idx);
        }

        (-1, 0, 0)
    }
    /// 是否包含指定key
    pub fn contains_key<Q>(&self, k: &Q) -> bool
    where
        K: Borrow<Q> + Eq,
        Q: Hash + Eq + ?Sized,
    {
        match self.index_of_key(k) {
            (0 | 1, _, _) => true,
            _ => false,
        }
    }
    /// 移除指定key,key存在返回true,否则返回false
    pub fn remove<Q>(&mut self, k: &Q) -> bool
    where
        K: Borrow<Q> + Eq,
        Q: Hash + Eq + ?Sized,
    {
        match self.index_of_key(k) {
            (0, p, idx) => {
                self.used -= 1;
                self.bucket_one[p][idx].take();
                true
            }
            (1, p, idx) => {
                self.used -= 1;
                self.bucket_two[p][idx].take();
                true
            }
            _ => false,
        }
    }
    /// 获取key对应的 mut value
    pub fn get<Q>(&self, k: &Q) -> Option<&V>
    where
        K: Borrow<Q> + Eq,
        Q: Hash + Eq + ?Sized,
    {
        match self.index_of_key(k) {
            (0, p, idx) => Some(&self.bucket_one[p][idx].as_ref().unwrap().1),
            (1, p, idx) => Some(&self.bucket_two[p][idx].as_ref().unwrap().1),
            _ => None,
        }
    }
    /// 获取key对应的value
    pub fn get_mut<Q>(&mut self, k: &Q) -> Option<&mut V>
    where
        K: Borrow<Q> + Eq,
        Q: Hash + Eq + ?Sized,
    {
        match self.index_of_key(k) {
            (0, p, idx) => Some(&mut self.bucket_one[p][idx].as_mut().unwrap().1),
            (1, p, idx) => Some(&mut self.bucket_two[p][idx].as_mut().unwrap().1),
            _ => None,
        }
    }
    /// 获取空间利用率
    pub fn utilization_rate(&self) -> f64 {
        self.used as f64 / (self.bucket_size * self.slot_size) as f64
    }
    /// Hash表存储的元素数量
    pub fn size(&self) -> usize {
        self.used
    }
    /// 清空
    pub fn clear(&mut self) {
        self.used = 0;
        self.bucket_one
            .iter_mut()
            .chain(self.bucket_two.iter_mut())
            .flatten()
            .for_each(|op| {
                op.take();
            });
    }
    /// 是否为空
    pub fn empty(&self) -> bool {
        self.used == 0
    }
    /// 设置扩容因子,new_capacity = old_capacity * factor
    pub fn set_factor(&mut self, factor: usize) {
        self.factor = factor;
    }
    /// 设置最大循环次数
    pub fn set_max_loop_times(&mut self, max_loop_times: usize) {
        self.max_loop_times = max_loop_times;
    }
    /// 将内部结构自带的迭代器串联并展开作为Cuckoo的迭代器
    pub fn iter(&self) -> impl Iterator<Item = (&K, &V)> {
        self.bucket_one
            .iter()
            .chain(self.bucket_two.iter())
            .flatten()
            .filter(|op| op.is_some())
            .map(|op| {
                let (k, v) = op.as_ref().unwrap();
                (k, v)
            })
    }
    #[allow(unused)]
    pub fn iter_mut(&mut self) -> impl Iterator<Item = (&mut K, &mut V)> {
        self.bucket_one
            .iter_mut()
            .chain(self.bucket_two.iter_mut())
            .flatten()
            .filter(|op| op.is_some())
            .map(|op| {
                let (k, v) = op.as_mut().unwrap();
                (k, v)
            })
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

/// 默认初始化
/// bucket_size:16
/// slot_size:4
/// max_loop_times:4
/// factor:4
impl<K, V> Default for Cuckoo<K, V>
where
    K: Clone,
    V: Clone,
{
    fn default() -> Self {
        let bucket_size = Cuckoo::<K, V>::BUCKET_SIZE;
        let slot_size = Cuckoo::<K, V>::SLOT_SIZE;
        let factor = Cuckoo::<K, V>::FACTOR;
        let max_loop_times = Cuckoo::<K, V>::MAX_LOOP_TIMES;
        Cuckoo {
            bucket_size,
            slot_size,
            factor,
            max_loop_times,
            used: 0,
            bucket_one: vec![vec![None; slot_size]; bucket_size],
            bucket_two: vec![vec![None; slot_size]; bucket_size],
        }
    }
}
/// 重载运算符[],不存在对应的key就Panic
impl<K, V, Q> Index<&Q> for Cuckoo<K, V>
where
    K: Eq + Hash + Borrow<Q>,
    Q: Eq + Hash + ?Sized,
{
    type Output = V;
    fn index(&self, k: &Q) -> &Self::Output {
        self.get(k).unwrap()
    }
}

impl<K, V, Q> IndexMut<&Q> for Cuckoo<K, V>
where
    K: Eq + Hash + Borrow<Q>,
    Q: Eq + Hash + ?Sized,
{
    fn index_mut(&mut self, k: &Q) -> &mut Self::Output {
        self.get_mut(k).unwrap()
    }
}
