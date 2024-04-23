cd D:\Download
echo "Start objectSize test"
./s3bench -accessKey=test -accessSecret=testtest -bucket=big-data-storage -endpoint=http://localhost:9000 -numClients=2 -numSamples=10 -objectNamePrefix=loadgen -objectSize=512 | Out-File -Encoding utf8 ./output/objectSize512.txt
echo "objectSize512 test finished"
./s3bench -accessKey=test -accessSecret=testtest -bucket=big-data-storage -endpoint=http://localhost:9000 -numClients=2 -numSamples=10 -objectNamePrefix=loadgen -objectSize=1024 | Out-File -Encoding utf8 ./output/objectSize1024.txt
echo "objectSize1024 test finished"
./s3bench -accessKey=test -accessSecret=testtest -bucket=big-data-storage -endpoint=http://localhost:9000 -numClients=2 -numSamples=10 -objectNamePrefix=loadgen -objectSize=2048 | Out-File -Encoding utf8 ./output/objectSize2048.txt
echo "objectSize2048 test finished"
./s3bench -accessKey=test -accessSecret=testtest -bucket=big-data-storage -endpoint=http://localhost:9000 -numClients=2 -numSamples=10 -objectNamePrefix=loadgen -objectSize=4096 | Out-File -Encoding utf8 ./output/objectSize4096.txt
echo "objectSize4096 test finished"
./s3bench -accessKey=test -accessSecret=testtest -bucket=big-data-storage -endpoint=http://localhost:9000 -numClients=2 -numSamples=10 -objectNamePrefix=loadgen -objectSize=8192 | Out-File -Encoding utf8 ./output/objectSize8192.txt
echo "objectSize8192 test finished"
echo "objectSize test finished"

echo "Start numClients test"
./s3bench -accessKey=test -accessSecret=testtest -bucket=big-data-storage -endpoint=http://localhost:9000 -numClients=2 -numSamples=10 -objectNamePrefix=loadgen -objectSize=512 | Out-File -Encoding utf8 ./output/numClients2.txt
echo "numClients2 test finished"
./s3bench -accessKey=test -accessSecret=testtest -bucket=big-data-storage -endpoint=http://localhost:9000 -numClients=4 -numSamples=10 -objectNamePrefix=loadgen -objectSize=512 | Out-File -Encoding utf8 ./output/numClients4.txt
echo "numClients4 test finished"
./s3bench -accessKey=test -accessSecret=testtest -bucket=big-data-storage -endpoint=http://localhost:9000 -numClients=6 -numSamples=10 -objectNamePrefix=loadgen -objectSize=512 | Out-File -Encoding utf8 ./output/numClients6.txt
echo "numClients6 test finished"
./s3bench -accessKey=test -accessSecret=testtest -bucket=big-data-storage -endpoint=http://localhost:9000 -numClients=8 -numSamples=10 -objectNamePrefix=loadgen -objectSize=512 | Out-File -Encoding utf8 ./output/numClients8.txt
echo "numClients8 test finished"
./s3bench -accessKey=test -accessSecret=testtest -bucket=big-data-storage -endpoint=http://localhost:9000 -numClients=10 -numSamples=10 -objectNamePrefix=loadgen -objectSize=512 | Out-File -Encoding utf8 ./output/numClients10.txt
echo "numClients10 test finished"
echo "numclients test finished"

echo "Start numSamples test"
./s3bench -accessKey=test -accessSecret=testtest -bucket=big-data-storage -endpoint=http://localhost:9000 -numClients=2 -numSamples=10 -objectNamePrefix=loadgen -objectSize=512 | Out-File -Encoding utf8 ./output/numSamples10.txt
echo "numSamples10 test finished"
./s3bench -accessKey=test -accessSecret=testtest -bucket=big-data-storage -endpoint=http://localhost:9000 -numClients=2 -numSamples=20 -objectNamePrefix=loadgen -objectSize=512 | Out-File -Encoding utf8 ./output/numSamples20.txt
echo "numSamples20 test finished"
./s3bench -accessKey=test -accessSecret=testtest -bucket=big-data-storage -endpoint=http://localhost:9000 -numClients=2 -numSamples=30 -objectNamePrefix=loadgen -objectSize=512 | Out-File -Encoding utf8 ./output/numSamples30.txt
echo "numSamples30 test finished"
./s3bench -accessKey=test -accessSecret=testtest -bucket=big-data-storage -endpoint=http://localhost:9000 -numClients=2 -numSamples=40 -objectNamePrefix=loadgen -objectSize=512 | Out-File -Encoding utf8 ./output/numSamples40.txt
echo "numSamples40 test finished"
./s3bench -accessKey=test -accessSecret=testtest -bucket=big-data-storage -endpoint=http://localhost:9000 -numClients=2 -numSamples=50 -objectNamePrefix=loadgen -objectSize=512 | Out-File -Encoding utf8 ./output/numSamples50.txt
echo "numSamples50 test finished"