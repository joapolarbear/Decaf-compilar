%{
	#include<stdio.h>
	#include<stdlib.h>
	#include<unistd.h>
	#include "main.h"
	#include "syntax.h"
	#include "genCode.h"

	extern Global *glo;
	extern Class *pclass;
	extern Fun *pfun;
	extern Error *error_h, *error_t;
	extern int break_flag;
	extern ThreeAddCode *code;
%}
%union{
	struct ast* a;
	double d;
}
/* declared tokens */
%token <a> INT FLOAT BOOL STRING VOID CLASS STATIC EXTENDS FOR BREAK PRINT THIS NEW RETURN IF ELSE WHILE READINTEGER READLINE INSTANCEOF ID
%token <a> INTC FLOATC BOOLC STRINGC
%token <a> ASSIGNOP LESS MORE LESSEQUAL MOREEQUAL EQUAL NOTEQUAL 
%token <a> PLUS MINUS STAR DIV AND OR DOT MOD NOT
%token <a> LP RP LB RB LC RC
%token <a> SEMI COMMA SPACE EOL

%type <a>  Program ClassDef ProgramList VariableDef Variable Type Formals FormalsList FunctionDef StmtBlock StmtList Field Stmt SimpleStmt IfStmt WhileStmt ForStmt BreakStmt ReturnStmt PrintStmt LValue Expr Call Actuals ActualsList BoolExpr Constant 

/*priority define*/
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%right ASSIGNOP
%left OR
%left AND
%left LESSEQUAL MOREEQUAL LESS MORE EQUAL NOTEQUAL
%left PLUS MINUS
%left STAR DIV MOD
%right NOT
%left LP RP LB RB LC RC DOT

%%
Program : ClassDef ProgramList		{ $$ = newast("Program", 2, $1, $2);
										//printf("print start:\n");
										eval($$, 0);
										//printf("print  OK!!\n\n");
										addGlo(glo, $1, $2);
										map1($$, &error_h, &error_t, glo, &pclass, &pfun);
										pclass = NULL; pfun = NULL;
										map2($$, &error_h, &error_t, glo, &pclass, &pfun);
										pclass = NULL; pfun = NULL;
										if(error_h == NULL) gen($$);
									}
	;

ProgramList : ClassDef 				{ $$ = newast("ProgramList", 1, $1);
										passClass($$, 1, $1);
									}
	| 								{ $$ = newast("ProgramList", 0, -1);}
	;

VariableDef : Variable SEMI 		{ $$ = newast("VariableDef", 2, $1, $2);
										passVar($$, 1, $1);
									}
	;

Variable : Type ID 					{ $$ = newast("Variable", 2, $1, $2);
										//addVar($2, $1, &var_h, &var_t);
										addVar($$, $2, $1);
									}
	;

Type : INT  						{ $$ = newast("Type", 1, $1);addType($$, 0, NULL);}
	| BOOL 							{ $$ = newast("Type", 1, $1);addType($$, 1, NULL);}
	| FLOAT  						{ $$ = newast("Type", 1, $1);addType($$, 2, NULL);}
	| STRING  						{ $$ = newast("Type", 1, $1);addType($$, 3, NULL);}
	| VOID 							{ $$ = newast("Type", 1, $1);addType($$, 4, NULL);}
	| CLASS ID 						{ $$ = newast("Type", 2, $1, $2);addType($$, 5, $2);}
	| Type LB RB 					{ $$ = newast("Type", 3, $1, $2, $3);addType($$, 6, $1);}
	;
Formals : FormalsList 				{ $$ = newast("Formals", 1, $1);
										passVar($$, 1, $1);
									}
	| 								{ $$ = newast("Formals", 0, -1);}
	;
FormalsList : Variable COMMA FormalsList 	{ $$ = newast("FormalsList", 3, $1, $2, $3);
												//addForm(&form_h, &form_t, &var_h, &var_t);
												passVar($$, 2, $1, $3);
											}
	| Variable 						{ $$ = newast("FormalsList", 1, $1);
										//addForm(&form_h, &form_t, &var_h, &var_t);
										passVar($$, 1, $1);
									}
	;

