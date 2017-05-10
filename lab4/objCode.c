#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include "main.h"
#include "genCode.h"
#include "syntax.h"
#include "objCode.h"

extern ThreeAddCode *code;



RegDes regList[ALLREG];
VarDes* varList = NULL;
int offset = FSIZE;                //every time fuction start , fp-sp = FSIZE

void clearVarList(){
	while(varList!=NULL){
		VarDes* tem = varList;
		varList = varList->next;
		free(tem);
	}
}

void addVarToList(VarDes* p){
	assert(p!=NULL);
	p->next = varList;
	varList = p;
}

void stVar(VarDes* var, FILE* fp){
	char stCode[64];
	/*if(var->isaddr == 1){
		int addrReg = allocateRegForOp(var->op->u.addr, fp);
		memset(stCode, 0, sizeof(stCode));
		sprintf(stCode, "sw $%s, 0($%s)\n", regList[var->regNo].name, regList[addrReg].name);
	}
	else if(var->op->kind == VAR_OP || var->op->kind == TEM_VAR_OP){*/
		memset(stCode, 0, sizeof(stCode));
		sprintf(stCode, "subu $v1, $fp, %d\n", var->offset);
		fputs(stCode, fp);
		memset(stCode, 0, sizeof(stCode));
		sprintf(stCode, "sw $%s, 0($v1)\n", regList[var->regNo].name);
	//}
	//else
	//	assert(0);
	regList[var->regNo].old = 0;
	regList[var->regNo].var = NULL;
	var->regNo = -1;
	fputs(stCode, fp);
}

void ldVar(VarDes* var, FILE* fp){
	//需要将内存中的变量load到寄存器中
	char ldCode[64];
	if(var->op[1] == 'V' || var->op[1] == 'T' ){
		memset(ldCode, 0, sizeof(ldCode));
		sprintf(ldCode, "subu $v1, $fp, %d\n", var->offset);
		fputs(ldCode, fp);
		memset(ldCode, 0, sizeof(ldCode));
		sprintf(ldCode, "lw $%s, 0($v1)\n", regList[var->regNo].name);
	}
	else if(var->op[0] == '#'){
		memset(ldCode, 0, sizeof(ldCode));
		sprintf(ldCode, "li $%s, %d\n", regList[var->regNo].name, atoi(var->op+1));
	}
	else{
		//不会出现 LABEL_OP FUNC_OP 
		assert(0);
	}
	fputs(ldCode, fp);
}


void initAllRegDes(){
	int i;
	for(i=0; i<ALLREG; i++){
		regList[i].old = 0;
		regList[i].var = NULL;
	}
	//为了方便编码 寄存器描述符表的序号并不是寄存器的实际序号
	strcpy(regList[0].name, "t0");
	strcpy(regList[1].name, "t1");
	strcpy(regList[2].name, "t2");
	strcpy(regList[3].name, "t3");
	strcpy(regList[4].name, "t4");
	strcpy(regList[5].name, "t5");
	strcpy(regList[6].name, "t6");
	strcpy(regList[7].name, "t7");
	strcpy(regList[8].name, "t8");
	strcpy(regList[9].name, "t9");
	strcpy(regList[10].name, "s0");
	strcpy(regList[11].name, "s1");
	strcpy(regList[12].name, "s2");
	strcpy(regList[13].name, "s3");
	strcpy(regList[14].name, "s4");
	strcpy(regList[15].name, "s5");
	strcpy(regList[16].name, "s6");
	strcpy(regList[17].name, "s7");

	strcpy(regList[18].name, "0");
	strcpy(regList[19].name, "at");
	//return
	strcpy(regList[20].name, "v0");
	strcpy(regList[21].name, "v1");
	//arg
	strcpy(regList[22].name, "a0");
	strcpy(regList[23].name, "a1");
	strcpy(regList[24].name, "a2");
	strcpy(regList[25].name, "a3");

	strcpy(regList[28].name, "gp");
	strcpy(regList[29].name, "sp");
	strcpy(regList[30].name, "fp");
	strcpy(regList[31].name, "ra");
}

