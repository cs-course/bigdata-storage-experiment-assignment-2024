#!/bin/bash

# 检查并安装 Go
if ! command -v go &> /dev/null
then
    echo "Go is not installed. Installing Go..."
    wget https://dl.google.com/go/go1.18.1.linux-amd64.tar.gz
    sudo tar -xvf go1.18.1.linux-amd64.tar.gz
    sudo mv go /usr/local
    export GOROOT=/usr/local/go
    export PATH=$PATH:/usr/local/go/bin
    echo "Go installed successfully."
fi

# 安装 s3bench
echo "Installing s3bench..."
go install github.com/igneous-systems/s3bench@latest
echo "s3bench installed."

# 配置 MinIO 参数
export S3_ENDPOINT="127.0.0.1:9000" 
export ACCESS_KEY="test"
export SECRET_KEY="test"
export BUCKET="mybucket"  # 测试桶名称

# 运行 s3bench 测试
echo "Running s3bench to test MinIO performance..."
$s3bench -accessKey $ACCESS_KEY -secretKey $SECRET_KEY -endpoint $S3_ENDPOINT -ssl false -numClients 10 -numSamples 1000 -objectNamePrefix benchtest -bucket $BUCKET
echo "s3bench testing completed."
