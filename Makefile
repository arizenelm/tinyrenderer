.PHONY: all clean
	
all: main

clean:
	rm -rf main *.o
	
tgaimage.o: tgaimage/tgaimage.cpp
	g++ -std=c++17 -c -o tgaimage.o tgaimage/tgaimage.cpp

main.o: main.cpp
	g++ -std=c++17 -c -o main.o main.cpp

main: tgaimage.o main.o
	rm -f main
	g++ -o main main.o tgaimage.o
	rm main.o tgaimage.o