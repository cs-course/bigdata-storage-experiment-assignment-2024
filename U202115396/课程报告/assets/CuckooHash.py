import random
import time
import sys
import mmh3
import heapq
from collections import defaultdict,deque

class Bucket:
    def __init__(self, key=None, value=None):
        self.key = key
        self.value = value

class Graph:
    def __init__(self, max_id):
        self.max_id = max_id
        self.graph = defaultdict(set)
        self.free_list = set(range(max_id + 1))

    def add_free_node(self, id):
        self.free_list.add(id)

    def remove_free_node(self, id):
        self.free_list.discard(id)

    def add_edge(self, id_x, id_y):
        self.graph[id_x].add(id_y)
        self.graph[id_y].add(id_x)

    def shortest_path_from_src(self, src_node_id):
        visited = set()
        queue = deque([(src_node_id, [src_node_id])])
        while queue:
            node, path = queue.popleft()
            if node in self.free_list:
                return path
            for neighbor in self.graph[node]:
                if neighbor not in visited:
                    visited.add(neighbor)
                    queue.append((neighbor, path + [neighbor]))
        return None  # No path found

    def reconstruct(self):
        # Reconstruct the graph
        self.graph = defaultdict(set)
        self.free_list = set(range(self.max_id + 1))

class CuckooHash:
    def __init__(self, capacity, entry_cnt, max_depth):
        self.capacity = capacity
        self.entry_cnt = entry_cnt
        self.max_depth = max_depth
        self.table1 = [[Bucket() for _ in range(entry_cnt)] for _ in range(capacity//2)]
        self.table2 = [[Bucket() for _ in range(entry_cnt)] for _ in range(capacity//2)]
        self.graph = Graph(capacity *2)
        self.seed1 = random.randint(0, 2**12 - 1)
        self.seed2 = random.randint(0, 2**12 - 1)
        self.total_insert_time = 0
        self.total_insert_count = 0
        self.total_kick_count = 0

        for i in range(capacity):
            self.graph.add_free_node(i)

    def hash1(self, key):
        return mmh3.hash(str(key), self.seed1) % self.capacity //2

    def hash2(self, key):
        return mmh3.hash(str(key), self.seed2) % self.capacity//2

    def insert(self, key, value):
        start_time = time.time()
        # print("insert::  key: ", key, " value: ", value)
        for table, hash_func, other_table, other_hash_func in ((self.table1, self.hash1, self.table2, self.hash2), (self.table2, self.hash2, self.table1, self.hash1)):
            if self.lookup(key, table, hash_func)[0]:
                # 如果key已经存在，则直接返回true
                end_time = time.time()
                self.total_insert_time += end_time - start_time
                self.total_insert_count += 1
                return True
            index = hash_func(key)
            for i in range(self.entry_cnt):
                # print("index: ", index, " i: ", i, " table[index][i]: ", table[index][i])
                if table[index][i] is None or table[index][i].key is None:
                    table[index][i] = Bucket(key, value)
                    end_time = time.time()
                    # 添加一条驱逐路径的边
                    self.graph.add_edge(index, other_hash_func(key))
                    self.total_insert_time += end_time - start_time
                    self.total_insert_count += 1
                    return True
        # 如果插入失败，进行踢出操作
        for table, hash_func, other_table, other_hash_func in ((self.table1, self.hash1, self.table2, self.hash2), (self.table2, self.hash2, self.table1, self.hash1)):
                index = hash_func(key)
                if index in self.graph.free_list:
                    # 当一个哈希桶的所有entry都被占用时，将该哈希桶从free_list中移除
                    self.graph.remove_free_node(index)
                for i in range(self.entry_cnt):
                    old_key, old_value = table[index][i].key, table[index][i].value
                    # 将当前的key-value插入到哈希桶中
                    table[index][i] = Bucket(key, value)
                    # 添加一条驱逐路径的边
                    self.graph.add_edge(index, other_hash_func(old_key))
                    print(f"old_key: {old_key}, old_value: {old_value}")

                    if self.kick(old_key, old_value, hash_func,table,other_table, other_hash_func,1):
                            return True
                    else :
                        # 如果踢出失败，将原来的key-value重新插入到哈希桶中
                        table[index][i] = Bucket(old_key, old_value)
                        continue

        # 如果所有的踢出操作都失败，进行重构操作
        self.reconstruction()
        if self.insert(key, value):
            end_time = time.time()
            self.total_insert_time += end_time - start_time
            self.total_insert_count += 1
            return True
        else:
            end_time = time.time()
            self.total_insert_time += end_time - start_time
            self.total_insert_count += 1
            return False

    # def kick(self, kick_key, kick_value, hash_func, table, other_hash_func,depth):
    #         if(depth > self.max_depth):
    #             return False
    #         self.total_kick_count += 1
    #         kick_pos = other_hash_func(kick_key)
    #         # 如果踢出的kick_key在另一个哈希桶中的entry中有空位，则直接插入
    #         for i in range(self.entry_cnt):
    #             if table[kick_pos][i] is None:
    #                 table[kick_pos][i] = Bucket(kick_key, kick_value)
    #                 # self.graph.remove_free_node(kick_pos)
    #                 return True
    #         # 如果踢出的kick_key在另一个哈希桶中的entry都被占用，则进行踢出操作
    #         for i in range(self.entry_cnt):
    #             old_key, old_value = table[kick_pos][i].key, table[kick_pos][i].value
    #             table[kick_pos][i] = Bucket(kick_key, kick_value)
    #             # self.graph.add_edge(kick_pos, hash_func(old_key))
    #             # 递归实现kick操作，直到找到一个空位插入
    #             # 递归的时候要注意哈希函数的选择，如果当前哈希函数是hash1，则递归时要选择hash2，反之亦然
    #             if self.kick(old_key, old_value, other_hash_func, table, hash_func,depth+1):
    #                 return True
    #             else:
    #                 # 如果踢出失败，将原来的key-value重新插入到哈希桶中
    #                 table[kick_pos][i] = Bucket(old_key, old_value)
    #                 continue

    def kick(self, kick_key, kick_value, hash_func, table, other_table, other_hash_func, depth):
        if depth > self.max_depth:
            return False
        self.total_kick_count += 1
        kick_pos = other_hash_func(kick_key) % (self.capacity // 2)  # Ensure the index is within the correct range
        for i in range(self.entry_cnt):
            if other_table[kick_pos][i] is None:
                other_table[kick_pos][i] = Bucket(kick_key, kick_value)
                self.graph.add_edge(kick_pos, hash_func(kick_key) % (self.capacity // 2))  # Add edge between the two hash positions
                return True
            elif other_table[kick_pos][i].key == kick_key:
                return True
            else:
                old_key, old_value = other_table[kick_pos][i].key, other_table[kick_pos][i].value
                other_table[kick_pos][i] = Bucket(kick_key, kick_value)
                # Recursively kick the old key, using the other hash function
                if self.kick(old_key, old_value, other_hash_func, other_table, table, hash_func, depth+1):
                    return True
                else:
                    # If the kick fails, put the old key-value back
                    other_table[kick_pos][i] = Bucket(old_key, old_value)
        return False

    def lookup(self, key,table,hash_func):
        """
        用来查询key在当前table和hash中是否存在,如果存在同时返回key所在的位置
        """

        index = hash_func(key)
        for j in range(self.entry_cnt):
            if table[index][j] is not None and table[index][j].key == key:
                    return True,j
        return False,None

    def copy_to_buffer(self):
        # 将哈希表中的所有key-value对拷贝到缓冲区中
        self.buffer = []
        for table in (self.table1, self.table2):
            for bucket_list in table:
                for bucket in bucket_list:
                    if bucket is not None:
                        self.buffer.append((bucket.key, bucket.value))

    def reconstruction(self):
        # 重构哈希表同时将缓冲区中的key-value对重新插入到哈希表中
        self.copy_to_buffer()
        self.table1 = [[Bucket() for _ in range(self.entry_cnt)] for _ in range(self.capacity//2)]
        self.table2 = [[Bucket() for _ in range(self.entry_cnt)] for _ in range(self.capacity//2)]
        self.seed1 = (self.seed1*2)%2**32
        self.seed2 = (self.seed2*2)%2**32
        # 重构图
        self.graph.reconstruct()
        for key, value in self.buffer:
            if not self.insert(key, value):
                return False
        return True
    
    def get_value(self, key):
        # 用来查询key对应的value
        for i, (table, hash_func) in enumerate([(self.table1, self.hash1), (self.table2, self.hash2)]):
            index = hash_func(key)
            for j in range(self.entry_cnt):
                if table[index][j] is not None and table[index][j].key == key:
                    return table[index][j].value
        return None
    
    def print_info(self):
            occupancy = len([1 for table in (self.table1, self.table2) for bucket_list in table for bucket in bucket_list if bucket.key is not None]) / (self.capacity * self.entry_cnt ) * 100
            average_kick_count = self.total_kick_count / self.total_insert_count if self.total_insert_count > 0 else 0
            average_insert_time = self.total_insert_time / self.total_insert_count if self.total_insert_count > 0 else 0
            print(f"Cuckoo capacity: {self.capacity}")
            print(f"Entry count per bucket: {self.entry_cnt}")
            print(f"Max depth: {self.max_depth}")
            print(f"Occupancy: {occupancy:.2f}%")
            print(f"Average kick count: {average_kick_count:.2f}")
            print(f"Average insert time: {average_insert_time:.6f} seconds")

def init_test_array(array, size):
    for i in range(1, size):
        array[i] = random.randint(0, int(sys.argv[1])*10)

def check(test_array, cuckoo_hash_instance, test_size):
    for i in range(1, test_size):
        hash_value = cuckoo_hash_instance.get_value(i)
        if hash_value is None or hash_value != test_array[i]:
            print(f"value of {i} should be {test_array[i]}, but be {hash_value}")
            return False
    return True

def main():
    if len(sys.argv) != 5:
        print("Usage:\tcapacity\tentry_cnt\tmax_depth\ttest_size")
        sys.exit(0)
    test_capacity = int(sys.argv[1])
    test_entry_cnt = int(sys.argv[2])
    test_max_depth = int(sys.argv[3])
    test_size = int(sys.argv[4])

    mycuckoo = CuckooHash(test_capacity, test_entry_cnt, test_max_depth)
    test_array = [0] * test_size
    init_test_array(test_array, test_size)

    print("insert begin\n")
    start_time = time.time()
    for i in range(1, test_size):
        # print("i: ", i, " test_array[i]: ", test_array[i])
        if not mycuckoo.insert(i, test_array[i]):
            if mycuckoo.graph.shortest_path_from_src(mycuckoo.hash1(i)) is None and \
               mycuckoo.graph.shortest_path_from_src(mycuckoo.hash2(i)) is None:
                print(f"insert {i} error, no available path found, reconstruction needed")
                mycuckoo.reconstruction()
                mycuckoo.insert(i, test_array[i])
            # print(f"insert {i, test_array[i]} error, has been reconstruction\n")
            # # Implement reconstruction logic here if needed
            # mycuckoo.reconstruction()
            # mycuckoo.insert(i, test_array[i])
    print("insert done\n")
    end_time = time.time()
    print(f"time:\t{end_time - start_time} ms\n")
    print(f"time per item:\t{(end_time - start_time) / test_size}\n")

    if not check(test_array, mycuckoo, test_size):
        sys.exit(0)
    print("insert correct\n")
    mycuckoo.print_info()

if __name__ == "__main__":
    main()