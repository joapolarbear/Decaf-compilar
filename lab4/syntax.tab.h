/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_SYNTAX_TAB_H_INCLUDED
# define YY_YY_SYNTAX_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    INT = 258,
    FLOAT = 259,
    BOOL = 260,
    STRING = 261,
    VOID = 262,
    CLASS = 263,
    STATIC = 264,
    EXTENDS = 265,
    FOR = 266,
    BREAK = 267,
    PRINT = 268,
    THIS = 269,
    NEW = 270,
    RETURN = 271,
    IF = 272,
    ELSE = 273,
    WHILE = 274,
    READINTEGER = 275,
    READLINE = 276,
    INSTANCEOF = 277,
    ID = 278,
    INTC = 279,
    FLOATC = 280,
    BOOLC = 281,
    STRINGC = 282,
    ASSIGNOP = 283,
    LESS = 284,
    MORE = 285,
    LESSEQUAL = 286,
    MOREEQUAL = 287,
    EQUAL = 288,
    NOTEQUAL = 289,
    PLUS = 290,
    MINUS = 291,
    STAR = 292,
    DIV = 293,
    AND = 294,
    OR = 295,
    DOT = 296,
    MOD = 297,
    NOT = 298,
    LP = 299,
    RP = 300,
    LB = 301,
    RB = 302,
    LC = 303,
    RC = 304,
    SEMI = 305,
    COMMA = 306,
    SPACE = 307,
    EOL = 308,
    LOWER_THAN_ELSE = 309
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 16 "syntax.y" /* yacc.c:1909  */

	struct ast* a;
	double d;

#line 114 "syntax.tab.h" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_SYNTAX_TAB_H_INCLUDED  */
