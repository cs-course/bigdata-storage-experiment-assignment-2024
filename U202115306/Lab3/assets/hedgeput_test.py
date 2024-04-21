# -*- coding: utf-8 -*-
import os
import time
from threading import current_thread
import threading
from tqdm import tqdm
from concurrent.futures import ThreadPoolExecutor, wait, FIRST_COMPLETED,as_completed

import swiftclient

hedge_cnt = 1
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

    with ThreadPoolExecutor(max_workers=5) as executor:
        future=executor.submit(conn.get_object,bucket_name,obj_name)
        wait([future],timeout=hedge_time)
        if not future.done():
            hedge_requests = [executor.submit(conn.get_object,bucket_name,obj_name) for i in range(hedge_cnt)]
            hedge_requests.append(future)
            done, not_done = wait(hedge_requests, return_when=FIRST_COMPLETED)
            for future in not_done:
                future.cancel()
            resp_headers, obj_contents = done.pop().result()
        else:
            resp_headers, obj_contents = future.result()
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


switch = {'put': bench_put, 'get': bench_get, 'delete': bench_delete}
def run_test(test_type):
    print('-----' + test_type + '-----')
    latency = []
    test_start_time = time.time()
    with tqdm(desc="Accessing S3", total=num_samples) as pbar:
        with ThreadPoolExecutor(max_workers=num_client) as executor:
            futures = [executor.submit(switch[test_type],i) for i in range(num_samples)]
            for future in as_completed(futures):
                if future.exception():
                    os._exit(-1) 
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

    # tracefile_name = test_type + '_' + str(object_size) + '_' + str(num_clients) + '_' + str(num_samples) + '.csv'
    # with open(tracefile_name, "w+") as tracefile:
    #     tracefile.write("id,latency,start,end,client\n")
    #     tracefile.writelines([','.join(map(str, (i,) + latency[i])) + '\n' for i in range(len(latency))])
    if test_type=="get":
        tracefile_name = test_type + '_' + str(num_client) + tag + '.csv'
        with open(tracefile_name, "w+") as tracefile:
            tracefile.write("id,latency\n")
            for i in range(len(latency)):
                tracefile.write(str(i) + ',' + str(latency[i][0]) + '\n')
# para init
server_addr = 'http://127.0.0.1:12345/'
_user = 'chris:chris1234'
_key = 'testing'
bucket_name = 'testbucket'
object_name_prefix = 'testObj'
# object_sizes=[1,2,4,8,16,32,64,128,256,512,1024] # /KB
total_size= 4*128 # /KB  
object_size=4 # /KB
num_samples=128

# num_clients = list(range(1,20,2))
num_client = 1
result_file="result.out"
print('server_addr: ', server_addr)
print('bucket_name: ', bucket_name)
print('object_name_prefix: ', object_name_prefix)
print("total_size:",total_size,"KB")
print("object_size:",object_size,"KB")
print("num_samples:",num_samples)
print("num_client:",num_client)

if os.path.exists(result_file):
    os.remove(result_file)

# connect to swift
conn = swiftclient.Connection(authurl = server_addr + 'auth/v1.0', user = _user,key = _key)
hedge_times = [1, 0.015]
tags = ['old', 'new']
for i in range(2):
    tag = tags[i]
    hedge_time = hedge_times[i]
    print("=====TEST=====")
    print("num_client:", num_client)

    # 建立新容器
    conn.put_container(bucket_name)
    print('Test bucket %s created.' % bucket_name)

    # 初始化大小为object_size的文件
    local_file = "_test.bin"
    with open(local_file, 'wb') as file:
        file.write(b'\0' * object_size * 1024)
    print('Test file %s created.' % local_file)

    # 运行测试
    run_test('put')
    run_test('get')
    run_test('delete')

    # 删除本地文件
    for i in range(num_samples):
        obj_name = '%s%08d' % (object_name_prefix, i)
        os.remove(obj_name)
    print('Downloaded files deleted.')

    # 删除数据文件
    os.remove(local_file)
    print('Test file %s deleted.' % local_file)

    # 删除容器
    conn.delete_container(bucket_name)
    print('Test bucket %s deleted.' % bucket_name)

    time.sleep(10)

