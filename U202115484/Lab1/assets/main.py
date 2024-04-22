from swiftclient import client
auth_url="http://127.0.0.1:9090/auth/v1.0"
auth_version="1"
user="chris:chris1234"
key="testing"

conn=client.Connection(authurl=auth_url,user=user,key=key,tenant_name="user")
auth=conn.get_auth()
resp_readers,containers=conn.get_account()
for container in containers:
    print("container:"+container['name'])
    header,objects=conn.get_container(container="test")
    for object in objects:
        print("     object:"+object['name'])