void rstAllReg(){
	//只重置变量寄存器
	int i;
	for(i=0; i<REGNUM; i++){
		regList[i].old = 0;
		regList[i].var = NULL;
	}
}

void freeOneReg(int regNo){
	//释放regNo对应的寄存器
	VarDes* tem = varList;
	while(tem!=NULL){
		if(tem->regNo == regNo)
			tem->regNo = -1;
		tem = tem->next;
	}
	regList[regNo].old = 0;
	regList[regNo].var = NULL;
}

int allocateRegForOp(char* opin, FILE* fp){
	if(strcmp(opin, "#0") == 0) return 18;      //zhi jie fan hui 0 hao ji cun qi
	//在varList中寻找op对应的变量的VarDes
	VarDes* var = varList;
	char op[5];
	if(opin[0] == '*') strcpy(op, opin+1);
	else strcpy(op, opin);

	while(var!=NULL){
		if(strcmp(var->op, op)==0)
			break;
		var = var->next;
	}
	
	//如果op对应的变量第一次被使用 则记录栈中相对于栈底($fp)的位置(偏移量)
	int isFirstUsed = FALSE;
	if(var == NULL){
		isFirstUsed = TRUE;
		var = malloc(sizeof(VarDes));
		memset(var, 0, sizeof(VarDes));
		strcpy(var->op, op);
		var->regNo = -1;
		if(op[1] == 'T' || op[1] == 'V'){
			offset += 4;
			var->offset = offset;
		}
		if(op[0] != '#')
			addVarToList(var);
	}
	
	//如果op对应的变量没有被分配寄存器 则为其分配寄存器
	if(var->regNo < 0){
		int regNo = getReg(fp);
		var->regNo = regNo;
		regList[regNo].var = var;
		
		if(op[0] == '#' || (isFirstUsed == FALSE && var->offset>=0)){
			//不是第一次出现的变量 
			ldVar(var, fp);
		}
	}
	//每次寄存器中的变量被使用 old字段就会置零
	regList[var->regNo].old = 0;
	return var->regNo;
}

int getReg(FILE* fp){
	//获取一个可用的寄存器
	//所有没被释放的变量寄存器的old都加一
	int i;
	int freedReg = -1;
	for(i=0; i<REGNUM; i++){
		//if(regList[i].var->op[1] == 'T')
		//	freeOneReg(i);
		if(regList[i].var!=NULL)
			regList[i].old++;
		else
		{
			freedReg = i;
			break;
		}
	}
	//如果有没被释放的变量寄存器 则直接使用它
	if(freedReg >= 0)
		return freedReg; //huhanpen
	
	//找出最久未被使用的reg
	int oldestReg = -1;
	int oldest = -1;
	for(i=0; i<REGNUM; i++){
		if(regList[i].old>oldest){
			oldest = regList[i].old;
			oldestReg = i;
		}
	}
	VarDes* var = varList;
	while(var!=NULL){
		if(var == regList[oldestReg].var)
			break;
		else
			var = var->next;
	}
	if(var!=NULL){
		//将oldestReg中的变量store
		assert(oldestReg==var->regNo);
		stVar(var, fp);
	}
	else
		assert(0);
	return oldestReg;
}

