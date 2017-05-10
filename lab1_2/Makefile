result: main.c lab1.l syntax.y
	bison -d syntax.y
	flex lab1.l
	gcc syntax.tab.c main.c -lfl -ly -o parser