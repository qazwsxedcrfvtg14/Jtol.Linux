main: main.cpp Jtol.h Jtol.o lodepng.o md5.o
	g++ -Wall -std=c++17 -O3 main.cpp Jtol.o lodepng.o md5.o -lpthread -lcurl -ldl -static-libstdc++ -static-libgcc -o main

Jtol.o: Jtol.cpp Jtol.h
	g++ -Wall -std=c++17 -O3 -c Jtol.cpp -o Jtol.o

lodepng.o: lodepng.cpp
	g++ -Wall -std=c++17 -O3 -c lodepng.cpp -o lodepng.o

md5.o: md5.cpp
	g++ -Wall -std=c++17 -O3 -c md5.cpp -o md5.o

