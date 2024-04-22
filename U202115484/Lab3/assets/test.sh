./plot.py clear
for i in {1..8};do
    echo test $i
    echo $((2**$i))
    ./swift-bench-master/bin/swift-bench -A http://127.0.0.1:9090/auth/v1.0 -U chris:chris1234 -K testing -n 2000  -c $((2**$i)) -g 2000 -s 1000
    sleep 2
done