void fwriteAllObjCode(char* fileName){
	initAllRegDes();
	FILE* fp = fopen(fileName, "w");
	if(fp==NULL){
		printf("ERROR: open file \"%s\" fail!",fileName);
		return;
	}
	fputs(".data\n", fp);
	fputs("_prompt: .asciiz \"Enter an integer:\"\n", fp);
	fputs("_ret: .asciiz \"\\n\"\n", fp);
	fputs(".globl main\n", fp);
	fputs(".text\n", fp);
	fputs("\n", fp);
	fputs("read:\n", fp);
	fputs("subu $sp, $sp, 8\n", fp); 
	fputs("sw $ra, 4($sp)\n", fp);
	fputs("sw $fp, 0($sp)\n", fp);
	fputs("addi $fp, $sp, 8\n", fp);
	fputs("li $v0, 4\n", fp);
	fputs("la $a0, _prompt\n", fp);
	fputs("syscall\n", fp);
	fputs("li $v0, 5\n", fp);
	fputs("syscall\n", fp);
	fputs("subu $sp, $fp, 8\n", fp);
	fputs("lw $ra, 4($sp)\n", fp);
	fputs("lw $fp, 0($sp)\n", fp);
	fputs("jr $ra\n", fp);
	fputs("\n", fp);
	fputs("write:\n", fp);
	fputs("subu $sp, $sp, 8\n", fp); 
	fputs("sw $ra, 4($sp)\n", fp);
	fputs("sw $fp, 0($sp)\n", fp);
	fputs("addi $fp, $sp, 8\n", fp);
	fputs("li $v0, 1\n", fp);
	fputs("syscall\n", fp);
	fputs("li $v0, 4\n", fp);
	fputs("la $a0, _ret\n", fp);
	fputs("syscall\n", fp);
	fputs("subu $sp, $fp, 8\n", fp);
	fputs("lw $ra, 4($sp)\n", fp);
	fputs("lw $fp, 0($sp)\n", fp);
	fputs("move $v0, $0\n", fp);
	fputs("jr $ra\n", fp);
	
	CodeLine* ir = code->head;
	while(ir!=NULL){
		fwriteOneObjCode(ir, fp);
		ir = ir->next;
	}
	fclose(fp);
}

int argc = 0;

