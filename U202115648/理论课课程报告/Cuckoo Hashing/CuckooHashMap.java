package CuckooHash;

import java.util.AbstractMap;
import java.util.ArrayList;
import java.util.Map;
import java.util.Set;

public class NecklaceHashMap<K, V> extends AbstractMap<K, V> implements Map<K, V> {

    private int CAPACITY = 16; // 默认容量
    private int size = 0;
    private Entry<K, V>[] table;
    private boolean[] idOccupied; // bucket是否被占用
    private boolean[] isHashed; // bucket是否有元素能哈希到
    private ArrayList<AuxiliaryEntry<K, V>> auxiliary = new ArrayList<>(); // 辅助表

    public NecklaceHashMap() {
        table = new Entry[CAPACITY];
        idOccupied = new boolean[CAPACITY];
        isHashed = new boolean[CAPACITY];
    }

    public NecklaceHashMap(int capacity) {
        CAPACITY = capacity;
        table = new Entry[CAPACITY];
        idOccupied = new boolean[CAPACITY];
        isHashed = new boolean[CAPACITY];
        for (int i = 0; i < CAPACITY; i++) {
            idOccupied[i] = false;
            isHashed[i] = false;
        }
    }

    private int hash1(Object key) {
        return (key == null) ? 0 : key.hashCode() % CAPACITY;
    }

    private int hash2(Object key) {
        if (key == null) return 0;
        int result = 17;
        result = 31 * result + key.getClass().getName().hashCode(); // 类名的哈希
        result = 31 * result + System.identityHashCode(key); // 系统身份哈希
        return Math.abs(result) % CAPACITY;
    }

    private int hash3(Object key) {
        if (key == null) return 0;
        long constant = 0x9e3779b97f4a7c15L; // 一个较大的质数的黄金分割数
        long keyHash = key.hashCode();
        long mix = (keyHash ^ (keyHash >>> 16)) * constant;
        mix ^= (mix >>> 30) + (System.nanoTime() & 0xFF); // 添加纳秒级时间的低8位以引入更多随机性
        return Math.abs((int) (mix % CAPACITY));
    }

    @Override
    public V put(K key, V value) {
        int index1 = hash1(key);
        int index2 = hash2(key);
        int index3 = hash3(key);
        if (table[index1] == null || table[index2] == null || table[index3] == null) {
            Entry<K, V> newEntry = new Entry<>(key, value);
            if (table[index1] == null) {
                table[index1] = newEntry;
                isHashed[index1] = true;
                idOccupied[index1] = true;
            } else if (table[index2] == null) {
                table[index2] = newEntry;
                isHashed[index2] = true;
                idOccupied[index2] = true;
            } else {
                table[index3] = newEntry;
                isHashed[index3] = true;
                idOccupied[index3] = true;
            }
            auxiliary.add(new AuxiliaryEntry<>(key, index1, index2, index3));
            size++;
            return null;
        } else {
            return handleConflict(key, value);
        }
    }

private void handleConflict(K key, V value) {
    int index1 = hash1(key);
    int index2 = hash2(key);
    int index3 = hash3(key);

    // Try to relocate existing entries
    if (!relocate(index1) && !relocate(index2) && !relocate(index3)) {
        System.out.println("Rehash needed: no available slots found through relocation");
        // Rehash or resize logic could be implemented here
    }
}

// Attempts to relocate the item at the specified index
private boolean relocate(int index) {
    AuxiliaryEntry<K, V> entry = findAuxiliaryEntryByIndex(index);
    if (entry != null) {
        // Try the other two indices for this entry
        int newIndex1 = entry.h1_valid && entry.h1_item != index ? entry.h1_item : -1;
        int newIndex2 = entry.h2_valid && entry.h2_item != index ? entry.h2_item : -1;
        int newIndex3 = entry.h3_valid && entry.h3_item != index ? entry.h3_item : -1;

        // Check if new indices are available
        if (newIndex1 != -1 && !idOccupied[newIndex1]) {
            moveEntry(index, newIndex1);
            return true;
        }
        if (newIndex2 != -1 && !idOccupied[newIndex2]) {
            moveEntry(index, newIndex2);
            return true;
        }
        if (newIndex3 != -1 && !idOccupied[newIndex3]) {
            moveEntry(index, newIndex3);
            return true;
        }
    }
    return false;
}

// Move entry from one index to another
private void moveEntry(int fromIndex, int toIndex) {
    table[toIndex] = table[fromIndex];
    table[fromIndex] = null;
    idOccupied[fromIndex] = false;
    idOccupied[toIndex] = true;
}

// Find auxiliary entry by table index
private AuxiliaryEntry<K, V> findAuxiliaryEntryByIndex(int index) {
    for (AuxiliaryEntry<K, V> entry : auxiliary) {
        if (entry.h1_item == index || entry.h2_item == index || entry.h3_item == index) {
            return entry;
        }
    }
    return null;
}


    @Override
    public V get(Object key) {
        int index1 = hash1(key);
        int index2 = hash2(key);
        int index3 = hash3(key);
        if (table[index1] != null && table[index1].key.equals(key)) {
            return table[index1].value;
        }
        if (table[index2] != null && table[index2].key.equals(key)) {
            return table[index2].value;
        }
        if (table[index3] != null && table[index3].key.equals(key)) {
            return table[index3].value;
        }
        return null;
    }

    @Override
    public Set<Map.Entry<K, V>> entrySet() {
        throw new UnsupportedOperationException();
    }

    private static class Entry<K, V> implements Map.Entry<K, V> {
        final K key;
        V value;

        Entry(K key, V value) {
            this.key = key;
            this.value = value;
        }

        @Override
        public K getKey() {
            return key;
        }

        @Override
        public V getValue() {
            return value;
        }

        @Override
        public V setValue(V value) {
            V oldValue = this.value;
            this.value = value;
            return oldValue;
        }
    }

    private class AuxiliaryEntry<K, V> {
        public K item;
        public int index1, index2, index3;

        AuxiliaryEntry(K item, int index1, int index2, int index3) {
            this.item = item;
            this.index1 = index1;
            this.index2 = index2;
            this.index3 = index3;
        }
    }
}
