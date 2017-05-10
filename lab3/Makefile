result: main.c lab2.l syntax.y
	bison -d syntax.y
	flex lab2.l
	gcc syntax.tab.c syntax.c genCode.c main.c -lfl -ly -o parser