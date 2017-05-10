result: 
	reset
	bison -d syntax.y
	flex lab2.l
	gcc syntax.tab.c syntax.c genCode.c objCode.c main.c -lfl -ly -o parser
test:
	reset
	./parser test.cmm out.cmm
	./parser test2.cmm out2.cmm