/*
 ****************************************************************
 *								*
 *			symtb.h					*
 *								*
 *	Estrutura das Tabelas Hash e de Símbolos		*
 *								*
 *	Versão	1.0.0, de 05.08.88				*
 *		4.1.0, de 22.08.01				*
 *								*
 *	Módulo: CPP						*
 *		Tradutores					*
 *		Categoria D					*	
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

/*
 ******	Estrutura da Tabela de Símbolos	*************************
 */
typedef struct symtb 	SYMTB;

struct symtb
{
	const char	*s_name;	/* Identificador do Símbolo */
	int		s_flags;	/* Tipo + modificadores do Símbolo */
	int		s_index;	/* Índice do Keyword/builtin */
	int		s_nargs;	/* No. de argumentos da Macro */
	int		s_expand;	/* No. de expansões desta Macro */
	const char	*s_macro;	/* Corpo da Macro */
	SYMTB		*s_link;	/* Lista da symtb (deve ser o último item) */
};

#define	NOSYMTB		(SYMTB *)0

/*
 ******	Tipo + modificadores do símbolo	*************************
 *
 *	Repare que um símbolo pode ser simultanamente "keyword" e "macro"
 */
#define	S_NULL		0x00	/* O Identificador não está em uso */
#define	S_KEYWORD	0x01	/* Palavra Reservada */
#define	S_MACRO		0x02	/* Macro */
#define	S_PREUNDEF	0x04	/* Não deve ser macro (opção "-U") */

#define	S_PREDEF	0x20	/* Macros Definidas por "-D" */
#define	S_ENV		0x40	/* Macros do Ambiente */
#define	S_BUILTIN	0x80	/* Macros Intrínsecas */

/*
 ******	Para saber se um identificador é de determinado tipo ****	
 */
#define	TYPE(sp, name, flag)				\
(							\
	(sp = hash (name, &link_place)) != NOSYMTB &&	\
	((sp)->s_flags & (flag))			\
)

/*
 ******	Variáveis globais ***************************************
 */
#define	HASHSZ	1009			/* Recomendável ser primo */

extern SYMTB	*hashtb[HASHSZ];	/* A Tabela HASH */

/*
 ******	Protótipos **********************************************
 */
extern const char	*builtin (SYMTB *, const char *, char *, char **);
extern const char	*macexpand (SYMTB *, const char *, char *, char **);
extern void		printargs (SYMTB *, const char *[], int);
extern SYMTB		*hash (const char *, SYMTB ***);
