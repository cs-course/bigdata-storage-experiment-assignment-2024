import random
from time import time_ns


# 定义一个计时装饰器
def timer(func):
    def wrapper(*args, **kwargs):
        start_time = time_ns()
        result = func(*args, **kwargs)
        end_time = time_ns()
        print(f"HashTable大小:{args[0]} 执行时间: {(end_time - start_time) / 1e6:.6f} 毫秒")
        return result

    return wrapper


class CuckooHashing:
    def __init__(self, size):
        self.size = size
        self.table1 = [None] * size
        self.table2 = [None] * size
        self.max_try = 32

    def hash_func1(self, key):
        return hash(key) % self.size

    def hash_func2(self, key):
        return (hash(key) // self.size) % self.size

    def insert(self, key):
        i1 = self.hash_func1(key)
        i2 = self.hash_func2(key)

        for i in range(self.max_try):
            if self.table1[i1] is None:
                self.table1[i1] = key
                return True
            elif self.table2[i2] is None:
                self.table2[i2] = key
                return True
            else:
                # Randomly choose which table to kick out key from
                table_index = random.choice([1, 2])
                if table_index == 1:
                    evict_key = self.table1[i1]
                    self.table1[i1] = key
                    key = evict_key
                    i1 = self.hash_func1(key)
                else:
                    evict_key = self.table2[i2]
                    self.table2[i2] = key
                    key = evict_key
                    i2 = self.hash_func2(key)

        # If reached maximum tries, rehash and try again
        self.rehash()
        return self.insert(key)

    def search(self, key):
        i1 = self.hash_func1(key)
        i2 = self.hash_func2(key)

        if self.table1[i1] == key or self.table2[i2] == key:
            return True

        alt_i1 = self.hash_func2(self.table1[i1]) if self.table1[i1] is not None else None
        alt_i2 = self.hash_func1(self.table2[i2]) if self.table2[i2] is not None else None

        if alt_i1 is not None and self.table2[alt_i1] == key:
            return True
        elif alt_i2 is not None and self.table1[alt_i2] == key:
            return True

        return False

    def rehash(self):
        # Double the size of tables and rehash all keys
        self.size *= 2
        new_table1 = [None] * self.size
        new_table2 = [None] * self.size

        for key in self.table1:
            if key is not None:
                index = self.hash_func1(key)
                new_table1[index] = key

        for key in self.table2:
            if key is not None:
                index = self.hash_func2(key)
                new_table2[index] = key

        self.table1 = new_table1
        self.table2 = new_table2


# 创建布谷鸟哈希表实例
hash_table = CuckooHashing(1000)
hash_table1 = CuckooHashing(10000)
hash_table2 = CuckooHashing(50000)
hash_table3 = CuckooHashing(100000)
# 生成一组随机键值
keys = list(range(50000))


# 使用装饰器测试函数
@timer
def test_insert_search(size, hash_table, keys):
    for key in keys:
        hash_table.insert(key)
    for key in keys:
        hash_table.search(key)


# 执行测试
test_insert_search(1000, hash_table, keys)
test_insert_search(10000, hash_table1, keys)
test_insert_search(50000, hash_table2, keys)
test_insert_search(100000, hash_table3, keys)
