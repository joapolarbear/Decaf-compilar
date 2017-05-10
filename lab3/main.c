#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "main.h"
#include "syntax.h"
#include "genCode.h"
char *typeArray[] = {"INT", "BOOL", "FLOAT", "STRING", "VOID", "CLASS", "ARRAY"};

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
		a->rol = temp->rol;
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
		if(t != -1) 
		{
			a->nt = 1;
			t = va_arg(valist, int);
			a->rol = t;
		}
		else 
		{
			a->nt = 0;
			a->rol = -1;
		}
		if(!strcmp(a->name, "ID"))
		{
			char *s;
			s = (char *)malloc(sizeof(char *)*40);
			strcpy(s, yytext);
			a->idtype = s;
		}
		else if(!strcmp(a->name, "INTC"))
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
		else if(!strcmp(a->name, "FLOATC"))
		{
			a->flo = atof(yytext);
		}
	}
	a->type = NULL;
	a->var_h = a->var_t = NULL;
	a->fun_h = a->fun_t = NULL;
	a->class_h = a->class_t = NULL;
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
			
			if(!strcmp(a->name, "ID"))
			{
				printf(":%s", a->idtype);
			}
			else if(!strcmp(a->name, "INTC"))
			{
				printf(":%d", a->inter);
			}
			else if(!strcmp(a->name, "FLOATC"))
			{
				printf(":%.4f", a->flo);
			}
			if(a->nt)
			{
				printf("(%d:%d)\n", a->line, a->rol);
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
void printFun(Fun *fun)
{
	Var *pv = fun->var_l;
	while(pv != NULL)
	{
		printf("  %s\n", pv->name);
		pv = pv->next;
	}
	pv = fun->form_l;
	while(pv != NULL)
	{
		printf("  %s\n", pv->name);
		pv = pv->next;
	}
}

void printGlobal(Global *glo)
{
	Class *pc = glo->class_l;
	Var *pv = glo->var_l;
	Fun *pf = glo->fun_l;
	printf("\n\n---------------------------------------------\n");
	printf("              -- Symbol table -- \n\nGlobal\n");
	while(pc != NULL)
	{
		printf("Class:%s  line:%d  rol:%d\n", pc->name, pc->line, pc->rol);
		pc = pc->next;
	}
	while(pf != NULL)
	{
		printf("Function:%s  line:%d  rol:%d\n", pf->name, pf->line, pf->rol);
		pf = pf->next;
	}
	while(pv != NULL)
	{
		printf("Variable:%s  line:%d  rol:%d  type:%s\n", pv->name, pv->line, pv->rol, typeArray[pv->type->type]);
		pv = pv->next;
	}
	printf("---------------------------------------------");
	pc = glo->class_l;
	while(pc!= NULL)
	{
		printf("\nClass:%s\n", pc->name);
		pv = pc->var_l;
		pf = pc->fun_l;
		while(pf != NULL)
		{
			printf("Function:%s  line:%d  rol:%d\n", pf->name, pf->line, pf->rol);
			pf = pf->next;
		}
		while(pv != NULL)
		{
			printf("Variable:%s  line:%d  rol:%d  type:%s\n", pv->name, pv->line, pv->rol, typeArray[pv->type->type]);
			pv = pv->next;
		}
		pc = pc->next;
	}
	printf("---------------------------------------------");
	pc = glo->class_l;
	while(pc != NULL)
	{
		pf = pc->fun_l;
		while(pf != NULL)
		{
			pv = pf->form_l;
			printf("\nClass:%s Function:%s  ReturnType:%s\n", pc->name, pf->name, typeArray[pf->returnType->type]);
			while(pv != NULL)
			{
				printf("Formal:%s  line:%d  rol:%d\n", pv->name, pv->line, pv->rol);
				pv = pv->next;
			}
			pv = pf->var_l;
			while(pv != NULL)
			{
				printf("Variable:%s  line:%d  rol:%d  type:%s\n", pv->name, pv->line, pv->rol, typeArray[pv->type->type]);
				pv = pv->next;
			}
			pf = pf->next;
		}
		pc = pc->next;
	}
	printf("---------------------------------------------");
	printf("\nprint Symbol table ok!\n");
	printf("---------------------------------------------\n\n");
}
void printError(Error *error_h)
{
	Error *pe = error_h;
	while(pe != NULL)
	{
		printf("Error:%d:%d: %s\n", pe->row, pe->rol, pe->s);
		pe = pe->next;
	}
}

Global *glo = NULL;
Class *pclass = NULL;
Fun *pfun = NULL;
Error *error_h = NULL, *error_t = NULL;
int yyrol = 1;
int break_flag = 0;

int main(int argc, char ** argv)
{
	if(argc < 1) return 1;
	FILE *f = fopen(argv[1], "r");
	if(!f)
	{
		perror(argv[1]);
		return 1;
	}

	glo = (Global *)malloc(sizeof(Global));
	glo->var_l = NULL;
	glo->fun_l = NULL;
	glo->class_l = NULL;

	yyrestart(f);
	yyparse();
	//printGlobal(glo);
	printError(error_h);	

	if(error_h == NULL) printTAC();
	return 0;
}