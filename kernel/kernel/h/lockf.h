/*
 ****************************************************************
 *								*
 *			<sys/lockf.h>				*
 *								*
 *	Tabela de LOCKs de regiões de arquivos			*
 *								*
 *	Versão	3.0.0, de 07.09.94				*
 *		3.0.0, de 07.09.94				*
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

#define	LOCKF_H			/* Para definir os protótipos */

/*
 ******	Formato da estrutura ************************************
 */
struct lockf
{
	EVENT	k_free;		/* Evento de liberação do LOCK */
	char	k_reser[3];	/* o EVENT é o primeiro membro da estrutura */

	UPROC	*k_uproc;	/* Processo dono do LOCK */

	off_t	k_begin;	/* Início da região */
	off_t	k_end;		/* Final da região */

	LOCKF	*k_next;	/* Próximo LOCK */
};

/*
 ******	Modos de ação do LOCKF **********************************
 */
typedef enum
{
	F_ULOCK,	/* Libera */
	F_LOCK,		/* Lock com espera */
	F_TLOCK,	/* Lock com retorno de erro */
	F_TEST		/* Apenas consulta */

}	LOCKFMODE;
