/*
 ****************************************************************
 *								*
 *			common.h				*
 *								*
 *	Definições Universais					*
 *								*
 *	Versão  3.0.0, de 29.03.96				*
 *		3.2.3, de 09.01.00				*
 *								*
 *	Módulo: Make: Construtor de Programas			*
 *		Interpretador de Comandos			*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 								*
 ****************************************************************
 */

/*
 ****************************************************************
 *	Definições de caráter geral				*
 ****************************************************************
 */
#define	EVER	;;
#define	elif	else if

#define	NIL	(-1)
#define	NULL	0

#define	NOINT	(int  *)NULL
#define	NOVOID	(void *)NULL
#define	NOSTR	(char *)NULL
#define	NOCHR	'\0'
#define	NOSSTR	(char **)NULL

#define	FALSE	0
#define	TRUE	1

/*
 ****************************************************************
 *	As diversas estruturas utilizadas e seus nulos		*
 ****************************************************************
 */
typedef struct sym	SYM;		/* s_ */
typedef	struct hash	HASH;		/* h_ */
typedef struct nlist	NLIST;		/* n_ */
typedef struct clist	CLIST;		/* c_ */
typedef struct depen	DEPEN;		/* d_ */
typedef	struct macro	MACRO;		/* m_ */
typedef	struct range	RANGE;		/* r_ */
typedef	struct token	TOKEN;		/* t_ */

#define	NOSYM		(SYM   *)NULL
#define	NOHASH		(HASH  *)NULL
#define	NONLIST		(NLIST *)NULL
#define	NOCLIST		(CLIST *)NULL
#define	NODEPEN		(DEPEN *)NULL
#define	NOMACRO		(MACRO *)NULL
#define	NORANGE		(RANGE *)NULL
#define	NOTOKEN		(TOKEN *)NULL
