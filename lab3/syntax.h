#ifndef _SYNTAX
#define _SYNTAX
void addGlo(Global *glo, struct ast* a, struct ast* b);
void addClass(struct ast* dest, struct ast* name, struct ast* child);

void passClass(struct ast* dest, int num, ...);
void addFun(struct ast* dest, struct ast* name, struct ast* reType, struct ast* formal, struct ast* stmt, int is_static, Global *glo);
void passFun(struct ast* dest, int num, ...);
void addVar(struct ast* dest, struct ast* name, struct ast* type);
void passVar(struct ast* dest, int num, ...);
void addType(struct ast* a, int type, struct ast* b);
void addError(Error **perror_h, Error **perror_t, int row, int rol, char *descri);
void checkType(Error **perror_h, Error **perror_t, int calType, int row, int rol, int num, ...);
void yyerror(char *msg, ...);
int typeMatch(struct typelist *a, struct typelist *b);
int formalTypeMatch(struct typelist *a, Var *form);
void copy(struct typelist *a, struct typelist *b);
void checkReturnType(struct ast *a, struct typelist *returnType, Error **perror_h, Error **perror_t);
Class *findIdClass(char *classname, Global *glo, int line, int rol);
Var *findIdVar(char *varname, Global *glo, Class *pclass,Fun *pfun, int line, int rol);
Fun *findIdFun(char *funname, Global *glo, Class *pclass, int line ,int rol);
void map2(struct ast *tree, Error **perror_h, Error **perror_t, Global *glo, Class **pclass, Fun **pfun);
void map1(struct ast *tree, Error **perror_h, Error **perror_t, Global *glo, Class **pclass, Fun **pfun);

#endif