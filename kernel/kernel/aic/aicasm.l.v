%{
/*
 ****************************************************************
 *								*
 *			asm.2940.l				*
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
#include <string.h>

#include "../aic/sysexits.h"
#include "../aic/aicasm.h"
#include "../aic/asm_symbol.h"
#include "../aic/aicasm.tab.h"

/*
 ****************************************************************
 *	Definições globais					*
 ****************************************************************
 */
#define MAX_STR_CONST 256
#define	PATH_MAX	1024		/* PROVISÓRIO */

char	string_buf[MAX_STR_CONST];
char	*string_buf_ptr;
int	parren_count;

#define	YY_NO_SCAN_STRING
#define	YY_NO_SCAN_BYTES
#define	YY_NO_SCAN_BUFFER

/*
 ******	Protótipos de funções ***********************************
 */
symbol_t *symtable_get (const char *name);

%}

PATH		[-/A-Za-z0-9_.]*[./][-/A-Za-z0-9_.]*
WORD		[A-Za-z_][-A-Za-z_0-9]*
SPACE		[ \t]+

%x COMMENT
%x CEXPR
%x INCLUDE

%%
\n			{ ++yylineno; }
"/*"			{ BEGIN COMMENT;  /* Enter comment eating state */ }
<COMMENT>"/*"		{ fprintf(stderr, "Warning! Comment within comment."); }
<COMMENT>\n		{ ++yylineno; }
<COMMENT>[^*/\n]*	;
<COMMENT>"*"+[^*/\n]*	;
<COMMENT>"/"+[^*/\n]*	;
<COMMENT>"*"+"/"	{ BEGIN INITIAL; }
if[ \t]*\(		{
				string_buf_ptr = string_buf;
				parren_count = 1;
				BEGIN CEXPR;
				return T_IF;
			}
<CEXPR>\(		{	*string_buf_ptr++ = '('; parren_count++; }
<CEXPR>\)		{
				parren_count--;
				if (parren_count == 0) {
					/* All done */
					BEGIN INITIAL;
					*string_buf_ptr = '\0';
					yylval.sym = symtable_get(string_buf);
					return T_CEXPR;
				} else {
					*string_buf_ptr++ = ')';
				}
			}
<CEXPR>\n		{ ++yylineno; }
<CEXPR>[^()\n]+	{
				char *yptr = yytext;

				while (*yptr != '\0') {
					/* Remove duplicate spaces */
					if (*yptr == '\t')
						*yptr = ' ';
					if (*yptr == ' '
					 && string_buf_ptr != string_buf
					 && string_buf_ptr[-1] == ' ')
						yptr++;
					else 
						*string_buf_ptr++ = *yptr++;
				}
			}

{SPACE}			;

	/* Register/SCB/SRAM definition keywords */
register		{ return T_REGISTER; }
const			{ yylval.value = FALSE; return T_CONST; }
download		{ return T_DOWNLOAD; }
address			{ return T_ADDRESS; }
access_mode		{ return T_ACCESS_MODE; }
RW|RO|WO		{
				 if (strcmp(yytext, "RW") == 0)
					yylval.value = RW;
				 else if (strcmp(yytext, "RO") == 0)
					yylval.value = RO;
				 else
					yylval.value = WO;
				 return T_MODE;
			}
BEGIN_CRITICAL		{ return T_BEGIN_CS; }
END_CRITICAL		{ return T_END_CS; }
bit			{ return T_BIT; }
mask			{ return T_MASK; }
alias			{ return T_ALIAS; }
size			{ return T_SIZE; }
scb			{ return T_SCB; }
scratch_ram		{ return T_SRAM; }
accumulator		{ return T_ACCUM; }
allones			{ return T_ALLONES; }
allzeros		{ return T_ALLZEROS; }
none			{ return T_NONE; }
sindex			{ return T_SINDEX; }
A			{ return T_A; }

	/* Opcodes */
shl			{ return T_SHL; }
shr			{ return T_SHR; }
ror			{ return T_ROR; }
rol			{ return T_ROL; }
mvi			{ return T_MVI; }
mov			{ return T_MOV; }
clr			{ return T_CLR; }
jmp			{ return T_JMP; }
jc			{ return T_JC;	}
jnc			{ return T_JNC;	}
je			{ return T_JE;	}
jne			{ return T_JNE;	}
jz			{ return T_JZ;	}
jnz			{ return T_JNZ;	}
call			{ return T_CALL; }
add			{ return T_ADD; }
adc			{ return T_ADC; }
bmov			{ return T_BMOV; }
inc			{ return T_INC; }
dec			{ return T_DEC; }
stc			{ return T_STC;	}
clc			{ return T_CLC; }
cmp			{ return T_CMP;	}
not			{ return T_NOT;	}
xor			{ return T_XOR;	}
test			{ return T_TEST;}
and			{ return T_AND;	}
or			{ return T_OR;	}
ret			{ return T_RET; }
nop			{ return T_NOP; }
else			{ return T_ELSE; }

	/* Allowed Symbols */
[-+,:()~|&."{};<>[\]!]	{ return yytext[0]; }

	/* Number processing */
0[0-7]*			{
				yylval.value = strtol(yytext, NULL, 8);
				return T_NUMBER;
			}

0[xX][0-9a-fA-F]+	{
				yylval.value = strtoul(yytext + 2, NULL, 16);
				return T_NUMBER;
			}

[1-9][0-9]*		{
				yylval.value = strtol(yytext, NULL, 10);
				return T_NUMBER;
			}

	/* Include Files */
#include		{ return T_INCLUDE; BEGIN INCLUDE;}
<INCLUDE>[<>\"]		{ return yytext[0]; }
<INCLUDE>{PATH}		{ yylval.str = strdup(yytext); return T_PATH; }
<INCLUDE>;		{ BEGIN INITIAL; return yytext[0]; }
<INCLUDE>.		{ stop("Invalid include line", EX_DATAERR); }

	/* For parsing C include files with #define foo */
#define			{ yylval.value = TRUE; return T_CONST; }
	/* Throw away macros */
#define[^\n]*[()]+[^\n]* ;
{PATH}			{ yylval.str = strdup(yytext); return T_PATH; }

{WORD}			{ yylval.sym = symtable_get(yytext);  return T_SYMBOL; }

.			{ 
				char buf[255];

				snprintf(buf, sizeof(buf), "Invalid character "
					 "'%c'", yytext[0]);
				stop(buf, EX_DATAERR);
			}
%%

typedef struct include
{
        YY_BUFFER_STATE  buffer;
        int              lineno;
        const char       *filename;
	SLIST_ENTRY(include) links;

}	include_t;

SLIST_HEAD(, include) include_stack;

void
include_file (const char *file_name, include_type type)
{
	FILE *newfile;
	include_t *include;

	newfile = NULL;
	/* Try the current directory first */
	if (includes_search_curdir != 0 || type == SOURCE_FILE)
		newfile = fopen(file_name, "r");

	if (newfile == NULL && type != SOURCE_FILE) {
                path_entry_t include_dir;
                for (include_dir = search_path.slh_first;
                     include_dir != NULL;                
                     include_dir = include_dir->links.sle_next) {
			char fullname[PATH_MAX];

			if ((include_dir->quoted_includes_only == TRUE)
			 && (type != QUOTED_INCLUDE))
				continue;

			snprintf(fullname, sizeof(fullname),
				 "%s/%s", include_dir->directory, file_name);

			if ((newfile = fopen(fullname, "r")) != NULL)
				break;
                }
        }

	if (newfile == NULL) {
		perror(file_name);
		stop("Unable to open input file", EX_SOFTWARE);
		/* NOTREACHED */
	}

	if (type != SOURCE_FILE) {
		include = (include_t *)malloc(sizeof(include_t));
		if (include == NULL) {
			stop("Unable to allocate include stack entry",
			     EX_SOFTWARE);
			/* NOTREACHED */
		}
		include->buffer = YY_CURRENT_BUFFER;
		include->lineno = yylineno;
		include->filename = yyfilename;
		SLIST_INSERT_HEAD(&include_stack, include, links);
	}
	yy_switch_to_buffer(yy_create_buffer(newfile, YY_BUF_SIZE));
	yylineno = 1;
	yyfilename = strdup(file_name);
}

int
yywrap (void)
{
	include_t *include;

	yy_delete_buffer(YY_CURRENT_BUFFER);
	(void)fclose(yyin);
	if (yyfilename != NULL)
		free ((char *)yyfilename);
	yyfilename = NULL;
	include = include_stack.slh_first;
	if (include != NULL) {
		yy_switch_to_buffer(include->buffer);
		yylineno = include->lineno;
		yyfilename = include->filename;
		SLIST_REMOVE_HEAD(&include_stack, links);
		free(include);
		return (0);
	}
	return (1);
}
