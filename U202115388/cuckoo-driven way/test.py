"""
    cuckoo hash table implementation
"""


class HashTableInfiniteLoopException(Exception):
    """ Exception raised when an infinite loop is detected in the hash table """
    pass


class CuckooHash:
    """ Cuckoo hash table implementation """
    def __init__(self, size):
        self.size = size
        self.hash_table1 = [None]*size
        self.hash_table2 = [None]*size
        self.loop_count = 0
        self.move_counts = {}  # 新增：用于跟踪每个元素的移动次数

    def hash1(self, key):
        """ hash function 1 """
        return key % self.size

    def hash2(self, key):
        """ hash function 2 """
        return (key // self.size) % self.size

    def insert(self, key):
        """ insert key into hash table """
        while True:
            # self.loop_count += 1
            print("key: ", key)
            print("table1: ", self.hash_table1)
            print("table2: ", self.hash_table2)
            print("move_counts: ", self.move_counts)
            self.move_counts[key] = self.move_counts.get(key, 0) + 1
            if self.hash_table1[self.hash1(key)] is None:
                self.hash_table1[self.hash1(key)] = key
                return
            key, self.hash_table1[self.hash1(
                key)] = self.hash_table1[self.hash1(key)], key
            # print("move_counts: ", self.move_counts)

            if self.move_counts[key] >= self.size:
                raise HashTableInfiniteLoopException("Infinite loop detected")

            self.move_counts[key] = self.move_counts.get(key, 0) + 1
            if self.hash_table2[self.hash2(key)] is None:
                self.hash_table2[self.hash2(key)] = key
                return
            key, self.hash_table2[self.hash2(
                key)] = self.hash_table2[self.hash2(key)], key

            # print("key: ", key)
            # print("table1: ", self.hash_table1)
            # print("table2: ", self.hash_table2)
            # print("move_counts: ", self.move_counts)

            if self.move_counts[key] >= self.size:
                raise HashTableInfiniteLoopException("Infinite loop detected")

    def search(self, key):
        """ search key in hash table """
        if self.hash_table1[self.hash1(key)] == key or self.hash_table2[self.hash2(key)] == key:
            return True
        return False


def main():
    """ main function """
    cuckoo = CuckooHash(3)  # 创建一个大小为3的Cuckoo哈希表

    # 插入一些元素
    cuckoo.insert(1)
    # cuckoo.insert(2)
    cuckoo.insert(3)
    # print(cuckoo.loop_count)
    # 尝试插入一个会导致循环次数达到上限的元素
    cuckoo.insert(4)  # 这将导致抛出异常
    cuckoo.insert(9)
    cuckoo.insert(12)
    # print(cuckoo.loop_count)

    print(cuckoo.search(4))
    print(cuckoo.search(3))
    print(cuckoo.search(2))
    print(cuckoo.search(1))
    print(cuckoo.hash_table1)
    print(cuckoo.hash_table2)


if __name__ == "__main__":
    main()
