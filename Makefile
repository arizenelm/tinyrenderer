.PHONY: all clean
flags = -pedantic -std=c++17 -Wall -Wextra -O2 -c
sources = tgaimage.cpp main.cpp model.cpp
objects = $(subst .cpp,.o,$(sources))
	
all: main

clean:
	rm -rf main *.o
	
tgaimage.o: tgaimage/tgaimage.cpp
	g++ $(flags) -o tgaimage.o tgaimage/tgaimage.cpp

main.o: main.cpp
	g++ $(flags) -o main.o main.cpp

model.o: model.cpp
	g++ $(flags) -o model.o model.cpp

main: $(objects)
	rm -f main
	g++ -o main $(objects)
	rm $(objects)