s=0,100,200,500
b=1,2,4,8
cuckoos=cuckoo,blocked_cuckoo,



#bin/cuckoo-hash -t 50000 -n 8 -l 10

#bin/cuckoo-hash -t 50000  -l 10 -c blocked_cuckoo -b 8

#bin/cuckoo-hash -t 50000  -l 10 -c stash_cuckoo -s 500

bin/cuckoo-hash -t 500000  -l 5 -c stash_blocked_cuckoo -s 500 -b 8