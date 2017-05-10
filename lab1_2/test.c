

%{
	#include<stdio.h>
	#include<stdlib.h>
	#include<unistd.h>
	#include "main.h"

%}
%union{
	struct ast* a;
	double d;
}
/* declared tokens */
%token <a> INT FLOAT ID
%token <a> SEMI COMMA 
%token <a> ASSIGNOP RELOP
%token <a> PLUS MINUS STAR DIV
%token <a> AND OR
%token <a> DOT NOT TYPE
%token <a> LP RP LB RB LC RC
%token <a> STRUCT RETURN IF ELSE WHILE

%type <a>  Program ExtDefList ExtDef Specifier ExtDecList FunDec CompSt VarDec StructSpecifier OptTag DefList Tag VarList ParamDec StmtList Stmt Exp Def Dec Args DecList

/*priority define*/
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left LP RP LB RB LC RC DOT

%%
Program : ExtDefList 		{$$ = newast("Program", 1, $1);printf("print start:\n");eval($$, 0);printf("print  OK!!\n\n");}
	;

ExtDefList : 				{$$ = newast("ExtDefList", 0, -1);}
	| ExtDef ExtDefList 	{$$ = newast("ExtDefList", 2, $1, $2);}
	;
ExtDef : Specifier ExtDecList SEMI 		{$$ = newast("ExtDef", 3, $1, $2, $3);}
	| Specifier SEMI 					{$$ = newast("ExtDef", 2, $1, $2);}
	| Specifier FunDec CompSt 			{$$ = newast("ExtDef", 3, $1, $2, $3);}
	;
ExtDecList : VarDec 			{$$ = newast("ExtDecList", 1, $1);}
	| VarDec COMMA ExtDecList 	{$$ = newast("ExtDecList", 3, $1, $2, $3);}
	;
/*Spefifire*/
Specifier : TYPE 				{$$ = newast("Specifier", 1, $1);}
	| StructSpecifier 			{$$ = newast("Specifier", 1, $1);}
	;
StructSpecifier : STRUCT OptTag LC DefList RC 		{$$ = newast("StructSpecifier", 5, $1, $2, $3, $4, $5);}
	| STRUCT Tag 									{$$ = newast("StructSpecifier", 2, $1, $2);}
	;
OptTag : 	{$$ = newast("OptTag", 0, -1);}
	| ID 	{$$ = newast("OptTag", 1, $1);}
	;
Tag : ID 	{$$ = newast("Tag", 1, $1);}
	;

/*Declaration*/
VarDec : ID 			{$$ = newast("VarDec", 1, $1);}
	| VarDec LB INT RB	{$$ = newast("VarDec", 4, $1, $2, $3, $4);}
	;
FunDec : error 			{printf("error type B at line %d:function declared error!!\n", yylineno);exit(0);}
	| ID LP VarList RP 	{$$ = newast("FunDec", 4, $1, $2, $3, $4);}
	| ID LP RP 				{$$ = newast("FunDec", 3, $1, $2, $3);}
	;
VarList : ParamDec COMMA VarList 	{$$ = newast("VarList", 3, $1, $2, $3);}
	| ParamDec						{$$ = newast("VarList", 1, $1);}
	;
ParamDec : Specifier VarDec 		{$$ = newast("ParamDec", 2, $1, $2);}
	;

/*Statement*/
CompSt : LC DefList StmtList RC 	{$$ = newast("CompSt", 4, $1, $2, $3, $4);}
	;

StmtList :  			{$$ = newast("StmtList", 0, -1);}
	| Stmt StmtList		{$$ = newast("StmtList", 2, $1, $2);}
	;
Stmt : error SEMI 		{printf("error type B at line %d:illegle parse!!\n", yylineno);exit(0);}
	| Exp SEMI     		{$$ = newast("Stmt", 2, $1, $2);}
	| CompSt 			{$$ = newast("Stmt", 1, $1);}
	| RETURN Exp SEMI 	{$$ = newast("Stmt", 3, $1, $2, $3);}
	| IF LP Exp RP Stmt 	%prec LOWER_THAN_ELSE {$$ = newast("Stmt", 5, $1, $2, $3, $4, $5);}
	| IF LP Exp RP Stmt ELSE Stmt 	{$$ = newast("Stmt", 7, $1, $2, $3, $4, $5, $6, $7);}
	| WHILE LP Exp RP Stmt 			{$$ = newast("Stmt", 5, $1, $2, $3, $4, $5);}	
	;

