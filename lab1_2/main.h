#ifndef _MAIN
#define _MAIN
extern int yylineno;
extern char* yytext;


struct ast{
	int line;
	char *name;
	int nt;
	struct ast *l;
	struct ast *r;
	union
	{
		char *idtype;
		int inter;
		float flo;
	};
};


struct ast *newast(char *name, int num, ...);

void eval(struct ast *a, int count);

void yyerror(char *s, ...);

#endif