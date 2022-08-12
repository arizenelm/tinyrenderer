.PHONY: all clean
	
all: main

clean:
	rm -rf main *.o
	
tgaimage.o: tinyrenderer/tgaimage.cpp
	g++ -c -o tgaimage.o tinyrenderer/tgaimage.cpp

main.o: main.cpp
	g++ -c -o main.o main.cpp

main: tgaimage.o main.o
	rm -f main
	g++ -o main main.o tgaimage.o
	rm main.o tgaimage.o