package main

import (
	"context"
	"fmt"
	"os"

	"github.com/ncw/swift/v2"
)

func NewConnection() *swift.Connection {
	conn := &swift.Connection{
		UserName: "test:tester",
		ApiKey:   "testing",
		AuthUrl:  "http://127.0.0.1:18080/auth/v1.0",
		Tenant:   "tester",
	}
	err := conn.Authenticate(context.Background())
	if err != nil {
		panic(err)
	}
	return conn
}

func PutObject(conn *swift.Connection, ctx context.Context, filename string, containerName string) {
	file, err := os.Open(filename)
	if err != nil {
		panic(err)
	}
	defer file.Close()
	_, err = conn.ObjectPut(ctx, containerName, filename, file, false, "", "", nil)
	if err != nil {
		panic(err)
	}
}
func GetObject(conn *swift.Connection, ctx context.Context, filename string, containerName string, fileWriteName string) {
	file, err := os.OpenFile(fileWriteName, os.O_CREATE|os.O_WRONLY, 0644)
	if err != nil {
		panic(err)
	}
	defer file.Close()
	_, err = conn.ObjectGet(ctx, containerName, filename, file, false, nil)
	if err != nil {
		panic(err)
	}
}

func DeleteObject(conn *swift.Connection, ctx context.Context, filename string, contanier string) {
	err := conn.ObjectDelete(ctx, contanier, filename)
	if err != nil {
		panic(err)
	}
}
func main() {
	c := NewConnection()
	container_name := "newContanier"
	err := c.ContainerCreate(context.Background(), container_name, nil)
	if err != nil {
		panic(err)
	}
	containers, err := c.ContainerNames(context.Background(), nil)
	fmt.Println(containers)
	if err != nil {
		panic(err)
	}
	PutObject(c, context.Background(), "testfile", container_name)
	GetObject(c, context.Background(), "testfile", container_name, "newtestfile")
	DeleteObject(c, context.Background(), "testfile", container_name)
	err = c.ContainerDelete(context.Background(), container_name)
	if err != nil {
		panic(err)
	}
	containers, err = c.ContainerNames(context.Background(), nil)
	if err != nil {
		panic(err)
	}
	fmt.Println(containers)
}
