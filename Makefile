all: list.c query.c
	gcc -o shell shell.c -Wall

list.c : listar.c
	gcc -o listar listar.c -Wall

query.c : info.c
		gcc -o info info.c -Wall
