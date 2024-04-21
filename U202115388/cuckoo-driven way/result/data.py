""" data process """
import csv
import os
import numpy as np
import matplotlib.pyplot as plt


def process_data(sizes):
    """ Process the data and generate the plots"""
    # 获取当前文件的路径
    current_path = os.path.dirname(os.path.abspath(__file__))

    # 创建用于存储结果的列表
    infinite_counts = []
    strategy_with_infinite = []
    strategy_without_infinite = []

    # 发生无限循环的概率
    infinite_probabilities = []

    # 分别存储两种策略的结果
    strategy_with_infinite_counts = []
    strategy_without_infinite_counts = []
    strategy_with_infinite_avg = []
    strategy_without_infinite_avg = []
    improvement_ratios = []

    for SIZE in sizes:
        # 设置txt文件相对于当前文件的路径
        relative_path = f'experiment_results_{SIZE}.txt'
        # 使用os.path.join来获取txt文件的完整路径
        filepath = os.path.join(current_path, relative_path)
        data = []
        with open(filepath, 'r', encoding='utf-8') as f:
            reader = csv.reader(f, delimiter=' ')
            for row in reader:
                data.append(row)

        # 分别存储两种策略的结果
        infinite_count = 0

        for row in data:
            if row[1] == 'Infinite':
                infinite_count += 1
            else:
                strategy_with_infinite.append(int(row[0]))
                strategy_without_infinite.append(int(row[1]))
        
        infinite_probabilities.append((infinite_count / len(data))*100)

        infinite_counts.append(infinite_count)

        for row in data:
            strategy_with_infinite_counts.append(int(row[0]))
            if row[1] != 'Infinite':
                strategy_without_infinite_counts.append(int(row[1]))

        # 计算平均值并添加到结果列表中
        strategy_with_infinite_avg.append(np.mean(strategy_with_infinite_counts))
        strategy_without_infinite_avg.append(np.mean(strategy_without_infinite_counts))

        # 计算提升率并存储
        if strategy_without_infinite:
            improvement_ratio = (sum(strategy_without_infinite) -
                                 sum(strategy_with_infinite)) / sum(strategy_without_infinite)
            improvement_ratios.append(improvement_ratio)

    # 1. 不对无限循环进行处理导致发生无限循环的概率(饼状图)
    plt.figure()
    plt.pie([sum(infinite_counts), len(strategy_with_infinite) +
            len(strategy_without_infinite)], labels=['Infinite', 'Finite'], autopct='%1.1f%%')
    plt.title('Infinite Loop Probability')

    # 2. SIZE大小对于发生无限循环的影响(曲线图)
    plt.figure()
    # infinite_probabilities = [count / (count + len(strategy_with_infinite) + len(
    #     strategy_without_infinite)) * 100 for count in infinite_counts]
    plt.plot(sizes, infinite_probabilities)
    plt.title('Infinite Loop Probability vs SIZE')
    plt.xlabel('SIZE')
    plt.ylabel('Infinite Loop Probability (%)')

    # 3. 当不发生无限循环时，不同策略对于loop count的影响
    # 创建一个新的图表
    plt.figure()
    # 设置柱子的宽度和位置
    bar_width = 0.35
    index = np.arange(len(sizes))

    # 绘制每个策略的柱子
    plt.bar(index, strategy_with_infinite_avg, bar_width,
            label='With Infinite Loop Strategy')
    plt.bar(index + bar_width, strategy_without_infinite_avg,
            bar_width, label='Without Infinite Loop Strategy')

    # 设置图表的标题和标签
    plt.title('Average Loop Count for Different Strategies')
    plt.xlabel('SIZE')
    plt.ylabel('Average Loop Count')
    plt.xticks(index + bar_width / 2, sizes)  # 设置x轴的标签位置
    plt.legend()

    # 4. SIZE大小对于提升率的影响(曲线图)
    plt.figure()
    plt.plot(sizes, improvement_ratios)
    plt.title('Improvement Ratio vs SIZE')
    plt.xlabel('SIZE')
    plt.ylabel('Improvement Ratio') 

    # 显示图表
    plt.show()


if __name__ == '__main__':
    process_data(range(100, 1100, 100))  # 将所有需要处理的SIZE放入一个列表中
