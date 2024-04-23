.PHONY: clean data cuckoohash run
build: data cuckoohash
data: 
	gcc -o data data.c
cuckoohash: 
	gcc -o cuckoohash cuckoohash.c
run:
	./data
	./cuckoohash
clean:
	rm -R $(wildcard *.o)