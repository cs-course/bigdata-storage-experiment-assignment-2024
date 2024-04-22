use cuckoo::Cuckoo;
use rand::{random, Rng};
use std::iter::from_fn;

/// 自定义类型作为Cuckoo的key必须实现 Clone+ Eq+ Hash
#[derive(Clone, Hash, PartialEq, Eq, Default)]
struct Custom {
    id: usize,
    name: String,
}
fn main() {
    let (key, value) = generate_data();
    // 使用默认构造
    let mut kuku: Cuckoo<Custom, usize> = Cuckoo::default();
    // 指定参数构造
    let _: Cuckoo<Custom, usize> = Cuckoo::new(8, 4, 2);
    // 使用 set_max_loop_times设置插入时最大循环次数
    kuku.set_max_loop_times(4);
    // 使用insert插入
    for (k, v) in key.clone().into_iter().zip(value.clone().into_iter()) {
        kuku.insert(k, v);
    }
    // 使用empty检验是否为空
    assert_eq!(false, kuku.empty());
    // 使用size获取大小
    assert_eq!(kuku.size(), key.len());
    // 使用utilization_rate查看空间利用率
    println!("utilization_rate = {}", kuku.utilization_rate());
    // 使用set_factor设置扩容因子
    kuku.set_factor(2);
    // 查询是否存在指定key
    assert_eq!(true, kuku.contains_key(&key[0]));
    let not_exist = Custom::default();
    assert_eq!(false, kuku.contains_key(&not_exist));
    // 使用迭代器遍历
    for (k, v) in kuku.iter() {
        assert_eq!(key.contains(k), true);
        assert_eq!(value.contains(v), true);
    }
    // 使用get 或 []获取指定key
    assert_eq!(kuku.get(&key[0]), Some(&value[0]));
    assert_eq!(kuku[&key[0]], value[0]);
    // 使用remove删除指定key
    assert_eq!(true, kuku.remove(&key[0]));
    assert_eq!(false, kuku.remove(&key[0]));
    assert_eq!(false, kuku.contains_key(&key[0]));
    // 使用clear清空
    kuku.clear();
    assert_eq!(0, kuku.size());
}

fn generate_data() -> (Vec<Custom>, Vec<usize>) {
    let size: usize = 10usize;
    let str_len: usize = 10;
    let alphabet: Vec<char> = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
        .chars()
        .collect();
    let mut rng = rand::thread_rng();
    // 生成数据
    let ss: Vec<Custom> = from_fn(|| {
        Some(Custom {
            id: random(),
            name: (0..str_len)
                .map(|_| rng.gen_range(0..alphabet.len()))
                .map(|index| alphabet[index])
                .collect(),
        })
    })
    .take(size)
    .collect();
    let value: Vec<usize> = from_fn(|| Some(random())).take(size).collect();
    (ss, value)
}
