%{
	#include<stdio.h>
	// #include "syntax.tab.h"
	int chars = 0;
	int words = 0;
	int lines = 0;
%}
%option yylineno
letter [a-zA-Z]
hex		0[xX][0-9A-Fa-f]{1,8}

INTC 	{hex}|(0[1-7][0-7]{0,9})|([1-9][0-9]*)|0
FLOATC   ([0-9]+\.[0-9]+)|([0-9]*\.[0-9]*[Ee][+-]?[0-9]{1,2})
STRINGC  \".*\"
BOOLC 	true|false

ID      [a-zA-Z_][0-9A-Za-z_]*	

INT 	int 
BOOL 	bool 
STRING  string 
VOID 	void 
CLASS 	class 

STATIC  static 
EXTENDS extends
FOR 	for  
RETURN 	return
IF 		if
ELSE 	else
WHILE 	whlie
BREAK 	break 
PRINT 	print 
THIS 	this 
NEW 	new 
NULL 	null

READINTEGER ReadInteger
READLINE 	ReadLine 
INSTANCEOF 	instanceof 


SEMI	;
COMMA	,
ASSIGNOP	=
LESS 	<
MORE 	>
LESSEQUAL <=
MOREEQUAL >=
EQUAL 	==
NOTEQUAL !=

PLUS	\+
MINUS	-
STAR	\*
DIV		\/
AND		&&
OR		\|\|
DOT		\.
NOT 	!
MOD 	%

LP		\(
RP 		\)
LB 		\[
RB 		\]
LC		\{
RC 		\}

SPACE 	[ \t\r]*
EOL 	\n

%%
{INT} 	{printf("INT %s\n", yytext);}
{BOOL}		{printf("BOOL %s\n", yytext);}
{STRING}		{printf("STRING %s\n", yytext);}
{VOID}		{printf("VOID %s\n", yytext);}
{CLASS}		{printf("CLASS %s\n", yytext);}
{STATIC} 	{printf("STATIC %s\n", yytext);}
{EXTENDS}		{printf("EXTENDS %s\n", yytext);}
{FOR}		{printf("FOR %s\n", yytext);}
{BREAK}		{printf("BREAK %s\n", yytext);}
{PRINT}		{printf("PRINT %s\n", yytext);}
{THIS}		{printf("THIS %s\n", yytext);}
{NEW}		{printf("NEW %s\n", yytext);}
{NULL}		{printf("NULL %s\n", yytext);}
{RETURN}	{printf("RETURN %s\n", yytext);}
{IF}		{printf("IF %s\n", yytext);}
{ELSE}		{printf("ELSE %s\n", yytext);}
{WHILE}		{printf("WHILE %s\n", yytext);}
{READINTEGER}		{printf("READINTEGER %s\n", yytext);}
{READLINE}		{printf("READLINE %s\n", yytext);}
{INSTANCEOF}		{printf("INSTANCEOF %s\n", yytext);}

{ID}	{printf("ID %s\n", yytext);}

{INTC}	{printf("INTC %s\n", yytext);}
{FLOATC} {printf("FLOATC %s\n", yytext);}
{STRINGC} {printf("STRINGC %s\n", yytext);}
{BOOLC} {printf("BOOLC %s\n", yytext);}

{SEMI} 	{printf("SEMI %s\n", yytext);}
{COMMA} {printf("COMMA %s\n", yytext);}

{LESSEQUAL}		{printf("LESSEQUAL %s\n", yytext);}
{MOREEQUAL}		{printf("MOREEQUAL %s\n", yytext);}
{EQUAL}		{printf("EQUAL %s\n", yytext);}
{NOTEQUAL}		{printf("NOTEQUAL %s\n", yytext);}
{ASSIGNOP}	{printf("ASSIGNOP %s\n", yytext);}
{LESS}		{printf("LESS %s\n", yytext);}
{MORE}		{printf("MORE %s\n", yytext);}

{PLUS}		{printf("PLUS %s\n", yytext);}
{MINUS}		{printf("MINUS %s\n", yytext);}
{STAR}		{printf("STAR %s\n", yytext);}
{DIV} 		{printf("DIV %s\n", yytext);}
{AND}		{printf("AND %s\n", yytext);}	
{OR}		{printf("OR %s\n", yytext);}
{DOT}		{printf("DOT %s\n", yytext);}
{NOT}		{printf("NOT %s\n", yytext);}
{MOD}		{printf("MOD %s\n", yytext);}

{LP}		{printf("LP %s\n", yytext);}
{RP}		{printf("RP %s\n", yytext);}
{LB}		{printf("LB %s\n", yytext);}
{RB}		{printf("RB %s\n", yytext);}
{LC}		{printf("LC %s\n", yytext);}
{RC} 		{printf("RC %s\n", yytext);}
{SPACE}		{}
{EOL} 		{}
.			{ chars++; printf("Error type A an Line %d: Mysterious characters \'%s\'\n", yylineno, yytext); }
%%
int main(int argc, char ** argv)
{
	if(argc > 1)
	{
		if(!(yyin = fopen(argv[1], "r")))
		{	perror(argv[1]);
			return 1;
		}
	}
	yylex();
	printf("%8d%8d%8d\n", lines, words, chars);
	return 0;
}