/*Location Definitions*/
DefList : Def DefList 			{$$ = newast("DefList", 2, $1, $2);}	
	| 						{$$ = newast("DefList", 0, -1);}	
	;
Def : Specifier DecList SEMI 	{$$ = newast("Def", 3, $1, $2, $3);}
	;
DecList : Dec  				{$$ = newast("DecList", 1, $1);}
 	| Dec COMMA DecList 	{$$ = newast("DecList", 3, $1, $2, $3);}
 	;
Dec : error 				{printf("error type B at line %d:variable declared error!!\n", yylineno);exit(0);}
	| VarDec    			{$$ = newast("Dec", 1, $1);}
	| VarDec ASSIGNOP Exp 	{$$ = newast("Dec", 3, $1, $2, $3);}
	;

Exp : Exp ASSIGNOP Exp 		{$$ = newast("Exp", 3, $1, $2, $3);}
	| Exp AND Exp 			{$$ = newast("Exp", 3, $1, $2, $3);}
	| Exp OR Exp  			{$$ = newast("Exp", 3, $1, $2, $3);}
	| Exp RELOP Exp  		{$$ = newast("Exp", 3, $1, $2, $3);}
	| Exp PLUS Exp 			{$$ = newast("Exp", 3, $1, $2, $3);}
	| Exp MINUS Exp  		{$$ = newast("Exp", 3, $1, $2, $3);}
	| Exp STAR Exp 			{$$ = newast("Exp", 3, $1, $2, $3);}
	| Exp DIV Exp 			{$$ = newast("Exp", 3, $1, $2, $3);}
	| LP Exp RP  			{$$ = newast("Exp", 3, $1, $2, $3);}
	| MINUS Exp 			{$$ = newast("Exp", 2, $1, $2);}
	| NOT Exp				{$$ = newast("Exp", 2, $1, $2);}
	| ID LP Args RP 		{$$ = newast("Exp", 4, $1, $2, $3, $4);}
	| ID LP RP  			{$$ = newast("Exp", 3, $1, $2, $3);}
	| Exp LB Exp RB 		{$$ = newast("Exp", 4, $1, $2, $3, $4);}
	| Exp DOT ID 			{$$ = newast("Exp", 3, $1, $2, $3);}
	| ID 					{$$ = newast("Exp", 1, $1);}
	| INT 					{$$ = newast("Exp", 1, $1);}
	| FLOAT 				{$$ = newast("Exp", 1, $1);}
	;
Args : Exp COMMA Args  		{$$ = newast("Args", 3, $1, $2, $3);}
	| Exp  					{$$ = newast("Args", 1, $1);}
	;
%%
#include "lex.yy.c"
/*
int main(){
	yyparse();
}*/


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "main.h"

struct ast *newast(char *name, int num, ...)
{
	int i;
	va_list valist;
	struct ast *a = (struct ast*)malloc(sizeof(struct ast));
	struct ast *temp = (struct ast*)malloc(sizeof(struct ast));
	if(!a)
	{
		yyerror("out of space!!\n");
		exit(0);
	}
	a->name = name;
	va_start(valist, num);

	if(num > 0)
	{
		temp = va_arg(valist, struct ast*);
		a->l = temp;
		a->line = temp->line;

		if(num > 1)
		{
			for(i = 1; i < num; i++)
			{
				temp->r = va_arg(valist, struct ast*);       //??????????
				temp = temp->r;
			}
		}
	}
	else //num = 0
	{
		int t = va_arg(valist, int);
		a->line = t;
		if(!strcmp(a->name, "ID")||!strcmp(a->name, "TYPE")||!strcmp(a->name, "RELOP"))
		{
			char *s;
			s = (char *)malloc(sizeof(char *)*40);
			strcpy(s, yytext);
			a->idtype = s;
		}
		else if(!strcmp(a->name, "INT"))
		{
			a->inter = atoi(yytext);
		}
		else if(!strcmp(a->name, "FLOAT"))
		{
			a->flo = atof(yytext);
		}
	}
	return a;
}

