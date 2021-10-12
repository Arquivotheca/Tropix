
/*  A Bison parser, made from aic/aicasm.y with Bison version GNU Bison version 1.21
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	T_REGISTER	258
#define	T_CONST	259
#define	T_DOWNLOAD	260
#define	T_SCB	261
#define	T_SRAM	262
#define	T_ALIAS	263
#define	T_SIZE	264
#define	T_ADDRESS	265
#define	T_ACCESS_MODE	266
#define	T_MODE	267
#define	T_BEGIN_CS	268
#define	T_END_CS	269
#define	T_BIT	270
#define	T_MASK	271
#define	T_NUMBER	272
#define	T_PATH	273
#define	T_CEXPR	274
#define	T_EOF	275
#define	T_INCLUDE	276
#define	T_SHR	277
#define	T_SHL	278
#define	T_ROR	279
#define	T_ROL	280
#define	T_MVI	281
#define	T_MOV	282
#define	T_CLR	283
#define	T_BMOV	284
#define	T_JMP	285
#define	T_JC	286
#define	T_JNC	287
#define	T_JE	288
#define	T_JNE	289
#define	T_JNZ	290
#define	T_JZ	291
#define	T_CALL	292
#define	T_ADD	293
#define	T_ADC	294
#define	T_INC	295
#define	T_DEC	296
#define	T_STC	297
#define	T_CLC	298
#define	T_CMP	299
#define	T_NOT	300
#define	T_XOR	301
#define	T_TEST	302
#define	T_AND	303
#define	T_OR	304
#define	T_RET	305
#define	T_NOP	306
#define	T_ACCUM	307
#define	T_ALLONES	308
#define	T_ALLZEROS	309
#define	T_NONE	310
#define	T_SINDEX	311
#define	T_A	312
#define	T_SYMBOL	313
#define	T_NL	314
#define	T_IF	315
#define	T_ELSE	316
#define	T_ELSE_IF	317
#define	T_ENDIF	318
#define	UMINUS	319

#line 1 "aic/aicasm.y"

/*
 ****************************************************************
 *								*
 *			asm.2940.y				*
 *								*
 *	Montador para a linguagem simbólica da placa 2940	*
 *								*
 *	Versão	4.0.0, de 08.03.01				*
 *		4.0.0, de 08.03.01				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/types.h"
#include "../h/queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../aic/sysexits.h"
#include "../aic/aicasm.h"
#include "../aic/asm_symbol.h"
#include "../aic/asm_format.h"

int yylineno;
const char *yyfilename;
static symbol_t *cur_symbol;
static symtype cur_symtype;
static symbol_t *accumulator;
static symbol_ref_t allones;
static symbol_ref_t allzeros;
static symbol_ref_t none;
static symbol_ref_t sindex;
static int instruction_ptr;
static int sram_or_scb_offset;
static int download_constant_count;
static int in_critical_section;

static void process_bitmask(int mask_type, symbol_t *sym, int mask);
static void initialize_symbol(symbol_t *symbol);
static void process_register(symbol_t **p_symbol);
static void format_1_instr(int opcode, symbol_ref_t *dest,
			   expression_t *immed, symbol_ref_t *src, int ret);
static void format_2_instr(int opcode, symbol_ref_t *dest,
			   expression_t *places, symbol_ref_t *src, int ret);
static void format_3_instr(int opcode, symbol_ref_t *src,
			   expression_t *immed, symbol_ref_t *address);
static void test_readable_symbol(symbol_t *symbol);
static void test_writable_symbol(symbol_t *symbol);
static void type_check(symbol_t *symbol, expression_t *expression, int and_op);
static void make_expression(expression_t *immed, int value);
static void add_conditional(symbol_t *symbol);
static int  is_download_const(expression_t *immed);

/*
 ******	Protótipos de funções ***********************************
 */
extern void		symlist_free (symlist_t *);
extern void		symlist_merge (symlist_t *, symlist_t *, symlist_t *);
extern void		symlist_add (symlist_t *, symbol_t *, int);
extern symbol_t		*symtable_get (const char *);
extern symbol_node_t	*symlist_search (const symlist_t *, const char *);

extern int		yylex (void);
extern void		yyerror (const char *string);

#define YYDEBUG 1
#define SRAM_SYMNAME "SRAM_BASE"
#define SCB_SYMNAME "SCB_BASE"

#line 80 "aic/aicasm.y"
typedef union {
	int		value;
	char		*str;
	symbol_t	*sym;
	symbol_ref_t	sym_ref;
	expression_t	expression;
} YYSTYPE;

#ifndef YYLTYPE
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;

#define YYLTYPE yyltype
#endif

#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		276
#define	YYFLAG		-32768
#define	YYNTBASE	83

#define YYTRANSLATE(x) ((unsigned)(x) <= 319 ? yytranslate[x] : 130)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    72,     2,     2,     2,    65,     2,    75,
    76,     2,    66,    79,    67,    81,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    80,    82,    70,
     2,    71,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    77,     2,    78,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    73,    64,    74,    68,     2,     2,     2,     2,
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
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
    46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
    56,    57,    58,    59,    60,    61,    62,    63,    69
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     5,     7,    10,    12,    15,    17,    20,    22,
    25,    27,    30,    32,    35,    37,    40,    42,    45,    47,
    50,    55,    60,    61,    65,    66,    72,    74,    77,    79,
    81,    83,    85,    87,    89,    91,    93,    95,    97,    99,
   102,   105,   108,   112,   116,   119,   121,   123,   125,   127,
   129,   133,   137,   141,   145,   149,   152,   155,   157,   159,
   163,   167,   169,   172,   173,   174,   182,   183,   184,   192,
   194,   197,   199,   204,   209,   211,   213,   215,   217,   219,
   221,   222,   225,   226,   228,   230,   232,   235,   237,   241,
   245,   247,   251,   255,   259,   264,   267,   269,   271,   273,
   275,   277,   285,   293,   299,   305,   309,   317,   321,   326,
   335,   342,   349,   355,   360,   364,   367,   369,   371,   373,
   375,   383,   385,   387,   389,   391,   393,   395,   397,   399,
   403,   411,   419,   427,   433
};

