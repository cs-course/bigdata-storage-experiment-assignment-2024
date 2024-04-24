# -*- coding: utf-8 -*-
import os
import time
from concurrent.futures import ThreadPoolExecutor, as_completed
from threading import current_thread
from tqdm import tqdm

import swiftclient


def bench_put(i):
    obj_name = '%s%08d' % (object_name_prefix, i)  
    start = time.time()
    with open(local_file, 'rb') as f:
        conn.put_object(bucket_name, obj_name, f)  
    end = time.time()
    duration = end - start
    client = current_thread().name
    return (duration, start, end, client)


def bench_get(i):
    obj_name = '%s%08d' % (object_name_prefix, i)  
    start = time.time()
    resp_headers, obj_contents = conn.get_object(bucket_name, obj_name)
    with open(obj_name, 'wb') as f:
        f.write(obj_contents)
    end = time.time()
    duration = end - start
    client = current_thread().name
    return (duration, start, end, client)


def bench_delete(i):
    obj_name = '%s%08d' % (object_name_prefix, i) 
    start = time.time()
    conn.delete_object(bucket_name, obj_name)
    end = time.time()
    duration = end - start
    client = current_thread().name
    return (duration, start, end, client)


switch = {'write': bench_put, 'read': bench_get, 'delete': bench_delete}
def run_test(test_type):
    print('-----' + test_type + '-----')
    latency = []
    failed_requests = []
    test_start_time = time.time()
    with tqdm(desc="Accessing S3", total=num_samples) as pbar:
        with ThreadPoolExecutor(max_workers=num_client) as executor:
            futures = [executor.submit(switch[test_type],i) for i in range(num_samples)]
            for future in as_completed(futures):
                if future.exception():
                    failed_requests.append(future)
                else:
                    latency.append(future.result()) 
                pbar.update(1)
    test_end_time = time.time()
    test_duration = test_end_time - test_start_time # s
    test_transferred = len(latency) * object_size # KB
    test_total_throughput=test_transferred/test_duration # KB/s
    test_average_latency=sum([latency[i][0] for i in range(len(latency))]) / len(latency) # s
    print('total duration: ', test_duration , 's')
    print('average latency: ', test_average_latency,"s")
    print('total transferred: ', test_transferred , 'KB')
    print('total throughput: ', test_transferred / test_duration, 'KB/s')
    print('success rate: ', len(latency) / num_samples * 100, '%')
    if test_type=="write":
        with open(result_file,"a") as rf:
            rf.write("num_client: " + str(num_client) + " \n")
            rf.write("write_total_throughput: " + str(test_total_throughput) + " KB/s\n")
            rf.write("write_average_latency: " +str(test_average_latency) + " s\n")
    if test_type == "read":
        with open(result_file, "a") as rf:
            rf.write("num_client: " + str(num_client) + " \n")
            rf.write("read_total_throughput: " + str(test_total_throughput) + " KB/s\n")
            rf.write("read_average_latency: " + str(test_average_latency) + " s\n")


endpoint = 'http://127.0.0.1:12345/'
_user = 'test:tester'
_key = 'testing'
bucket_name = 'testbucket'
object_name_prefix = 'testObj'

total_size= 4*1024 # /KB    4MB
object_size=8 # /KB
num_samples=512
num_clients = list(range(1,10,1)) + list(range(10,200,10))
result_file="result.out"
print('endpoint: ', endpoint)
print('bucket_name: ', bucket_name)
print('object_name_prefix: ', object_name_prefix)
print("total_size:",total_size,"KB")
print("object_size:",object_size,"KB")
print("num_samples:",num_samples)


if os.path.exists(result_file):
    os.remove(result_file)


conn = swiftclient.Connection(authurl=endpoint + 'auth/v1.0',user=_user,key=_key)
print("I am starting")


for num_client in num_clients:
    print("=====TEST=====")
    print("num_client:",num_client)

    conn.put_container(bucket_name)
    print('Test bucket %s created.' % bucket_name)

    local_file = "_test.bin"
    test_bytes = [0xFF for i in range(object_size*1024)]
    with open(local_file, "wb") as lf:
        lf.write(bytearray(test_bytes))
    print('Test file %s created.' % local_file)

    run_test('write')
    run_test('read')
    run_test('delete')

    for i in range(num_samples):
        obj_name = '%s%08d' % (object_name_prefix, i)
        try:
            os.remove(obj_name)
        except:
            print("warning: deleting %s failed", obj_name)
    print('Downloaded files deleted.')

    os.remove(local_file)
    print('Test file %s deleted.' % local_file)


    conn.delete_container(bucket_name)
    print('Test bucket %s deleted.' % bucket_name)