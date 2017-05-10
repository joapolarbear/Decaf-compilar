#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "main.h"
#include "genCode.h"
#include "syntax.h"

extern Global *glo;
extern Class *pclass;
extern Fun *pfun;
extern Error *error_h, *error_t;
extern ThreeAddCode *code;

#define SIZE 5
ThreeAddCode *LinkTAC(ThreeAddCode *code1, ThreeAddCode *code2)
{
	if (code1 == NULL) return code2;
	if (code2 == NULL) return code1;
	code1->tail->next = code2->head;
	code1->tail = code2->tail;
	return code1;
}
ThreeAddCode *CreateTAC(char *newOp, char *place1, char *place2, char *place3)
{
	ThreeAddCode *temp;
	CodeLine *codeLine;
	temp = (ThreeAddCode *)malloc(sizeof(ThreeAddCode));
	codeLine = (CodeLine *)malloc(sizeof(CodeLine));
	strcpy(codeLine->op, newOp);
	if (place1 == NULL) codeLine->place[0][0] = '\0';
	else strcpy(codeLine->place[0], place1);
	if (place2 == NULL) codeLine->place[1][0] = '\0';
	else strcpy(codeLine->place[1], place2);
	if (place3 == NULL) codeLine->place[2][0] = '\0';
	else strcpy(codeLine->place[2], place3);
	codeLine->next = NULL;
	temp->head = codeLine;
	temp->tail = codeLine;
	return temp;
}

