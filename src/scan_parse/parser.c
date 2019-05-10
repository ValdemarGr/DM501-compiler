/* A Bison parser, made by GNU Bison 3.3.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.3.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "parser.y" /* yacc.c:337  */

#include <stdio.h>
#include "scan_parse.h"

extern char *yytext;
extern Body *theexpression;
extern int lineno;

void yyerror(char const *s) {
   fprintf(stderr, "syntax error before %s, near line %d: %s\n",yytext,lineno,s);
}

#line 83 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:337  */
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "parser.h".  */
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
#line 14 "parser.y" /* yacc.c:352  */

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

#line 189 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:352  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_HOME_VALDE_GIT_DM501_COMPILER_SRC_SCAN_PARSE_PARSER_H_INCLUDED  */



#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  18
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   485

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  57
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  24
/* YYNRULES -- Number of rules.  */
#define YYNRULES  95
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  242

#define YYUNDEFTOK  2
#define YYMAXUTOK   294

/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  ((unsigned) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    56,     2,     2,     2,     2,     2,     2,
      48,    49,    42,    40,    45,    41,    55,    43,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    44,    52,
       2,    53,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    46,     2,    47,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    50,    54,    51,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   104,   104,   108,   113,   114,   119,   120,   124,   126,
     128,   130,   134,   136,   138,   140,   144,   148,   150,   152,
     154,   156,   158,   160,   162,   165,   167,   169,   171,   175,
     177,   179,   181,   183,   185,   187,   189,   191,   193,   195,
     197,   201,   203,   206,   209,   211,   213,   215,   217,   219,
     221,   223,   225,   229,   233,   235,   239,   243,   247,   251,
     254,   257,   259,   263,   267,   269,   271,   273,   275,   277,
     279,   281,   283,   285,   287,   289,   291,   295,   298,   301,
     303,   333,   335,   337,   341,   343,   345,   347,   349,   351,
     353,   355,   357,   359,   361,   363
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "tINTCONST", "tIDENTIFIER", "tFUNC",
  "tEND", "tTYPE", "tVAR", "tINT", "tBOOL", "tRETURN", "tWRITE",
  "tALLOCATE", "tIF", "tWHILE", "tARRAY_OF", "tRECORD_OF", "tTHEN", "tDO",
  "tOF_LEN", "tELSE", "tEQ", "tINEQ", "tGREATER", "tLESS", "tGEQ", "tLEQ",
  "tAND", "tOR", "tTRUE", "tFALSE", "tNULL", "tLAMBDA_ARROW", "tVAL",
  "tCLASS", "tWITH", "tVOID", "tCONSTRUCTOR", "tGC", "'+'", "'-'", "'*'",
  "'/'", "':'", "','", "'['", "']'", "'('", "')'", "'{'", "'}'", "';'",
  "'='", "'|'", "'.'", "'!'", "$accept", "program", "body", "decl_list",
  "stm_list", "generic_type_list", "class_extension_list", "constructor",
  "declaration", "statement", "type_list", "type", "voidType", "lambda",
  "function", "head", "tail", "par_decl_list", "var_decl_list",
  "expression", "act_list", "exp_list", "variable", "term", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
      43,    45,    42,    47,    58,    44,    91,    93,    40,    41,
     123,   125,    59,    61,   124,    46,    33
};
# endif

