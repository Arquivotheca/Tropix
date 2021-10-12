/*
 ****************************************************************
 *								*
 *			<sys/times.h>				*
 *								*
 *	Estruturas manipuladas pelas chamadas de tempo		*
 *								*
 *	Versão	1.0.0, de 02.09.86				*
 *		3.0.0, de 11.05.95				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *		/usr/include/sys				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 1999 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#define	TIMES_H			/* Para definir os protótipos */

/*
 ****************************************************************
 *	Chamada ao sistema "times"				*
 ****************************************************************
 */
typedef	struct	tms	TMS;

struct	tms
{
	time_t	tms_utime;	/* Tempo de usuário */
	time_t	tms_stime;	/* Tempo de sistema */
	time_t	tms_cutime;	/* Tempo de usuário dos filhos */ 
	time_t	tms_cstime;	/* Tempo de sistema dos filhos */
};

/*
 ****************************************************************
 *	Chamada ao sistema "mutime"				*
 ****************************************************************
 */
typedef	struct	mutm	MUTM;

struct	mutm
{
	time_t	mu_time;		/* Tempo (segundos) */
	time_t	mu_utime;		/* Tempo (micro-segundos) */
};

/*
 ****************************************************************
 *	Chamada ao sistema "mutimes"				*
 ****************************************************************
 */
typedef	struct	mutms	MUTMS;

struct	mutms
{
	time_t	mu_utime;		/* Tempo usuário */
	time_t	mu_uutime;
	time_t	mu_stime;		/* Tempo sistema */
	time_t	mu_sutime;
	time_t	mu_cutime;		/* Tempo usuário filhos */
	time_t	mu_cuutime;
	time_t	mu_cstime;		/* Tempo sistema filhos */
	time_t	mu_csutime;
};

/*
 ****************************************************************
 *	Chamada ao sistema "pmutimes"				*
 ****************************************************************
 */
typedef	struct	pmutms	PMUTMS;

struct	pmutms
{
	time_t	mu_seqtime;	/* Tempo estimado do processamento sequencial */
	time_t	mu_sequtime;
	time_t	mu_cseqtime;	/* Soma dos tempos dos filhos */
	time_t	mu_csequtime;
	time_t	mu_partime;	/* Tempo estimado de processamento paralelo */
	time_t	mu_parutime;
	time_t	mu_cpartime;	/* Maior tempo estimado proc. paralelo filhos */
	time_t	mu_cparutime;
};

/*
 ****************************************************************
 *	Protótipos						*
 ****************************************************************
 */
extern time_t	times (TMS *);
extern int	stime (const time_t *);
extern int	mutime (MUTM *);
extern time_t	mutimes (MUTMS *);
extern time_t	pmutimes (PMUTMS *);
extern int	utime (const char *, const time_t []);
