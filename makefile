all: a3search

a3search: a3search.o
	g++ a3search.o -o a3search

a3search.o: a3search.cpp
	g++ -std=c++11 -Wall -Werror -O2 -c -g a3search.cpp 
