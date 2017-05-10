result: main.c lab2.l syntax.y
	bison -d syntax.y
	flex lab2.l
	gcc syntax.tab.c main.c -lfl -ly -o parser