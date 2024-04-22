use std::time;

#[derive(Clone, Debug)]
pub enum TestType {
    PutObject,
    GetObject,
}

pub struct TestDifferentSizesResult {
    pub test_type: TestType,           // 测试类型
    pub total_object_size: u32,        // 传送的总字节数 KB
    pub object_size: f64,              // 传送的单个文件字节数 KB
    pub duration: f64,                 // 传送的总时间 s
    pub throughput: f64,               // 传送的吞吐量 KB/s
    pub latencys: Vec<time::Duration>, // 传送的延迟 s
    pub avg_latency: time::Duration,   // 传送的平均延迟 s
}

pub struct TestConcurrencyResult {
    pub test_type: TestType,           // 测试类型
    pub total_object_size: u32,        // 传送的总字节数 KB
    pub object_size: f64,              // 传送的单个文件字节数 KB
    pub concurrency_count: u32,        // 传送的并发数
    pub duration: f64,                 // 传送的总时间 s
    pub throughput: f64,               // 传送的吞吐量 KB/s
    pub latencys: Vec<time::Duration>, // 传送的延迟 s
    pub avg_latency: time::Duration,   // 传送的平均延迟 s
}
