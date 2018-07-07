all:
	g++ basic-search.cpp -o 15p-basic -g -Wall -std=c++11
	g++ astar-search.cpp -o 15p-astar -g -Wall -std=c++11
run:
	./15p-$(search)
