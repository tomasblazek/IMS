CC= g++
flags= -std=c++11 -pedantic -Wall -Wextra
all: ims	

ims: main.cpp
	$(CC) $(flags) main.cpp -o ims -lsimlib -lm

run: ims
	./ims -t 91 -p 76 -f experiment1.out
	./ims -t 274 -p 678 -f experiment2.out
	./ims -t 91 -p 76 -f experiment3.out -s 41
	./ims -t 274 -p 678 -f experiment4.out -s 17
	./ims -t 91 -p 46 -f experiment5.out



.PHONY: clear
clear:
	rm -f *.out
	rm -f ims
