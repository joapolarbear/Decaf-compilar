#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "main.h"
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
void addGlo(Global *glo, struct ast* a, struct ast* b)
{
	if(a->class_t == NULL) 
	{
		glo->class_l = b->class_h;

	}
	else
	{
		a->class_t->next = b->class_h;
		glo->class_l = a->class_h;	
	}
}
void addClass(struct ast* dest, struct ast* name, struct ast* child)
{
	Class *new_class = (Class *)malloc(sizeof(Class));
	strcpy(new_class->name, name->idtype);
	new_class->line = name->line;
	new_class->rol = name->rol;
	new_class->var_l = child->var_h;
	new_class->fun_l = child->fun_h;
	new_class->next = NULL;
	dest->class_h = dest->class_t = new_class;
}
void passClass(struct ast* dest, int num, ...)
{
	va_list valist;
	int i = 1;
	struct ast* temp = (struct ast*)malloc(sizeof(struct ast));
	va_start(valist, num);
	while(i <= num)
	{
		temp = va_arg(valist, struct ast*);
		if(dest->class_t == NULL)
		{
			dest->class_h = temp->class_h;
			dest->class_t = temp->class_t;
		}
		else
		{
			dest->class_t->next = temp->class_h;
			dest->class_t = temp->class_t;
		}
		i++;
	}
}
void addFun(struct ast* dest, struct ast* name, struct ast* reType, struct ast* formal, struct ast* stmt, int is_static, Global *glo)
{
	Fun* new_fun = (Fun *)malloc(sizeof(Fun));
	strcpy(new_fun->name, name->idtype);
	new_fun->returnType = reType->type;
	new_fun->line = reType->line;
	new_fun->rol = reType->rol;
	new_fun->form_l = formal->var_h;
	new_fun->var_l = stmt->var_h;
	new_fun->next = NULL;
	dest->fun_h = dest->fun_t = new_fun;
	if(is_static) //is_static = 1, is static, is global
	{
 		Fun *pf = glo->fun_l;
		if(pf == NULL) glo->fun_l = new_fun;
		else
		{
			while(pf->next != NULL) pf = pf->next;
			pf->next = new_fun;
		}
	}
}
void passFun(struct ast* dest, int num, ...)
{
	va_list valist;
	int i = 1;
	struct ast* temp = (struct ast*)malloc(sizeof(struct ast));
	va_start(valist, num);
	while(i <= num)
	{
		temp = va_arg(valist, struct ast*);
		if(dest->fun_t == NULL)
		{
			dest->fun_h = temp->fun_h;
			dest->fun_t = temp->fun_t;
		}
		else
		{
			dest->fun_t->next = temp->fun_h;
			dest->fun_t = temp->fun_t;
		}
		i++;
	}
}
void addVar(struct ast* dest, struct ast* name, struct ast* type)
{
	Var *new_var = (Var *)malloc(sizeof(Var));
	strcpy(new_var->name, name->idtype);
	new_var->type = type->type;
	new_var->line = type->line;
	new_var->rol = type->rol;
	new_var->next = NULL;
	if(dest->var_t == NULL)
	{
		dest->var_h = dest->var_t = new_var;
	}
	else
	{
		dest->var_t->next = new_var;
		dest->var_t = new_var;
	}
}
void passVar(struct ast* dest, int num, ...)
{
	va_list valist;
	int i = 1;
	struct ast* temp = (struct ast*)malloc(sizeof(struct ast));
	va_start(valist, num);
	while(i <= num)
	{
		temp = va_arg(valist, struct ast*);
		if(dest->var_t == NULL)
		{
			dest->var_h = temp->var_h;
			dest->var_t = temp->var_t;
		}
		else
		{
			dest->var_t->next = temp->var_h;
			dest->var_t = temp->var_t;
		}
		i++;
	}
}
void addType(struct ast* a, int type, struct ast* b)
{
	if(a != NULL)
	{
		struct typelist *temp = (struct typelist *)malloc(sizeof(struct typelist));
		temp->next = NULL; 
		if(type >= 0 && type <= 4)
		{
			temp->type = type;
			a->type = temp;
		}
		else if(type == -1)
		{
			temp->type = -1;
			a->type = temp;
		}
		else if(type == 5)
		{
			temp->type = 5;
			temp->idtype = (char *)malloc(sizeof(char)*40);
			strcpy(temp->idtype, b->idtype);
			a->type = temp; 
		}
		else if(type == 6)
		{
			temp->type = 6;
			temp->next = b->type;
			a->type = temp;
		}
	}
}
void addError(Error **perror_h, Error **perror_t, int row, int rol, char *descri)
{
	Error *temp = (Error *)malloc(sizeof(Error));
	temp->row = row;
	temp->rol = rol;
	temp->s = (char *)malloc(100*sizeof(char));
	strcpy(temp->s, descri);
	temp->next = NULL;
	if(*perror_t == NULL)
	{
		*perror_h = *perror_t = temp;
	}
	else
	{
		(*perror_t)->next = temp;
		*perror_t = temp;
	}
}
void checkType(Error **perror_h, Error **perror_t, int calType, int row, int rol, int num, ...)
//num = 1,check arg is bool,if num > 1, check if args match
{
	va_list valist;
	int i = 1, first, follow;
	va_start(valist, num);
	struct ast* temp = (struct ast*)malloc(sizeof(struct ast));
	temp = va_arg(valist, struct ast*);
	first = temp->type->type;
	if(calType == 0 && first != 0 && first != 2)
	{
		addError(perror_h, perror_t, row, rol, "Expression type error!");
	}
	else if(calType == 1 && first != 1)
		addError(perror_h, perror_t, row, rol, "Expression type error!Need bool!");
	if(num > 1)
	{
		while(i < num)
		{
			temp = va_arg(valist, struct ast*);
			follow = temp->type->type;
			if(calType == 0)
			{
				if(follow != 0 && follow != 2)
					addError(perror_h, perror_t, row, rol, "Expression type error!");
				else if(first != follow)
					addError(perror_h, perror_t, row, rol, "Expression type dismatch!");
			}
			else if(calType = 1)
			{
				if(follow != 1)
					addError(perror_h, perror_t, row, rol, "Expression type error!Need bool!");
			}
			i++;
		}
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
int typeMatch(struct typelist *a, struct typelist *b)
//类型列表相同则返回0.否则返回1
{
	if(a == NULL && b == NULL) return 0;
	else if(a == NULL || b == NULL) return 1;
	else if(a->type == 5 && b->type == 5 && strcmp(a->idtype, b->idtype) == 0)
		return typeMatch(a->next, b->next);
	else if(a->type == b->type)
		return typeMatch(a->next, b->next);
	else return 1;
}
int formalTypeMatch(struct typelist *a, Var *form)
{
	if(a == NULL && form == NULL) return 0;
	else{
		struct typelist *formtype = form->type;
		while(a != NULL && form != NULL)
		{
			if(a->type == 5 && form->type->type == 5 && strcmp(a->idtype, form->type->idtype) == 0)
			{
				a = a->next; form = form->next;
			}
			else if(a->type == form->type->type){
				a = a->next; form = form->next;
			}
			else return 1;
		}
		if(a == NULL && form != NULL) return 1;
		else return formalTypeMatch(a, form->next);
	}
}
void copy(struct typelist *a, struct typelist *b)
{
	if(a != NULL)
	{
		a->type = b->type;
		if(b->type == 5)
		{
			a->idtype = (char *)malloc(sizeof(char)*40);
			strcpy(a->idtype, b->idtype);
		}
		a->next = b->next;
	}
}
void checkReturnType(struct ast *a, struct typelist *returnType, Error **perror_h, Error **perror_t)
{
	if(a == NULL) return;
	else if(strcmp(a->name, "ReturnStmt") == 0)
	{
		//printf("%d\n", a->type->type);
		if(typeMatch(returnType, a->type) == 1) addError(perror_h, perror_t, a->line, a->rol, "Function definition error, return type does not match!");
	}
	else
	{
		checkReturnType(a->l, returnType, perror_h, perror_t);
		checkReturnType(a->r, returnType, perror_h, perror_t);
	}
}
Class *findIdClass(char *classname, Global *glo, int line, int rol)
{
	if(glo != NULL)
	{
		Class *pc = glo->class_l;
		while(pc != NULL && (pc->line < line ||(pc->line == line && pc->rol < rol))) 
		{
			if(strcmp(classname, pc->name) == 0) return pc;
			pc = pc->next;
		}
	}
	return NULL;
}
Var *findIdVar(char *varname, Global *glo, Class *pclass,Fun *pfun, int line, int rol)
{
	Var *pv;
	if(pfun != NULL)
	{
		pv = pfun->var_l;
		while(pv != NULL && (pv->line < line ||(pv->line == line && pv->rol < rol))) 
		{
			if(strcmp(varname, pv->name) == 0) return pv;
			pv = pv->next;
		}
		//检查函数参数表中是否有该变量
		pv = pfun->form_l;
		while(pv != NULL && (pv->line < line ||(pv->line == line && pv->rol < rol))) 
		{
			if(strcmp(varname, pv->name) == 0) return pv;
			pv = pv->next;
		}
	}
	if(pclass != NULL)
	{
		pv = pclass->var_l;
		while(pv != NULL && (pv->line < line ||(pv->line == line && pv->rol < rol))) 
		{
			if(strcmp(varname, pv->name) == 0) return pv;
			pv = pv->next;
		}
	}
	if(glo != NULL)
	{
		pv = glo->var_l;
		while(pv != NULL && (pv->line < line ||(pv->line == line && pv->rol < rol))) 
		{
			if(strcmp(varname, pv->name) == 0) return pv;
			pv = pv->next;
		}
	}
	return NULL;
}
Fun *findIdFun(char *funname, Global *glo, Class *pclass, int line ,int rol)
{
	Fun *pf;
	if(pclass != NULL)
	{
		pf = pclass->fun_l;
		while(pf != NULL && (pf->line < line ||(pf->line == line && pf->rol < rol))) 
		{
			if(strcmp(funname, pf->name) == 0) return pf;
			pf = pf->next;
		}
	}
	if(glo != NULL)
	{
		pf = glo->fun_l;
		while(pf != NULL && (pf->line < line ||(pf->line == line && pf->rol < rol))) 
		{
			if(strcmp(funname, pf->name) == 0) return pf;
			pf = pf->next;
		}
	}
	return NULL;
}
void map2(struct ast *tree, Error **perror_h, Error **perror_t, Global *glo, Class **pclass, Fun **pfun)
{

	if(tree == NULL || tree->rol == 0) return;   //空或者产生式为空的 直接返回
	//printf("%s\n", tree->name);
	if(strcmp(tree->name, "Program") == 0)
	{
		map2(tree->l, perror_h, perror_t, glo, pclass, pfun);
	}
	else if(strcmp(tree->name, "ClassDef") == 0)
	{
		//pclass 指向当前class
		*pclass = tree->class_h;
		*pfun = NULL;
		map2(tree->l, perror_h, perror_t, glo, pclass, pfun);
		map2(tree->r, perror_h, perror_t, glo, pclass, pfun);
	}
	else if(strcmp(tree->name, "FunctionDef") == 0)
	{
		*pfun = tree->fun_h;
		//printf("%d\n", (*pfun)->returnType->type);
		struct ast *temp = tree->l;
		while(strcmp(temp->name,"StmtBlock")!=0) temp = temp->r;
		checkReturnType(temp, (*pfun)->returnType, perror_h, perror_t);
		map2(tree->l, perror_h, perror_t, glo, pclass, pfun);
		map2(tree->r, perror_h, perror_t, glo, pclass, pfun);
	}
	else if(strcmp(tree->name, "Call") == 0)
	{
		struct ast *temp = tree->l;
		Fun *pf;
		Class *c;
		if(strcmp(temp->name, "ID") == 0)
		{
			pf = findIdFun(temp->idtype, glo, *pclass, tree->line, tree->rol);
			if(pf != NULL) 
			{
				temp = temp->r->r;
				if(formalTypeMatch(temp->type, pf->form_l) == 1) addError(perror_h, perror_t, tree->line, tree->rol, "Function formals do not match!");
			}
			map2(temp->r, perror_h, perror_t, glo, pclass, pfun);
			map2(tree->r, perror_h, perror_t, glo, pclass, pfun);
		}
		else
		{
			map2(temp, perror_h, perror_t, glo, pclass, pfun);
			if(temp->type->type == 5)
			{
				//判断被引用类是否已经在被引用前声明
				c = findIdClass(temp->type->idtype, glo, tree->line, tree->rol);
				if(c != NULL)  //class has been defined
				{
					pf = findIdFun(temp->r->r->idtype, NULL, c, tree->line, tree->rol);
					if(pf != NULL) 
					{
						temp = temp->r->r->r->r;
						if(formalTypeMatch(temp->type, pf->form_l) == 1) addError(perror_h, perror_t, tree->line, tree->rol, "Function formals do not match!");
					}
				}	
			}
			map2(tree->r, perror_h, perror_t, glo, pclass, pfun);
		}
	}
	else if(strcmp(tree->name, "Expr") == 0)
	{
		struct ast *temp = tree->l;
		if(strcmp(temp->name, "Expr") == 0)
		{
			if(typeMatch(temp->type, temp->r->r->type) == 1) 
				addError(perror_h, perror_t, temp->r->line, temp->r->rol, "Invalid operands to binary operation!");
			if(strcmp(temp->r->name, "PLUS") == 0 || strcmp(temp->r->name, "MINUS") == 0 || strcmp(temp->r->name, "STAR") == 0\
				|| strcmp(temp->r->name, "DIV") == 0 || strcmp(temp->r->name, "MOD") == 0)
			{
				if((temp->type->type != 0 && temp->type->type != 2)||(temp->r->r->type->type != 0 && temp->r->r->type->type != 2))
					addError(perror_h, perror_t, temp->r->line, temp->r->rol, "Invalid operands domain to binary operation!");
			}
			else //true or false
			{
				if(temp->type->type != 1 ||temp->r->r->type->type != 1)
					addError(perror_h, perror_t, temp->r->line, temp->r->rol, "Invalid operands domain to binary operation!Need type Bool!");
			}
		}
		else if(strcmp(temp->name, "MINUS") == 0)
		{
			if(temp->r->type->type != 0 && temp->r->type->type != 2) 
				addError(perror_h, perror_t, temp->line, temp->rol, "Invalid operands domain to binary operation!");
		}
		else if(strcmp(temp->name, "NOT") == 0)
		{
			if(temp->r->type->type != 1) 
				addError(perror_h, perror_t, temp->line, temp->rol, "Invalid operands domain to binary operation!Need type Bool!");
		}
		map2(tree->l, perror_h, perror_t, glo, pclass, pfun);
		map2(tree->r, perror_h, perror_t, glo, pclass, pfun);
	}
	else if(strcmp(tree->name, "BoolExpr") == 0)
	{
		if(tree->type->type != 1) addError(perror_h, perror_t, tree->line, tree->rol, "Type error, type Bool is wanted!");
		map2(tree->l, perror_h, perror_t, glo, pclass, pfun);
		map2(tree->r, perror_h, perror_t, glo, pclass, pfun);
	}
	else if(strcmp(tree->name, "LValue") == 0)
	{
		struct ast *temp = tree->l;
		if(strcmp(temp->name, "Expr") == 0 && strcmp(temp->r->r->name, "Expr") == 0)
		{
			if(temp->type->type != 6) 
				addError(perror_h, perror_t, temp->line, temp->rol, "Undefined array!");
			if(temp->r->r->type->type != 0)
				addError(perror_h, perror_t, temp->r->r->line, temp->r->r->rol, "Do not use an integer to access array!");
		}
		map2(tree->l, perror_h, perror_t, glo, pclass, pfun);
		map2(tree->r, perror_h, perror_t, glo, pclass, pfun);
	}
	else
	{
		map2(tree->l, perror_h, perror_t, glo, pclass, pfun);
		map2(tree->r, perror_h, perror_t, glo, pclass, pfun);
	}
}
void map1(struct ast *tree, Error **perror_h, Error **perror_t, Global *glo, Class **pclass, Fun **pfun)
{
	if(tree == NULL || tree->rol == 0) return;   //空或者产生式为空的 直接返回
	//printf("%s\n", tree->name);
	if(strcmp(tree->name, "Program") == 0)
	{
		map1(tree->l, perror_h, perror_t, glo, pclass, pfun);
	}
	else if(strcmp(tree->name, "ClassDef") == 0)
	{
		//pclass 指向当前class
		//if(*pclass == NULL)  *pclass = glo->class_l;
		//else *pclass = (*pclass)->next;
		*pclass = tree->class_h;
		*pfun = NULL;	
		//检查是否重复定义
		struct ast *temp = tree->l;
		Class *pc;
		pc = findIdClass(temp->r->idtype, glo, tree->line, tree->rol);
		if(pc != NULL) addError(perror_h, perror_t, temp->r->line, temp->r->rol, "Redefinition!");
		//检查扩展是否正确
		pc = glo->class_l;
		while(temp!=NULL && strcmp(temp->name, "EXTENDS") != 0) temp = temp->r; 	
		if(temp != NULL)
		{
			temp = temp->r;
			while(strcmp(pc->name, (*pclass)->name)!= 0 && strcmp(pc->name, temp->idtype)!=0) pc = pc->next;	//查找父亲
			if(strcmp(pc->name, (*pclass)->name) == 0) addError(perror_h, perror_t, temp->line, temp->rol, "Illegal inheritance!"); 	//未找到父亲，报错
			else	//找到父亲,则把父亲的变量和函数都链接到该子类的变量函数链表上，且能实现重名变量就近引用。
			{
				Fun *pf = (*pclass)->fun_l;
				Var *pv = (*pclass)->var_l;
				if(pf == NULL) pf = pc->fun_l;
				else
				{
					while(pf->next != NULL) pf = pf->next;
					pf->next = pc->fun_l;
				}
				if(pv == NULL) pv = pc->var_l;
				else
				{
					while(pv->next != NULL) pv = pv->next;
					pv->next = pc->var_l;
				}

			}
		}
		temp = tree->l;
		while(temp!=NULL && strcmp(temp->name, "Field") != 0) temp = temp->r; 
		map1(temp, perror_h, perror_t, glo, pclass, pfun);
		map1(tree->r, perror_h, perror_t, glo, pclass, pfun);
	}
	else if(strcmp(tree->name, "FunctionDef") == 0)
	{
		//if(*pfun == NULL)  *pfun = (*pclass)->fun_l;
		//else *pfun = (*pfun)->next;
		*pfun = tree->fun_h;
		struct ast *temp = tree->l;

		while(strcmp(temp->name, "Type") != 0) temp = temp->r;
		map1(temp, perror_h, perror_t, glo, pclass, pfun);
		(*pfun)->returnType = temp->type;
		//printf("%d\n", (*pfun)->returnType->type);

		while(strcmp(temp->name, "ID") != 0) temp = temp->r;
		Fun *pf = findIdFun(temp->idtype, glo, *pclass, tree->line, tree->rol);
		Class *pc = findIdClass(temp->idtype, glo, tree->line, tree->rol);
		if(pf != NULL || pc != NULL) addError(perror_h, perror_t, temp->line, temp->rol, "Redefinition!");
		map1(temp->r, perror_h, perror_t, glo, pclass, pfun);
		map1(tree->r, perror_h, perror_t, glo, pclass, pfun);
	}
	else if(strcmp(tree->name, "LValue") == 0)
	{
		struct ast *temp = tree->l;
		Var *v;
		Class *c;
		if(strcmp(temp->name, "ID") == 0)
		{
			v = findIdVar(temp->idtype, glo, *pclass, *pfun, tree->line, tree->rol);
			c = findIdClass(temp->idtype, glo, tree->line, tree->rol);
			if( v == NULL && c == NULL) addError(perror_h, perror_t, temp->line, temp->rol, "Undefined id!");
			else
			{
				if(v == NULL) 
				{
					tree->type->type = 5;
					tree->type->idtype = (char *)malloc(sizeof(char)*40);
					strcpy(tree->type->idtype, c->name);
				}
				else copy(tree->type, v->type);        //can not assign directory,because someone use the typelist of tree,it has its own space
			}
			map1(tree->r, perror_h, perror_t, glo, pclass, pfun);
		}
		else if(strcmp(temp->name, "Expr") == 0)
		{
			map1(temp, perror_h, perror_t, glo, pclass, pfun);
			if(temp->type->type == 5)
			{
				c = findIdClass(temp->type->idtype, glo, tree->line, tree->rol);
				if(c == NULL) addError(perror_h, perror_t, temp->line, temp->rol, "Undefined class id!");
				else   //class has been defined
				{
					if(strcmp(temp->r->name, "DOT") == 0)
					{
						v = findIdVar(temp->r->r->idtype, NULL, c, NULL, tree->line, tree->rol);
						if(v == NULL) addError(perror_h, perror_t, temp->r->r->line, temp->r->r->rol, "Undefined id in class!");
					}
					else addError(perror_h, perror_t, temp->line, temp->rol, "Undefined array id!");
				}
				
			}
			else if(temp->type->type == 6)
			{
				if(strcmp(temp->r->name, "LB") != 0) addError(perror_h, perror_t, temp->line, temp->rol, "Undefined array id!");
			}
			map1(tree->r, perror_h, perror_t, glo, pclass, pfun);
		}
	}
	else if(strcmp(tree->name, "Type") == 0)
	{
		Class *c;
		if(tree->type->type == 5)
		{
			c = findIdClass(tree->type->idtype, glo, tree->line, tree->rol);
			if(c == NULL) addError(perror_h, perror_t, tree->line, tree->rol, "Undefined class id!");
		}
		map1(tree->r, perror_h, perror_t, glo, pclass, pfun);
	}
	else if(strcmp(tree->name, "Call") == 0)
	{
		struct ast *temp = tree->l;
		Fun *pf;
		Class *c;
		if(strcmp(temp->name, "ID") == 0)
		{
			pf = findIdFun(temp->idtype, glo, *pclass, tree->line, tree->rol);
			if(pf == NULL) addError(perror_h, perror_t, temp->line, temp->rol, "Undefined function id!");
			else copy(tree->type, pf->returnType);
			map1(temp->r, perror_h, perror_t, glo, pclass, pfun);
			map1(tree->r, perror_h, perror_t, glo, pclass, pfun);
		}
		else
		{
			map1(temp, perror_h, perror_t, glo, pclass, pfun);
			if(temp->type->type == 5)
			{
				//判断被引用类是否已经在被引用前声明
				c = findIdClass(temp->type->idtype, glo, tree->line, tree->rol);
				if(c == NULL) addError(perror_h, perror_t, temp->line, temp->rol, "Undefined class id!");
				else   //class has been defined
				{
					pf = findIdFun(temp->r->r->idtype, NULL, c, tree->line, tree->rol);
					if(pf == NULL) addError(perror_h, perror_t, temp->r->r->line, temp->r->r->rol, "Undefined function id in class!");
					else copy(tree->type, pf->returnType);
				}	
			}
			else addError(perror_h, perror_t, temp->line, temp->rol, "Undefined class id");
			map1(tree->r, perror_h, perror_t, glo, pclass, pfun);
		}
	}
	else if(strcmp(tree->name, "Variable") == 0)
	{
		Class *c;
		Fun *f;
		Var *v;
		struct ast *temp = tree->l->r;
		c = findIdClass(temp->idtype, glo, tree->line, tree->rol);
		f = findIdFun(temp->idtype, glo, *pclass, tree->line, tree->rol);
		//类名，函数名，与变量名不能重名，不同作用域变量名可以相同
		if(*pclass == NULL)
		{
			v = findIdVar(temp->idtype, glo, NULL, NULL, tree->line, tree->rol);
		}
		else if(*pfun == NULL)
		{
			v = findIdVar(temp->idtype, NULL, *pclass, NULL, tree->line, tree->rol);
		}
		else
		{
			v = findIdVar(temp->idtype, NULL, NULL, *pfun, tree->line, tree->rol);
		}
		if(c != NULL || f != NULL || v != NULL) addError(perror_h, perror_t, temp->line, temp->rol, "Redefinition!");
		map1(tree->l, perror_h, perror_t, glo, pclass, pfun);
		map1(tree->r, perror_h, perror_t, glo, pclass, pfun);
	}
	else
	{
		map1(tree->l, perror_h, perror_t, glo, pclass, pfun);
		map1(tree->r, perror_h, perror_t, glo, pclass, pfun);
	}	
}

Global *glo = NULL;
Class *pclass = NULL;
Fun *pfun = NULL;
Error *error_h = NULL, *error_t = NULL;
int yyrol = 1;

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

	printGlobal(glo);
	printError(error_h);
	return 0;
}