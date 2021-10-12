/*
 ****************************************************************
 *								*
 *			<sys/uarg.h>				*
 *								*
 *	Estrutura de argumentos do usuário do EXEC		*
 *								*
 *	Versão	3.0.0, de 11.12.94				*
 *		3.0.0, de 11.12.94				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *		/usr/include/sys				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 1999 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#define	UARG_H			/* Para definir os protótipos */

/*
 ******	Estrutura UARG (final da PILHA do usuário ***************
 */
typedef	struct
{
	char	*g_envp;	/* Ponteiro para a Lista de ENV's */
	int	g_envc;		/* No. de ENV's */
	char	*g_argp;	/* Ponteiro para a Lista de ARG's */
	int	g_argc;		/* No. de ARG's */
	int	g_null;		/* Nulo no final da STACK */

}	UARG;