void CreatePlace(char *place)
{
	static int i = 0;
	sprintf(place, "_T%d", i);
	i++;
}
void CreateLable(char *l)
{
	static int j = 0;
	sprintf(l, "_L%d", j);
	j++;
}
void itoa(int i,char*string)
{
	int power,j;
	j=i;
	for(power=1;j>=10;j/=10) power*=10;
	*string++='#';
	for(;power>0;power/=10)
	{
	   *string++='0'+i/power;
	   i%=power;
	}
	*string='\0';
}
//+-*/ call  = 
// ** the adress of dest
// *** the adress of source
void gen(struct ast *tree)
{
	if(tree == NULL || tree->rol == 0) return;   //空或者产生式为空的 直接返回
	//printf("%s\n", tree->name);
	if(strcmp(tree->name, "Program") == 0)
	{
		gen(tree->l);
	}
	else if(strcmp(tree->name, "ClassDef") == 0)
	{

		pclass = tree->class_h;
		pfun = NULL;
		char s1[20], s2[20];
		char t1[SIZE], t2[SIZE];
		strcat(strcpy(s1, "_"), strcat(strcat(strcpy(s2, pclass->name), "_"), "New"));
		ThreeAddCode *temp = CreateTAC("FUNCTION", s1, ":", NULL);code = LinkTAC(code, temp);

		CreatePlace(t1);
		CreatePlace(t2);
	
		temp = CreateTAC(strcat(strcpy(s1, "_"), strcat(strcat(strcpy(s2, pclass->name), "_"), "New")), ":", NULL, NULL);code = LinkTAC(code, temp);
		CreatePlace(t1);
		itoa(pclass->size, s1);
		temp = CreateTAC("=", s1, NULL, t1);code = LinkTAC(code, temp);
		temp = CreateTAC("ARG", t1, NULL, NULL);code = LinkTAC(code, temp); 		
		CreatePlace(t1);
		temp = CreateTAC("CALL", "Alloc", NULL, t1);code = LinkTAC(code, temp);//malloc

		Var *pv = pclass->var_l;
		int addr = 4;
		while(pv != NULL)
		{
			CreatePlace(t2);
			itoa(addr, s1);
			temp = CreateTAC("+", t1, s1, t2);code = LinkTAC(code, temp);
			temp = CreateTAC("=", "#0", NULL, strcat(strcpy(s1, "*"),t2));code = LinkTAC(code, temp);
			addr += 4;
			pv = pv->next;
		}

		//virtaul table
		CreatePlace(t2);
		temp = CreateTAC("=", "VTBL", strcat(strcpy(s1, "<"), strcat(strcpy(s2, pclass->name), ">")), t2);code = LinkTAC(code, temp);
		temp = CreateTAC("=", t2, NULL, strcat(strcpy(s1, "*"),t1));code = LinkTAC(code, temp);
		temp = CreateTAC("return", t1, NULL, NULL);code = LinkTAC(code, temp);

		gen(tree->l);
		gen(tree->r);
	}
	else if(strcmp(tree->name, "FunctionDef") == 0)
	{
		pfun = tree->fun_h;
		Var *pv = pfun->form_l;
		ThreeAddCode *code1;
		struct ast *temp = tree->l;
		char t1[SIZE], s1[20], s2[20];
		int i = 4;
		if(strcmp(temp->name, "STATIC") == 0)
		{
			strcat(strcpy(s1, "_"), strcat(strcat(strcpy(s2, pclass->name), "_"), pfun->name));
			code1 = CreateTAC("FUNCTION", s1, ":", NULL);code = LinkTAC(code, code1);
		}
		else 
		{
			strcat(strcpy(s1, "_"), strcat(strcat(strcpy(s2, pclass->name), "_"), pfun->name));
			code1 = CreateTAC("FUNCTION", s1, ":", NULL);code = LinkTAC(code, code1);
			// no static with "this"
			CreatePlace(t1);
			itoa(i, s1);
			code1 = CreateTAC("PARAM", t1, ":", s1);
			code = LinkTAC(code, code1);
			i = i + 4;
		}

		while(pv != NULL)
		{
			CreatePlace(t1);
			itoa(i, s1);
			pv->place = atoi(t1+2);
			code1 = CreateTAC("PARAM", t1, ":", s1);
			code = LinkTAC(code, code1);
			i = i + 4;
			pv = pv->next;
		}
		while(strcmp(temp->name, "StmtBlock") != 0)
		{
			temp = temp->r;
		}
		code1 = translate_StmtBlock(temp, NULL);
		code = LinkTAC(code, code1);
		gen(tree->r);
	}
	else if(strcmp(tree->name, "Variable") == 0)
	{
		Var *pv = tree->var_h;
		char t1[SIZE];
		CreatePlace(t1);
		pv->place = atoi(t1+2);
		gen(tree->r);
	}
	else
	{
		gen(tree->l);
		gen(tree->r);
	}
}
ThreeAddCode *translate_BoolExp(struct ast *a)
{
	ThreeAddCode *code1 = NULL, *code2 = NULL;
	struct ast* temp = a->l;   //expr
	char t1[SIZE], t2[SIZE], t3[SIZE], s1[SIZE];
	temp = temp->l;
	if(strcmp(temp->name, "Constant") == 0)
	{
		if(strcmp(temp->l->name, "INTC") == 0) 
			sprintf(s1, "#%d", temp->l->inter);
		else if(strcmp(a->l->name, "FLOATC") == 0)
			sprintf(s1, "#%f", temp->l->flo);
		code1 = CreateTAC("if", s1, ">", "#0");
		return code1;
	}
	else if(strcmp(temp->name, "LValue") == 0)
	{
		CreatePlace(t1);
		code1 = translate_Lvalue(temp, t1);
		code2 = CreateTAC("if", t1, ">", "#0");
		return LinkTAC(code1, code2);
	}
	else if(strcmp(temp->name, "THIS") == 0)
	{

	}
	else if(strcmp(temp->name, "Call") == 0)
	{
		CreatePlace(t1);
		code1 = translate_Call(temp, t1);
		code2 = CreateTAC("if", t1, ">", "#0");
		return LinkTAC(code1, code2);
	}
	else if(strcmp(temp->name, "LP") == 0)
	{
		if(strcmp(temp->r->name, "Expr") == 0) 
		{
			CreatePlace(t1);
			code1 = translate_Exp(a->r, t1);
			code2 = CreateTAC("if", t1, ">", "#0");
			return LinkTAC(code1, code2);
		}
		else //lp class id rp expr
		{
           	return NULL;                              //hhhhhhhhhhhhhhhhhh
		}
	}
	else if(strcmp(temp->name, "Expr") == 0)
	{
		CreatePlace(t1);
		CreatePlace(t2);
		code1 = translate_Exp(temp, t1);
		code2 = translate_Exp(temp->r->r, t2);
		code1 = LinkTAC(code1, code2);
		if(strcmp(temp->r->name, "PLUS") == 0)
		{
			CreatePlace(t3);
			code2 = CreateTAC("+", t1, t2, t3);
			LinkTAC(code1, code2);
			code2 = CreateTAC("if", t3, ">", "#0");
		}
		else if(strcmp(temp->r->name, "MINUS") == 0)
		{
			CreatePlace(t3);
			code2 = CreateTAC("-", t1, t2, t3);
			LinkTAC(code1, code2);
			code2 = CreateTAC("if", t3, ">", "#0");
		}
		else if(strcmp(temp->r->name, "STAR") == 0)
		{
			CreatePlace(t3);
			code2 = CreateTAC("*", t1, t2, t3);
			LinkTAC(code1, code2);
			code2 = CreateTAC("if", t3, ">", "#0");
		}
		else if(strcmp(temp->r->name, "DIV") == 0)
		{
			CreatePlace(t3);
			code2 = CreateTAC("/", t1, t2, t3);
			LinkTAC(code1, code2);
			code2 = CreateTAC("if", t3, ">", "#0");
		}
		else if(strcmp(temp->r->name, "MOD") == 0)
		{
			CreatePlace(t3);
			code2 = CreateTAC("\%", t1, t2, t3);
			LinkTAC(code1, code2);
			code2 = CreateTAC("if", t3, ">", "#0");
		}
		else if(strcmp(temp->r->name, "LESS") == 0)
			code2 = CreateTAC("if", t1, "<", t2);
		else if(strcmp(temp->r->name, "MORE") == 0)
			code2 = CreateTAC("if", t1, ">", t2);
		else if(strcmp(temp->r->name, "EQUAL") == 0)
			code2 = CreateTAC("if", t1, "==", t2);
		else if(strcmp(temp->r->name, "LESSEQUAL") == 0)
			code2 = CreateTAC("if", t1, "<=", t2);
		else if(strcmp(temp->r->name, "MOREEQUAL") == 0)
			code2 = CreateTAC("if", t1, ">=", t2);
		else if(strcmp(temp->r->name, "AND") == 0)
			code2 = CreateTAC("if", t1, "&&", t2);
		else if(strcmp(temp->r->name, "OR") == 0)
			code2 = CreateTAC("if", t1, "||", t2);
		else if(strcmp(temp->r->name, "NOTEQUAL") == 0)
			code2 = CreateTAC("if", t1, "!=", t2);
		return LinkTAC(code1, code2);
	}
	else if(strcmp(temp->name, "NOT") == 0)
	{
		CreatePlace(t1);
		code1 = translate_Exp(temp->r, t1);
		code2 = CreateTAC("if", t1, "==", "#0");
		return LinkTAC(code1, code2);
	}
	else if(strcmp(temp->name, "MINUS") == 0)
	{
		CreatePlace(t1);
		CreatePlace(t2);
		code1 = translate_Exp(temp->r, t1);
		code2 = CreateTAC("-", "#0", t1, t2);
		LinkTAC(code1, code2);
		code2 = CreateTAC("if", t2, ">", "#0");
		return LinkTAC(code1, code2);
	}
	else if(strcmp(temp->name, "READINTEGER") == 0)
	{
		CreatePlace(t1);
		code1 = translate_ReadInteger(temp, t1);
		code2 = CreateTAC("if", t1, ">", "#0");
		return LinkTAC(code1, code2);
	}
	else if(strcmp(temp->name, "NEW") == 0)
	{
		return NULL;
	}
	else if(strcmp(temp->name, "INSTANCEOF") == 0)
	{
		return NULL;                                           // hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh
	}
}
ThreeAddCode *translate_IfStmt(struct ast *a, char *l_out)
{
	ThreeAddCode *code1 = NULL, *code2 = NULL;
	struct ast* temp = a->l; 
	char t1[SIZE], l1[SIZE], l2[SIZE], l3[SIZE];
	temp = temp->r->r;                //boolexpr

	/*CreatePlace(t1);
	code1 = translate_Exp(temp->l, t1);
	temp = temp->r->r;
	code2 = CreateTAC("if", "t1", "==", "#1");code1 = LinkTAC(code1, code2);*/
	code1 = translate_BoolExp(temp);temp = temp->r->r;

	CreateLable(l1);
	code2 = CreateTAC("Goto", l1, NULL, NULL);code1 = LinkTAC(code1, code2);
	CreateLable(l2);
	code2 = CreateTAC("Goto", l2, NULL, NULL);code1 = LinkTAC(code1, code2);
	code2 = CreateTAC("Lable", l1, ":", NULL);code1 = LinkTAC(code1, code2);
	code2 = translate_Stmt(temp, l_out);code1 = LinkTAC(code1, code2);
	if(temp->r == NULL) 
	{
		code2 = CreateTAC("Lable", l2, ":", NULL);code1 = LinkTAC(code1, code2);
	}
	else    // else
	{
		CreateLable(l3);
		code2 = CreateTAC("Goto", l3, NULL, NULL);code1 = LinkTAC(code1, code2);
		code2 = CreateTAC("Lable", l2, ":", NULL);code1 = LinkTAC(code1, code2);
		code2 = translate_Stmt(temp->r->r, l_out);code1 = LinkTAC(code1, code2);
		code2 = CreateTAC("Lable", l3, ":", NULL);code1 = LinkTAC(code1, code2);
	}
	return code1;
}
ThreeAddCode *translate_WhileStmt(struct ast *a, char *l_out)
{
	ThreeAddCode *code1, *code2;
	struct ast* temp = a->l;
	char t1[SIZE], l1[SIZE], l2[SIZE];
	temp = temp->r->r;          //boolexpr
	CreateLable(l1);
	CreateLable(l2);
	code1 = CreateTAC("Lable", l1, ":",  NULL);

	/*CreatePlace(t1);
	code2 = translate_Exp(temp->l, t1);code1 = LinkTAC(code1, code2);
	code2 = CreateTAC("if", "t1", "==", "#1");code1 = LinkTAC(code1, code2);*/
	code2 = translate_BoolExp(temp); code1 = LinkTAC(code1, code2);

	code2 = CreateTAC("Goto", l2, NULL, NULL);code1 = LinkTAC(code1, code2);
	code2 = CreateTAC("Goto", l_out, NULL, NULL);code1 = LinkTAC(code1, code2);
	code2 = CreateTAC("Lable", l2, ":", NULL);code1 = LinkTAC(code1, code2);
	temp = temp->r->r;
	code2 = translate_Stmt(temp, l_out);code1 = LinkTAC(code1, code2);
	code2 = CreateTAC("Goto", l1, NULL, NULL);code1 = LinkTAC(code1, code2);
	return code1;
}
ThreeAddCode *translate_ReturnStmt(struct ast *a)
{
	ThreeAddCode *code1 = NULL, *code2 = NULL;
	struct ast *temp = a->l;            //return
	char t1[SIZE];
	if(temp->r == NULL)
	{
		return CreateTAC("return", NULL, NULL, NULL);
	}
	else               //return expr
	{
		CreatePlace(t1);
		code1 = translate_Exp(temp->r, t1);
		code2 = CreateTAC("return", t1, NULL, NULL);
		return LinkTAC(code1, code2);
	}
}
ThreeAddCode *translate_PrintStmt(struct ast *a)
{
	ThreeAddCode *code1 = NULL, *code2 = NULL;
	ParmList *parm_list = NULL;
	struct ast *temp = a->l->r->r;
	code1 = translate_Parm(temp, &parm_list);
	while(parm_list != NULL)
	{
		code2 = CreateTAC("PARAM", parm_list->t, NULL, NULL);
		LinkTAC(code1, code2);
		parm_list = parm_list->next;
	}
	code2 = CreateTAC("CALL", "Print", NULL, NULL);
	return LinkTAC(code1, code2);
}
ThreeAddCode *translate_StmtBlock(struct ast *a, char *l_out)
{
	struct ast *temp = a->l->r;   //stmtlist
	ThreeAddCode *code1 = NULL, *code2 = NULL;
	while(temp->l != NULL)
	{
		temp = temp->l;
		code2 = translate_Stmt(temp, l_out);
		code1 = LinkTAC(code1, code2);
		temp = temp->r;
	}
	return code1;
}
ThreeAddCode *translate_Stmt(struct ast *a, char *l_out)
{
	struct ast *temp = a->l;
	char l1[SIZE];
	ThreeAddCode *code1, *code2;
	if(strcmp(temp->name, "IfStmt") == 0)
	{
		return translate_IfStmt(temp, l_out);
	}
	else if(strcmp(temp->name, "Variable") == 0)
	{
		Var *pv = temp->var_h;
		char t1[SIZE];
		CreatePlace(t1);
		pv->place = atoi(t1+2);
		return NULL;
	}
	else if(strcmp(temp->name, "SimpleStmt") == 0)
	{
		return translate_SimpleStmt(temp);
	}
	else if(strcmp(temp->name, "WhileStmt") == 0)
	{
		CreateLable(l1);
		code1 = translate_WhileStmt(temp, l1);
		code2 = CreateTAC("Lable", l1, ":", NULL);
		return LinkTAC(code1, code2);
	}
	else if(strcmp(temp->name, "ForStmt") == 0)
	{
		CreateLable(l1);
		code1 = translate_ForStmt(temp, l1);
		code2 = CreateTAC("Lable", l1, ":", NULL);
		return LinkTAC(code1, code2);
	}
	else if(strcmp(temp->name, "BreakStmt") == 0)
	{
		code1 = CreateTAC("Goto", l_out, NULL, NULL);
		return code1;
	}
	else if(strcmp(temp->name, "ReturnStmt") == 0)
	{
		return translate_ReturnStmt(temp);
	}
	else if(strcmp(temp->name, "PrintStmt") == 0)
	{
		return translate_PrintStmt(temp);
	}
	else if(strcmp(temp->name, "StmtBlock") == 0)
	{
		return translate_StmtBlock(temp, l_out);
	}
	else return NULL;
}
ThreeAddCode *translate_SimpleStmt(struct ast* a)
{
	struct ast *temp = a->l;
	char t1[SIZE], t2[SIZE];
	ThreeAddCode *code1 = NULL, *code2 = NULL;
	if(strcmp(temp->name, "LValue") == 0)
	{
		CreatePlace(t1);
		code1 = translate_Lvalue(temp, t1);
		temp = temp->r->r;
		CreatePlace(t2);
		code2 = translate_Exp(temp, t2);
		code1 = LinkTAC(code1, code2);
		code2 = CreateTAC("=", t2, NULL, t1);
		return LinkTAC(code1, code2);
	}
	else     //call
	{
		CreatePlace(t1);
		return translate_Call(temp, t1);
	}
}
ThreeAddCode *translate_ForStmt(struct ast* a, char *l_out)
{
	ThreeAddCode *code1, *code2;
	struct ast* temp = a->l;
	char t1[SIZE], l1[SIZE], l2[SIZE];
	temp = temp->r->r;              //simplestmt
	code1 = translate_SimpleStmt(temp);
	temp = temp->r->r;             //boolexpr
	CreateLable(l1);
	CreateLable(l2);
	code2 = CreateTAC("Lable", l1, ":",  NULL);code1 = LinkTAC(code1, code2);

	/*CreatePlace(t1);
	code2 = translate_Exp(temp->l, t1);code1 = LinkTAC(code1, code2);
	code2 = CreateTAC("if", "t1", "==", "#1");code1 = LinkTAC(code1, code2);*/
	code2 = translate_BoolExp(temp); code1 = LinkTAC(code1, code2);

	code2 = CreateTAC("Goto", l2, NULL, NULL);code1 = LinkTAC(code1, code2);
	code2 = CreateTAC("Goto", l_out, NULL, NULL);code1 = LinkTAC(code1, code2);
	code2 = CreateTAC("Lable", l2, ":", NULL);code1 = LinkTAC(code1, code2);
	temp = temp->r->r;
	code2 = translate_Stmt(temp->r->r, l_out);code1 = LinkTAC(code1, code2);
	code2 = translate_SimpleStmt(temp);code1 = LinkTAC(code1, code2);
	code2 = CreateTAC("Goto", l1, NULL, NULL);code1 = LinkTAC(code1, code2);
	return code1;
}
ThreeAddCode *translate_Lvalue(struct ast* a, char *t_value)
{
	struct ast *temp = a->l;
	Var *pv;
	Class *pc;
	char t1[SIZE], t2[SIZE],s[20];
	ThreeAddCode *code1 = NULL, *code2;
	if(strcmp(temp->name, "ID")==0)
	{
		pv = findIdVar(temp->idtype, glo, pclass, pfun, temp->line, temp->rol);
		sprintf(t_value, "_T%d", pv->place);
		return code1;
	}
	else if(temp->type->type == 6) //lvalue : expr lp expr rp
	{
		// array  ,it can be a[][][]; come on
		CreatePlace(t1);
		code1 = translate_Exp(temp, t1);
		CreatePlace(t2);
		code2 = translate_Exp(temp->r->r, t2);code1 = LinkTAC(code1, code2);
		code2 = CreateTAC("*", t2, "#4", t2);code1 = LinkTAC(code1, code2);
		code2 = CreateTAC("+", t1, t2, t_value);code1 = LinkTAC(code1, code2);
		return code1;
	}
	else // lvalue : expr dot id
	{    
		pc = findIdClass(temp->type->idtype, glo, temp->line, temp->rol);
		pv = pc->var_l;
		CreatePlace(t1);
		code1 = translate_Exp(temp, t1);
		temp = temp->r->r;
		int i = 4;
		while(strcmp(pv->name, temp->idtype) != 0) 
		{
			i = i+ 4;
			pv = pv->next;
		}
		itoa(i, s);
		code2 = CreateTAC("+", t1, s, t_value);
		return LinkTAC(code1, code2);
	}
}
ThreeAddCode *translate_Parm(struct ast* a, ParmList **pph)
{
	if(a->l == NULL) return NULL;
	else
	{
		ParmList *pm, *ph;
		ThreeAddCode *code1 = NULL, *code2 = NULL;
		struct ast *temp = a->l->l;			//actual: actualList : expr comma actualList
		pm = (ParmList *)malloc(sizeof(ParmList));
		CreatePlace(pm->t);
		code2 = translate_Exp(temp, pm->t);
		LinkTAC(code1, code2);
		ph = pm;
		while(temp->r != NULL)
		{	
			temp = temp->r->r->l;
			pm = (ParmList *)malloc(sizeof(ParmList));
			CreatePlace(pm->t);
			code2 = translate_Exp(temp, pm->t);
			code1 = LinkTAC(code1, code2);
		 	pm->next = ph;                          //ni xu ru zhan
		 	ph = pm;
		}
		*pph = pm;	
		return code1;
	}
}
ThreeAddCode *translate_Call(struct ast* a, char *t_in)
{
	struct ast *temp = a->l;
	Fun *pf;
	Class *pc;
	ThreeAddCode *code1 = NULL, *code2 = NULL;
	char s[50];
	int i;
	ParmList *parm_list = NULL;
	if(strcmp(temp->name, "ID") == 0)
	{
		pf = findIdFun(temp->idtype, glo, pclass, temp->line, temp->rol);
		strcat(strcat(strcat(strcpy(s, "_"), pclass->name),"_"), pf->name);
		code1 = translate_Parm(temp->r->r, &parm_list);
	}
	else   //fun from other class
	{
		pc = findIdClass(temp->type->idtype, glo, temp->line, temp->rol);
		temp = temp->r->r;
		pf = findIdFun(temp->idtype, NULL, pc, temp->line, temp->rol);
		strcat(strcat(strcat(strcpy(s, "_"), pc->name),"_"), pf->name);
		code1 = translate_Parm(temp->r->r, &parm_list);
	}
	while(parm_list != NULL)
	{
		code2 = CreateTAC("ARG", parm_list->t, NULL, NULL);
		code1 = LinkTAC(code1, code2);
		parm_list = parm_list->next;
	}
	if(pf->returnType->type == 4)        //void
		code2 = CreateTAC("CALL", s, NULL, NULL);
	else 
		code2 = CreateTAC("CALL", s, NULL, t_in);
	return LinkTAC(code1, code2);
}
ThreeAddCode *translate_ReadInteger(struct ast *a, char *t_in)
{
	ThreeAddCode *code1, *code2;
	code1 = CreateTAC("ARG", t_in, NULL, NULL);
	code2 = CreateTAC("CALL", "ReadInteger", NULL, NULL);
	return LinkTAC(code1, code2);
}
ThreeAddCode *translate_Exp(struct ast *a, char *t_in)
{
	ThreeAddCode *code1, *code2, *code3;
	struct ast *temp = a->l;
	char s1[20], s2[20];
	char t1[SIZE], t2[SIZE];
	int place;
	if(strcmp(temp->name, "Constant") == 0)
	{
		if(strcmp(temp->l->name, "INTC") == 0) 
			sprintf(s1, "#%d", temp->l->inter);
		else if(strcmp(a->l->name, "FLOATC") == 0)
			sprintf(s1, "#%f", temp->l->flo);
		code1 = CreateTAC("=", s1, NULL, t_in);
		return code1;
	}
	else if(strcmp(temp->name, "LValue") == 0)
	{
		CreatePlace(t1);
		code1 = translate_Lvalue(temp, t1);
		strcpy(t_in, t1);
		return code1;
	}
	else if(strcmp(temp->name, "THIS") == 0)
	{

	}
	else if(strcmp(temp->name, "Call") == 0)
	{
		CreatePlace(t1);
		code1 = translate_Call(temp, t1);
		code2 = CreateTAC("=", t1, NULL, t_in);
		return LinkTAC(code1, code2);
	}
	else if(strcmp(temp->name, "LP") == 0)
	{
		if(strcmp(temp->r->name, "Expr") == 0) 
		{
			return translate_Exp(a->r, t_in);
		}
		else //lp class id rp expr
		{
			return NULL; 								//hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh
		}
	}
	else if(strcmp(temp->name, "Expr") == 0)
	{
		CreatePlace(t1);
		CreatePlace(t2);
		code1 = translate_Exp(temp, t1);
		code2 = translate_Exp(temp->r->r, t2);
		if(strcmp(temp->r->name, "PLUS") == 0)
			code3 = CreateTAC("+", t1, t2, t_in);
		else if(strcmp(temp->r->name, "MINUS") == 0)
			code3 = CreateTAC("-", t1, t2, t_in);
		else if(strcmp(temp->r->name, "STAR") == 0)
			code3 = CreateTAC("*", t1, t2, t_in);
		else if(strcmp(temp->r->name, "DIV") == 0)
			code3 = CreateTAC("/", t1, t2, t_in);
		else if(strcmp(temp->r->name, "MOD") == 0)
			code3 = CreateTAC("\%", t1, t2, t_in);
		else if(strcmp(temp->r->name, "LESS") == 0)
			code3 = CreateTAC("<", t1, t2, t_in);
		else if(strcmp(temp->r->name, "MORE") == 0)
			code3 = CreateTAC(">", t1, t2, t_in);
		else if(strcmp(temp->r->name, "EQUAL") == 0)
			code3 = CreateTAC("==", t1, t2, t_in);
		else if(strcmp(temp->r->name, "LESSEQUAL") == 0)
			code3 = CreateTAC("<=", t1, t2, t_in);
		else if(strcmp(temp->r->name, "MOREEQUAL") == 0)
			code3 = CreateTAC(">=", t1, t2, t_in);
		else if(strcmp(temp->r->name, "AND") == 0)
			code3 = CreateTAC("&&", t1, t2, t_in);
		else if(strcmp(temp->r->name, "OR") == 0)
			code3 = CreateTAC("||", t1, t2, t_in);
		else if(strcmp(temp->r->name, "NOTEQUAL") == 0)
			code3 = CreateTAC("!=", t1, t2, t_in);
		return LinkTAC(LinkTAC(code1, code2), code3);
	}
	else if(strcmp(temp->name, "NOT") == 0)
	{
		CreatePlace(t1);
		code1 = translate_Exp(temp->r, t1);
		code2 = CreateTAC("~", t1, NULL, t_in);
		return LinkTAC(code1, code2);
	}
	else if(strcmp(temp->name, "MINUS") == 0)
	{
		CreatePlace(t1);
		code1 = translate_Exp(temp->r, t1);
		code2 = CreateTAC("-", "#0", t1, t_in);
		return LinkTAC(code1, code2);
	}
	else if(strcmp(temp->name, "READINTEGER") == 0)
	{
		CreatePlace(t1);
		code1 = translate_ReadInteger(temp, t1);
		code2 = CreateTAC("=", t1, NULL, t_in);
		return LinkTAC(code1, code2);
	}
	else if(strcmp(temp->name, "NEW") == 0)
	{
		temp = temp->r;
		if(strcmp(temp->name, "ID") == 0) //class
		{
			strcat(strcpy(s1, "_"), strcat(strcat(strcpy(s2, temp->idtype), "_"), "New"));
			code1 = CreateTAC("CALL", s1, NULL, NULL);
			return code1;
		}
		else    // array    new type lp expr rp
		{
			CreatePlace(t1);
			code1 = translate_Exp(temp->r->r, t1);
			CreatePlace(t2);
			code2 = CreateTAC("*", t1, "#4", t2); code1 = LinkTAC(code1, code2);
			code2 = CreateTAC("ARG", t2, NULL, NULL);code1 = LinkTAC(code1, code2); 		
		    code1 = CreateTAC("CALL", "Alloc", NULL, t_in);code1 = LinkTAC(code1, code2);//malloc
		    return code1;
		}
	}
	else if(strcmp(temp->name, "INSTANCEOF") == 0)
	{
		return NULL;                               //hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh
	}
}
	