void fwriteOneObjCode(CodeLine* ir, FILE* fp){
	//printInterCode(ir);
	switch(ir->kind){
		case LABEL_CODE:{
			//一个语句块的边界 应该将所有变量store
			/*VarDes* var = varList;
			while(var!=NULL){
				if(var->regNo>=0)
					stVar(var, fp);
				var = var->next;
			}*/
			char str[64];
			memset(str, 0, sizeof(str));
			sprintf(str, "label%d:\n", atoi(ir->place[0] + 2));
			fputs(str, fp);
			break;
		}
		case FUNCTION_CODE:{
			char str[64];
			memset(str, 0, sizeof(str));
			if(strcmp(ir->place[0], "_main_main") == 0) fputs("\nmain:\n", fp);
			else
			{
				sprintf(str, "\n%s:\n", ir->place[0]);
				fputs(str, fp);
			}
			
			//Prologue
			//将ra寄存器和fp寄存器压栈
			memset(str, 0, sizeof(str));
			sprintf(str, "subu $sp, $sp, %d\n", FSIZE);
			fputs(str, fp);
			fputs("sw $ra, 4($sp)\n", fp);
			fputs("sw $fp, 0($sp)\n", fp);
			//重置fp寄存器
			fputs("addi $fp, $sp, 8\n", fp);
			
			//清空varList 重置所有RegDes和offset
			clearVarList();
			rstAllReg();
			offset = FSIZE;
			
			ir = ir->next;
			int i = 0;
			while(ir->kind == PARAM_CODE ){
				int paramReg = allocateRegForOp(ir->place[0], fp);
				memset(str, 0, sizeof(str));
				if(i<4){
					//前四个参数暂存在a0~a3寄存器中
					sprintf(str, "move $a%d, $%s\n", i, regList[paramReg].name);
				}
				else{
					//余下的参数在栈中
					sprintf(str, "lw $%s, %d($sp)\n", regList[paramReg].name, FSIZE+(i-4)*4);
				}
				fputs(str, fp);
				i++;
				ir = ir->next;
			}
			break;
		}
		case ASSIGN_CODE:{
			int leftReg = allocateRegForOp(ir->place[2], fp);
			int rightReg = allocateRegForOp(ir->place[0], fp);
			char str[64];
			if(ir->place[2][0] == '*') 
			{
				int TempReg = getReg(fp);
				memset(str, 0, sizeof(str));
				sprintf(str, "move $%s, $%s\n", regList[TempReg].name, regList[rightReg].name);
				fputs(str, fp);
				memset(str, 0, sizeof(str));
				sprintf(str, "sw $%s, 0($%s)\n", regList[TempReg].name, regList[leftReg].name);
				fputs(str, fp);
			}
			else
			{
				memset(str, 0, sizeof(str));
				sprintf(str, "move $%s, $%s\n", regList[leftReg].name, regList[rightReg].name);
				fputs(str, fp);
			}
			break;
		}
		case ADD_CODE:
		case SUB_CODE:
		case MUL_CODE:
		case DIV_CODE:
		case LESS_CODE:
		//case MORE_CODE:
		//case LESSEQ_CODE:
		//case MOREEQ_CODE:
		case AND_CODE:
		case OR_CODE:
		case EUQAL_CODE:
		{
			char op[4];
			memset(op, 0, sizeof(op));
			switch(ir->kind){
				case ADD_CODE:
					strcpy(op, "add");
					break;
				case SUB_CODE:
					strcpy(op, "sub");
					break;
				case MUL_CODE:
					strcpy(op, "mul");
					break;
				case DIV_CODE:
					strcpy(op, "div");
					break;
				//case MORE_CODE:
				case LESS_CODE:
					strcpy(op, "slt");
					break;
				case AND_CODE:
					strcpy(op, "and");
					break;
				case OR_CODE:
					strcpy(op, "or");
					break;
				case EUQAL_CODE:
					strcpy(op, "xor");
					break;
			}
			int op1Reg, op2Reg, resultReg;
			char op1[5], op2[5], str[64];
			if(ir->place[1][0] == '#')
			{
				if(ir->place[1][1] == '0')
				{
					strcpy(op2, "$0");
				}
				else if(strcmp(op, "sub") == 0)
				{
					strcpy(op, "addi");
					sprintf(op2, "-%d", atoi(ir->place[1]+1));
				}
				else
				{
					strcat(op, "i");
					strcpy(op2, ir->place[1]+1);
				}
			}
			else
			{
				op2Reg = allocateRegForOp(ir->place[1], fp);
				if(ir->place[1][0] == '*')
				{
					sprintf(op2, "0($%s)", regList[op2Reg].name);
				}
				else strcat(strcpy(op2, "$"), regList[op2Reg].name);
			}
			assert(ir->place[0][0] != '#');
			op1Reg = allocateRegForOp(ir->place[0], fp);
			resultReg = allocateRegForOp(ir->place[2], fp);
			
			if(ir->place[0][0] == '*')     //*_T2  
			{
				sprintf(op1, "0($%s)", regList[op1Reg].name);
			}
			else strcat(strcpy(op1, "$"), regList[op1Reg].name);

			if(ir->place[2][0] == '*') 
			{
				int TempReg = getReg(fp);
				memset(str, 0, sizeof(str));
				sprintf(str, "%s $%s, %s, %s\n", op, regList[TempReg].name, op1, op2);
				fputs(str, fp);
				memset(str, 0, sizeof(str));
				sprintf(str, "sw $%s, 0($%s)\n", regList[TempReg].name, regList[resultReg].name);
				fputs(str, fp);
				//stVar(regList[resultReg].var, fp);
			}
			else
			{
				memset(str, 0, sizeof(str));
				sprintf(str, "%s $%s, %s, %s\n", op, regList[resultReg].name, op1, op2);
				fputs(str, fp);
			}	
			break;
		}
		case MINUS_CODE:
		{
			char str[64];
			int opReg = allocateRegForOp(ir->place[0], fp);
			int resultReg = allocateRegForOp(ir->place[2], fp);
			memset(str, 0, sizeof(str));
			sprintf(str, "neg $%s, $%s\n", regList[resultReg].name, regList[opReg].name);
			fputs(str, fp);
			if(ir->place[0][1] == 'T')
				freeOneReg(opReg);
			break;
		}
		case NOT_CODE:
		{
			char str[64];
			int opReg = allocateRegForOp(ir->place[0], fp);
			int resultReg = allocateRegForOp(ir->place[2], fp);
			memset(str, 0, sizeof(str));
			sprintf(str, "xori $%s, $%s, -1\n", regList[resultReg].name, regList[opReg].name);
			fputs(str, fp);
			if(ir->place[0][1] == 'T')
				freeOneReg(opReg);
			break;
		}
		case NOTEQ_CODE:
		{
			break;
		}
		case ADDRESS_CODE:{
			//已经在DEC_CODE部分处理过
			break;
		}
		case GOTO_CODE:{
			CodeLine *irPre = ir->pre;
			//if(irPre->kind != IF_GOTO_CODE)
			if(ir->place[2][0]	!= 'h')
			{
				//一个语句块的边界 应该将所有变量store
				/*VarDes* var = varList;
				while(var!=NULL){
					if(var->regNo>=0)
						stVar(var, fp);
					var = var->next;
				}*/ //huhanpeng
				char str[64];
				memset(str, 0, sizeof(str));
				sprintf(str, "j label%d\n", atoi(ir->place[0]+2));
				fputs(str, fp);
			}
			break;
		}
		case IF_GOTO_CODE:{
			//一个语句块的边界 应该将所有变量store
			/*VarDes* var = varList;
			while(var!=NULL){
				if(var->regNo>=0)
					stVar(var, fp);
				var = var->next;
			}*/     //huhanpeng
			char op[4];
			if(strcmp(ir->place[1],"==")==0)
				strcpy(op,"bne");
			else if(strcmp(ir->place[1],"!=")==0)
				strcpy(op,"beq");
			else if(strcmp(ir->place[1],">")==0)
				strcpy(op,"blt");
			else if(strcmp(ir->place[1],"<")==0)
				strcpy(op,"bgt");
			else if(strcmp(ir->place[1],">=")==0)
				strcpy(op,"ble");
			else if(strcmp(ir->place[1],"<=")==0)
				strcpy(op,"bge");
			else
				assert(0);
			int xReg = allocateRegForOp(ir->place[0], fp);
			int yReg = allocateRegForOp(ir->place[2], fp);
			ir = ir->next;
			ir->place[2][0] = 'h';     //has translate, do not translate
			ir = ir->next;
			ir->place[2][0] = 'h';
			char str[64];
			memset(str, 0, sizeof(str));
			sprintf(str, "%s $%s, $%s, label%d\n", op, regList[xReg].name, regList[yReg].name, atoi(ir->place[0] + 2));
			fputs(str, fp);
			break;
		}
		case RETURN_CODE:{
			int retReg;
			//Epilogue
			//sp寄存器指向栈帧
			fputs("subu $sp, $fp, 8\n", fp);
			//恢复ra寄存器和fp寄存器的值
			fputs("lw $ra, 4($sp)\n", fp);
			fputs("lw $fp, 0($sp)\n", fp);
			if(strlen(ir->place[0]) != 0)
			{
				//将返回值放入v0寄存器内 
				//由于函数即将返回 v0中的变量无需store
				char str[64];
				memset(str, 0, sizeof(str));
				if(ir->place[0][0] == '#' && ir->place[0][1] != '0')
					sprintf(str, "li $v0, %s\n", ir->place[0]+1);
				else
				{
					retReg = allocateRegForOp(ir->place[0], fp);
					sprintf(str, "move $v0, $%s\n", regList[retReg].name);
				}
				fputs(str, fp);
			}
			fputs("jr $ra\n", fp);     // return to ra
			break;
		}
		case DEC_CODE:{       //huhanpeng
			CodeLine* irPre = ir->pre;
			int retReg = allocateRegForOp(ir->place[2], fp);
			assert(irPre->place[0][0] == '#');
			//开空间
			int opensize = atoi(irPre->place[0]+1) * 4;
			offset += opensize;
			//将空间的首地址存入寄存器
			char str[64];
			memset(str, 0, sizeof(str));
			sprintf(str, "subu $%s, $fp, %d\n", regList[retReg].name, offset);
			fputs(str, fp);
			argc = 0;
			break;
		}
		case ARG_CODE:{
			argc++;
			break;
		}
		case CALL_CODE:{
			//CALL是一个语句块的边界 应该将所有变量store
			VarDes* var = varList;
			while(var!=NULL){
				if(var->regNo>=0)
					stVar(var, fp);
				var = var->next;
			}
			CodeLine* irPre = ir;
			//修改sp寄存器 为需要压栈的参数申请空间
			char str[64];
			if(argc>4){
				memset(str, 0, sizeof(str));
				sprintf(str, "subu $sp, $fp, %d\n", offset+(argc-4)*4);
				fputs(str, fp);
			}
			else{
				memset(str, 0, sizeof(str));
				sprintf(str, "subu $sp, $fp, %d\n", offset);
				fputs(str, fp);
			}
			//参数压栈
			int i;
			for(i=0; i<argc; i++){
				irPre = irPre->pre;
				int argReg = allocateRegForOp(irPre->place[0], fp);
				memset(str, 0, sizeof(str));
				if(i<4){
					sprintf(str, "move $a%d, $%s\n", i, regList[argReg].name);
				}
				else{
					sprintf(str, "sw $%s, %d($sp)\n", regList[argReg].name, (i-4)*4);
				}
				fputs(str, fp);
			}
			//跳转
			memset(str, 0, sizeof(str));
			sprintf(str, "jal %s\n", ir->place[0]);
			fputs(str, fp);
			argc = 0;
			//取v0寄存器中的返回值
			if(strlen(ir->place[2]) != 0)
			{
				int retReg = allocateRegForOp(ir->place[2], fp);
				memset(str, 0, sizeof(str));
				sprintf(str, "move $%s, $v0\n", regList[retReg].name);
				fputs(str, fp);
			}
			break;
		}
		case PARAM_CODE:{
			//已经在FUNCTION_CODE部分处理过
			break;
		}
		case READ_CODE:{             //huhanpeng
			//设置栈顶指针到正确位置
			char str[64];
			CodeLine *pre = ir->pre;
			memset(str, 0, sizeof(str));
			sprintf(str, "subu $sp, $fp, %d\n", offset);
			fputs(str, fp);
			//跳转
			fputs("jal read\n", fp);
			argc = 0;
			//取返回值
			int retReg = allocateRegForOp(pre->place[0], fp);
			memset(str, 0, sizeof(str));
			sprintf(str, "move $%s, $v0\n", regList[retReg].name);
			fputs(str, fp);
			break;
		}
		case WRITE_CODE:{               //huhanpeng
			//设置栈顶指针到正确位置
			char str[64];
			CodeLine *pre = ir->pre;
			while(strcmp(pre->op, "ARG") == 0)
			{
				memset(str, 0, sizeof(str));
				sprintf(str, "subu $sp, $fp, %d\n", offset);
				fputs(str, fp);
				//传参数到a0寄存器
				int paramReg = allocateRegForOp(pre->place[0], fp);
				memset(str, 0, sizeof(str));
				sprintf(str, "move $a0, $%s\n", regList[paramReg].name);
				fputs(str, fp);
				//跳转
				fputs("jal write\n", fp);
				pre = pre->pre;
			}
			argc = 0;
			break;
		}
		default:
			assert(0);
	}
}
