build:
	gcc -g -o main main.c graph.c fscs.c input.c -lpthread
buildBase:
	gcc -g -o base base.c graph.c fscs.c input.c -lpthread
run:
	./main
runBase:
	./base