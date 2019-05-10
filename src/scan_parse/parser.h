/* A Bison parser, made by GNU Bison 3.3.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2019 Free Software Foundation,
   Inc.

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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_HOME_VALDE_GIT_DM501_COMPILER_SRC_SCAN_PARSE_PARSER_H_INCLUDED
# define YY_YY_HOME_VALDE_GIT_DM501_COMPILER_SRC_SCAN_PARSE_PARSER_H_INCLUDED
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
    tINTCONST = 258,
    tIDENTIFIER = 259,
    tFUNC = 260,
    tEND = 261,
    tTYPE = 262,
    tVAR = 263,
    tINT = 264,
    tBOOL = 265,
    tRETURN = 266,
    tWRITE = 267,
    tALLOCATE = 268,
    tIF = 269,
    tWHILE = 270,
    tARRAY_OF = 271,
    tRECORD_OF = 272,
    tTHEN = 273,
    tDO = 274,
    tOF_LEN = 275,
    tELSE = 276,
    tEQ = 277,
    tINEQ = 278,
    tGREATER = 279,
    tLESS = 280,
    tGEQ = 281,
    tLEQ = 282,
    tAND = 283,
    tOR = 284,
    tTRUE = 285,
    tFALSE = 286,
    tNULL = 287,
    tLAMBDA_ARROW = 288,
    tVAL = 289,
    tCLASS = 290,
    tWITH = 291,
    tVOID = 292,
    tCONSTRUCTOR = 293,
    tGC = 294
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 14 "parser.y" /* yacc.c:1921  */

   int intconst;
   char *stringconst;
   struct Expression *expression;
   struct Body *body;
   struct DeclarationList *declarationList;
   struct VarDelList *varDelList;
   struct Declaration *declaration;
   struct Type *type;
   struct Function *function;
   struct Lambda *lambda;
   struct FunctionHead *functionHead;
   struct FunctionTail *functionTail;
   struct StatementList *statementList;
   struct Statement *statement;
   struct Operator *operator;
   struct Variable *variable;
   struct ExpressionList *expressionList;
   struct Term *term;
   struct TypeList *typeList;
   struct Constructor *constructor;

#line 121 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.h" /* yacc.c:1921  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_HOME_VALDE_GIT_DM501_COMPILER_SRC_SCAN_PARSE_PARSER_H_INCLUDED  */
