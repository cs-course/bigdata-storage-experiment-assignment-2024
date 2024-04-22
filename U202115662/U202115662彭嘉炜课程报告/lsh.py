import numpy as np
import random


class MultiProbeLSH:
    def __init__(self, num_tables, num_hashes, dimensionality):
        """
        初始化Multi-Probe LSH对象

        参数:
        - num_tables: 哈希表的数量
        - num_hashes: 每个哈希函数的数量
        - dimensionality: 数据点的维度
        """
        self.num_tables = num_tables
        self.num_hashes = num_hashes
        self.dimensionality = dimensionality
        self.tables = [{} for _ in range(num_tables)]  # 初始化哈希表列表
        self.hash_functions = [self._generate_hash_function() for _ in range(num_tables)]  # 初始化哈希函数列表

    def _generate_hash_function(self):
        """
        生成哈希函数

        返回值:
        - 哈希函数
        """
        random_vector = np.random.randn(self.dimensionality)

        def hash_function(point):
            return int(np.dot(point, random_vector) > 0)

        return hash_function

    def _compute_hash_values(self, point):
        """
        计算数据点的哈希值

        参数:
        - point: 数据点

        返回值:
        - 哈希值列表
        """
        return [hash_fn(point) for hash_fn in self.hash_functions]

    def insert(self, data):
        """
        将数据插入哈希表

        参数:
        - data: 待插入的数据点列表
        """
        for point_index, point in enumerate(data):
            hash_values = self._compute_hash_values(point)  # 计算数据点的哈希值
            for table_index, hash_val in enumerate(hash_values):
                if hash_val not in self.tables[table_index]:
                    self.tables[table_index][hash_val] = []  # 初始化哈希表桶
                self.tables[table_index][hash_val].append(point_index)  # 将数据点索引存储在对应的桶中

    def _estimate_success_probabilities(self):
        """
        估计每个桶的成功概率

        返回值:
        - 每个哈希表中每个桶的成功概率列表
        """
        success_probabilities = []
        for table in self.tables:
            table_success_prob = {}
            for hash_val, bucket in table.items():
                num_objects = len(bucket)
                success_prob = 1 - (1 - 1 / self.num_tables) ** num_objects
                table_success_prob[hash_val] = success_prob
            success_probabilities.append(table_success_prob)
        return success_probabilities

    def evaluate_and_rank(self, query_point, neighbors, data):
        """
        评估并排名候选集中的对象

        参数:
        - query_point: 查询点
        - neighbors: 候选集
        - data: 数据点列表

        返回值:
        - 按距离排名的最近邻列表
        """
        distances = []
        for neighbor_index in neighbors:
            neighbor = data[neighbor_index]
            distance = np.linalg.norm(query_point - neighbor)  # 计算与查询点的距离
            distances.append((neighbor_index, distance))
        distances.sort(key=lambda x: x[1])  # 按距离排序
        return distances

    def query(self, query_point, num_probes=5):
        """
        执行查询

        参数:
        - query_point: 查询点
        - num_probes: 多探测策略中的探测次数

        返回值:
        - 近似最近邻的索引列表
        """
        query_hash_values = self._compute_hash_values(query_point)  # 计算查询点的哈希值
        success_probabilities = self._estimate_success_probabilities()  # 估计成功概率
        neighbors = set()

        for table_index, hash_val in enumerate(query_hash_values):
            # 先探测当前桶
            if hash_val in self.tables[table_index]:
                neighbors.update(self.tables[table_index][hash_val])

            # 多探测策略
            for i in range(1, num_probes + 1):
                adjacent_index = (hash_val + i) % self.num_tables
                if adjacent_index in self.tables:
                    probe_success_prob = success_probabilities[adjacent_index].get(hash_val, 0)
                    if random.random() < probe_success_prob:  # 根据成功概率进行探测
                        neighbors.update(self.tables[adjacent_index][hash_val])

        return list(neighbors)


# 示例数据
num_data_points = 1000
dimensionality = 10
X = np.random.rand(num_data_points, dimensionality)  # 1000个10维的随机数据点
query = np.random.rand(dimensionality)  # 一个查询点

# 初始化 Multi-Probe LSH
num_tables = 10
num_hashes = 5
multi_probe_lsh = MultiProbeLSH(num_tables, num_hashes, dimensionality)

# 插入数据
multi_probe_lsh.insert(X)

# 查询
neighbors = multi_probe_lsh.query(query)

# 输出结果
print("查询点:", query)

# 评估与排名
ranked_neighbors = multi_probe_lsh.evaluate_and_rank(query, neighbors, X)

# 输出前十个近邻的数据点及其距离
print("前十个近邻的数据点及其距离:")
for i in range(10):
    neighbor_index = ranked_neighbors[i][0]  # 获取排名列表中第i个索引
    neighbor_distance = ranked_neighbors[i][1]  # 获取对应的距离
    neighbor_data_point = X[neighbor_index]  # 获取对应的数据点
    print("近邻索引:", neighbor_index)
    print("距离:", neighbor_distance)
    print("近邻数据点:", neighbor_data_point)
