import random
from re import search
import mmh3
import time
import sys

sys.setrecursionlimit(8000)
class HashTable:
    def __init__(self,size,bucketnum=8,hashnum=2,loadfactor=0.7,stashsize=4,maxrehash=3) -> None:
        self.bucketnum=bucketnum
        self.size=size
        self.hasharray0=([[None for i in range(self.bucketnum)]for j in range(self.size//self.bucketnum//2)])
        self.hasharray1=([[None for i in range(self.bucketnum)]for j in range(self.size//self.bucketnum//2)])
        self.hashFuncNum=[0,0]
        self.hashFuncNum[0]=random.randint(0,10000)
        self.hashFuncNum[1]=random.randint(0,10000)
        while self.hashFuncNum[1]==self.hashFuncNum[0]:
            self.hashFuncNum[1]=random.randint(0,10000)
        self.objnums=0
        self.maxloop=8
        self.rehashcnt=0
        self.stash=[]
        self.stashMaxSize=stashsize
        self.maxrehash=maxrehash
        self.hashnum=hashnum
        self.loadfactor=loadfactor
    def __len__(self):
        return self.objnums
    def expand(self):
        oldsize=self.size//self.bucketnum//2
        self.size=int(self.size/self.loadfactor)
        self.hasharray0.extend([[None for i in range(self.bucketnum)]for j in range(self.size//self.bucketnum//2-oldsize)])
        self.hasharray1.extend([[None for i in range(self.bucketnum)]for j in range(self.size//self.bucketnum//2-oldsize)]) 
        self.rehashcnt=0
        # print("expand",self.size,self.objnums,self.max,self.hashFuncNum)  
    def rehash(self,newdata,havebuf=None):
        # print("rehash")
        buf=[]
        self.rehashcnt+=1
        if newdata==None:
            buf=havebuf
        else:
            buf.append(newdata)
        l=self.size//self.bucketnum//2
        if newdata!=None:
            buf+=self.stash
            for i in range(l):
                for j in range(self.bucketnum):
                    if self.hasharray0[i][j]!=None:
                        buf.append(self.hasharray0[i][j])
                        self.hasharray0[i][j]=None
                    if self.hasharray1[i][j]!=None:
                        buf.append(self.hasharray1[i][j])
                        self.hasharray1[i][j]=None
            if self.rehashcnt>self.maxrehash:
                self.expand()
        else:
            for i in range(l):
                self.hasharray0[i]=[None]*self.bucketnum
                self.hasharray1[i]=[None]*self.bucketnum
            if self.rehashcnt>self.maxrehash:
                self.expand()
            # self.expand(self.size)    
        self.hashFuncNum[0]=random.randint(0,10000)
        self.hashFuncNum[1]=random.randint(0,10000)
        while self.hashFuncNum[1]==self.hashFuncNum[0]:
            self.hashFuncNum[1]=random.randint(0,10000)
        flag=True
        self.objnums=0
        self.stash.clear()
        # print("qingling")
        for j in buf:
            if not self.insert(j,rehashing=True):
                flag=False
                break
        if not flag:
            # print("hashing hash",buf)
            self.rehash(None,havebuf=buf)
        return 
    
    def drive(self,newdata,array,loop,rehashing):
        if loop>self.maxloop:
            if len(self.stash)<self.stashMaxSize:
                self.stash.append(newdata)
                return True
            if not rehashing:
                self.rehash(newdata=newdata,havebuf=None)
            return False
        if array==0:
            index=mmh3.hash(str(newdata),self.hashFuncNum[1])%(self.size//self.bucketnum//2)
            for i in range(self.bucketnum):
                if self.hasharray1[index][i]==None:
                    self.hasharray1[index][i]=newdata
                    return True
            else:
                bnum=random.randint(0,self.bucketnum-1)
                data=self.hasharray1[index][bnum]
                self.hasharray1[index][bnum]=newdata
                return self.drive(newdata=data,array=1-array,loop=loop+1,rehashing=rehashing)
        else:
            index=mmh3.hash(str(newdata),self.hashFuncNum[0])%(self.size//self.bucketnum//2)
            for i in range(self.bucketnum):
                if self.hasharray0[index][i]==None:
                    self.hasharray0[index][i]=newdata
                    return True
            else:
                bnum=random.randint(0,self.bucketnum-1)
                data=self.hasharray0[index][bnum]
                self.hasharray0[index][bnum]=newdata
                return self.drive(newdata=data,array=1-array,loop=loop+1,rehashing=rehashing)
                              
    def insert(self,data,rehashing=False):
        if self.search(data=data):
            return True
        index0=mmh3.hash(str(data),self.hashFuncNum[0])%(self.size//self.bucketnum//2)
        index1=mmh3.hash(str(data),self.hashFuncNum[1])%(self.size//self.bucketnum//2)
        if random.randint(0,1)==0:   
            for i in range(self.bucketnum):
                if self.hasharray0[index0][i]==None:
                    self.hasharray0[index0][i]=data    
                    self.objnums+=1
                    return True
            for i in range(self.bucketnum):
                if self.hasharray1[index1][i]==None:
                    self.hasharray1[index1][i]=data     
                    self.objnums+=1
                    return True
        else:
            for i in range(self.bucketnum):
                if self.hasharray1[index1][i]==None:
                    self.hasharray1[index1][i]=data    
                    self.objnums+=1
                    return True
            for i in range(self.bucketnum):
                if self.hasharray0[index0][i]==None:
                    self.hasharray0[index0][i]=data    
                    self.objnums+=1
                    return True
        bnum=random.randint(0,self.bucketnum-1)
        if random.randint(0,1)==0:
            newdata=self.hasharray0[index0][bnum]
            self.hasharray0[index0][bnum]=data
            self.objnums+=1
            return self.drive(newdata=newdata,array=0,loop=0,rehashing=rehashing)
        else:
            newdata=self.hasharray1[index1][bnum]
            self.hasharray1[index1][bnum]=data
            self.objnums+=1
            return self.drive(newdata=newdata,array=1,loop=0,rehashing=rehashing)
    def delete(self,data):
        index0=mmh3.hash(str(data),seed=self.hashFuncNum[0])%(self.size//self.bucketnum//2)
        index1=mmh3.hash(str(data),seed=self.hashFuncNum[1])%(self.size//self.bucketnum//2)
        for i in range(self.bucketnum):
            if data==self.hasharray0[index0][i]:
                self.hasharray0[index0][i]=None
                self.objnums-=1
                return True
        for i in range(self.bucketnum):
            if data==self.hasharray1[index1][i]:
                self.hasharray1[index1][i]=None
                self.objnums-=1
                return True
        return False
    def search(self,data):
        index0=mmh3.hash(str(data),seed=self.hashFuncNum[0])%(self.size//self.bucketnum//2)
        index1=mmh3.hash(str(data),seed=self.hashFuncNum[1])%(self.size//self.bucketnum//2)
        if data in self.hasharray0[index0]:
            return True
        if data in self.hasharray1[index1]:
            return True  
        if data in self.stash:
            return True   
        return False
            