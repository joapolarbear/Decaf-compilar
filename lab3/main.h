#ifndef _MAIN
#define _MAIN
extern int yylineno;
extern char* yytext;



typedef struct ERROR
{
	int row;
	int rol;
	char *s;
	struct ERROR *next;
}Error;
struct typelist
{
	int type;
	char *idtype;
	struct typelist *next;
};
/*
char *typechoose[] = 
{
	"INT", "BOOL", "FLOAT", "STRING", "VOID", "CLASS", "ARRAY"
};
*/
typedef struct VAR
{
	char name[50];
	struct typelist *type;
	int line;
	int rol;
	int place;
	struct VAR *next;
}Var;

typedef struct FUNCTION
{
	char name[50];
	struct typelist *returnType;
	int line;
	int rol;
	Var *form_l;
	Var *var_l;
	struct FUNCTION *next;
}Fun;

typedef struct CLASS
{
	char name[50];
	int line;
	int rol;
	int size;
	Fun *fun_l;
	Var *var_l;
	struct CLASS *next;
}Class;

typedef struct GLOBAL
{
	Var *var_l;
	Fun *fun_l;
	Class *class_l;
}Global;


struct ast{
	int line;
	int rol;
	char *name;
	int nt;
	struct ast *l;
	struct ast *r;
	struct typelist *type;
	union{
		
		char *idtype;
		int inter;
		float flo;	
	};
	Var *var_h, *var_t;
	Fun *fun_h, *fun_t;
	Class *class_h, *class_t;
};

struct ast *newast(char *name, int num, ...);
void eval(struct ast *a, int count);
void yyerror(char *s, ...);
void addType(struct ast* a, int type, struct ast* b);

/*
void addGlo(Global *glo, Class **pclass_h, Class **pclass_t);
void addClass(struct ast* name, struct ast* parent, Var **pvar_h, Var **pvar_t, Fun **pfun_h, Fun **pfun_t, Class **pclass_h, Class **pclass_t);
void addFun(struct ast* name, struct ast* reType, int is_static, Var **pvar_h, Var **pvar_t, Fun **pfun_h, Fun **pfun_t, Var **pform_h, Var **pform_t, Global *glo);
void addForm(Var **pform_h, Var **pform_t, Var **pvar_h, Var **pvar_t);
void addVar(struct ast* name, struct ast* type, Var **pvar_h, Var **pvar_t);
*/

void addGlo(Global *glo, struct ast* a, struct ast* b);
void addClass(struct ast* dest, struct ast* name, struct ast* child);
void passClass(struct ast* dest, int num, ...);
void addFun(struct ast* dest, struct ast* name, struct ast* reType, struct ast* formal, struct ast* stmt, int is_static, Global *glo);
void passFun(struct ast* dest, int num, ...);
void addVar(struct ast* dest, struct ast* name, struct ast* type);
void passVar(struct ast* dest, int num, ...);


#endif