static const short yyrhs[] = {    84,
     0,    83,    84,     0,    85,     0,    83,    85,     0,   103,
     0,    83,   103,     0,   105,     0,    83,   105,     0,   108,
     0,    83,   108,     0,   121,     0,    83,   121,     0,   119,
     0,    83,   119,     0,   120,     0,    83,   120,     0,   123,
     0,    83,   123,     0,   125,     0,    83,   125,     0,    21,
    70,    18,    71,     0,    21,    72,    18,    72,     0,     0,
     3,    86,    87,     0,     0,    58,    73,    88,    89,    74,
     0,    90,     0,    89,    90,     0,    91,     0,    92,     0,
    93,     0,    94,     0,    95,     0,    96,     0,    97,     0,
    98,     0,    99,     0,   100,     0,   101,     0,    10,    17,
     0,     9,    17,     0,    11,    12,     0,    15,    58,    17,
     0,    16,    58,   102,     0,     8,    58,     0,    52,     0,
    53,     0,    54,     0,    55,     0,    56,     0,   102,    64,
   102,     0,   102,    65,   102,     0,   102,    66,   102,     0,
   102,    67,   102,     0,    75,   102,    76,     0,    68,   102,
     0,    67,   102,     0,    17,     0,    58,     0,     4,    58,
   104,     0,     4,    58,     5,     0,    17,     0,    67,    17,
     0,     0,     0,     7,    73,   106,    91,   107,   111,    74,
     0,     0,     0,     6,    73,   109,    91,   110,   111,    74,
     0,    87,     0,   111,    87,     0,    58,     0,    58,    77,
    58,    78,     0,    58,    77,    17,    78,     0,    57,     0,
   112,     0,   102,     0,   102,     0,    57,     0,   112,     0,
     0,    79,   116,     0,     0,    50,     0,    13,     0,    14,
     0,    58,    80,     0,    58,     0,    58,    66,    17,     0,
    58,    67,    17,     0,    81,     0,    81,    66,    17,     0,
    81,    67,    17,     0,    60,    19,    73,     0,    61,    60,
    19,    73,     0,    61,    73,     0,    74,     0,    48,     0,
    46,     0,    38,     0,    39,     0,   124,   113,    79,   115,
   117,   118,    82,     0,    49,   112,    79,   115,   117,   118,
    82,     0,    40,   113,   117,   118,    82,     0,    41,   113,
   117,   118,    82,     0,    43,   118,    82,     0,    43,    26,
   113,    79,   115,   118,    82,     0,    42,   118,    82,     0,
    42,   113,   118,    82,     0,    29,   113,    79,   116,    79,
   114,   118,    82,     0,    27,   113,    79,   116,   118,    82,
     0,    26,   113,    79,   115,   118,    82,     0,    45,   113,
   117,   118,    82,     0,    28,   113,   118,    82,     0,    51,
   118,    82,     0,    50,    82,     0,    23,     0,    22,     0,
    25,     0,    24,     0,   126,   113,    79,   102,   117,   118,
    82,     0,    30,     0,    31,     0,    32,     0,    37,     0,
    36,     0,    35,     0,    33,     0,    34,     0,   127,   122,
    82,     0,    49,   112,    79,   114,   127,   122,    82,     0,
    47,   116,    79,   115,   128,   122,    82,     0,    44,   116,
    79,   115,   129,   122,    82,     0,    27,   116,   127,   122,
    82,     0,    26,   114,   127,   122,    82,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   171,   173,   174,   175,   176,   177,   178,   179,   180,   181,
   182,   183,   184,   185,   186,   187,   188,   189,   190,   191,
   194,   197,   201,   203,   205,   217,   243,   245,   248,   250,
   251,   252,   253,   254,   255,   256,   257,   258,   259,   262,
   269,   276,   283,   290,   297,   311,   323,   335,   347,   359,
   371,   379,   386,   393,   400,   404,   409,   414,   419,   458,
   471,   490,   495,   501,   515,   519,   525,   541,   545,   551,
   553,   556,   563,   578,   589,   600,   608,   613,   618,   625,
   633,   638,   642,   644,   648,   663,   677,   690,   696,   701,
   706,   711,   716,   723,   734,   759,   784,   809,   811,   812,
   813,   816,   823,   830,   840,   850,   858,   864,   872,   881,
   888,   898,   905,   915,   925,   935,   954,   956,   957,   958,
   961,   968,   970,   971,   972,   975,   977,   980,   982,   985,
   995,  1002,  1009,  1016,  1026
};

static const char * const yytname[] = {   "$","error","$illegal.","T_REGISTER",
"T_CONST","T_DOWNLOAD","T_SCB","T_SRAM","T_ALIAS","T_SIZE","T_ADDRESS","T_ACCESS_MODE",
"T_MODE","T_BEGIN_CS","T_END_CS","T_BIT","T_MASK","T_NUMBER","T_PATH","T_CEXPR",
"T_EOF","T_INCLUDE","T_SHR","T_SHL","T_ROR","T_ROL","T_MVI","T_MOV","T_CLR",
"T_BMOV","T_JMP","T_JC","T_JNC","T_JE","T_JNE","T_JNZ","T_JZ","T_CALL","T_ADD",
"T_ADC","T_INC","T_DEC","T_STC","T_CLC","T_CMP","T_NOT","T_XOR","T_TEST","T_AND",
"T_OR","T_RET","T_NOP","T_ACCUM","T_ALLONES","T_ALLZEROS","T_NONE","T_SINDEX",
"T_A","T_SYMBOL","T_NL","T_IF","T_ELSE","T_ELSE_IF","T_ENDIF","'|'","'&'","'+'",
"'-'","'~'","UMINUS","'<'","'>'","'\"'","'{'","'}'","'('","')'","'['","']'",
"','","':'","'.'","';'","program","include","register","@1","reg_definition",
"@2","reg_attribute_list","reg_attribute","reg_address","size","access_mode",
"bit_defn","mask_defn","alias","accumulator","allones","allzeros","none","sindex",
"expression","constant","numerical_value","scratch_ram","@3","@4","scb","@5",
"@6","scb_or_sram_reg_list","reg_symbol","destination","immediate","immediate_or_a",
"source","opt_source","ret","critical_section_start","critical_section_end",
"label","address","conditional","f1_opcode","code","f2_opcode","jmp_jc_jnc_call",
"jz_jnz","je_jne",""
};
#endif

static const short yyr1[] = {     0,
    83,    83,    83,    83,    83,    83,    83,    83,    83,    83,
    83,    83,    83,    83,    83,    83,    83,    83,    83,    83,
    84,    84,    86,    85,    88,    87,    89,    89,    90,    90,
    90,    90,    90,    90,    90,    90,    90,    90,    90,    91,
    92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
   102,   102,   102,   102,   102,   102,   102,   102,   102,   103,
   103,   104,   104,   106,   107,   105,   109,   110,   108,   111,
   111,   112,   112,   112,   112,   113,   114,   115,   115,   116,
   117,   117,   118,   118,   119,   120,   121,   122,   122,   122,
   122,   122,   122,   123,   123,   123,   123,   124,   124,   124,
   124,   125,   125,   125,   125,   125,   125,   125,   125,   125,
   125,   125,   125,   125,   125,   125,   126,   126,   126,   126,
   125,   127,   127,   127,   127,   128,   128,   129,   129,   125,
   125,   125,   125,   125,   125
};

static const short yyr2[] = {     0,
     1,     2,     1,     2,     1,     2,     1,     2,     1,     2,
     1,     2,     1,     2,     1,     2,     1,     2,     1,     2,
     4,     4,     0,     3,     0,     5,     1,     2,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
     2,     2,     3,     3,     2,     1,     1,     1,     1,     1,
     3,     3,     3,     3,     3,     2,     2,     1,     1,     3,
     3,     1,     2,     0,     0,     7,     0,     0,     7,     1,
     2,     1,     4,     4,     1,     1,     1,     1,     1,     1,
     0,     2,     0,     1,     1,     1,     2,     1,     3,     3,
     1,     3,     3,     3,     4,     2,     1,     1,     1,     1,
     1,     7,     7,     5,     5,     3,     7,     3,     4,     8,
     6,     6,     5,     4,     3,     2,     1,     1,     1,     1,
     7,     1,     1,     1,     1,     1,     1,     1,     1,     3,
     7,     7,     7,     5,     5
};

static const short yydefact[] = {     0,
    23,     0,     0,     0,    85,    86,     0,   118,   117,   120,
   119,     0,     0,     0,     0,   122,   123,   124,   125,   100,
   101,     0,     0,    83,    83,     0,     0,    99,     0,    98,
     0,     0,    83,     0,     0,     0,    97,     0,     1,     3,
     5,     7,     9,    13,    15,    11,    17,     0,    19,     0,
     0,     0,     0,    67,    64,     0,     0,    58,    75,    59,
     0,     0,     0,    77,    76,     0,     0,    72,    80,     0,
     0,    83,     0,    81,    81,    84,    83,     0,     0,     0,
    80,     0,    81,     0,     0,   116,     0,    87,     0,     0,
    96,     2,     4,     6,     8,    10,    14,    16,    12,    18,
    20,     0,     0,    88,    91,     0,     0,    24,    61,    62,
     0,    60,     0,     0,     0,     0,     0,    59,    57,    56,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    83,    83,     0,   108,     0,   106,     0,    83,
     0,     0,   115,    94,     0,     0,     0,     0,     0,     0,
     0,   130,    25,    63,     0,    68,    65,    21,    22,     0,
     0,    55,    51,    52,    53,    54,    79,    78,    83,     0,
    83,     0,   114,     0,    82,     0,     0,   109,     0,     0,
     0,     0,    77,     0,    81,    95,    81,    81,    89,    90,
    92,    93,     0,    40,     0,     0,    74,    73,     0,   135,
     0,   134,     0,   104,   105,    83,   128,   129,     0,   113,
   127,   126,     0,     0,    83,    83,    83,     0,     0,     0,
     0,     0,    46,    47,    48,    49,    50,     0,    27,    29,
    30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
    70,     0,     0,   112,   111,    83,     0,     0,     0,     0,
     0,     0,     0,    45,    41,    42,     0,     0,    26,    28,
    69,    71,    66,     0,   107,   133,   132,   131,   103,   102,
   121,    43,    44,   110,     0,     0
};