void eval(struct ast *a, int level)
{
	int i;
	if(a != NULL)
	{
		for(i = 0; i < level; i++)
		{
			printf("  ");
		}
		if(a->line != -1)
		{
			printf("%s ", a->name);
			if(!strcmp(a->name, "ID")||!strcmp(a->name, "TYPE")||!strcmp(a->name, "RELOP"))
			{
				printf(":%s ", a->idtype);
			}
			else if(!strcmp(a->name, "INT"))
			{
				printf(":%d ", a->inter);
			}
			else if(!strcmp(a->name, "FLOAT"))
			{
				printf(":%f ", a->flo);
			}
		}
		printf("\n");

		eval(a->l, level + 1);
		eval(a->r, level);
	}
}

void yyerror(char *msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	fprintf(stderr, "error:%d:", yylineno);
	vfprintf(stderr, msg, ap);
	fprintf(stderr, "\n");
}

int main(int argc, char ** argv)
{
	if(argc < 1) return 1;
	FILE *f = fopen(argv[1], "r");
	if(!f)
	{
		perror(argv[1]);
		return 1;
	}
	yyrestart(f);
	yyparse();
	return 0;
}





%{
	#include<stdio.h>
	#include<stdlib.h>
	#include<unistd.h>
	#include "main.h"

%}
%union{
	struct ast* a;
	double d;
}
/* declared tokens */
%token <a> INT FLOAT ID
%token <a> SEMI COMMA 
%token <a> ASSIGNOP RELOP
%token <a> PLUS MINUS STAR DIV
%token <a> AND OR
%token <a> DOT NOT TYPE
%token <a> LP RP LB RB LC RC
%token <a> STRUCT RETURN IF ELSE WHILE

%type <a> Program ExtDefList ExtDef Specifier ExtDecList FunDec CompSt VarDec StructSpecifier OptTag DefList Tag VarList ParamDec StmtList Stmt Exp Def Dec Args DecList
%type <a> ExtDef_f ExtDecList_f StructSpecifier_f FunDec_f VarList_f Stmt_f DecList_f Dec_f Exp_double Exp_id Exp_fun

/*priority define*/
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left LP RP LB RB LC RC DOT

%%
Program : ExtDefList 		{$$ = newast("Program", 1, $1);printf("print start:\n");eval($$, 0);printf("print  OK!!\n\n");}
	;

ExtDefList : 				{$$ = newast("ExtDefList", 0, -1);}
	| ExtDef ExtDefList 	{$$ = newast("ExtDefList", 2, $1, $2);}
	;


ExtDef : Specifier ExtDef_f		{$$ = newast("ExtDef", 2, $1, $2);}
	;
ExtDef_f : ExtDefList SEMI 		{$$ = newast("ExtDef_f", 2, $1, $2);}
	| SEMI 						{$$ = newast("ExtDef_f", 1, $1);}
	| FunDec CompSt 			{$$ = newast("ExtDef_f", 2, $1, $2);}
	;

ExtDecList : VarDec ExtDecList_f {$$ = newast("ExtDecList", 2, $1, $2);}
	;
ExtDecList_f :  				{$$ = newast("ExtDecList_f", 0, -1);}
	| COMMA ExtDecList 			{$$ = newast("ExtDecList_f", 2, $1, $2);}
	;

Specifier : TYPE 				{$$ = newast("Specifier", 1, $1);}
	| StructSpecifier 			{$$ = newast("Specifier", 1, $1);}
	;

StructSpecifier : STRUCT StructSpecifier_f 			{$$ = newast("StructSpecifier", 2, $1, $2);}    
	;
StructSpecifier_f : OptTag LC DefList RC 		{$$ = newast("StructSpecifier_f", 4, $1, $2, $3, $4);}
	| Tag 									{$$ = newast("StructSpecifier_f", 1, $1);}
	;
OptTag : 	{$$ = newast("OptTag", 0, -1);}
	| ID 	{$$ = newast("OptTag", 1, $1);}
	;
Tag : ID 	{$$ = newast("Tag", 1, $1);}
	;


VarDec : ID 			{$$ = newast("VarDec", 1, $1);}
	| VarDec LB INT RB	{$$ = newast("VarDec", 4, $1, $2, $3, $4);}
	;

FunDec : error 			{printf("error type B at line %d:function declared error!!\n", yylineno);exit(0);}
	| ID LP FunDec_f	{$$ = newast("FunDec", 2, $1, $2);}
FunDec_f: VarList RP 	{$$ = newast("FunDec_f", 2, $1, $2);}
	| RP 				{$$ = newast("FunDec_f", 1, $1);}
	;

VarList : ParamDec VarList_f 		{$$ = newast("VarList", 2, $1, $2);}
VarList_f : COMMA VarList 			{$$ = newast("VarList_f", 2, $1, $2);}
	| 								{$$ = newast("VarList_f", 0, -1);}
	;
