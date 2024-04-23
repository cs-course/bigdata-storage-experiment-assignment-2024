@rem -accessKey        Access Key
@rem -accessSecret     Secret Key
@rem -bucket=loadgen   Bucket for holding all test objects.
@rem -endpoint=http://127.0.0.1:9000 Endpoint URL of object storage service being tested.
@rem -numClients=(1/2/4/8/16/32/64/128/8default)     Simulate 8 clients running concurrently.
@rem -numSamples=256   Test with 256 objects.
@rem -objectNamePrefix=loadgen Name prefix of test objects.
@rem -objectSize=(4096/16384/65536/262144/524288/1048576/2097152/4194304/1048576default)     Size of test objects.
@rem -verbose          Print latency for every request.

s3bench.exe ^
    -accessKey=minioadmin ^
    -accessSecret=minioadmin ^
    -bucket=test-bucket ^
    -endpoint=http://192.168.1.44:9000 ^
    -numClients=128 ^
    -numSamples=256 ^
    -objectNamePrefix=loadgen ^
    -objectSize=1048576 ^
    > 1.txt
pause