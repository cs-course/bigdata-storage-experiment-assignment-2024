# ʵ������

Lab1 �����洢

# ʵ�黷��

Minio Server: �������������CentOS 7.9
Minio Client:  Windows 10

# ʵ���¼

## ʵ��1-1��ʹ��docker�Minio Server
������������ϰ�װdocker��

``yum install docker-ce docker-ce-cli containerd.io``

���docker��װ�ɹ���

`systemctl start docker`

`docker run hello-world`

��dockerֱ������Minio���°澵��

`docker pull minio/minio`

���ò�����Minio Server�������Ƚ���/minio/config��dataĿ¼��
```bash
docker run -p 9000:9000 -p 9090:9090 \
>      --net=host \
>      --name minio \
>      -d --restart=always \
>      -e "MINIO_ACCESS_KEY=minioadmin" \
>      -e "MINIO_SECRET_KEY=minioadmin" \
>      -v /home/minio/data:/data \
>      -v /home/minio/config:/root/.minio \
>      minio/minio server \
>      /data --console-address ":9090" -address ":9000"

```

�����������Minio Server�ɹ���

<img src=".\figure\1 minioserver.png">

����һ����bucket��test-bucket

<img src=".\figure\2 test-bucket.png">

## ʵ��1-2������Minio Client
��Minio��������mc.exe������Minio Client��

`.\mc config host add myminio http://192.168.194.101:9000 minioadmin minioadmin`

�г���ǰServer�ϵ�Ͱ��

`.\mc ls myminio`

<img src=".\figure\3 minioclient.png">