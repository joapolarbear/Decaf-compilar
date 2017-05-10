%{
	#include<stdio.h>
	#include<stdlib.h>
	#include<string.h>
	#include "syntax.tab.h"
	#include "main.h"
	/*#define YY_USER_ACTION 
		yylloc->first_line = yylloc->last_line = yylineno;\ 
		yylloc->first_column = yycolumn; \
		yylloc->last_column = yycolumn + yyleng - 1; \
		yycolumn += yyleng;*/
	extern int yyrol;
%}
%option yylineno
letter [a-zA-Z]
hex		0[xX][0-9A-Fa-f]{1,8}

INTC 	{hex}|(0[1-7][0-7]{0,9})|([1-9][0-9]*)|0
FLOATC   ([0-9]+\.[0-9]+)|([0-9]*\.[0-9]*[Ee][+-]?[0-9]{1,2})
STRINGC  \".*\"
BOOLC 	true|false
	

INT 	int 
BOOL 	bool 
FLOAT  	float
STRING  string 
VOID 	void 
CLASS 	class 

STATIC  static 
EXTENDS extends
FOR 	for  
RETURN 	return
IF 		if
ELSE 	else
WHILE 	while
BREAK 	break 
PRINT 	print 
THIS 	this 
NEW 	new 

READINTEGER ReadInteger
READLINE 	ReadLine 
INSTANCEOF 	instanceof 

ID      [a-zA-Z_][0-9A-Za-z_]*

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

SPACE 	[ ]*
TAB 	\t
RET 	\r
EOL 	\n

%%
{INT}		{yylval.a = newast("INT", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return INT;}
{FLOAT}		{yylval.a = newast("FLOAT", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return FLOAT;}
{BOOL} 		{yylval.a = newast("BOOL", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return BOOL;}
{STRING} 	{yylval.a = newast("STRING", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return STRING;}
{VOID} 		{yylval.a = newast("VOID", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return VOID;}
{CLASS} 	{yylval.a = newast("CLASS", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return CLASS;}
{STATIC} 	{yylval.a = newast("STATIC", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return STATIC;}
{EXTENDS} 	{yylval.a = newast("EXTENDS", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return EXTENDS;}
{FOR} 		{yylval.a = newast("FOR", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return FOR;}
{BREAK} 	{yylval.a = newast("BREAK", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return BREAK;}
{PRINT} 	{yylval.a = newast("PRINT", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return PRINT;}
{THIS} 		{yylval.a = newast("THIS", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return THIS;}
{NEW} 		{yylval.a = newast("NEW", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return NEW;}

{RETURN}	{yylval.a = newast("RETURN", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return RETURN;}
{IF}		{yylval.a = newast("IF", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return IF;}
{ELSE}		{yylval.a = newast("ELSE", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return ELSE;}
{WHILE}		{yylval.a = newast("WHILE", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return WHILE;}
{READINTEGER} 	{yylval.a = newast("READINTEGER", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return READINTEGER;}
{READLINE} 		{yylval.a = newast("READLINE", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return READLINE;}
{INSTANCEOF} 	{yylval.a = newast("INSTANCEOF", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return INSTANCEOF;}


	
{INTC}		{yylval.a = newast("INTC", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return INTC;}
{FLOATC} 	{yylval.a = newast("FLOATC", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return FLOATC;}
{BOOLC} 	{yylval.a = newast("BOOLC", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return BOOLC;}

{ID}		{yylval.a = newast("ID", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return ID;}

{SEMI} 		{yylval.a = newast("SEMI", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return SEMI;}
{COMMA} 	{yylval.a = newast("COMMA", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return COMMA;}

{LESSEQUAL}	{yylval.a = newast("LESSEQUAL", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return LESSEQUAL;}
{MOREEQUAL}	{yylval.a = newast("MOREEQUAL", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return MOREEQUAL;}
{EQUAL} 	{yylval.a = newast("EQUAL", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return EQUAL;}
{NOTEQUAL}	{yylval.a = newast("NOTEQUAL", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return NOTEQUAL;}
{ASSIGNOP}	{yylval.a = newast("ASSIGNOP", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return ASSIGNOP;}
{LESS}		{yylval.a = newast("LESS", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return LESS;}
{MORE} 		{yylval.a = newast("MORE", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return MORE;}

{PLUS}		{yylval.a = newast("PLUS", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return PLUS;}
{MINUS}		{yylval.a = newast("MINUS", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return MINUS;}
{STAR}		{yylval.a = newast("STAR", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return STAR;}
{DIV} 		{yylval.a = newast("DIV", 0, yylineno, yyrol);;yyrol = yyrol + strlen(yytext);return DIV;}
{AND}		{yylval.a = newast("AND", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return AND;}
{OR}		{yylval.a = newast("OR", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return OR;}
{DOT}		{yylval.a = newast("DOT", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return DOT;}
{NOT}		{yylval.a = newast("NOT", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return NOT;}
{MOD}		{yylval.a = newast("MOD", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return MOD;}

{LP}		{yylval.a = newast("LP", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return LP;}
{RP}		{yylval.a = newast("RP", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return RP;}
{LB}		{yylval.a = newast("LB", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return LB;}
{RB}		{yylval.a = newast("RB", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return RB;}
{LC}		{yylval.a = newast("LC", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return LC;}
{RC} 		{yylval.a = newast("RC", 0, yylineno, yyrol);yyrol = yyrol + strlen(yytext);return RC;}

{SPACE}		{yyrol = yyrol + strlen(yytext);}
{TAB} 		{ yyrol = yyrol + 4;}
{RET} 		{ yyrol = 1; }
{EOL} 		{ yyrol = 1;/*yycolumn = 1;*/}
.			{ yyrol = yyrol + strlen(yytext);printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext);exit(0); }
%%
/*int main(int argc, char ** argv)
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
*/
int yywrap()
{
	return 1;
}