use bloom_filter::bloom::BloomFilter;

#[test]
fn test_bloom_filter() {
    let mut bf = BloomFilter::new(100, 2);

    bf.insert(10000);
    bf.insert(200000);

    assert!(bf.contains(10000));
    assert!(bf.contains(200000));
    assert!(!bf.contains(100));
}
