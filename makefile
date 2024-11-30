build:
	gcc -g -o main main.c graph.c fscs.c input.c -lpthread
run:
	./main