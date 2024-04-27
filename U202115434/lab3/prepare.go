package main

import (
	"context"
	"fmt"

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

func main() {
	c := NewConnection()
	container_name := "tester"
	err := c.ContainerCreate(context.Background(), container_name, nil)
	if err != nil {
		panic(err)
	}
	containers, err := c.ContainerNames(context.Background(), nil)
	if err != nil {
		panic(err)
	}
	fmt.Println(containers)
}