FunctionDef : Type ID LP Formals RP StmtBlock 	{ $$ = newast("FunctionDef", 6, $1, $2, $3, $4, $5, $6);
													//addFun($2, $1, 0, &var_h, &var_t, &fun_h, &fun_t, &form_h, &form_t, glo);
													addFun($$, $2, $1, $4, $6, 0, glo);
												}
	| STATIC Type ID LP Formals RP StmtBlock  	{ $$ = newast("FunctionDef", 7, $1, $2, $3, $4, $5, $6, $7);
													//addFun($2, $1, 1, &var_h, &var_t, &fun_h, &fun_t, &form_h, &form_t, glo);
													addFun($$, $2, $1, $4, $6, 1, glo);
												}
	;


ClassDef : CLASS ID LC Field RC  				{ $$ = newast("ClassDef", 5, $1, $2, $3, $4, $5);
													//addClass($2, NULL, &var_h, &var_t, &fun_h, &fun_t, &class_h, &class_t);
													addClass($$, $2, $4);
												}
	| CLASS ID EXTENDS ID  LC Field RC 			{ $$ = newast("ClassDef", 7, $1, $2, $3, $4, $5, $6, $7);
													//addClass($2, $4, &var_h, &var_t, &fun_h, &fun_t, &class_h, &class_t);
													addClass($$, $2, $6);
												}
	;

Field : VariableDef	Field				{ $$ = newast("Field", 2, $1, $2);
											passVar($$, 2, $1, $2);
											passFun($$, 1, $2);
										}
	| FunctionDef Field					{ $$ = newast("Field", 2, $1, $2);
											passFun($$, 2, $1, $2);
											passVar($$, 2, $2);
										}
	| 									{$$ = newast("Field", 0, -1);}
	;

StmtBlock : LC StmtList RC 			{ $$ = newast("StmtBlock", 3, $1, $2, $3);
										passVar($$, 1, $2);
									}
	;
StmtList :							{ $$ = newast("StmtList", 0, -1);}
	| Stmt StmtList					{ $$ = newast("StmtList", 2, $1, $2);
										passVar($$, 2, $1, $2);
									}
	;
Stmt : Variable SEMI				{ $$ = newast("Stmt", 2, $1, $2);
										passVar($$, 1, $1);
									}
	| SimpleStmt SEMI				{ $$ = newast("Stmt", 2, $1, $2);}
	| IfStmt  						{ $$ = newast("Stmt", 1, $1);}
	| WhileStmt  					{ $$ = newast("Stmt", 1, $1);}
	| ForStmt  						{ $$ = newast("Stmt", 1, $1);}
	| BreakStmt SEMI 				{ $$ = newast("Stmt", 2, $1, $2);}
	| ReturnStmt SEMI 				{ $$ = newast("Stmt", 2, $1, $2);}
	| PrintStmt SEMI 				{ $$ = newast("Stmt", 2, $1, $2);}
	| StmtBlock 					{ $$ = newast("Stmt", 1, $1);
										passVar($$, 1, $1);
									}
	;
SimpleStmt : LValue ASSIGNOP Expr  	{ $$ = newast("SimpleStmt", 3, $1, $2, $3);}
	| Call 							{ $$ = newast("SimpleStmt", 1, $1);}
	| 								{ $$ = newast("SimpleStmt", 0, -1);}
	;
LValue : ID  						{ $$ = newast("LValue", 1, $1);
										addType($$, -1, NULL);
									}
	| Expr DOT ID 					{ $$ = newast("LValue", 3, $1, $2, $3);
										addType($$, -1, NULL);
									}
	| Expr LB Expr RB 				{ $$ = newast("LValue", 4, $1, $2, $3, $4);
										addType($$, -1, NULL);
									}
	;
Call : ID LP Actuals RP 			{ $$ = newast("Call", 4, $1, $2, $3, $4);
										addType($$, -1, NULL);
									}
	| Expr DOT ID LP Actuals RP 	{ $$ = newast("Call", 6, $1, $2, $3, $4, $5, $6);
										addType($$, -1, NULL);
									}
	;

Actuals : ActualsList 				{ $$ = newast("Actuals", 1, $1);
										$$->type = $1->type;
									}
	| 								{ $$ = newast("Actuals", 0, -1);}
	;
ActualsList : Expr COMMA ActualsList 	{ $$ = newast("ActualsList", 3, $1, $2, $3);
											$$->type = $1->type;
											if($$->type == NULL) $$->type = $3->type;
											else  $$->type->next = $3->type;
										}
	| Expr 							    { $$ = newast("ActualsList", 1, $1);
											$$->type = $1->type;
									    }
	;