static const short yydefgoto[] = {    38,
    39,    40,    52,   241,   193,   228,   229,   230,   231,   232,
   233,   234,   235,   236,   237,   238,   239,   240,   168,    41,
   112,    42,   114,   196,    43,   113,   195,   242,    65,    66,
    67,   169,    71,   133,    78,    44,    45,    46,   106,    47,
    48,    49,    50,    51,   213,   209
};

static const short yypact[] = {   269,
-32768,   -33,   -26,   -14,-32768,-32768,   -24,-32768,-32768,-32768,
-32768,    81,    -2,    -2,    -2,-32768,-32768,-32768,-32768,-32768,
-32768,    -2,    -2,   -21,   -10,    -2,    -2,-32768,    -2,-32768,
    -2,    10,    13,     3,    51,   -52,-32768,   210,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,    -2,-32768,    -2,
   -40,    41,     7,-32768,-32768,    91,   108,-32768,-32768,    11,
    -6,    -6,    -6,   110,-32768,   -11,    70,    43,    52,    55,
    70,    13,    57,    63,    63,-32768,    13,    69,    -2,    73,
-32768,    66,    63,    99,   100,-32768,    98,-32768,   112,   164,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   105,   109,    29,    38,   104,   116,-32768,-32768,-32768,
   173,-32768,   184,   184,   124,   127,    -4,-32768,-32768,-32768,
   106,    -6,    -6,    -6,    -6,    86,   -40,    -2,   -40,   118,
    -2,    -2,    13,    13,   125,-32768,   129,-32768,    86,    13,
    86,    86,-32768,-32768,   133,    86,    -6,   192,   194,   195,
   198,-32768,-32768,-32768,   201,-32768,-32768,-32768,-32768,   141,
   142,-32768,     0,    50,-32768,-32768,-32768,   110,    13,   139,
    13,   140,-32768,   146,-32768,   144,   147,-32768,    86,   107,
   148,   111,   -22,    70,    63,-32768,    63,   102,-32768,-32768,
-32768,-32768,   149,-32768,    41,    41,-32768,-32768,   161,-32768,
   162,-32768,    -6,-32768,-32768,    13,-32768,-32768,   -40,-32768,
-32768,-32768,   -40,   -40,    13,    13,    13,   170,   228,   250,
   205,   206,-32768,-32768,-32768,-32768,-32768,    23,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   -48,   -39,-32768,-32768,    13,   183,   185,   187,   196,
   197,   199,   203,-32768,-32768,-32768,   249,    -6,-32768,-32768,
-32768,-32768,-32768,   204,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,   110,-32768,   274,-32768
};

static const short yypgoto[] = {-32768,
   239,   242,-32768,   -50,-32768,-32768,    59,    49,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   -12,   251,
-32768,   264,-32768,-32768,   265,-32768,-32768,    92,    -9,    58,
  -139,   -55,     1,   -60,   -19,   266,   267,   283,  -120,   284,
-32768,   285,-32768,   -66,-32768,-32768
};


#define	YYLAST		343


static const short yytable[] = {    64,
   127,   108,   184,    69,   129,    80,   170,    90,   172,   107,
    58,   109,   160,    87,   134,    79,    81,   104,   107,    81,
    91,    85,   140,   110,    53,   261,    82,   -78,    76,    84,
   218,   219,   155,   220,   263,    59,    68,   221,   222,    76,
   105,   122,   123,   124,   125,    56,    54,    57,   119,   120,
   121,   118,   130,   161,    59,    68,   -78,   135,    55,   -78,
    61,    62,    76,   246,   123,   124,   125,   126,    63,    89,
    70,    72,    73,   111,   223,   224,   225,   226,   227,    74,
    75,    77,    88,   180,    83,   182,   185,   117,   248,   -72,
   187,    86,   249,   250,   148,   149,   259,    58,   107,    16,
    17,    18,    58,   150,   151,   102,    19,   103,   115,   163,
   164,   165,   166,   176,   177,   124,   125,   214,    81,   117,
   181,    81,    81,   206,   215,   116,   216,   217,   171,   183,
   -76,   174,   175,   128,   188,   131,   137,    59,    60,   207,
   208,   132,   167,   118,   139,   211,   212,    61,    62,   199,
   136,   201,    61,    62,   138,    63,   218,   219,   155,   220,
    63,   156,   157,   221,   222,   122,   123,   124,   125,   122,
   123,   124,   125,   122,   123,   124,   125,   141,   142,   143,
   132,   162,   145,   146,   144,   152,   247,   147,   153,   154,
    64,   262,   262,   155,   158,   251,   252,   253,   159,   173,
   223,   224,   225,   226,   227,   186,   178,   179,   189,   275,
   190,   191,     1,     2,   192,     3,     4,   194,   197,   198,
   200,   202,     5,     6,   203,   204,   264,   254,   205,   210,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,   244,   245,   255,   273,    19,    20,    21,    22,
    23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
    33,   256,   257,   258,   265,   272,   266,    34,   267,    35,
    36,     1,     2,   276,     3,     4,    92,   268,   269,    93,
   270,     5,     6,    37,   271,   274,   260,   243,    94,     7,
     8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
    18,    95,    96,    97,    98,    19,    20,    21,    22,    23,
    24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
    99,   100,   101,     0,     0,     0,    34,     0,    35,    36,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    37
};

static const short yycheck[] = {    12,
    67,    52,   142,    13,    71,    25,   127,    60,   129,    58,
    17,     5,    17,    33,    75,    26,    26,    58,    58,    29,
    73,    31,    83,    17,    58,    74,    26,    50,    50,    29,
     8,     9,    10,    11,    74,    57,    58,    15,    16,    50,
    81,    64,    65,    66,    67,    70,    73,    72,    61,    62,
    63,    58,    72,    58,    57,    58,    79,    77,    73,    82,
    67,    68,    50,   203,    65,    66,    67,    79,    75,    19,
    13,    14,    15,    67,    52,    53,    54,    55,    56,    22,
    23,    24,    80,   139,    27,   141,   142,    77,   209,    79,
   146,    82,   213,   214,    66,    67,    74,    17,    58,    30,
    31,    32,    17,    66,    67,    48,    37,    50,    18,   122,
   123,   124,   125,   133,   134,    66,    67,   184,   128,    77,
   140,   131,   132,   179,   185,    18,   187,   188,   128,   142,
    79,   131,   132,    79,   147,    79,    79,    57,    58,    33,
    34,    79,    57,    58,    79,    35,    36,    67,    68,   169,
    82,   171,    67,    68,    82,    75,     8,     9,    10,    11,
    75,   113,   114,    15,    16,    64,    65,    66,    67,    64,
    65,    66,    67,    64,    65,    66,    67,    79,    79,    82,
    79,    76,    19,    79,    73,    82,   206,    79,    73,    17,
   203,   242,   243,    10,    71,   215,   216,   217,    72,    82,
    52,    53,    54,    55,    56,    73,    82,    79,    17,     0,
    17,    17,     3,     4,    17,     6,     7,    17,    78,    78,
    82,    82,    13,    14,    79,    82,   246,    58,    82,    82,
    21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
    31,    32,    82,    82,    17,   258,    37,    38,    39,    40,
    41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
    51,    12,    58,    58,    82,    17,    82,    58,    82,    60,
    61,     3,     4,     0,     6,     7,    38,    82,    82,    38,
    82,    13,    14,    74,    82,    82,   228,   196,    38,    21,
    22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
    32,    38,    38,    38,    38,    37,    38,    39,    40,    41,
    42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
    38,    38,    38,    -1,    -1,    -1,    58,    -1,    60,    61,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    74
};
/*
 ****************************************************************
 *								*
 *			bison.simple.c				*
 *								*
 *	Skeleton output parser for bison			*
 *								*
 *	Versão	3.0.0, de 10.07.93				*
 *		3.0.0, de 10.07.93				*
 *								*
 *	Módulo: GBISON						*
 *		Gerador de analisadores sintáticos		*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 		Baseado em software homônimo do GNU		*
 *		Copyright © 1999 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#line 23 "bison.simple"

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#define YYLEX		yylex(&yylval, &yylloc)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

#line 113 "bison.simple"
int
yyparse (void)
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
#ifdef YYLSP_NEEDED
		 &yyls1, size * sizeof (*yylsp),
#endif
		 &yystacksize);

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      memmove (yyss, yyss1, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      memmove (yyvs, yyvs1, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      memmove (yyls, yyls1, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 21:
#line 196 "aic/aicasm.y"
{ include_file(yyvsp[-1].str, BRACKETED_INCLUDE); ;
    break;}
case 22:
#line 198 "aic/aicasm.y"
{ include_file(yyvsp[-1].str, QUOTED_INCLUDE); ;
    break;}
case 23:
#line 202 "aic/aicasm.y"
{ cur_symtype = REGISTER; ;
    break;}
case 25:
#line 207 "aic/aicasm.y"
{
			if (yyvsp[-1].sym->type != UNINITIALIZED) {
				stop("Register multiply defined", EX_DATAERR);
				/* NOTREACHED */
			}
			cur_symbol = yyvsp[-1].sym; 
			cur_symbol->type = cur_symtype;
			initialize_symbol(cur_symbol);
		;
    break;}
