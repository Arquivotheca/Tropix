/*
 ****************************************************************
 *								*
 *			<sys/sync.h>				*
 *								*
 *	Definicões da sincronização				*
 *								*
 *	Versão	3.0.0, de 06.09.94				*
 *		4.6.0, de 21.09.04				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *		/usr/include/sys				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#define	SYNC_H		/* Para incluir os protótipos */

/*
 ******	Definições globais **************************************
 */
#define	NHIST	10	/* No. de Entradas do Historico do Sincr. */

typedef	uchar	LOCK;	/* SPINLOCK's e SLEEPLOCK's */

typedef	uchar	EVENT;	/* EVENTWAIT's */

typedef	struct		/* SEMALOCK's */
{
	LOCK	e_lock;	
	char	e_flag;
	short	e_count;

}	SEMA;

/*
 ******	Macros dos semaforos rápidos ****************************
 */
#define	SPINLOCK(s)	spinlock (s)

#define	SPINFREE(s)	spinfree (s)

#define	SPINTEST(s)	spintest (s)

#define	TAS(s)		tas (s)

#define	CLEAR(s)	*(s) = 0

/*
 ******	Macros do semaforo lento ********************************
 */
#define SLEEPLOCK(e,p)	sleeplock (e, p)

#define SLEEPFREE(e)	sleepfree (e)

#define	SLEEPTEST(e)	sleeptest (e)
 
#define	SLEEPSET(e)	*(e) = 0x80

#define	SLEEPCLEAR(e)	*(e) = 0x00

#define	SLEEPSYNC(e,p)	sleeplock (e, p); sleepfree (e)

/*
 ******	Macros do semaforo de multiplos recursos ****************
 */
#define SEMALOCK(e,p)	semalock (e, p)

#define SEMAFREE(e)	semafree (e)

#define	SEMATEST(e)	sematest (e)

#define SEMAINIT(e,n,f)	{ (e)->e_count = (n); (e)->e_flag = (f); }

/*
 ******	Macros dos eventos **************************************
 */
#define EVENTWAIT(e,p)	eventwait (e, p)

#define	EVENTCOUNTWAIT(n, m, e, p) eventcountwait (n, m, e, p)

#define EVENTDONE(e)	eventdone (e)

#define	EVENTTEST(e)	eventtest (e)
 
#define	EVENTSET(e)	*(e) = 0x80

#define	EVENTCLEAR(e)	*(e) = 0x00

/*
 ******	Entrada da tabela do historico de sincronismo ***********
 */
struct	synch
{
	char	e_type;		/* Tipo do Semaforo */
	char	e_state;	/* Estado */
	short	e_reser;
	void	*e_sema;	/* Endereco do Semaforo */
};

/*
 ******	Tipos ***************************************************
 */	
#define	E_NULL		0
#define	E_SLEEP		1
#define	E_SEMA		2
#define	E_EVENT		3

/*
 ******	Estados *************************************************
 */
#define	E_GOT		1
#define	E_WAIT		2
