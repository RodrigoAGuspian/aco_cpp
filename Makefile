all: main.o
	g++ -o main main.o

main: main.cpp
	g++ -c -o main.o main.cpp

clean:
	rm -rf *.o main html latex