case 26:
#line 218 "aic/aicasm.y"
{                    
			/*
			 * Default to allowing everything in for registers
			 * with no bit or mask definitions.
			 */
			if (cur_symbol->info.rinfo->valid_bitmask == 0)
				cur_symbol->info.rinfo->valid_bitmask = 0xFF;

			if (cur_symbol->info.rinfo->size == 0)
				cur_symbol->info.rinfo->size = 1;

			/*
			 * This might be useful for registers too.
			 */
			if (cur_symbol->type != REGISTER) {
				if (cur_symbol->info.rinfo->address == 0)
					cur_symbol->info.rinfo->address =
					    sram_or_scb_offset;
				sram_or_scb_offset +=
				    cur_symbol->info.rinfo->size;
			}
			cur_symbol = NULL;
		;
    break;}
case 40:
#line 264 "aic/aicasm.y"
{
		cur_symbol->info.rinfo->address = yyvsp[0].value;
	;
    break;}
case 41:
#line 271 "aic/aicasm.y"
{
		cur_symbol->info.rinfo->size = yyvsp[0].value;
	;
    break;}
case 42:
#line 278 "aic/aicasm.y"
{
		cur_symbol->info.rinfo->mode = yyvsp[0].value;
	;
    break;}
case 43:
#line 285 "aic/aicasm.y"
{
		process_bitmask(BIT, yyvsp[-1].sym, yyvsp[0].value);
	;
    break;}
case 44:
#line 292 "aic/aicasm.y"
{
		process_bitmask(MASK, yyvsp[-1].sym, yyvsp[0].expression.value);
	;
    break;}
case 45:
#line 299 "aic/aicasm.y"
{
		if (yyvsp[0].sym->type != UNINITIALIZED) {
			stop("Re-definition of register alias",
			     EX_DATAERR);
			/* NOTREACHED */
		}
		yyvsp[0].sym->type = ALIAS;
		initialize_symbol(yyvsp[0].sym);
		yyvsp[0].sym->info.ainfo->parent = cur_symbol;
	;
    break;}
case 46:
#line 313 "aic/aicasm.y"
{
		if (accumulator != NULL) {
			stop("Only one accumulator definition allowed",
			     EX_DATAERR);
			/* NOTREACHED */
		}
		accumulator = cur_symbol;
	;
    break;}
case 47:
#line 325 "aic/aicasm.y"
{
		if (allones.symbol != NULL) {
			stop("Only one definition of allones allowed",
			     EX_DATAERR);
			/* NOTREACHED */
		}
		allones.symbol = cur_symbol;
	;
    break;}
case 48:
#line 337 "aic/aicasm.y"
{
		if (allzeros.symbol != NULL) {
			stop("Only one definition of allzeros allowed",
			     EX_DATAERR);
			/* NOTREACHED */
		}
		allzeros.symbol = cur_symbol;
	;
    break;}
case 49:
#line 349 "aic/aicasm.y"
{
		if (none.symbol != NULL) {
			stop("Only one definition of none allowed",
			     EX_DATAERR);
			/* NOTREACHED */
		}
		none.symbol = cur_symbol;
	;
    break;}
case 50:
#line 361 "aic/aicasm.y"
{
		if (sindex.symbol != NULL) {
			stop("Only one definition of sindex allowed",
			     EX_DATAERR);
			/* NOTREACHED */
		}
		sindex.symbol = cur_symbol;
	;
    break;}
case 51:
#line 373 "aic/aicasm.y"
{
		 yyval.expression.value = yyvsp[-2].expression.value | yyvsp[0].expression.value;
		 symlist_merge(&yyval.expression.referenced_syms,
			       &yyvsp[-2].expression.referenced_syms,
			       &yyvsp[0].expression.referenced_syms);
	;
    break;}
case 52:
#line 380 "aic/aicasm.y"
{
		yyval.expression.value = yyvsp[-2].expression.value & yyvsp[0].expression.value;
		symlist_merge(&yyval.expression.referenced_syms,
			       &yyvsp[-2].expression.referenced_syms,
			       &yyvsp[0].expression.referenced_syms);
	;
    break;}
case 53:
#line 387 "aic/aicasm.y"
{
		yyval.expression.value = yyvsp[-2].expression.value + yyvsp[0].expression.value;
		symlist_merge(&yyval.expression.referenced_syms,
			       &yyvsp[-2].expression.referenced_syms,
			       &yyvsp[0].expression.referenced_syms);
	;
    break;}
case 54:
#line 394 "aic/aicasm.y"
{
		yyval.expression.value = yyvsp[-2].expression.value - yyvsp[0].expression.value;
		symlist_merge(&(yyval.expression.referenced_syms),
			       &(yyvsp[-2].expression.referenced_syms),
			       &(yyvsp[0].expression.referenced_syms));
	;
    break;}
case 55:
#line 401 "aic/aicasm.y"
{
		yyval.expression = yyvsp[-1].expression;
	;
    break;}
case 56:
#line 405 "aic/aicasm.y"
{
		yyval.expression = yyvsp[0].expression;
		yyval.expression.value = (~yyval.expression.value) & 0xFF;
	;
    break;}
case 57:
#line 410 "aic/aicasm.y"
{
		yyval.expression = yyvsp[0].expression;
		yyval.expression.value = -yyval.expression.value;
	;
    break;}
case 58:
#line 415 "aic/aicasm.y"
{
		yyval.expression.value = yyvsp[0].value;
		SLIST_INIT(&yyval.expression.referenced_syms);
	;
    break;}
case 59:
#line 420 "aic/aicasm.y"
{
		symbol_t *symbol;

		symbol = yyvsp[0].sym;
		switch (symbol->type) {
		case ALIAS:
			symbol = yyvsp[0].sym->info.ainfo->parent;
		case REGISTER:
		case SCBLOC:
		case SRAMLOC:
			yyval.expression.value = symbol->info.rinfo->address;
			break;
		case MASK:
		case BIT:
			yyval.expression.value = symbol->info.minfo->mask;
			break;
		case DOWNLOAD_CONST:
		case CONST:
			yyval.expression.value = symbol->info.cinfo->value;
			break;
		case UNINITIALIZED:
		default:
		{
			char buf[255];

			snprintf(buf, sizeof(buf),
				 "Undefined symbol %s referenced",
				 symbol->name);
			stop(buf, EX_DATAERR);
			/* NOTREACHED */
			break;
		}
		}
		SLIST_INIT(&yyval.expression.referenced_syms);
		symlist_add(&yyval.expression.referenced_syms, symbol, SYMLIST_INSERT_HEAD);
	;
    break;}