ForStmt: FOR LP SimpleStmt SEMI BoolExpr SEMI SimpleStmt RP Stmt 	{ $$ = newast("ForStmt", 9, $1, $2, $3, $4, $5, $6, $7, $8, $9);
																		//checkType(&error_h, &error_t, 1, $5->line, $5->rol, 1, $5);
																	}
	;
WhileStmt : WHILE LP BoolExpr RP Stmt 		{ $$ = newast("WhileStmt", 5, $1, $2, $3, $4, $5);
												//checkType(&error_h, &error_t, 1, $3->line, $3->rol, 1, $3);
											}
	;
IfStmt : IF LP BoolExpr RP Stmt 	%prec LOWER_THAN_ELSE 	{ $$ = newast("IfStmt", 5, $1, $2, $3, $4, $5);
																//checkType(&error_h, &error_t, 1, $3->line, $3->rol, 1, $3);
															}
	| IF LP BoolExpr RP Stmt ELSE Stmt  					{ $$ = newast("IfStmt", 7, $1, $2, $3, $4, $5, $6, $7);
																//checkType(&error_h, &error_t, 1, $3->line, $3->rol, 1, $3);
															}
	;

ReturnStmt : RETURN 				{ $$ = newast("ReturnStmt", 1, $1);
										addType($$, 4, NULL);
									}
 	| RETURN Expr 					{ $$ = newast("ReturnStmt", 2, $1, $2);
 										//addType($$, $2->type->type, NULL);
 										$$->type = $2->type;
 									}
 	;
BreakStmt : BREAK 					{ $$ = newast("BreakStmt", 1, $1);
										addType($$, 4, NULL);
									}
	;
PrintStmt :  PRINT LP ActualsList RP 	{ $$ = newast("PrintStmt", 4, $1, $2, $3, $4);
											addType($$, 4, NULL);
										}
	;
BoolExpr : Expr 					{ $$ = newast("BoolExpr", 1, $1);
										//addType($$, $1->type->type, NULL);
										$$->type = $1->type;
									}
	;
