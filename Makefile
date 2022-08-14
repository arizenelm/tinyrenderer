.PHONY: all clean
	
all: main

clean:
	rm -rf main *.o
	
tgaimage.o: tgaimage/tgaimage.cpp
	g++ -pedantic -std=c++17 -Wall -Wextra -O2 -c -o tgaimage.o tgaimage/tgaimage.cpp

main.o: main.cpp
	g++ -pedantic -std=c++17 -Wall -Wextra -O2 -c -o main.o main.cpp

model.o: model.cpp
	g++ -pedantic -std=c++17 -Wall -Wextra -O2 -c -o model.o model.cpp

main: tgaimage.o main.o model.o
	rm -f main
	g++ -o main main.o tgaimage.o model.o
	rm main.o tgaimage.o model.o