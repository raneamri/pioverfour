build:
	g++ -O2 main.cpp -o main -I$(shell brew --prefix)/include

run:
	make build && ./main