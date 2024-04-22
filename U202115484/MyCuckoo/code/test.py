
from better_cuckoo_hash import HashTable
import time
import random
testsize=1000000#2**20
#普通布谷鸟哈希
# ht=HashTable(size=testsize,bucketnum=1,hashnum=2,loadfactor=0.7,stashsize=0,maxrehash=3)
#带有8路相连的布谷鸟哈希
# ht=HashTable(size=testsize,bucketnum=8,hashnum=2,loadfactor=0.7,stashsize=0,maxrehash=3)
#带有额外缓冲的布谷鸟哈希
# ht=HashTable(size=testsize,bucketnum=1,hashnum=2,loadfactor=0.7,stashsize=4,maxrehash=0)
#同时带有8路相连和额外的布谷鸟哈希
ht=HashTable(size=testsize,bucketnum=8,hashnum=2,loadfactor=0.7,stashsize=4,maxrehash=0)
ran=[]
begin=time.time()
for i in range(testsize):
    t=random.randint(1,10000000)
    ran.append(t)
    ht.insert(t)     
end=time.time()
print("插入时间为",end-begin)
begin=time.time()
for i in range(testsize):
    if not ht.search(ran[i]):
        print("search ERROR")
        break 
end=time.time()
print("读取时间为",end-begin)
print("空间利用率为",ht.objnums/ht.size)
print("OK")