ParamDec : Specifier VarDec 		{$$ = newast("ParamDec", 2, $1, $2);}
	;


CompSt : LC DefList StmtList RC 	{$$ = newast("CompSt", 4, $1, $2, $3, $4);}
	;

StmtList :  			{$$ = newast("StmtList", 0, -1);}
	| Stmt StmtList		{$$ = newast("StmtList", 2, $1, $2);}
	;
Stmt : error SEMI 		{printf("error type B at line %d:illegle parse!!\n", yylineno);exit(0);}
	| Exp SEMI     		{$$ = newast("Stmt", 2, $1, $2);}
	| CompSt 			{$$ = newast("Stmt", 1, $1);}
	| RETURN Exp SEMI 	{$$ = newast("Stmt", 3, $1, $2, $3);}
	| IF LP Exp RP Stmt Stmt_f		{$$ = newast("Stmt", 6, $1, $2, $3, $4, $5, $6);}
	| WHILE LP Exp RP Stmt 			{$$ = newast("Stmt", 5, $1, $2, $3, $4, $5);}	
	;
Stmt_f : ELSE Stmt  	{$$ = newast("Stmt_f", 2, $1, $2);}
	|					{$$ = newast("Stmt_f", 0, -1);}
	;

DefList : Def DefList 		{$$ = newast("DefList", 2, $1, $2);}	
	| 					{$$ = newast("DefList", 0, -1);}	
	;
Def : Specifier DecList SEMI 	{$$ = newast("Def", 3, $1, $2, $3);}
	;

DecList : Dec DecList_f 	{$$ = newast("DefList", 2, $1, $2);}	
DecList_f :  				{$$ = newast("DecList_f", 0, -1);}
 	| COMMA DecList 	{$$ = newast("DecList_f", 2, $1, $2);}
 	;
Dec : error 				{printf("error type B at line %d:variable declared error!!\n", yylineno);exit(0);}
	| VarDec  Dec_f  		{$$ = newast("Dec", 2, $1, $2);}
	;
Dec_f : 					{$$ = newast("Dec_f", 0, -1);}
	| ASSIGNOP Exp 			{$$ = newast("Dec_f", 2, $1, $2);}
	;

Exp: Exp Exp_double 	{$$ = newast("Exp", 2, $1, $2);}
	| LP Exp RP  			{$$ = newast("Exp", 3, $1, $2, $3);}
	| MINUS Exp 			{$$ = newast("Exp", 2, $1, $2);}
	| NOT Exp				{$$ = newast("Exp", 2, $1, $2);}
	| ID Exp_id 			{$$ = newast("Exp", 2, $1, $2);}
	| INT 					{$$ = newast("Exp", 1, $1);}
	| FLOAT 				{$$ = newast("Exp", 1, $1);}
	;
Exp_double : ASSIGNOP Exp 		{$$ = newast("Exp_double", 2, $1, $2);}
	| AND Exp 			{$$ = newast("Exp_double", 2, $1, $2);}
	| OR Exp  			{$$ = newast("Exp_double", 2, $1, $2);}
	| RELOP Exp  		{$$ = newast("Exp_double", 2, $1, $2);}
	| PLUS Exp 			{$$ = newast("Exp_double", 2, $1, $2);}
	| MINUS Exp  		{$$ = newast("Exp_double", 2, $1, $2);}
	| STAR Exp 			{$$ = newast("Exp_double", 2, $1, $2);}
	| DIV Exp 			{$$ = newast("Exp_double", 2, $1, $2);}
	| LB Exp RB 		{$$ = newast("Exp_double", 3, $1, $2, $3);}
	| DOT ID 			{$$ = newast("Exp_double", 2, $1, $2);}
	;
Exp_id : LP Exp_fun  	{$$ = newast("Exp_id", 2, $1, $2);}
	| 					{$$ = newast("Exp_id", 0, -1);}
	;
Exp_fun : Args RP 		{$$ = newast("Exp_fun", 2, $1, $2);}   
	| RP 				{$$ = newast("Exp_fun", 1, $1);} 
	;

Args : Exp COMMA Args  		{$$ = newast("Args", 3, $1, $2, $3);}
	| Exp  					{$$ = newast("Args", 1, $1);}
	;
%%
#include "lex.yy.c"
/*
int main(){
	yyparse();
}*/