void printTAC()
{
	CodeLine *temp = NULL;
	if(code != NULL) temp = code->head;
	while(temp != NULL)
	{
		if(strcmp(temp->op, "+") == 0 || strcmp(temp->op, "-") == 0 ||strcmp(temp->op, "*") == 0 ||\
			strcmp(temp->op, "/") == 0 || strcmp(temp->op, "<") == 0 || strcmp(temp->op, ">") == 0 ||\
			strcmp(temp->op, "<=") == 0 || strcmp(temp->op, ">=") == 0 || strcmp(temp->op, "==") == 0 ||\
			strcmp(temp->op, "&&") == 0 || strcmp(temp->op, "||") == 0 || strcmp(temp->op, "!=") == 0 ||\
			strcmp(temp->op, "~") == 0)
		{
			printf("%s = %s %s %s\n", temp->place[2], temp->place[0], temp->op, temp->place[1]);
		}
		else if(strcmp(temp->op, "=") == 0)
		{
			printf("%s %s %s %s\n", temp->place[2], temp->op, temp->place[0], temp->place[1]);
		}
		else if(strcmp(temp->op, "**") == 0)
		{
			printf("*(%s) = %s\n", temp->place[2], temp->place[0]);
		}
		else if(strcmp(temp->op, "FUNCTION") == 0)
		{
			printf("\n%s %s %s\n", temp->op, temp->place[0], temp->place[1]);
		}
		else if(strcmp(temp->op, "if") ==0 )
		{
			printf("%s %s %s %s ", temp->op, temp->place[0], temp->place[1], temp->place[2]);
			temp = temp->next;
			printf("%s %s %s\n", temp->op, temp->place[0], temp->place[1]);
		}
		else if(strcmp(temp->op, "CALL") ==0 && strlen(temp->place[2]) != 0)
		{
			printf("%s = %s %s\n", temp->place[2], temp->op, temp->place[0]);
		}
		else printf("%s %s %s %s\n", temp->op, temp->place[0], temp->place[1], temp->place[2]);
		temp = temp->next;
	}
}