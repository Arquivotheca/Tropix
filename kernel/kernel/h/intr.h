/*
 ****************************************************************
 *								*
 *			intr.h					*
 *								*
 *	Definições relativas a interrupções			*
 *								*
 *	Versão	3.0.0, de 07.10.94				*
 *		4.5.0, de 23.12.03				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *		/usr/include/sys				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#define	INTR_H			/* Para definir os protótipos */

/*
 ******	Estrutura de definição de um vetor de interrupção *******
 */

/* Esta definição deve ser igual à de "ctl/common.s" */

typedef struct
{
	int	vec_unit;	/* Unidade (informação para o driver) */
	void	(*vec_func) ();	/* Função de interrupção */

}	VECDEF;

#define	VECLINESZ	8			/* No. máximo de compartilhamento + 1 */

typedef VECDEF		VECDEFLINE[VECLINESZ];	/* Compartilha até 7 unidades por IRQ */

extern VECDEFLINE	vecdef[16];		/* O vetor de controle */

extern void	(*vecdefaddr[16]) ();		/* Vetor de endereços dos vetores */

/*
 ******	Tabela de IRQs (em ordem de prioridade) *****************
 */
#define	IRQ0		0x0001		/* Mais alta: relógio */
#define	IRQ1		0x0002		/* pc */
#define	IRQ_SLAVE	0x0004		/* x */
#define	IRQ8		0x0100		/* x */
#define	IRQ9		0x0200		/* x */
#define	IRQ2		IRQ9		/* we, ne, ec */
#define	IRQ10		0x0400		/* is */
#define	IRQ11		0x0800		/* as */
#define	IRQ12		0x1000		/* x */
#define	IRQ13		0x2000		/* npx */
#define	IRQ14		0x4000		/* hd */
#define	IRQ15		0x8000		/* x */
#define	IRQ3		0x0008		/* comm */
#define	IRQ4		0x0010		/* comm */
#define	IRQ5		0x0020		/* wt */
#define	IRQ6		0x0040		/* fd */
#define	IRQ7		0x0080		/* Mais baixa: impressora */

/*
 ****** Definições globais **************************************
 */
#define	TRAP_GATE	15
#define	INTR_GATE	14

#define	KERNEL_PL	0
#define	USER_PL		3