case 60:
#line 460 "aic/aicasm.y"
{
		if (yyvsp[-1].sym->type != UNINITIALIZED) {
			stop("Re-definition of symbol as a constant",
			     EX_DATAERR);
			/* NOTREACHED */
		}
		yyvsp[-1].sym->type = CONST;
		initialize_symbol(yyvsp[-1].sym);
		yyvsp[-1].sym->info.cinfo->value = yyvsp[0].value;
		yyvsp[-1].sym->info.cinfo->define = yyvsp[-2].value;
	;
    break;}
case 61:
#line 472 "aic/aicasm.y"
{
		if (yyvsp[-2].value) {
			stop("Invalid downloaded constant declaration",
			     EX_DATAERR);
			/* NOTREACHED */
		}
		if (yyvsp[-1].sym->type != UNINITIALIZED) {
			stop("Re-definition of symbol as a downloaded constant",
			     EX_DATAERR);
			/* NOTREACHED */
		}
		yyvsp[-1].sym->type = DOWNLOAD_CONST;
		initialize_symbol(yyvsp[-1].sym);
		yyvsp[-1].sym->info.cinfo->value = download_constant_count++;
		yyvsp[-1].sym->info.cinfo->define = FALSE;
	;
    break;}
case 62:
#line 492 "aic/aicasm.y"
{
		yyval.value = yyvsp[0].value;
	;
    break;}
case 63:
#line 496 "aic/aicasm.y"
{
		yyval.value = -yyvsp[0].value;
	;
    break;}
case 64:
#line 503 "aic/aicasm.y"
{
			cur_symbol = symtable_get(SRAM_SYMNAME);
			cur_symtype = SRAMLOC;
			if (cur_symbol->type != UNINITIALIZED) {
				stop("Only one SRAM definition allowed",
				     EX_DATAERR);
				/* NOTREACHED */
			}
			cur_symbol->type = SRAMLOC;
			initialize_symbol(cur_symbol);
		;
    break;}
case 65:
#line 515 "aic/aicasm.y"
{
			sram_or_scb_offset = cur_symbol->info.rinfo->address;
		;
    break;}
case 66:
#line 520 "aic/aicasm.y"
{
			cur_symbol = NULL;
		;
    break;}
case 67:
#line 527 "aic/aicasm.y"
{
			cur_symbol = symtable_get(SCB_SYMNAME);
			cur_symtype = SCBLOC;
			if (cur_symbol->type != UNINITIALIZED) {
				stop("Only one SRAM definition allowed",
				     EX_SOFTWARE);
				/* NOTREACHED */
			}
			cur_symbol->type = SCBLOC;
			initialize_symbol(cur_symbol);
			/* 64 bytes of SCB space */
			cur_symbol->info.rinfo->size = 64;
		;
    break;}
case 68:
#line 541 "aic/aicasm.y"
{
			sram_or_scb_offset = cur_symbol->info.rinfo->address;
		;
    break;}
case 69:
#line 546 "aic/aicasm.y"
{
			cur_symbol = NULL;
		;
    break;}
case 72:
#line 558 "aic/aicasm.y"
{
		process_register(&yyvsp[0].sym);
		yyval.sym_ref.symbol = yyvsp[0].sym;
		yyval.sym_ref.offset = 0;
	;
    break;}
case 73:
#line 564 "aic/aicasm.y"
{
		process_register(&yyvsp[-3].sym);
		if (yyvsp[-1].sym->type != CONST) {
			stop("register offset must be a constant", EX_DATAERR);
			/* NOTREACHED */
		}
		if ((yyvsp[-1].sym->info.cinfo->value + 1) > yyvsp[-3].sym->info.rinfo->size) {
			stop("Accessing offset beyond range of register",
			     EX_DATAERR);
			/* NOTREACHED */
		}
		yyval.sym_ref.symbol = yyvsp[-3].sym;
		yyval.sym_ref.offset = yyvsp[-1].sym->info.cinfo->value;
	;
    break;}
case 74:
#line 579 "aic/aicasm.y"
{
		process_register(&yyvsp[-3].sym);
		if ((yyvsp[-1].value + 1) > yyvsp[-3].sym->info.rinfo->size) {
			stop("Accessing offset beyond range of register",
			     EX_DATAERR);
			/* NOTREACHED */
		}
		yyval.sym_ref.symbol = yyvsp[-3].sym;
		yyval.sym_ref.offset = yyvsp[-1].value;
	;
    break;}
case 75:
#line 590 "aic/aicasm.y"
{
		if (accumulator == NULL) {
			stop("No accumulator has been defined", EX_DATAERR);
			/* NOTREACHED */
		}
		yyval.sym_ref.symbol = accumulator;
		yyval.sym_ref.offset = 0;
	;
    break;}
case 76:
#line 602 "aic/aicasm.y"
{
		test_writable_symbol(yyvsp[0].sym_ref.symbol);
		yyval.sym_ref = yyvsp[0].sym_ref;
	;
    break;}
case 77:
#line 610 "aic/aicasm.y"
{ yyval.expression = yyvsp[0].expression; ;
    break;}
case 78:
#line 615 "aic/aicasm.y"
{
		yyval.expression = yyvsp[0].expression;
	;
    break;}
case 79:
#line 619 "aic/aicasm.y"
{
		SLIST_INIT(&yyval.expression.referenced_syms);
		yyval.expression.value = 0;
	;
    break;}
case 80:
#line 627 "aic/aicasm.y"
{
		test_readable_symbol(yyvsp[0].sym_ref.symbol);
		yyval.sym_ref = yyvsp[0].sym_ref;
	;
    break;}
case 81:
#line 634 "aic/aicasm.y"
{
		yyval.sym_ref.symbol = NULL;
		yyval.sym_ref.offset = 0;
	;
    break;}
case 82:
#line 639 "aic/aicasm.y"
{ yyval.sym_ref = yyvsp[0].sym_ref; ;
    break;}
case 83:
#line 643 "aic/aicasm.y"
{ yyval.value = 0; ;
    break;}
case 84:
#line 645 "aic/aicasm.y"
{ yyval.value = 1; ;
    break;}
case 85:
#line 650 "aic/aicasm.y"
{
		critical_section_t *cs;

		if (in_critical_section != FALSE) {
			stop("Critical Section within Critical Section",
			     EX_DATAERR);
			/* NOTREACHED */
		}
		cs = cs_alloc();
		cs->begin_addr = instruction_ptr;
		in_critical_section = TRUE;
	;
    break;}
case 86:
#line 665 "aic/aicasm.y"
{
		critical_section_t *cs;

		if (in_critical_section == FALSE) {
			stop("Unballanced 'end_cs'", EX_DATAERR);
			/* NOTREACHED */
		}
		cs = TAILQ_LAST(&cs_tailq, cs_tailq);
		cs->end_addr = instruction_ptr;
		in_critical_section = FALSE;
	;
    break;}
case 87:
#line 679 "aic/aicasm.y"
{
		if (yyvsp[-1].sym->type != UNINITIALIZED) {
			stop("Program label multiply defined", EX_DATAERR);
			/* NOTREACHED */
		}
		yyvsp[-1].sym->type = LABEL;
		initialize_symbol(yyvsp[-1].sym);
		yyvsp[-1].sym->info.linfo->address = instruction_ptr;
	;
    break;}
case 88:
#line 692 "aic/aicasm.y"
{
		yyval.sym_ref.symbol = yyvsp[0].sym;
		yyval.sym_ref.offset = 0;
	;
    break;}
case 89:
#line 697 "aic/aicasm.y"
{
		yyval.sym_ref.symbol = yyvsp[-2].sym;
		yyval.sym_ref.offset = yyvsp[0].value;
	;
    break;}
case 90:
#line 702 "aic/aicasm.y"
{
		yyval.sym_ref.symbol = yyvsp[-2].sym;
		yyval.sym_ref.offset = -yyvsp[0].value;
	;
    break;}
