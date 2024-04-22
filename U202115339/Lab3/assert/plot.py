import matplotlib.pyplot as plt
import pandas as pd

def parse_bench_file(file_path):
    """解析 s3bench 输出文件，返回 Read times 和 Write times 的分位数数据，以及 Total Throughput 和 Total Duration。"""
    data = {
        'Read times': {'Max': None, '99th %ile': None, '90th %ile': None, '50th %ile': None, '25th %ile': None},
        'Write times': {'Max': None, '99th %ile': None, '90th %ile': None, '50th %ile': None, '25th %ile': None},
        'Write': {'Total Transferred': None, 'Total Throughput': None, 'Total Duration': None},
        'Read': {'Total Transferred': None, 'Total Throughput': None, 'Total Duration': None}
    }

    with open(file_path, 'r') as f:
        in_read_section = False
        in_write_section = False

        for line in f:
            line = line.strip()

            if line.startswith('Results Summary for Read Operation(s)'):
                in_read_section = True
                in_write_section = False
            elif line.startswith('Results Summary for Write Operation(s)'):
                in_read_section = False
                in_write_section = True
            elif in_read_section or in_write_section:
                if line.startswith('Read times') and in_read_section:
                    key, value = line.split(':')
                    key = key.strip().replace('Read times', '').strip()
                    data['Read times'][key] = float(value.strip().split(' ')[0])
                elif line.startswith('Write times') and in_write_section:
                    key, value = line.split(':')
                    key = key.strip().replace('Write times', '').strip()
                    data['Write times'][key] = float(value.strip().split(' ')[0])
                elif line.startswith('Total Transferred'):
                    key, value = line.split(':')
                    if in_read_section:
                        data['Read']['Total Transferred'] = float(value.strip().split(' ')[0])
                    elif in_write_section:
                        data['Write']['Total Transferred'] = float(value.strip().split(' ')[0])
                elif line.startswith('Total Throughput'):
                    key, value = line.split(':')
                    if in_read_section:
                        data['Read']['Total Throughput'] = float(value.strip().split(' ')[0])
                    elif in_write_section:
                        data['Write']['Total Throughput'] = float(value.strip().split(' ')[0])
                elif line.startswith('Total Duration'):
                    key, value = line.split(':')
                    if in_read_section:
                        data['Read']['Total Duration'] = float(value.strip().split(' ')[0])
                    elif in_write_section:
                        data['Write']['Total Duration'] = float(value.strip().split(' ')[0])

    return data

def plot_results(file_paths, output_file_path):
    import pandas as pd

def plot_results(file_paths, output_file_path):
    """从多个 s3bench 输出文件解析数据，并绘制在同一张图中。"""
    # 创建一个空的 DataFrame，用于存储数据
    df_list = []

    # 解析所有文件
    for file_path in file_paths:
        data = parse_bench_file(file_path)

        # 提取 object size
        object_size = data['Read']['Total Transferred']

        # 提取 Read times 和 Write times 的分位数
        read_times = [data['Read times']['Max'], data['Read times']['90th %ile'],
                      data['Read times']['50th %ile'], data['Read times']['25th %ile']]
        write_times = [data['Write times']['Max'], data['Write times']['90th %ile'],
                       data['Write times']['50th %ile'], data['Write times']['25th %ile']]

        # 提取 Total Throughput 和 Total Duration
        read_total_throughput = data['Read']['Total Throughput']
        write_total_throughput = data['Write']['Total Throughput']
        read_total_duration = data['Read']['Total Duration']
        write_total_duration = data['Write']['Total Duration']

        # 将数据添加到临时列表中
        df_list.append({
            'Object Size': object_size,
            'Read times Max': read_times[0],
            'Read times 90th %ile': read_times[1],
            'Read times 50th %ile': read_times[2],
            'Read times 25th %ile': read_times[3],
            'Write times Max': write_times[0],
            'Write times 90th %ile': write_times[1],
            'Write times 50th %ile': write_times[2],
            'Write times 25th %ile': write_times[3],
            'Read Total Throughput': read_total_throughput,
            'Write Total Throughput': write_total_throughput,
            'Read Total Duration': read_total_duration,
            'Write Total Duration': write_total_duration
        })

    # 将列表转换为 DataFrame
    df = pd.DataFrame(df_list)

    # 绘制 Read times 和 Write times 的分位数数据
    plt.figure()
    plt.plot(df['Object Size'], df['Read times Max'], label='Read times Max', marker='o')
    plt.plot(df['Object Size'], df['Read times 90th %ile'], label='Read times 90th %ile', marker='o')
    plt.plot(df['Object Size'], df['Read times 50th %ile'], label='Read times 50th %ile', marker='o')
    plt.plot(df['Object Size'], df['Read times 25th %ile'], label='Read times 25th %ile', marker='o')

    plt.plot(df['Object Size'], df['Write times Max'], label='Write times Max', marker='o')
    plt.plot(df['Object Size'], df['Write times 90th %ile'], label='Write times 90th %ile', marker='o')
    plt.plot(df['Object Size'], df['Write times 50th %ile'], label='Write times 50th %ile', marker='o')
    plt.plot(df['Object Size'], df['Write times 25th %ile'], label='Write times 25th %ile', marker='o')

    plt.xlabel('Object Size (MB)')
    plt.ylabel('Time (s)')
    plt.title('Read and Write times by Percentiles')
    plt.legend()

    # 保存 Read and Write times 的分位数数据图像
    plt.savefig(f'{output_file_path}_times_percentiles.png')
    plt.show()

    # 绘制 Total Throughput 和 Total Duration 的数据
    plt.figure()
    plt.plot(df['Object Size'], df['Read Total Throughput'], label='Read Total Throughput', marker='o')
    plt.plot(df['Object Size'], df['Write Total Throughput'], label='Write Total Throughput', marker='o')

    plt.xlabel('Object Size (MB)')
    plt.ylabel('Throughput (MB/s)')
    plt.title('Total Throughput by Object Size')
    plt.legend()

    # 保存 Total Throughput 数据图像
    plt.savefig(f'{output_file_path}_throughput.png')
    plt.show()

    plt.figure()
    plt.plot(df['Object Size'], df['Read Total Duration'], label='Read Total Duration', marker='o')
    plt.plot(df['Object Size'], df['Write Total Duration'], label='Write Total Duration', marker='o')

    plt.xlabel('Object Size (MB)')
    plt.ylabel('Duration (s)')
    plt.title('Total Duration by Object Size')
    plt.legend()

    # 保存 Total Duration 数据图像
    plt.savefig(f'{output_file_path}_duration.png')
    plt.show()



def main():
    # 请根据实际情况调整文件路径和输出文件路径
    file_paths = [
        'test_objectsize_1024.txt',
        'test_objectsize_2048.txt',
        'test_objectsize_4096.txt',
        'test_objectsize_8192.txt',
        'test_objectsize_16384.txt'
    ]  # 请替换为 s3bench 输出文件的路径列表
    output_file_path = 'output'  # 请替换为您想要保存的图像文件路径

    # 绘制结果图像
    plot_results(file_paths, output_file_path)

if __name__ == '__main__':
    main()