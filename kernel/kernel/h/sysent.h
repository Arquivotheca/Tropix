/*
 ****************************************************************
 *								*
 *			<sys/sysent.h>				*
 *								*
 *	Definições da tabela de chamadas ao sistema		*
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

#define	SYSENT_H		/* Para definir os protótipos */

/*
 ******	Definição da estrutura **********************************
 */
typedef struct sysent
{
	int	(*s_syscall) (); /* Função executora */
	int	s_narg;		/* No. de argumentos da chamada */

}	SYSENT;