Expr : Constant 					{ $$ = newast("Expr", 1, $1);
										//addType($$, $1->type->type, NULL);
										$$->type = $1->type;
									}
	| LValue 						{ $$ = newast("Expr", 1, $1);
										//addType($$, $1->type->type, NULL);
										$$->type = $1->type;
									}
	| THIS 							{ $$ = newast("Expr", 1, $1);
										//addType($$, $1->type->type, NULL);
										$$->type = $1->type;
									}
	| Call 							{ $$ = newast("Expr", 1, $1);
										//addType($$, $1->type->type, NULL);
										$$->type = $1->type;
									}
	| LP Expr RP 					{ $$ = newast("Expr", 3, $1, $2, $3);
										//addType($$, $2->type->type, NULL);
										$$->type = $2->type;
									}
	| Expr PLUS Expr 				{ $$ = newast("Expr", 3, $1, $2, $3);
										//checkType(&error_h, &error_t, 0, $1->line, $1->rol, 2, $1, $3);
										//addType($$, $1->type->type, NULL);
										$$->type = $1->type;
									}
 	| Expr MINUS Expr 				{ $$ = newast("Expr", 3, $1, $2, $3);
 										//checkType(&error_h, &error_t, 0, $1->line, $1->rol, 2, $1, $3);
 										//addType($$, $1->type->type, NULL);
 										$$->type = $1->type;
 									}
 	| Expr STAR Expr 				{ $$ = newast("Expr", 3, $1, $2, $3);
 										//checkType(&error_h, &error_t, 0, $1->line, $1->rol, 2, $1, $3);
 										//addType($$, $1->type->type, NULL);
 										$$->type = $1->type;
 									}
 	| Expr DIV Expr 				{ $$ = newast("Expr", 3, $1, $2, $3);
 										//checkType(&error_h, &error_t, 0, $1->line, $1->rol, 2, $1, $3);
 										//addType($$, $1->type->type, NULL);
 										$$->type = $1->type;
 									}
 	| Expr MOD Expr 				{ $$ = newast("Expr", 3, $1, $2, $3);
 										//checkType(&error_h, &error_t, 0, $1->line, $1->rol, 2, $1, $3);
 										//addType($$, $1->type->type, NULL);
 										$$->type = $1->type;
 									}
 	| MINUS Expr  					{ $$ = newast("Expr", 2, $1, $2);
 										//checkType(&error_h, &error_t, 0, $2->line, $2->rol, 1, $2);
 										//addType($$, $2->type->type, NULL);
 										$$->type = $2->type;
 									}
 	| Expr LESS Expr 				{ $$ = newast("Expr", 3, $1, $2, $3);
 										//checkType(&error_h, &error_t, 0, $1->line, $1->rol, 2, $1, $3);
 										//addType($$, 1, NULL);
 										addType($$, 1, NULL);
 									}
 	| Expr MORE Expr 				{ $$ = newast("Expr", 3, $1, $2, $3);
 										//checkType(&error_h, &error_t, 0, $1->line, $1->rol, 2, $1, $3);
 										//addType($$, 1, NULL);
 										addType($$, 1, NULL);
 									}
 	| Expr LESSEQUAL Expr 			{ $$ = newast("Expr", 3, $1, $2, $3);
 										//checkType(&error_h, &error_t, 0, $1->line, $1->rol, 2, $1, $3);
 										//addType($$, 1, NULL);
 										addType($$, 1, NULL);
 									}
 	| Expr MOREEQUAL Expr 			{ $$ = newast("Expr", 3, $1, $2, $3);
 										//checkType(&error_h, &error_t, 0, $1->line, $1->rol, 2, $1, $3);
 										//addType($$, 1, NULL);
 										addType($$, 1, NULL);
 									}
 	| Expr EQUAL Expr 				{ $$ = newast("Expr", 3, $1, $2, $3);
 										//checkType(&error_h, &error_t, 0, $1->line, $1->rol, 2, $1, $3);
 										//addType($$, 1, NULL);
 										addType($$, 1, NULL);
 									}
 	| Expr NOTEQUAL Expr 			{ $$ = newast("Expr", 3, $1, $2, $3);
 										//checkType(&error_h, &error_t, 0, $1->line, $1->rol, 2, $1, $3);
 										//addType($$, 1, NULL);
 										addType($$, 1, NULL);
 									}
 	| Expr AND Expr 				{ $$ = newast("Expr", 3, $1, $2, $3);
 										//checkType(&error_h, &error_t, 1, $1->line, $1->rol, 2, $1, $3);
 										//addType($$, 1, NULL);
 										addType($$, 1, NULL);
 									}
 	| Expr OR Expr 					{ $$ = newast("Expr", 3, $1, $2, $3);
 										//checkType(&error_h, &error_t, 1, $1->line, $1->rol, 2, $1, $3);
 										//addType($$, 1, NULL);
 										addType($$, 1, NULL);
 									}
 	| NOT Expr 						{ $$ = newast("Expr", 2, $1, $2);
 										//checkType(&error_h, &error_t, 1, $2->line, $2->rol, 1, $2);
 										//addType($$, 1, NULL);
 										addType($$, 1, NULL);
 									}
 	| READINTEGER LP RP 			{ $$ = newast("Expr", 3, $1, $2, $3);
 										addType($$, 0, NULL);
 									}
 	| READLINE LP RP 				{ $$ = newast("Expr", 3, $1, $2, $3);
 										addType($$, 3, NULL);
 									}
 	| NEW ID LP RP 					{ $$ = newast("Expr", 4, $1, $2, $3, $4);}
 	| NEW Type LB Expr RB 			{ $$ = newast("Expr", 5, $1, $2, $3, $4, $5);}
 	| INSTANCEOF LP Expr COMMA ID RP 	{ $$ = newast("Expr", 6, $1, $2, $3, $4, $5, $6);}
 	| LP CLASS ID RP Expr 			{ $$ = newast("Expr", 5, $1, $2, $3, $4, $5);}
 	;
 Constant : INTC 					{ $$ = newast("Constant", 1, $1);
										addType($$, 0, NULL);
									}
	| BOOLC  						{ $$ = newast("Constant", 1, $1);
										addType($$, 1, NULL);
									}
 	| FLOATC 						{ $$ = newast("Constant", 1, $1);
 										addType($$, 2, NULL);
 									}
	| STRINGC 						{ $$ = newast("Constant", 1, $1);
										addType($$, 3, NULL);
									}
	;


%%
#include "lex.yy.c"
/*
int main(){
	yyparse();
}
*/