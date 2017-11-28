CC= g++
flags= -std=c++11 -pedantic -Wall -Wextra
all: ims

ims: main.cpp
	$(CC) $(flags) main.cpp -o ims -lsimlib -lm