case 91:
#line 707 "aic/aicasm.y"
{
		yyval.sym_ref.symbol = NULL;
		yyval.sym_ref.offset = 0;
	;
    break;}
case 92:
#line 712 "aic/aicasm.y"
{
		yyval.sym_ref.symbol = NULL;
		yyval.sym_ref.offset = yyvsp[0].value;
	;
    break;}
case 93:
#line 717 "aic/aicasm.y"
{
		yyval.sym_ref.symbol = NULL;
		yyval.sym_ref.offset = -yyvsp[0].value;
	;
    break;}
case 94:
#line 725 "aic/aicasm.y"
{
		scope_t *new_scope;

		add_conditional(yyvsp[-1].sym);
		new_scope = scope_alloc();
		new_scope->type = SCOPE_IF;
		new_scope->begin_addr = instruction_ptr;
		new_scope->func_num = yyvsp[-1].sym->info.condinfo->func_num;
	;
    break;}
case 95:
#line 735 "aic/aicasm.y"
{
		scope_t *new_scope;
		scope_t *scope_context;
		scope_t *last_scope;

		/*
		 * Ensure that the previous scope is either an
		 * if or and else if.
		 */
		scope_context = SLIST_FIRST(&scope_stack);
		last_scope = TAILQ_LAST(&scope_context->inner_scope,
					scope_tailq);
		if (last_scope == NULL
		 || last_scope->type == T_ELSE) {

			stop("'else if' without leading 'if'", EX_DATAERR);
			/* NOTREACHED */
		}
		add_conditional(yyvsp[-1].sym);
		new_scope = scope_alloc();
		new_scope->type = SCOPE_ELSE_IF;
		new_scope->begin_addr = instruction_ptr;
		new_scope->func_num = yyvsp[-1].sym->info.condinfo->func_num;
	;
    break;}
case 96:
#line 760 "aic/aicasm.y"
{
		scope_t *new_scope;
		scope_t *scope_context;
		scope_t *last_scope;

		/*
		 * Ensure that the previous scope is either an
		 * if or and else if.
		 */
		scope_context = SLIST_FIRST(&scope_stack);
		last_scope = TAILQ_LAST(&scope_context->inner_scope,
					scope_tailq);
		if (last_scope == NULL
		 || last_scope->type == SCOPE_ELSE) {

			stop("'else' without leading 'if'", EX_DATAERR);
			/* NOTREACHED */
		}
		new_scope = scope_alloc();
		new_scope->type = SCOPE_ELSE;
		new_scope->begin_addr = instruction_ptr;
	;
    break;}
case 97:
#line 786 "aic/aicasm.y"
{
		scope_t *scope_context;

		scope_context = SLIST_FIRST(&scope_stack);
		if (scope_context->type == SCOPE_ROOT) {
			stop("Unexpected '}' encountered", EX_DATAERR);
			/* NOTREACHED */
		}

		scope_context->end_addr = instruction_ptr;

		/* Pop the scope */
		SLIST_REMOVE_HEAD(&scope_stack, scope_stack_links);

		process_scope(scope_context);

		if (SLIST_FIRST(&scope_stack) == NULL) {
			stop("Unexpected '}' encountered", EX_DATAERR);
			/* NOTREACHED */
		}
	;
    break;}
case 98:
#line 810 "aic/aicasm.y"
{ yyval.value = AIC_OP_AND; ;
    break;}
case 99:
#line 811 "aic/aicasm.y"
{ yyval.value = AIC_OP_XOR; ;
    break;}
case 100:
#line 812 "aic/aicasm.y"
{ yyval.value = AIC_OP_ADD; ;
    break;}
case 101:
#line 813 "aic/aicasm.y"
{ yyval.value = AIC_OP_ADC; ;
    break;}
case 102:
#line 818 "aic/aicasm.y"
{
		format_1_instr(yyvsp[-6].value, &yyvsp[-5].sym_ref, &yyvsp[-3].expression, &yyvsp[-2].sym_ref, yyvsp[-1].value);
	;
    break;}
case 103:
#line 825 "aic/aicasm.y"
{
		format_1_instr(AIC_OP_OR, &yyvsp[-5].sym_ref, &yyvsp[-3].expression, &yyvsp[-2].sym_ref, yyvsp[-1].value);
	;
    break;}
case 104:
#line 832 "aic/aicasm.y"
{
		expression_t immed;

		make_expression(&immed, 1);
		format_1_instr(AIC_OP_ADD, &yyvsp[-3].sym_ref, &immed, &yyvsp[-2].sym_ref, yyvsp[-1].value);
	;
    break;}
case 105:
#line 842 "aic/aicasm.y"
{
		expression_t immed;

		make_expression(&immed, -1);
		format_1_instr(AIC_OP_ADD, &yyvsp[-3].sym_ref, &immed, &yyvsp[-2].sym_ref, yyvsp[-1].value);
	;
    break;}
case 106:
#line 852 "aic/aicasm.y"
{
		expression_t immed;

		make_expression(&immed, -1);
		format_1_instr(AIC_OP_ADD, &none, &immed, &allzeros, yyvsp[-1].value);
	;
    break;}
case 107:
#line 859 "aic/aicasm.y"
{
		format_1_instr(AIC_OP_ADD, &yyvsp[-4].sym_ref, &yyvsp[-2].expression, &allzeros, yyvsp[-1].value);
	;
    break;}
case 108:
#line 866 "aic/aicasm.y"
{
		expression_t immed;

		make_expression(&immed, 1);
		format_1_instr(AIC_OP_ADD, &none, &immed, &allones, yyvsp[-1].value);
	;
    break;}
case 109:
#line 873 "aic/aicasm.y"
{
		expression_t immed;

		make_expression(&immed, 1);
		format_1_instr(AIC_OP_ADD, &yyvsp[-2].sym_ref, &immed, &allones, yyvsp[-1].value);
	;
    break;}
case 110:
#line 883 "aic/aicasm.y"
{
		format_1_instr(AIC_OP_BMOV, &yyvsp[-6].sym_ref, &yyvsp[-2].expression, &yyvsp[-4].sym_ref, yyvsp[-1].value);
	;
    break;}
case 111:
#line 890 "aic/aicasm.y"
{
		expression_t immed;

		make_expression(&immed, 1);
		format_1_instr(AIC_OP_BMOV, &yyvsp[-4].sym_ref, &immed, &yyvsp[-2].sym_ref, yyvsp[-1].value);
	;
    break;}
case 112:
#line 900 "aic/aicasm.y"
{
		format_1_instr(AIC_OP_OR, &yyvsp[-4].sym_ref, &yyvsp[-2].expression, &allzeros, yyvsp[-1].value);
	;
    break;}
case 113:
#line 907 "aic/aicasm.y"
{
		expression_t immed;

		make_expression(&immed, 0xff);
		format_1_instr(AIC_OP_XOR, &yyvsp[-3].sym_ref, &immed, &yyvsp[-2].sym_ref, yyvsp[-1].value);
	;
    break;}
case 114:
#line 917 "aic/aicasm.y"
{
		expression_t immed;

		make_expression(&immed, 0xff);
		format_1_instr(AIC_OP_AND, &yyvsp[-2].sym_ref, &immed, &allzeros, yyvsp[-1].value);
	;
    break;}
case 115:
#line 927 "aic/aicasm.y"
{
		expression_t immed;

		make_expression(&immed, 0xff);
		format_1_instr(AIC_OP_AND, &none, &immed, &allzeros, yyvsp[-1].value);
	;
    break;}
case 116:
#line 937 "aic/aicasm.y"
{
		expression_t immed;

		make_expression(&immed, 0xff);
		format_1_instr(AIC_OP_AND, &none, &immed, &allzeros, TRUE);
	;
    break;}
case 117:
#line 955 "aic/aicasm.y"
{ yyval.value = AIC_OP_SHL; ;
    break;}
case 118:
#line 956 "aic/aicasm.y"
{ yyval.value = AIC_OP_SHR; ;
    break;}
