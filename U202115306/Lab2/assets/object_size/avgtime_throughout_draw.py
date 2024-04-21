import matplotlib.pyplot as plt

object_sizes = []
put_total_throughput = []
get_total_throughput = []
put_average_latency=[]
get_average_latency=[]
variable = "ObjectSize"
var_unit = "KB"



if __name__=="__main__":
    with open("./result.out", 'r') as f:
        lines = f.readlines()
        i = 0
        while i < len(lines):
            object_sizes.append(float(lines[i].split()[-1]))
            put_total_throughput.append(float(lines[i + 1].split()[-2]))
            put_average_latency.append(float(lines[i + 2].split()[-2]))
            get_total_throughput.append(float(lines[i + 4].split()[-2]))
            get_average_latency.append(float(lines[i + 5].split()[-2]))
            i += 6

    plt.clf()
    metrics = "Throughout"
    unit = "KB/s"
    plt.plot(object_sizes, put_total_throughput,marker='o', label="put " + metrics)
    plt.plot(object_sizes, get_total_throughput, marker='x',label="get " + metrics)
    plt.legend()
    
    plt.xlabel(variable + ' / ' + var_unit)
    plt.ylabel(metrics + ' / ' + unit)
    plt.title(metrics + ' varying over ' + variable)
    plt.savefig('./' + variable + '--' + metrics + '.png')

    plt.clf()
    metrics = "AverageLatency"
    unit = "s"
    plt.plot(object_sizes, put_average_latency, marker='o', label="put " + metrics)
    plt.plot(object_sizes, get_average_latency, marker='x', label="get " + metrics)
    plt.legend()

    plt.xlabel(variable + ' / ' + var_unit)
    plt.ylabel(metrics + ' / ' + unit)
    plt.title(metrics + ' varying over ' + variable)
    plt.savefig('./' + variable + '--' + metrics + '.png')
