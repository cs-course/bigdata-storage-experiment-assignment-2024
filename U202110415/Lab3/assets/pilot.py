import matplotlib.pyplot as plt

num_clients = []
write_total_throughput = []
read_total_throughput = []
write_average_latency=[]
read_average_latency=[]



if __name__=="__main__":
    with open("./result_remote.out", 'r') as f:
        lines = f.readlines()
        i = 0
        while i < len(lines):
            num_clients.append(float(lines[i].split()[-1]))
            write_total_throughput.append(float(lines[i + 1].split()[-2]))
            write_average_latency.append(float(lines[i + 2].split()[-2]))
            read_total_throughput.append(float(lines[i + 4].split()[-2]))
            read_average_latency.append(float(lines[i + 5].split()[-2]))
            i += 6


    plt.plot(num_clients, write_average_latency, marker='o', label="write average latency")
    plt.plot(num_clients, read_average_latency, marker='x', label="read average latency")
    plt.legend()

    plt.xlabel('num_client')
    plt.ylabel('average latency /s')
    plt.title('NoC WAL and RAL test REMOTE')
    plt.savefig('./num_clients_remote---average_latency.png')