case 119:
#line 957 "aic/aicasm.y"
{ yyval.value = AIC_OP_ROL; ;
    break;}
case 120:
#line 958 "aic/aicasm.y"
{ yyval.value = AIC_OP_ROR; ;
    break;}
case 121:
#line 963 "aic/aicasm.y"
{
		format_2_instr(yyvsp[-6].value, &yyvsp[-5].sym_ref, &yyvsp[-3].expression, &yyvsp[-2].sym_ref, yyvsp[-1].value);
	;
    break;}
case 122:
#line 969 "aic/aicasm.y"
{ yyval.value = AIC_OP_JMP; ;
    break;}
case 123:
#line 970 "aic/aicasm.y"
{ yyval.value = AIC_OP_JC; ;
    break;}
case 124:
#line 971 "aic/aicasm.y"
{ yyval.value = AIC_OP_JNC; ;
    break;}
case 125:
#line 972 "aic/aicasm.y"
{ yyval.value = AIC_OP_CALL; ;
    break;}
case 126:
#line 976 "aic/aicasm.y"
{ yyval.value = AIC_OP_JZ; ;
    break;}
case 127:
#line 977 "aic/aicasm.y"
{ yyval.value = AIC_OP_JNZ; ;
    break;}
case 128:
#line 981 "aic/aicasm.y"
{ yyval.value = AIC_OP_JE; ;
    break;}
case 129:
#line 982 "aic/aicasm.y"
{ yyval.value = AIC_OP_JNE; ;
    break;}
case 130:
#line 987 "aic/aicasm.y"
{
		expression_t immed;

		make_expression(&immed, 0);
		format_3_instr(yyvsp[-2].value, &sindex, &immed, &yyvsp[-1].sym_ref);
	;
    break;}
case 131:
#line 997 "aic/aicasm.y"
{
		format_3_instr(yyvsp[-2].value, &yyvsp[-5].sym_ref, &yyvsp[-3].expression, &yyvsp[-1].sym_ref);
	;
    break;}
case 132:
#line 1004 "aic/aicasm.y"
{
		format_3_instr(yyvsp[-2].value, &yyvsp[-5].sym_ref, &yyvsp[-3].expression, &yyvsp[-1].sym_ref);
	;
    break;}
case 133:
#line 1011 "aic/aicasm.y"
{
		format_3_instr(yyvsp[-2].value, &yyvsp[-5].sym_ref, &yyvsp[-3].expression, &yyvsp[-1].sym_ref);
	;
    break;}
case 134:
#line 1018 "aic/aicasm.y"
{
		expression_t immed;

		make_expression(&immed, 0);
		format_3_instr(yyvsp[-2].value, &yyvsp[-3].sym_ref, &immed, &yyvsp[-1].sym_ref);
	;
    break;}
case 135:
#line 1028 "aic/aicasm.y"
{
		format_3_instr(yyvsp[-2].value, &allzeros, &yyvsp[-3].expression, &yyvsp[-1].sym_ref);
	;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 386 "bison.simple"
  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}
#line 1033 "aic/aicasm.y"


