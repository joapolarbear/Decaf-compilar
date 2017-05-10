
%{
	#include<stdio.h>
	#include<stdlib.h>
	#include "syntax.tab.h"
	#include "main.h"
	/*#define YY_USER_ACTION 
		yylloc->first_line = yylloc->last_line = yylineno;\ 
		yylloc->first_column = yycolumn; \
		yylloc->last_column = yycolumn + yyleng - 1; \
		yycolumn += yyleng;*/
%}
%option yylineno
letter [a-zA-Z]
hex		0[xX][0-9A-Fa-f]{1,8}

INT 	{hex}|(0[1-7][0-7]{0,9})|([1-9][0-9]*)|0
FLOAT   ([0-9]+\.[0-9]+)|([0-9]*\.[0-9]*[Ee][+-]?[0-9]{1,2})
BOOL 	true|false
STRING 	\".*\"
ID      [a-zA-Z_][0-9A-Za-z_]*	

SEMI	;
COMMA	,
LP		\(
RP 		\)
LB 		\[
RB 		\]
LC		\{
RC 		\}
SPACE 	[ \t\r]*
EOL 	\n
DOT		\.

ASSIGNOP	=
RELOP	>|<|>=|<=|==|!=
PLUS	\+
MINUS	-
STAR	\*
DIV		\/
AND		&&
OR		\|\|
NOT 	!

TYPE	int|float|bool|string
STRUCT 	struct 
RETURN 	return
IF 		if
ELSE 	else
WHILE 	whlie
BREAK 	break
CLASS 	class 
EXTENDS extends 
FOR 	for
NEW  	new 
NULL 	null 
THIS 	this
VOID 	void 
STATIC 	static 
PRINT 	print 
READINTEGER 	ReadInteger 
READLINE 		ReadLine 
INSTANCEOF 		instanceof 

COMMENT 		\/\/.*$
%%
{TYPE}		{yylval.a = newast("TYPE", 0, yylineno);return TYPE;}
{STRUCT} 	{yylval.a = newast("STRUCT", 0, yylineno);return STRUCT;}
{RETURN}	{yylval.a = newast("RETURN", 0, yylineno);return RETURN;}
{IF}		{yylval.a = newast("IF", 0, yylineno);return IF;}
{ELSE}		{yylval.a = newast("ELSE", 0, yylineno);return ELSE;}
{WHILE}		{yylval.a = newast("WHILE", 0, yylineno);return WHILE;}
{BREAK}		{yylval.a = newast("BREAK", 0, yylineno);return BREAK;}
{CLASS}		{yylval.a = newast("CLASS", 0, yylineno);return CLASS;}
{EXTENDS}	{yylval.a = newast("EXTENDS", 0, yylineno);return EXTENDS;}
{FOR}		{yylval.a = newast("FOR", 0, yylineno);return FOR;}
{NEW}		{yylval.a = newast("NEW", 0, yylineno);return NEW;}
{NULL}		{yylval.a = newast("NULL", 0, yylineno);return NULL;}
{RETURN}	{yylval.a = newast("RETURN", 0, yylineno);return RETURN;}
{THIS}		{yylval.a = newast("THIS", 0, yylineno);return THIS;}
{VOID}		{yylval.a = newast("VOID", 0, yylineno);return VOID;}
{STATIC}	{yylval.a = newast("STATIC", 0, yylineno);return STATIC;}
{PRINT}		{yylval.a = newast("PRINT", 0, yylineno);return PRINT;}
{READINTEGER}	{yylval.a = newast("READINTEGER", 0, yylineno);return READINTEGER;}
{READLINE}		{yylval.a = newast("READLINE", 0, yylineno);return READLINE;}
{INSTANCEOF}	{yylval.a = newast("INSTANCEOF", 0, yylineno);return INSTANCEOF;}


{INT}	{yylval.a = newast("INT", 0, yylineno);return INT;}
{FLOAT} {yylval.a = newast("FLOAT", 0, yylineno);return FLOAT;}
{BOOL} {yylval.a = newast("BOOL", 0, yylineno);return BOOL;}
{STRING} {yylval.a = newast("STRING", 0, yylineno);return STRING;}
{ID}	{yylval.a = newast("ID", 0, yylineno);return ID;}

{COMMENT} {yylval.a = newast("COMMENT", 0, yylineno);return COMMENT;}

{SEMI} 	{yylval.a = newast("SEMI", 0, yylineno);return SEMI;}
{COMMA} {yylval.a = newast("COMMA", 0, yylineno);return COMMA;}
{ASSIGNOP}	{yylval.a = newast("ASSIGNOP", 0, yylineno);return ASSIGNOP;}
{RELOP}		{yylval.a = newast("RELOP", 0, yylineno);return RELOP;}
{PLUS}		{yylval.a = newast("PLUS", 0, yylineno);return PLUS;}
{MINUS}		{yylval.a = newast("MINUS", 0, yylineno);return MINUS;}
{STAR}		{yylval.a = newast("STAR", 0, yylineno);return STAR;}
{DIV} 		{yylval.a = newast("DIV", 0, yylineno);;return DIV;}
{AND}		{yylval.a = newast("AND", 0, yylineno);return AND;}
{OR}		{yylval.a = newast("OR", 0, yylineno);return OR;}
{DOT}		{yylval.a = newast("DOT", 0, yylineno);return DOT;}
{NOT}		{yylval.a = newast("NOT", 0, yylineno);return NOT;}

{LP}		{yylval.a = newast("LP", 0, yylineno);return LP;}
{RP}		{yylval.a = newast("RP", 0, yylineno);return RP;}
{LB}		{yylval.a = newast("LB", 0, yylineno);return LB;}
{RB}		{yylval.a = newast("RB", 0, yylineno);return RB;}
{LC}		{yylval.a = newast("LC", 0, yylineno);return LC;}
{RC} 		{yylval.a = newast("RC", 0, yylineno);return RC;}

{SPACE}		{}
{EOL} 		{ /*yycolumn = 1;*/}
.			{ printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext);exit(0); }
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