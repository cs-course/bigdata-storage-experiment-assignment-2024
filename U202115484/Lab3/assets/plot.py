#!/usr/bin/env python3
import sys
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import math
import os

def get_all_files(folder_path):
    file_list = []
    for root, dirs, files in os.walk(folder_path):
        for file in files:
            file_list.append(file)
    return file_list
path="data/"
# filename="PUTS_1000_20_10000.csv"
filename="GETS_2_10_100.csv"
if len(sys.argv)>1 and sys.argv[1]=="clear":
    for file in get_all_files(path):
        os.remove(path+file)
else:
    concurrency=[]
    maxlatency=[]
    avglatency=[]
    throughout=[]
    for i in range(1,int(sys.argv[1])+1):
        filename="PUTS_"+"1000_"+str(2**i)+"_2000.csv"
        with open(path+filename,"r")as file:
            concurrency.append(2**i)
            id=[]
            latency=[]
            lines=file.readlines()
            for line in lines[1:-1]:
                tline=line.split(',')
                id.append(int(tline[0]))
                latency.append(float(tline[3]))
            maxlatency.append(max(latency))
            avglatency.append(sum(latency)/len(latency))
            throughout.append(float(lines[-1]))
            print("Throughput is",lines[-1])
    plt.xticks(concurrency)
    plt.plot(concurrency,throughout)
    plt.xlabel("concurrency")
    plt.ylabel("throughout/byte/s")
    plt.title("PUT_Throughput")
    plt.savefig("../figure/PUT_Throughput"+".png")
    plt.close()
    plt.xticks(concurrency)
    plt.xlabel("concurrency")
    plt.ylabel("latency/ms")
    plt.plot(concurrency,maxlatency,'r',label='maxlatency')
    plt.plot(concurrency,avglatency,'b',label='avglatency')
    plt.legend(loc="upper left")
    plt.title("PUT_latency")
    plt.savefig("../figure/PUT_latency"+".png")
    plt.close()


    concurrency=[]
    maxlatency=[]
    avglatency=[]
    throughout=[]
    for i in range(1,int(sys.argv[1])+1):
        filename="GETS_"+"1000_"+str(2**i)+"_2000.csv"
        with open(path+filename,"r")as file:
            concurrency.append(2**i)
            id=[]
            latency=[]
            lines=file.readlines()
            for line in lines[1:-1]:
                tline=line.split(',')
                id.append(int(tline[0]))
                latency.append(float(tline[3]))
            maxlatency.append(max(latency))
            avglatency.append(sum(latency)/len(latency))
            throughout.append(float(lines[-1]))
            print("Throughput is",lines[-1])
    plt.xticks(concurrency)
    plt.plot(concurrency,throughout)
    plt.xlabel("concurrency")
    plt.ylabel("throughout/byte/s")
    plt.title("GET_Throughput")
    plt.savefig("../figure/GET_Throughput"+".png")
    plt.close()
    plt.xticks(concurrency)
    plt.xlabel("concurrency")
    plt.ylabel("latency/ms")
    plt.plot(concurrency,maxlatency,'r',label='maxlatency')
    plt.plot(concurrency,avglatency,'b',label='avglatency')
    plt.legend(loc="upper left")
    plt.title("GET_latency")
    plt.savefig("../figure/GET_latency"+".png")
