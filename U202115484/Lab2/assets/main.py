from swiftclient import client
auth_url="http://127.0.0.1:9090/auth/v1.0"
auth_version="1"
user="chris:chris1234"
key="testing"

conn=client.Connection(authurl=auth_url,user=user,key=key,tenant_name="user")
auth=conn.get_auth()

def create(container,object=None,content=None):
    if object==None:
        try:
            conn.put_container(container=container)
        except client.ClientException as e:
            print(e)
            return
        else:
            print("Container",container,"Create Success")
        return
    try:
        conn.put_object(container=container,obj=object,contents=content)
    except client.ClientException as e:
        match e.http_status:
            case 404:print("Cant Find This Container")
    else:
        print("Object",object,"Create Success")
    return

def read(container=None,object=None):
    if container==None:
        res=conn.get_account()
        containers=res[1]
        for cont in containers:
            print(cont["name"])
        return
    if object==None:
        try:
            res=conn.get_container(container=container)
        except client.ClientException as e:
            print(e)
        else:
            objects=res[1]
            for obj in objects:
                print(obj["name"])
    else:
        try:
            res=conn.get_object(container=container,obj=object)
        except client.ClientException as e:
            print(e)
        else:
            print(res[1])
    return 

def update(container,object=None,content=None):
    if object==None:
        try:
            conn.get_container(container=container)
            res=conn.put_container(container=container)
        except client.ClientException as e:
            match e.http_status:
                case 404:print("Cant Find Resource")
        else:
            print("Update Container Success")
        return
    else:
        try:
            conn.get_object(container=container,obj=object)
            conn.put_object(container=container,obj=object,contents=content)
        except client.ClientException as e:
            match e.http_status:
                case 404:print("Cant Find Resource")
        else:
            print("Update Object Success")
        return
    
def delete(container,object=None):
    if object==None:
        try:
            conn.delete_container(container=container)
        except client.ClientException as e:
            # print(e)
            match e.http_status:
                case 404:print("Cant Find Resource")
                case 409:print("Still Have File")
        else:print("Delete Success")
    else:
        try:
            conn.delete_object(container=container,obj=object)
        except client.ClientException as e:
            match e.http_status:
                case 404:print("Cant Find Resource")
        else:print("Delete Success")
print("---------------Begin Create------------------")
create("lab2")
create("lab2","test","123456")
print("---------------Begin Read------------------")
read()
read("lab2","test")
print("---------------Begin Update------------------")
update("lab2","test","987654321")
read("lab2","test")
print("---------------Begin Delete------------------")
delete("lab2","test")
read("lab2")
delete("lab2")
read()