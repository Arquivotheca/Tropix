/*
 ****************************************************************
 *								*
 *			<sys/timeout.h>				*
 *								*
 *	Estrutura da Tabela TIMEOUT				*
 *								*
 *	Versão	3.0.0, de 07.09.94				*
 *		3.2.3, de 23.10.99				*
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

#define	TIMEOUT_H		/* Para definir os protótipos */

/*
 ******	A Entrada ***********************************************
 */
typedef struct timeout	TIMEOUT;

struct timeout
{
	ulong	o_time;		/* Tempo absoluto em "ticks" */
	void	(*o_func) (int); /* Rotina a ser chamada */
	int	o_arg;		/* Argumento */
	TIMEOUT	*o_next;	/* Ponteiro para a próxima entrada */
};

#define	NOTIMEOUT (TIMEOUT *)NULL

/*
 ******	A entrada da fila "hash" do timeout *********************
 */
typedef struct
{
	LOCK	o_lock;		/* Spinlock de cada fila */
	ushort	o_count;	/* Contador (para depuração) */
	TIMEOUT	*o_first;	/* Ponteiro para o início da fila */

}	TIMEOUT_HASH;

#define TIMEOUT_HASH_SZ	64	/* Naturalmente potência de 2 */
