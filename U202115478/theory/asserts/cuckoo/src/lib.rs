pub mod cuckoo;

pub use cuckoo::*;

#[cfg(test)]
mod tests {
    use super::*;
    use rand::random;
    use std::collections::HashMap;
    use std::iter::from_fn;
    #[test]
    fn test_consistency() {
        let mut kuku: Cuckoo<usize, usize> = Cuckoo::default();
        let mut mp: HashMap<usize, usize> = HashMap::new();
        let size: usize = 10000usize;
        let key: Vec<usize> = from_fn(|| Some(random())).take(size).collect();
        let value: Vec<usize> = from_fn(|| Some(random())).take(size).collect();

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
}
