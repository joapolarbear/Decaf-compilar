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
		a->nt = 0;
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
		a->l = NULL;
		a->r = NULL;
		if(t != -1) a->nt = 1;
		else a->nt = 0;
		if(!strcmp(a->name, "ID")||!strcmp(a->name, "TYPE")||!strcmp(a->name, "RELOP"))
		{
			char *s;
			s = (char *)malloc(sizeof(char *)*40);
			strcpy(s, yytext);
			a->idtype = s;
		}
		else if(!strcmp(a->name, "INT"))
		{
			char s[20];                        //store yytext
			strcpy(s, yytext);
			if(s[0] == '0')
			{
				if(s[1] == 'x' || s[1] == 'X')             //hex
					a->inter = strtol(s, NULL, 16); 	  
				else a->inter = strtol(s, NULL, 8);        // 8 jinzhi
			}
			else a->inter = atoi(yytext);  				// 10 jinzhi
		}
		else if(!strcmp(a->name, "FLOAT"))
		{
			a->flo = atof(yytext);
		}
	}
	return a;
}

void eval(struct ast *a, int count_in)
{
	int i;
	int count;
	if(a != NULL)
	{	
		if(a->line != -1)
		{
			if(count_in)
			{
				printf("-");
			}
			printf("%s", a->name);
			
			if(!strcmp(a->name, "ID")||!strcmp(a->name, "TYPE")||!strcmp(a->name, "RELOP"))
			{
				printf(":%s", a->idtype);
			}
			else if(!strcmp(a->name, "INT"))
			{
				printf(":%d", a->inter);
			}
			else if(!strcmp(a->name, "FLOAT"))
			{
				printf(":%f", a->flo);
			}
			if(a->nt)
			{
				printf("(%d)\n", a->line);
			}
			count = count_in + strlen(a->name) + (!!count_in);
		}
		eval(a->l, count);
		if(a->r && a->r->line != -1)     //a->r->line != -1   is for avoiding the situatoin " CompSt : LC DefList StmtList RC" and "DefList: ~" and "StmtList : ~"
			for(i = 0; i < count_in; i++)
			{
				printf(" ");
			}
		eval(a->r, count_in);
	}
}

void yyerror(char *msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	fprintf(stderr, "error:%d:", yylineno);
	vfprintf(stderr, msg, ap);
	fprintf(stderr, "\n");
	// exit(0);
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