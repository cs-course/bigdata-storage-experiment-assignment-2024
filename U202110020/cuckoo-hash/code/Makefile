# Rule for helloworld
helloworld: main.o helloworld.o
	g++ -o helloworld main.o helloworld.o

main.o: main.cc include/helloworld.hh
	g++ -c main.cc -Iinclude -o main.o

# Rule for helloworld.o
helloworld.o: helloworld.cc include/helloworld.hh
	g++ -c helloworld.cc -Iinclude -o helloworld.o

# Rule for clean
clean:
	rm -f helloworld main.o helloworld.o