#define YYPACT_NINF -140

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-140)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      77,    26,    27,    39,    46,    51,    68,  -140,    90,    77,
    -140,    77,    44,    42,    36,    65,    66,   -14,  -140,  -140,
      79,    83,    83,   143,    83,    83,  -140,  -140,  -140,    97,
      94,    90,    83,   103,  -140,    90,  -140,   205,   -20,  -140,
    -140,   146,    39,   157,   157,  -140,    83,   150,   152,   180,
      83,   236,   149,   244,  -140,    -1,   378,   412,  -140,   -21,
     104,  -140,   322,   109,   197,    94,  -140,  -140,    83,    83,
      83,    83,    83,    83,    83,    83,    83,    83,    83,    83,
    -140,   157,    83,    83,    83,   165,   171,  -140,   113,  -140,
    -140,  -140,   157,   126,   177,   157,   131,   139,   275,    37,
     136,     4,   156,   142,   145,    77,   420,   160,  -140,  -140,
    -140,    83,    83,  -140,    90,    90,   162,  -140,  -140,  -140,
     350,   442,   442,    48,    48,    48,    48,   158,   137,    32,
      32,  -140,  -140,  -140,   386,   164,   283,  -140,  -140,   163,
    -140,    39,   170,   186,   191,  -140,    39,  -140,   150,   157,
     180,   237,   152,    80,    39,   190,   192,    83,  -140,   314,
     195,   228,  -140,   154,  -140,  -140,  -140,  -140,   157,   199,
     157,   219,   157,  -140,  -140,   206,   203,    77,   210,  -140,
     150,   180,   207,  -140,   222,  -140,  -140,   223,    90,  -140,
     247,   248,  -140,  -140,   235,   154,  -140,   253,   231,   239,
     152,   241,   242,    77,   245,  -140,  -140,  -140,   263,   264,
    -140,  -140,  -140,   150,  -140,   240,  -140,   180,   267,   243,
      77,    77,    77,  -140,  -140,   269,    77,  -140,   270,   277,
     278,   279,   280,   282,  -140,   300,  -140,  -140,  -140,   301,
    -140,  -140
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       4,     0,     0,     0,     0,     0,     0,     2,     6,     4,
      18,     4,     0,     0,     0,     0,     0,     0,     1,    89,
      81,     0,     0,     0,     0,     0,    90,    91,    92,     0,
      60,     6,     0,     0,     3,     6,    93,     0,    84,    76,
       5,     0,    60,     0,     0,    17,     0,     0,     0,     4,
      78,     0,    84,     0,    81,     0,     0,     0,    40,    81,
       0,    59,     0,     0,     0,    60,    87,     7,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      39,     0,     0,    78,     0,     0,     0,    56,     0,    44,
      47,    48,     0,     0,     0,    43,     0,    62,     0,    12,
       0,    10,     0,     0,     0,     4,    79,     0,    77,    29,
      36,     0,    78,    32,     0,     0,     0,    63,    38,    88,
       0,    68,    69,    70,    71,    72,    73,    74,    75,    66,
      67,    64,    65,    94,     0,     0,     0,    83,    58,     0,
      49,     0,    45,     0,    42,    19,    60,    20,     0,    43,
       4,     0,     0,     0,    60,     0,     0,     0,    85,     0,
       0,    31,    37,     0,    86,    82,    95,    35,     0,     0,
      43,     0,    43,    61,    14,     0,     0,     4,    11,     9,
       0,     4,     0,    21,     0,    80,    34,     0,     0,    53,
       0,     0,    57,    50,     0,     0,    41,    13,     0,     0,
       0,     0,     0,     4,     0,    25,    33,    30,     0,     0,
      46,    51,    52,     0,    24,     0,     8,     4,     0,     0,
       4,     4,     4,    15,    28,     0,     4,    22,     0,     0,
       0,     0,     0,     0,    26,     0,    54,    55,    23,     0,
      16,    27
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -140,  -140,   -10,    -9,   101,  -131,  -134,  -139,  -140,  -107,
    -133,   -40,   166,  -140,  -140,  -140,  -140,   -36,     2,   -12,
     -68,   174,    -6,   288
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     6,     7,     8,    34,   102,   100,   105,     9,    35,
     143,   144,   191,    36,    10,    11,    87,    60,    61,    37,
     107,   108,    52,    39
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      40,    41,    38,    96,    97,    15,    88,   161,   162,    51,
      53,   177,    56,    57,   174,   135,   175,    55,    62,   111,
      64,   179,    47,    44,    81,    38,    82,    50,    83,    38,
      12,    13,    48,    84,    98,    85,    49,   194,   106,   196,
     104,   133,   203,    14,   160,    82,   201,   112,   151,   152,
      16,   113,   140,   120,    85,    17,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,    18,   216,
     134,   106,   136,   148,    78,    79,    74,    75,   226,   223,
      44,   207,     1,   149,     2,     3,    19,    20,    76,    77,
      78,    79,    42,    19,    20,    43,   156,    19,    59,   159,
     106,    21,    22,    23,    24,    25,    19,    20,    38,    38,
     173,     4,     5,    26,    27,    28,   180,    45,   182,    46,
      26,    27,    28,   190,    26,    27,    28,    50,   192,    29,
     181,    30,    63,    26,    27,    28,    67,    32,    30,    33,
      31,   176,    30,   169,    32,   106,    33,    54,    32,    58,
      33,    65,    86,   116,    99,   211,   101,    32,    89,    33,
     118,    89,   139,    90,    91,    74,    90,    91,   199,   137,
      92,    93,   202,    92,    93,   138,   141,    76,    77,    78,
      79,   142,    38,   145,   146,     1,   150,     2,     3,    94,
     154,   189,    94,    81,   219,    82,   155,    83,    76,    77,
      78,    79,    95,   153,    85,    95,   163,   168,   225,   158,
     229,   230,   231,   166,     4,     5,   170,   233,   103,    68,
      69,    70,    71,    72,    73,    74,    75,    68,    69,    70,
      71,    72,    73,    74,    75,   171,   172,    76,    77,    78,
      79,   178,   183,   184,   187,    76,    77,    78,    79,   188,
     193,   119,   195,   197,   198,   200,   204,    80,    68,    69,
      70,    71,    72,    73,    74,    75,    68,    69,    70,    71,
      72,    73,    74,    75,   205,   206,    76,    77,    78,    79,
     208,   209,   210,   214,    76,    77,    78,    79,   109,   213,
     215,   217,   224,   218,   228,   220,   110,    68,    69,    70,
      71,    72,    73,    74,    75,    68,    69,    70,    71,    72,
      73,    74,    75,   221,   222,    76,    77,    78,    79,   227,
     232,    66,   234,    76,    77,    78,    79,   147,   235,   236,
     237,   185,   238,   239,     0,   167,    68,    69,    70,    71,
      72,    73,    74,    75,    68,    69,    70,    71,    72,    73,
      74,    75,   240,   241,    76,    77,    78,    79,     0,     0,
       0,   212,    76,    77,    78,    79,   186,     0,     0,     0,
       0,   117,    68,    69,    70,    71,    72,    73,    74,    75,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      76,    77,    78,    79,     0,     0,   114,     0,     0,   164,
      68,    69,    70,    71,    72,    73,    74,    75,    68,    69,
      70,    71,    72,    73,    74,    75,     0,     0,    76,    77,
      78,    79,     0,     0,     0,     0,    76,    77,    78,    79,
       0,   115,     0,   165,    68,    69,    70,    71,    72,    73,
      74,    75,    68,    69,    70,    71,    72,    73,    74,    75,
       0,     0,    76,    77,    78,    79,     0,     0,     0,     0,
      76,    77,    78,    79,     0,   157,    70,    71,    72,    73,
      74,    75,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    76,    77,    78,    79
};

