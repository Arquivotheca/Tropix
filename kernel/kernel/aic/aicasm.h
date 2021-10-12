/*
 ****************************************************************
 *								*
 *			aicasm.h				*
 *								*
 *	Definições para o Montador da placa 2940		*
 *								*
 *	Versão	4.0.0, de 15.03.01				*
 *		4.0.0, de 15.03.01				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 *		Baseado no FreeBSD 4.2				*
 *								*
 ****************************************************************
 */

/*
 ******	Definições **********************************************
 */
#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef struct path_entry
{
	const char		 *directory;
	int			 quoted_includes_only;
	SLIST_ENTRY (path_entry) links;

}	*path_entry_t;

typedef enum
{
	QUOTED_INCLUDE,
	BRACKETED_INCLUDE,
	SOURCE_FILE

}	include_type;

SLIST_HEAD (path_list, path_entry);

extern struct path_list		search_path;
extern struct cs_tailq		cs_tailq;
extern struct scope_list	scope_stack;
extern struct symlist		patch_functions;
extern int			includes_search_curdir;	/* False if we've seen -I- */
extern const char		*appname;
extern int			yylineno;
extern const char		*yyfilename;

/*
 ******	Protótipos de funções ***********************************
 */
extern void			stop (const char *errstring, int code);
extern void			include_file (const char *file_name, include_type type);
extern struct instruction	*seq_alloc (void);
extern struct critical_section	*cs_alloc (void);
extern struct scope		*scope_alloc (void);
extern void			process_scope (struct scope *);
