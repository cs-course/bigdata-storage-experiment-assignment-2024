# ʵ������

Lab2 ʵ����������

# ʵ�黷��

Minio Server: �������������CentOS 7.9
Minio Client:  Windows 10

# ʵ���¼

## ʵ��2��ִ��CRUD����
ͨ�����ͽ�������� Lab 1 �д��ϵͳ��ִ�з��ʳ־ô洢��4�����������

��Windows��ʹ��Minio Client��ɡ�

Create ������bucket��

`.\mc mb myminio/mybucket`

<img src=".\figure\create.png">

Update �ϴ��ļ���bucket��

`.\mc cp test.txt myminio/mybucket`

<img src=".\figure\update.png">

Read �ӷ���������ļ���

<img src=".\figure\download.png">

Delete ɾ��bucket�е��ļ���ɾ��bucket��

`.\mc rm myminio/mybucket/test.txt`

`.\mc rb myminio/mybucket`

<img src=".\figure\delete.png">