static const yytype_int16 yycheck[] =
{
       9,    11,     8,    43,    44,     3,    42,   114,   115,    21,
      22,   150,    24,    25,   148,    83,   149,    23,    30,    20,
      32,   152,    36,    44,    44,    31,    46,    48,    48,    35,
       4,     4,    46,    53,    46,    55,    50,   170,    50,   172,
      49,    81,   181,     4,   112,    46,   180,    48,    44,    45,
       4,    52,    92,    65,    55,     4,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,     0,   200,
      82,    83,    84,    36,    42,    43,    28,    29,   217,   213,
      44,   188,     5,    46,     7,     8,     3,     4,    40,    41,
      42,    43,    48,     3,     4,    53,   105,     3,     4,   111,
     112,    11,    12,    13,    14,    15,     3,     4,   114,   115,
     146,    34,    35,    30,    31,    32,    36,    52,   154,    53,
      30,    31,    32,   163,    30,    31,    32,    48,   168,    39,
      50,    48,    31,    30,    31,    32,    35,    54,    48,    56,
      50,   150,    48,   141,    54,   157,    56,     4,    54,    52,
      56,    48,     6,    49,     4,   195,     4,    54,     4,    56,
      51,     4,    49,     9,    10,    28,     9,    10,   177,     4,
      16,    17,   181,    16,    17,     4,    50,    40,    41,    42,
      43,     4,   188,    52,    45,     5,    50,     7,     8,    35,
      48,    37,    35,    44,   203,    46,    51,    48,    40,    41,
      42,    43,    48,    47,    55,    48,    44,    44,   217,    49,
     220,   221,   222,    49,    34,    35,    46,   226,    38,    22,
      23,    24,    25,    26,    27,    28,    29,    22,    23,    24,
      25,    26,    27,    28,    29,    49,    45,    40,    41,    42,
      43,     4,    52,    51,    49,    40,    41,    42,    43,    21,
      51,    54,    33,    47,    51,    45,    49,    52,    22,    23,
      24,    25,    26,    27,    28,    29,    22,    23,    24,    25,
      26,    27,    28,    29,    52,    52,    40,    41,    42,    43,
      33,    33,    47,    52,    40,    41,    42,    43,    52,    36,
      51,    50,    52,    51,    51,    50,    52,    22,    23,    24,
      25,    26,    27,    28,    29,    22,    23,    24,    25,    26,
      27,    28,    29,    50,    50,    40,    41,    42,    43,    52,
      51,    33,    52,    40,    41,    42,    43,    52,    51,    51,
      51,   157,    52,    51,    -1,    52,    22,    23,    24,    25,
      26,    27,    28,    29,    22,    23,    24,    25,    26,    27,
      28,    29,    52,    52,    40,    41,    42,    43,    -1,    -1,
      -1,   195,    40,    41,    42,    43,    52,    -1,    -1,    -1,
      -1,    49,    22,    23,    24,    25,    26,    27,    28,    29,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      40,    41,    42,    43,    -1,    -1,    18,    -1,    -1,    49,
      22,    23,    24,    25,    26,    27,    28,    29,    22,    23,
      24,    25,    26,    27,    28,    29,    -1,    -1,    40,    41,
      42,    43,    -1,    -1,    -1,    -1,    40,    41,    42,    43,
      -1,    19,    -1,    47,    22,    23,    24,    25,    26,    27,
      28,    29,    22,    23,    24,    25,    26,    27,    28,    29,
      -1,    -1,    40,    41,    42,    43,    -1,    -1,    -1,    -1,
      40,    41,    42,    43,    -1,    45,    24,    25,    26,    27,
      28,    29,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    40,    41,    42,    43
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     5,     7,     8,    34,    35,    58,    59,    60,    65,
      71,    72,     4,     4,     4,    75,     4,     4,     0,     3,
       4,    11,    12,    13,    14,    15,    30,    31,    32,    39,
      48,    50,    54,    56,    61,    66,    70,    76,    79,    80,
      60,    59,    48,    53,    44,    52,    53,    36,    46,    50,
      48,    76,    79,    76,     4,    79,    76,    76,    52,     4,
      74,    75,    76,    61,    76,    48,    80,    61,    22,    23,
      24,    25,    26,    27,    28,    29,    40,    41,    42,    43,
      52,    44,    46,    48,    53,    55,     6,    73,    74,     4,
       9,    10,    16,    17,    35,    48,    68,    68,    76,     4,
      63,     4,    62,    38,    60,    64,    76,    77,    78,    52,
      52,    20,    48,    52,    18,    19,    49,    49,    51,    54,
      76,    76,    76,    76,    76,    76,    76,    76,    76,    76,
      76,    76,    76,    68,    76,    77,    76,     4,     4,    49,
      68,    50,     4,    67,    68,    52,    45,    52,    36,    46,
      50,    44,    45,    47,    48,    51,    60,    45,    49,    76,
      77,    66,    66,    44,    49,    47,    49,    52,    44,    75,
      46,    49,    45,    74,    63,    67,    60,    64,     4,    62,
      36,    50,    74,    52,    51,    78,    52,    49,    21,    37,
      68,    69,    68,    51,    67,    33,    67,    47,    51,    60,
      45,    63,    60,    64,    49,    52,    52,    66,    33,    33,
      47,    68,    69,    36,    52,    51,    62,    50,    51,    60,
      50,    50,    50,    63,    52,    60,    64,    52,    51,    59,
      59,    59,    51,    60,    52,    51,    51,    51,    52,    51,
      52,    52
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    57,    58,    59,    60,    60,    61,    61,    62,    62,
      62,    62,    63,    63,    63,    63,    64,    65,    65,    65,
      65,    65,    65,    65,    65,    65,    65,    65,    65,    66,
      66,    66,    66,    66,    66,    66,    66,    66,    66,    66,
      66,    67,    67,    67,    68,    68,    68,    68,    68,    68,
      68,    68,    68,    69,    70,    70,    71,    72,    73,    74,
      74,    75,    75,    76,    76,    76,    76,    76,    76,    76,
      76,    76,    76,    76,    76,    76,    76,    77,    77,    78,
      78,    79,    79,    79,    80,    80,    80,    80,    80,    80,
      80,    80,    80,    80,    80,    80
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     0,     2,     0,     2,     5,     3,
       1,     3,     1,     4,     3,     6,     8,     3,     1,     5,
       5,     6,     9,    11,     8,     7,    10,    12,     9,     3,
       6,     4,     3,     6,     5,     4,     3,     4,     3,     2,
       2,     3,     1,     0,     1,     2,     5,     1,     1,     2,
       4,     5,     5,     1,     9,     9,     3,     7,     2,     1,
       0,     5,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     1,     1,     0,     1,
       3,     1,     4,     3,     1,     4,     3,     2,     3,     1,
       1,     1,     1,     1,     3,     4
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return (YYSIZE_T) (yystpcpy (yyres, yystr) - yyres);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yynewstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  *yyssp = (yytype_int16) yystate;

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = (YYSIZE_T) (yyssp - yyss + 1);

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 105 "parser.y" /* yacc.c:1652  */
    { theexpression = (yyvsp[0].body);}
#line 1487 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 3:
#line 109 "parser.y" /* yacc.c:1652  */
    {(yyval.body) = makeBody((yyvsp[-1].declarationList), (yyvsp[0].statementList));}
#line 1493 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 4:
#line 113 "parser.y" /* yacc.c:1652  */
    {(yyval.declarationList) = NULL;}
#line 1499 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 5:
#line 115 "parser.y" /* yacc.c:1652  */
    {(yyval.declarationList) = makeDeclarationList((yyvsp[-1].declaration), (yyvsp[0].declarationList));}
#line 1505 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 6:
#line 119 "parser.y" /* yacc.c:1652  */
    {(yyval.statementList) = NULL;}
#line 1511 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 7:
#line 121 "parser.y" /* yacc.c:1652  */
    {(yyval.statementList) = makeStatementList((yyvsp[-1].statement), (yyvsp[0].statementList));}
#line 1517 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 8:
#line 125 "parser.y" /* yacc.c:1652  */
    {(yyval.typeList) = makeGenericTypeList((yyvsp[0].typeList), (yyvsp[-4].stringconst), (yyvsp[-2].stringconst));}
#line 1523 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 9:
#line 127 "parser.y" /* yacc.c:1652  */
    {(yyval.typeList) = makeGenericTypeList((yyvsp[0].typeList), (yyvsp[-2].stringconst), NULL);}
#line 1529 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 10:
#line 129 "parser.y" /* yacc.c:1652  */
    {(yyval.typeList) = makeGenericTypeList(NULL, (yyvsp[0].stringconst), NULL);}
#line 1535 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 11:
#line 131 "parser.y" /* yacc.c:1652  */
    {(yyval.typeList) = makeGenericTypeList(NULL, (yyvsp[-2].stringconst), (yyvsp[0].stringconst));}
#line 1541 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 12:
#line 135 "parser.y" /* yacc.c:1652  */
    {(yyval.typeList) = makeExtensionList(NULL, (yyvsp[0].stringconst), NULL);}
#line 1547 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 13:
#line 137 "parser.y" /* yacc.c:1652  */
    {(yyval.typeList) = makeExtensionList(NULL, (yyvsp[-3].stringconst), (yyvsp[-1].typeList));}
#line 1553 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 14:
#line 139 "parser.y" /* yacc.c:1652  */
    {(yyval.typeList) = makeExtensionList((yyvsp[0].typeList), (yyvsp[-2].stringconst), NULL);}
#line 1559 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 15:
#line 141 "parser.y" /* yacc.c:1652  */
    {(yyval.typeList) = makeExtensionList((yyvsp[0].typeList), (yyvsp[-5].stringconst), (yyvsp[-3].typeList));}
#line 1565 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 16:
#line 145 "parser.y" /* yacc.c:1652  */
    {(yyval.constructor) = makeClassConstructor((yyvsp[-5].varDelList), (yyvsp[-2].body)); }
#line 1571 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 17:
#line 149 "parser.y" /* yacc.c:1652  */
    {(yyval.declaration) = makeVarDeclarations((yyvsp[-1].varDelList)); }
#line 1577 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 18:
#line 151 "parser.y" /* yacc.c:1652  */
    {(yyval.declaration) = makeFunctionDecleration((yyvsp[0].function)); }
#line 1583 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 19:
#line 153 "parser.y" /* yacc.c:1652  */
    {(yyval.declaration) = makeTypeDeclaration((yyvsp[-3].stringconst), (yyvsp[-1].type)); }
#line 1589 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 20:
#line 155 "parser.y" /* yacc.c:1652  */
    {(yyval.declaration) = makeValDeclaration((yyvsp[-3].stringconst), (yyvsp[-1].expression));}
#line 1595 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 21:
#line 157 "parser.y" /* yacc.c:1652  */
    {(yyval.declaration) = makeClassDeclaration((yyvsp[-4].stringconst), (yyvsp[-2].declarationList), NULL, NULL, NULL);}
#line 1601 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 22:
#line 159 "parser.y" /* yacc.c:1652  */
    {(yyval.declaration) = makeClassDeclaration((yyvsp[-7].stringconst), (yyvsp[-2].declarationList), (yyvsp[-5].typeList), NULL, NULL);}
#line 1607 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 23:
#line 161 "parser.y" /* yacc.c:1652  */
    {(yyval.declaration) = makeClassDeclaration((yyvsp[-9].stringconst), (yyvsp[-2].declarationList), (yyvsp[-7].typeList), (yyvsp[-4].typeList), NULL);}
#line 1613 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 24:
#line 163 "parser.y" /* yacc.c:1652  */
    {(yyval.declaration) = makeClassDeclaration((yyvsp[-6].stringconst), (yyvsp[-2].declarationList), NULL, (yyvsp[-4].typeList), NULL);}
#line 1619 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 25:
#line 166 "parser.y" /* yacc.c:1652  */
    {(yyval.declaration) = makeClassDeclaration((yyvsp[-5].stringconst), (yyvsp[-2].declarationList), NULL, NULL, (yyvsp[-3].constructor));}
#line 1625 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 26:
#line 168 "parser.y" /* yacc.c:1652  */
    {(yyval.declaration) = makeClassDeclaration((yyvsp[-8].stringconst), (yyvsp[-2].declarationList), (yyvsp[-6].typeList), NULL, (yyvsp[-3].constructor));}
#line 1631 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 27:
#line 170 "parser.y" /* yacc.c:1652  */
    {(yyval.declaration) = makeClassDeclaration((yyvsp[-10].stringconst), (yyvsp[-2].declarationList), (yyvsp[-8].typeList), (yyvsp[-5].typeList), (yyvsp[-3].constructor));}
#line 1637 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 28:
#line 172 "parser.y" /* yacc.c:1652  */
    {(yyval.declaration) = makeClassDeclaration((yyvsp[-7].stringconst), (yyvsp[-2].declarationList), NULL, (yyvsp[-5].typeList), (yyvsp[-3].constructor));}
#line 1643 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 29:
#line 176 "parser.y" /* yacc.c:1652  */
    {(yyval.statement) = makeReturnStatement((yyvsp[-1].expression));}
#line 1649 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 30:
#line 178 "parser.y" /* yacc.c:1652  */
    {(yyval.statement) = makeIfElseStatement((yyvsp[-4].expression), (yyvsp[-2].statement), (yyvsp[0].statement));}
#line 1655 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 31:
#line 180 "parser.y" /* yacc.c:1652  */
    {(yyval.statement) = makeIfStatement((yyvsp[-2].expression), (yyvsp[0].statement));}
#line 1661 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 32:
#line 182 "parser.y" /* yacc.c:1652  */
    {(yyval.statement) = makeAllocateStatement((yyvsp[-1].variable));}
#line 1667 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 33:
#line 184 "parser.y" /* yacc.c:1652  */
    {(yyval.statement) = makeAllocateWithConstructorStatement((yyvsp[-4].variable), (yyvsp[-2].expressionList));}
#line 1673 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 34:
#line 186 "parser.y" /* yacc.c:1652  */
    {(yyval.statement) = makeAllocateOfLenStatement((yyvsp[-3].variable), (yyvsp[-1].expression));}
#line 1679 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 35:
#line 188 "parser.y" /* yacc.c:1652  */
    {(yyval.statement) = makeAssignment((yyvsp[-3].variable), (yyvsp[-1].expression));}
#line 1685 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 36:
#line 190 "parser.y" /* yacc.c:1652  */
    {(yyval.statement) = makeWriteStatement((yyvsp[-1].expression));}
#line 1691 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 37:
#line 192 "parser.y" /* yacc.c:1652  */
    {(yyval.statement) = makeWhileStatement((yyvsp[-2].expression), (yyvsp[0].statement));}
#line 1697 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 38:
#line 194 "parser.y" /* yacc.c:1652  */
    {(yyval.statement) = makeStatementFromList((yyvsp[-1].statementList));}
#line 1703 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 39:
#line 196 "parser.y" /* yacc.c:1652  */
    {(yyval.statement) = makeEmptyExpression((yyvsp[-1].expression));}
#line 1709 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 40:
#line 198 "parser.y" /* yacc.c:1652  */
    {(yyval.statement) = makeGCStatement();}
#line 1715 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 41:
#line 202 "parser.y" /* yacc.c:1652  */
    {(yyval.typeList) = makeTypeList((yyvsp[0].typeList), (yyvsp[-2].type));}
#line 1721 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 42:
#line 204 "parser.y" /* yacc.c:1652  */
    {(yyval.typeList) = makeTypeList(NULL, (yyvsp[0].type));}
#line 1727 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 43:
#line 206 "parser.y" /* yacc.c:1652  */
    {(yyval.typeList) = NULL;}
#line 1733 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 44:
#line 210 "parser.y" /* yacc.c:1652  */
    {(yyval.type) = makeIdType((yyvsp[0].stringconst)); }
#line 1739 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 45:
#line 212 "parser.y" /* yacc.c:1652  */
    {(yyval.type) = makeClassType((yyvsp[0].stringconst), NULL); }
#line 1745 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 46:
#line 214 "parser.y" /* yacc.c:1652  */
    {(yyval.type) = makeClassType((yyvsp[-3].stringconst), (yyvsp[-1].typeList)); }
#line 1751 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 47:
#line 216 "parser.y" /* yacc.c:1652  */
    {(yyval.type) = makeIntType(); }
#line 1757 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 48:
#line 218 "parser.y" /* yacc.c:1652  */
    {(yyval.type) = makeBoolType(); }
#line 1763 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 49:
#line 220 "parser.y" /* yacc.c:1652  */
    {(yyval.type) = makeArrayType((yyvsp[0].type)); }
#line 1769 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 50:
#line 222 "parser.y" /* yacc.c:1652  */
    {(yyval.type) = makeRecordType((yyvsp[-1].varDelList)); }
#line 1775 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 51:
#line 224 "parser.y" /* yacc.c:1652  */
    {(yyval.type) = makeLambdaType((yyvsp[-3].typeList), (yyvsp[0].type));}
#line 1781 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 52:
#line 226 "parser.y" /* yacc.c:1652  */
    {(yyval.type) = makeLambdaType((yyvsp[-3].typeList), (yyvsp[0].type));}
#line 1787 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 53:
#line 230 "parser.y" /* yacc.c:1652  */
    {(yyval.type) = makeVoidType(); }
#line 1793 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 54:
#line 234 "parser.y" /* yacc.c:1652  */
    {(yyval.lambda) = makeLambda((yyvsp[-7].varDelList), (yyvsp[-4].type), (yyvsp[-1].body));}
#line 1799 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 55:
#line 236 "parser.y" /* yacc.c:1652  */
    {(yyval.lambda) = makeLambda((yyvsp[-7].varDelList), (yyvsp[-4].type), (yyvsp[-1].body));}
#line 1805 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 56:
#line 240 "parser.y" /* yacc.c:1652  */
    {(yyval.function) = makeFunction((yyvsp[-2].functionHead), (yyvsp[-1].body), (yyvsp[0].functionTail));}
#line 1811 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 57:
#line 244 "parser.y" /* yacc.c:1652  */
    {(yyval.functionHead) = makeFunctionHead((yyvsp[-5].stringconst), (yyvsp[-3].varDelList), (yyvsp[0].type));}
#line 1817 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 58:
#line 248 "parser.y" /* yacc.c:1652  */
    {(yyval.functionTail) = makeFunctionTail((yyvsp[0].stringconst));}
#line 1823 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 59:
#line 252 "parser.y" /* yacc.c:1652  */
    {(yyval.varDelList)=(yyvsp[0].varDelList);}
#line 1829 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 60:
#line 254 "parser.y" /* yacc.c:1652  */
    {(yyval.varDelList)=NULL;}
#line 1835 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 61:
#line 258 "parser.y" /* yacc.c:1652  */
    {(yyval.varDelList) = makeVarDelList((yyvsp[-4].stringconst), (yyvsp[-2].type), (yyvsp[0].varDelList));}
#line 1841 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 62:
#line 260 "parser.y" /* yacc.c:1652  */
    {(yyval.varDelList) = makeVarDelList((yyvsp[-2].stringconst), (yyvsp[0].type), NULL); }
#line 1847 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 63:
#line 264 "parser.y" /* yacc.c:1652  */
    {(yyval.expression) = (yyvsp[-1].expression);}
#line 1853 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 64:
#line 268 "parser.y" /* yacc.c:1652  */
    {(yyval.expression) = makeEXPOpEXP((yyvsp[-2].expression), makeMultOp(), (yyvsp[0].expression));}
#line 1859 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 65:
#line 270 "parser.y" /* yacc.c:1652  */
    {(yyval.expression) = makeEXPOpEXP((yyvsp[-2].expression), makeDivOp(), (yyvsp[0].expression));}
#line 1865 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 66:
#line 272 "parser.y" /* yacc.c:1652  */
    {(yyval.expression) = makeEXPOpEXP((yyvsp[-2].expression), makePlusOp(), (yyvsp[0].expression));}
#line 1871 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 67:
#line 274 "parser.y" /* yacc.c:1652  */
    {(yyval.expression) = makeEXPOpEXP((yyvsp[-2].expression), makeMinusOp(), (yyvsp[0].expression));}
#line 1877 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 68:
#line 276 "parser.y" /* yacc.c:1652  */
    {(yyval.expression) = makeEXPOpEXP((yyvsp[-2].expression), makeEqualityOp(), (yyvsp[0].expression));}
#line 1883 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 69:
#line 278 "parser.y" /* yacc.c:1652  */
    {(yyval.expression) = makeEXPOpEXP((yyvsp[-2].expression), makeInequalityOp(), (yyvsp[0].expression));}
#line 1889 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 70:
#line 280 "parser.y" /* yacc.c:1652  */
    {(yyval.expression) = makeEXPOpEXP((yyvsp[-2].expression), makeGreaterOp(), (yyvsp[0].expression));}
#line 1895 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 71:
#line 282 "parser.y" /* yacc.c:1652  */
    {(yyval.expression) = makeEXPOpEXP((yyvsp[-2].expression), makeLessOp(), (yyvsp[0].expression));}
#line 1901 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 72:
#line 284 "parser.y" /* yacc.c:1652  */
    {(yyval.expression) = makeEXPOpEXP((yyvsp[-2].expression), makeGeqOp(), (yyvsp[0].expression));}
#line 1907 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 73:
#line 286 "parser.y" /* yacc.c:1652  */
    {(yyval.expression) = makeEXPOpEXP((yyvsp[-2].expression), makeLeqOp(), (yyvsp[0].expression));}
#line 1913 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 74:
#line 288 "parser.y" /* yacc.c:1652  */
    {(yyval.expression) = makeEXPOpEXP((yyvsp[-2].expression), makeAndOp(), (yyvsp[0].expression));}
#line 1919 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 75:
#line 290 "parser.y" /* yacc.c:1652  */
    {(yyval.expression) = makeEXPOpEXP((yyvsp[-2].expression), makeOrOp(), (yyvsp[0].expression));}
#line 1925 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 76:
#line 292 "parser.y" /* yacc.c:1652  */
    {(yyval.expression) = makeEXPFromTerm((yyvsp[0].term));}
#line 1931 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 77:
#line 296 "parser.y" /* yacc.c:1652  */
    {(yyval.expressionList) = (yyvsp[0].expressionList);}
#line 1937 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 78:
#line 298 "parser.y" /* yacc.c:1652  */
    {(yyval.expressionList) = NULL;}
#line 1943 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 79:
#line 302 "parser.y" /* yacc.c:1652  */
    {(yyval.expressionList) = makeExpList((yyvsp[0].expression), NULL); }
#line 1949 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 80:
#line 304 "parser.y" /* yacc.c:1652  */
    {(yyval.expressionList) = makeExpList((yyvsp[-2].expression), (yyvsp[0].expressionList)); }
#line 1955 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 81:
#line 334 "parser.y" /* yacc.c:1652  */
    {(yyval.variable) = makeVariable((yyvsp[0].stringconst));}
#line 1961 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 82:
#line 336 "parser.y" /* yacc.c:1652  */
    {(yyval.variable) = makeArraySubscript((yyvsp[-3].variable), (yyvsp[-1].expression));}
#line 1967 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 83:
#line 338 "parser.y" /* yacc.c:1652  */
    {(yyval.variable) = makeRecordSubscript((yyvsp[-2].variable), (yyvsp[0].stringconst));}
#line 1973 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 84:
#line 342 "parser.y" /* yacc.c:1652  */
    {(yyval.term) = makeTermFromVariable((yyvsp[0].variable));}
#line 1979 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 85:
#line 344 "parser.y" /* yacc.c:1652  */
    {(yyval.term) = makeFunctionCallTerm((yyvsp[-3].stringconst), (yyvsp[-1].expressionList));}
#line 1985 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 86:
#line 346 "parser.y" /* yacc.c:1652  */
    {(yyval.term) = makeParentheses((yyvsp[-1].expression));}
#line 1991 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 87:
#line 348 "parser.y" /* yacc.c:1652  */
    {(yyval.term) = makeNegatedTerm((yyvsp[0].term));}
#line 1997 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 88:
#line 350 "parser.y" /* yacc.c:1652  */
    {(yyval.term) = makeAbsTerm((yyvsp[-1].expression));}
#line 2003 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 89:
#line 352 "parser.y" /* yacc.c:1652  */
    {(yyval.term) = makeNumTerm((yyvsp[0].intconst));}
#line 2009 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 90:
#line 354 "parser.y" /* yacc.c:1652  */
    {(yyval.term) = makeTrueTerm();}
#line 2015 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 91:
#line 356 "parser.y" /* yacc.c:1652  */
    {(yyval.term) = makeFalseTerm();}
#line 2021 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 92:
#line 358 "parser.y" /* yacc.c:1652  */
    {(yyval.term) = makeNullTerm();}
#line 2027 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 93:
#line 360 "parser.y" /* yacc.c:1652  */
    {(yyval.term) = makeLambdaTerm((yyvsp[0].lambda));}
#line 2033 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 94:
#line 362 "parser.y" /* yacc.c:1652  */
    {(yyval.term) = makeDowncastTerm((yyvsp[-2].variable), (yyvsp[0].type));}
#line 2039 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;

  case 95:
#line 364 "parser.y" /* yacc.c:1652  */
    {(yyval.term) = makeShorthandLambdaCall((yyvsp[-3].variable), (yyvsp[-1].expressionList));}
#line 2045 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
    break;


#line 2049 "/home/valde/Git/DM501-compiler/src/scan_parse/parser.c" /* yacc.c:1652  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 367 "parser.y" /* yacc.c:1918  */