static void
process_bitmask(int mask_type, symbol_t *sym, int mask)
{
	/*
	 * Add the current register to its
	 * symbol list, if it already exists,
	 * warn if we are setting it to a
	 * different value, or in the bit to
	 * the "allowed bits" of this register.
	 */
	if (sym->type == UNINITIALIZED) {
		sym->type = mask_type;
		initialize_symbol(sym);
		if (mask_type == BIT) {
			if (mask == 0) {
				stop("Bitmask with no bits set", EX_DATAERR);
				/* NOTREACHED */
			}
			if ((mask & ~(0x01 << (ffbs (mask)))) != 0) {
#if (0)	/*******************************************************/
			if ((mask & ~(0x01 << (ffs(mask) - 1))) != 0) {
#endif	/*******************************************************/
				stop("Bitmask with more than one bit set",
				     EX_DATAERR);
				/* NOTREACHED */
			}
		}
		sym->info.minfo->mask = mask;
	} else if (sym->type != mask_type) {
		stop("Bit definition mirrors a definition of the same "
		     " name, but a different type", EX_DATAERR);
		/* NOTREACHED */
	} else if (mask != sym->info.minfo->mask) {
		stop("Bitmask redefined with a conflicting value", EX_DATAERR);
		/* NOTREACHED */
	}
	/* Fail if this symbol is already listed */
	if (symlist_search(&(sym->info.minfo->symrefs),
			   cur_symbol->name) != NULL) {
		stop("Bitmask defined multiple times for register", EX_DATAERR);
		/* NOTREACHED */
	}
	symlist_add(&(sym->info.minfo->symrefs), cur_symbol,
		    SYMLIST_INSERT_HEAD);
	cur_symbol->info.rinfo->valid_bitmask |= mask;
	cur_symbol->info.rinfo->typecheck_masks = TRUE;
}

static void
initialize_symbol(symbol_t *symbol)
{
	switch (symbol->type) {
        case UNINITIALIZED:
		stop("Call to initialize_symbol with type field unset",
		     EX_SOFTWARE);
		/* NOTREACHED */
		break;
        case REGISTER:
        case SRAMLOC:
        case SCBLOC:
		symbol->info.rinfo =
		    (struct reg_info *)malloc(sizeof(struct reg_info));
		if (symbol->info.rinfo == NULL) {
			stop("Can't create register info", EX_SOFTWARE);
			/* NOTREACHED */
		}
		memset(symbol->info.rinfo, 0,
		       sizeof(struct reg_info));
		break;
        case ALIAS:
		symbol->info.ainfo =
		    (struct alias_info *)malloc(sizeof(struct alias_info));
		if (symbol->info.ainfo == NULL) {
			stop("Can't create alias info", EX_SOFTWARE);
			/* NOTREACHED */
		}
		memset(symbol->info.ainfo, 0,
		       sizeof(struct alias_info));
		break;
        case MASK:
        case BIT:
		symbol->info.minfo =
		    (struct mask_info *)malloc(sizeof(struct mask_info));
		if (symbol->info.minfo == NULL) {
			stop("Can't create bitmask info", EX_SOFTWARE);
			/* NOTREACHED */
		}
		memset(symbol->info.minfo, 0, sizeof(struct mask_info));
		SLIST_INIT(&(symbol->info.minfo->symrefs));
		break;
        case CONST:
        case DOWNLOAD_CONST:
		symbol->info.cinfo =
		    (struct const_info *)malloc(sizeof(struct const_info));
		if (symbol->info.cinfo == NULL) {
			stop("Can't create alias info", EX_SOFTWARE);
			/* NOTREACHED */
		}
		memset(symbol->info.cinfo, 0,
		       sizeof(struct const_info));
		break;
	case LABEL:
		symbol->info.linfo =
		    (struct label_info *)malloc(sizeof(struct label_info));
		if (symbol->info.linfo == NULL) {
			stop("Can't create label info", EX_SOFTWARE);
			/* NOTREACHED */
		}
		memset(symbol->info.linfo, 0,
		       sizeof(struct label_info));
		break;
	case CONDITIONAL:
		symbol->info.condinfo =
		    (struct cond_info *)malloc(sizeof(struct cond_info));
		if (symbol->info.condinfo == NULL) {
			stop("Can't create conditional info", EX_SOFTWARE);
			/* NOTREACHED */
		}
		memset(symbol->info.condinfo, 0,
		       sizeof(struct cond_info));
		break;
	default:
		stop("Call to initialize_symbol with invalid symbol type",
		     EX_SOFTWARE);
		/* NOTREACHED */
		break;
	}
}

static void
process_register(symbol_t **p_symbol)
{
	char buf[255];
	symbol_t *symbol = *p_symbol;

	if (symbol->type == UNINITIALIZED) {
		snprintf(buf, sizeof(buf), "Undefined register %s",
			 symbol->name);
		stop(buf, EX_DATAERR);
		/* NOTREACHED */
	} else if (symbol->type == ALIAS) {
		*p_symbol = symbol->info.ainfo->parent;
	} else if ((symbol->type != REGISTER)
		&& (symbol->type != SCBLOC)
		&& (symbol->type != SRAMLOC)) {
		snprintf(buf, sizeof(buf),
			 "Specified symbol %s is not a register",
			 symbol->name);
		stop(buf, EX_DATAERR);
	}
}

static void
format_1_instr(int opcode, symbol_ref_t *dest, expression_t *immed,
	       symbol_ref_t *src, int ret)
{
	struct instruction *instr;
	struct ins_format1 *f1_instr;

	if (src->symbol == NULL)
		src = dest;

	/* Test register permissions */
	test_writable_symbol(dest->symbol);
	test_readable_symbol(src->symbol);

	/* Ensure that immediate makes sense for this destination */
	type_check(dest->symbol, immed, opcode);

	/* Allocate sequencer space for the instruction and fill it out */
	instr = seq_alloc();
	f1_instr = &instr->format.format1;
	f1_instr->ret = ret ? 1 : 0;
	f1_instr->opcode = opcode;
	f1_instr->destination = dest->symbol->info.rinfo->address
			      + dest->offset;
	f1_instr->source = src->symbol->info.rinfo->address
			 + src->offset;
	f1_instr->immediate = immed->value;

	if (is_download_const(immed))
		f1_instr->parity = 1;

	symlist_free(&immed->referenced_syms);
	instruction_ptr++;
}

static void
format_2_instr(int opcode, symbol_ref_t *dest, expression_t *places,
	       symbol_ref_t *src, int ret)
{
	struct instruction *instr;
	struct ins_format2 *f2_instr;
	uchar shift_control;

	if (src->symbol == NULL)
		src = dest;

	/* Test register permissions */
	test_writable_symbol(dest->symbol);
	test_readable_symbol(src->symbol);

	/* Allocate sequencer space for the instruction and fill it out */
	instr = seq_alloc();
	f2_instr = &instr->format.format2;
	f2_instr->ret = ret ? 1 : 0;
	f2_instr->opcode = AIC_OP_ROL;
	f2_instr->destination = dest->symbol->info.rinfo->address
			      + dest->offset;
	f2_instr->source = src->symbol->info.rinfo->address
			 + src->offset;
	if (places->value > 8 || places->value <= 0) {
		stop("illegal shift value", EX_DATAERR);
		/* NOTREACHED */
	}
	switch (opcode) {
	case AIC_OP_SHL:
		if (places->value == 8)
			shift_control = 0xf0;
		else
			shift_control = (places->value << 4) | places->value;
		break;
	case AIC_OP_SHR:
		if (places->value == 8) {
			shift_control = 0xf8;
		} else {
			shift_control = (places->value << 4)
				      | (8 - places->value)
				      | 0x08;
		}
		break;
	case AIC_OP_ROL:
		shift_control = places->value & 0x7;
		break;
	case AIC_OP_ROR:
		shift_control = (8 - places->value) | 0x08;
		break;
	default:
		shift_control = 0; /* Quiet Compiler */
		stop("Invalid shift operation specified", EX_SOFTWARE);
		/* NOTREACHED */
		break;
	};
	f2_instr->shift_control = shift_control;
	symlist_free(&places->referenced_syms);
	instruction_ptr++;
}

static void
format_3_instr(int opcode, symbol_ref_t *src,
	       expression_t *immed, symbol_ref_t *address)
{
	struct instruction *instr;
	struct ins_format3 *f3_instr;
	int addr;

	/* Test register permissions */
	test_readable_symbol(src->symbol);

	/* Ensure that immediate makes sense for this source */
	type_check(src->symbol, immed, opcode);

	/* Allocate sequencer space for the instruction and fill it out */
	instr = seq_alloc();
	f3_instr = &instr->format.format3;
	if (address->symbol == NULL) {
		/* 'dot' referrence.  Use the current instruction pointer */
		addr = instruction_ptr + address->offset;
	} else if (address->symbol->type == UNINITIALIZED) {
		/* forward reference */
		addr = address->offset;
		instr->patch_label = address->symbol;
	} else
		addr = address->symbol->info.linfo->address + address->offset;
	f3_instr->opcode = opcode;
	f3_instr->address = addr;
	f3_instr->source = src->symbol->info.rinfo->address
			 + src->offset;
	f3_instr->immediate = immed->value;

	if (is_download_const(immed))
		f3_instr->parity = 1;

	symlist_free(&immed->referenced_syms);
	instruction_ptr++;
}

static void
test_readable_symbol(symbol_t *symbol)
{
	if (symbol->info.rinfo->mode == WO) {
		stop("Write Only register specified as source",
		     EX_DATAERR);
		/* NOTREACHED */
	}
}

static void
test_writable_symbol(symbol_t *symbol)
{
	if (symbol->info.rinfo->mode == RO) {
		stop("Read Only register specified as destination",
		     EX_DATAERR);
		/* NOTREACHED */
	}
}

static void
type_check(symbol_t *symbol, expression_t *expression, int opcode)
{
	symbol_node_t *node;
	int and_op;
	char buf[255];

	and_op = FALSE;
	if (opcode == AIC_OP_AND || opcode == AIC_OP_JNZ || AIC_OP_JZ)
		and_op = TRUE;

	/*
	 * Make sure that we aren't attempting to write something
	 * that hasn't been defined.  If this is an and operation,
	 * this is a mask, so "undefined" bits are okay.
	 */
	if (and_op == FALSE
	 && (expression->value & ~symbol->info.rinfo->valid_bitmask) != 0) {
		snprintf(buf, sizeof(buf),
			 "Invalid bit(s) 0x%x in immediate written to %s",
			 expression->value & ~symbol->info.rinfo->valid_bitmask,
			 symbol->name);
		stop(buf, EX_DATAERR);
		/* NOTREACHED */
	}

	/*
	 * Now make sure that all of the symbols referenced by the
	 * expression are defined for this register.
	 */
	if(symbol->info.rinfo->typecheck_masks != FALSE) {
		for(node = expression->referenced_syms.slh_first;
		    node != NULL;
		    node = node->links.sle_next) {
			if ((node->symbol->type == MASK
			  || node->symbol->type == BIT)
			 && symlist_search(&node->symbol->info.minfo->symrefs,
					   symbol->name) == NULL) {
				snprintf(buf, sizeof(buf),
					 "Invalid bit or mask %s "
					 "for register %s",
					 node->symbol->name, symbol->name);
				stop(buf, EX_DATAERR);
				/* NOTREACHED */
			}
		}
	}
}

static void
make_expression(expression_t *immed, int value)
{
	SLIST_INIT(&immed->referenced_syms);
	immed->value = value & 0xff;
}

static void
add_conditional(symbol_t *symbol)
{
	static int numfuncs;

	if (numfuncs == 0) {
		/* add a special conditional, "0" */
		symbol_t *false_func;

		false_func = symtable_get("0");
		if (false_func->type != UNINITIALIZED) {
			stop("Conditional expression '0' "
			     "conflicts with a symbol", EX_DATAERR);
			/* NOTREACHED */
		}
		false_func->type = CONDITIONAL;
		initialize_symbol(false_func);
		false_func->info.condinfo->func_num = numfuncs++;
		symlist_add(&patch_functions, false_func, SYMLIST_INSERT_HEAD);
	}

	/* This condition has occurred before */
	if (symbol->type == CONDITIONAL)
		return;

	if (symbol->type != UNINITIALIZED) {
		stop("Conditional expression conflicts with a symbol",
		     EX_DATAERR);
		/* NOTREACHED */
	}

	symbol->type = CONDITIONAL;
	initialize_symbol(symbol);
	symbol->info.condinfo->func_num = numfuncs++;
	symlist_add(&patch_functions, symbol, SYMLIST_INSERT_HEAD);
}

void
yyerror (const char *string)
{
	stop(string, EX_DATAERR);
}

static int
is_download_const(expression_t *immed)
{
	if ((immed->referenced_syms.slh_first != NULL)
	 && (immed->referenced_syms.slh_first->symbol->type == DOWNLOAD_CONST))
		return (TRUE);

	return (FALSE);
}
