/*
 ****************************************************************
 *								*
 *			<sys/devhead.h>				*
 *								*
 *	Cabeça de fila de driver				*
 *								*
 *	Versão	3.0.0, de 06.10.94				*
 *		3.0.0, de 07.03.96				*
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

#define	DEVHEAD_H			/* Para definir os protótipos */

/*
 ******	A estrutura *********************************************
 */
struct	devhead
{
	LOCK	v_lock;			/* Semaforo de mexendo na fila */
	LOCK	v_busy;			/* Semaforo de Driver Ativo */
	ushort	v_flags;		/* Flags: veja abaixo */

	BHEAD	*v_first;		/* Comeco da Fila */
	BHEAD	*v_last;		/* Final da fila */
};

/*
 ******	Indicadores *********************************************
 */
#define	V_TAPE		0x0001	/* Fita Magnetica - escreva na ordem dada */
#define	V_DMA_24	0x0004	/* Usa DMA de 24 bits */
#define	V_UP		B_UP	/* Sentido do Algoritmo do Elevador */
