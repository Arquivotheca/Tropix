/*
 ****************************************************************
 *								*
 *			<sys/ipc.h>				*
 *								*
 *	Tabelas de eventos e semáforos do usuário		*
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

#define	IPC_H			/* Para definir os protótipos */

/*
 ******	Formato das estruturas **********************************
 */
struct	ueventl
{
	UEVENTG	*uel_uevent;	/* Ponteiro para o evento global */
};

struct	ueventg
{
	EVENT	ueg_event;	/* Evento propriamente dito */
	char	ueg_reser;
	short	ueg_eventid;	/* Identificação do evento */
	long	ueg_count;	/* Contador de ocorrências */
	UEVENTG	*ueg_next;	/* Próximo evento associado */
};

struct	usemal
{
	USEMAG	*usl_usema;	/* Ponteiro para o semáforo global */
	short	usl_count;	/* Contador de recursos alocados */
	short	usl_reser;
};

struct	usemag
{
	SEMA	usg_sema;	/* Semáforo propriamente dito */
	short	usg_semaid;	/* Identificação do semáforo */
	short	usg_reser;
	USEMAG	*usg_next;	/* Próximo semáforo associado */
};

/*
 ******	Modos de ação do "ueventctl" e "usemactl" ***************
 */
typedef enum
{
	UE_ALLOC,	/* Aloca UEVENTLs */
	UE_GET		/* Obtém o descritor de um UEVENTL*/

}	UEVENTCTLFUNC;

typedef enum
{
	US_ALLOC,	/* Aloca USEMALs */
	US_GET,		/* Obtém o descritor de um USEMAL */
	US_AVAIL,	/* No. de recursos ainda disponíveis */
	US_WAIT		/* No. de threads/procs esperando pelo sem. */

}	USEMACTLFUNC;
