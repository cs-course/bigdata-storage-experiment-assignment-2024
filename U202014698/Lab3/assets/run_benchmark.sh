~/go/bin/s3bench \
    -accessKey=x \
    -accessSecret=x \
    -bucket=abucket \
    -endpoint=http://localhost:80 \
    -numClients=1000 -numSamples=100000 \
    -objectNamePrefix=loadgen -objectSize=1024 \
    > benchmark_result_1000_100000.txt