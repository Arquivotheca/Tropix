/*
 ****************************************************************
 *								*
 *			<sys/default.h>				*
 *								*
 *	Valores default para alguns parametros do sistema	*
 *								*
 *	Versão	3.0.0, de 06.09.94				*
 *		4.9.0, de 28.07.06				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *		/usr/include/sys				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2006 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

/*
 ******	Tamanho das Tabelas *************************************
 */
#define	NPROC		100	/* No. de entradas da tabela PROC */
enum {	NCON =		4 };	/* No. de consoles virtuais */
#define	NINODE		150	/* No. de entradas da tabela INODE */
#define	NKFILE		150	/* No. de entradas da tabela FILE */
#define	NMAP         (NPROC+2)	/* No. de entradas da tabela MAP */
#define	NHSHLIB		10007	/* No. de entradas da tabela HASH da SHLIB */
#define	NTOUT		40	/* No. de entradas da tabela TIMEOUT, por CPU */
#define	NPTY		64	/* No. de pseudo-terminais */
#define	NCLIST		1024	/* No. de elementos da CLIST */
#define	NBUF		0	/* No. de blocos do "cache de buffers" (auto) */
#define	NMOUNT		16	/* No. Maximo de MOUNTs permitidos */
#define	NLOCKF		100	/* No. de entradas da tabela LOCKF */

#define	NUEVENTL	50	/* No. de entradas da tabela UEVENTL */
#define	NUEVENTG	25	/* No. de entradas da tabela UEVENTG */
#define	NUSEMAL		50	/* No. de entradas da tabela USEMAL */
#define	NUSEMAG		25	/* No. de entradas da tabela USEMAG */

#define	NREGIONL	0	/* No. de entradas da tabela REGIONL */
#define	NREGIONG    (NPROC*4)	/* No. de entradas da tabela REGIONG */

#define	N_DNS		200	/* No. total de DNS */
#define	N_ROUTE		20	/* No. total de ROUTE */
#define	N_ETHER		512	/* No. total de ETHER */
#define	N_EXPORT	32	/* No. total de ETHER */
#define	N_RAW_EP	20	/* No. total de RAW EPs */
#define	N_UDP_EP	20	/* No. total de UDP EPs */
#define	N_TCP_EP	64	/* No. total de TCP EPs */
#define	N_ITBLOCK	120	/* No. máximo de blocos para INTERNET */
#define	N_NFS_DAEMON	4	/* No. de "daemon"s do servidor NFS */

#define	DMESGSZ     (3 * KBSZ)	/* Tamanho da área para "dmesg" (printf) */

/*
 ******	Processo do Usuario *************************************
 */
#define	MAXPPU	10		/* No. max. de processos por usuario */
#define	MAXPSZ	(4096*KBSZ)	/* No. max. de Bytes por processo */
#define	UMASK	007		/* Mascara de criação de arquivos "default" */

#define	STKSZ	(3*KBSZ)	/* Tamanho inicial da stack */
#define	STKINCR	(4*KBSZ)	/* Incremento da stack */

#define	MAXNUEVENT	10	/* No. max. de uevent por processo */	
#define	MAXNUSEMA	10	/* No. max. de usema por processo */	
#define MAXNSHMEM	4	/* No. max. de shmem por processo */

/*
 ******	Parâmetros para shmem ****************************
 */
/*
 *	Na page table para a MMU foi alocada apenas uma página de ponteiros.
 *	Isto significa 4 MB totais para o conjunto das MAXNSHMEM alocáveis 
 *	por um processo. Para o cálculo do endereço virtual, cada shmem 
 *	terá um número de segmentos de endereços virtuais cujo total,
 *	por processo, não deve ultrapassar FREGIONSEGNO. 
 *	O valor de FREGIONSEGNO está limitado a 32 porque vaddrmask em ipc.c
 *	foi implementado como um longo (32 bits).
 */
#define	FREGIONSEGNO	32	/* No. de segmentos de shmem alocáveis por processo */
#define	FREGIONVABASE	(USER_IPC_PGA)		/* VIRTADDR inicial (páginas) */
#define	FREGIONVATOTSZ	((4*KBSZ*KBSZ) >> PGSHIFT) /* VIRTADDR total (páginas) */
#define	FREGIONVASEGSZ	(FREGIONVATOTSZ / FREGIONSEGNO) /* Tam. max. de um seg. (páginas) */

/*
 ******	Diversos *************************************
 */
#define	HZ		100		/* Ticks/second do relogio */
#define	NCEXEC		(8*KBSZ)	/* No. max. de caracteres do EXEC */
#define	SCREEN_SAVER_TIME (5 * 60) 	/* Tempo para o protetor da tela */
#define	MMU_PG_LIMIT	(BYUPPG (32 * KBSZ)) /* Tabelas de MMU próprias */
#define	TZMIN_DEF	(-180) 		/* Fuso horário padrão, em minutos */

/*
 ******	Dispositivos *************************************
 */
#define	ROOTDEV		MAKEDEV (-1, -1)	/* Indefinido */
#define	PIPEDEV		MAKEDEV (-1, -1)	/* Indefinido */
#define	SWAPDEV		MAKEDEV (-1, -1)	/* Indefinido */

#define	RAMD0SZ		512		/* Tamanho do RAM DISK0 em KB */
#define	RAMD1SZ		0		/* Tamanho do RAM DISK1 em KB */
#define	SWAPLOW		0		/* Inicio  do SWAP */
#define	NSWAP		0		/* Tamanho do SWAP */

/*
 ******	Swapper/Pager *************************************
 */
#define	NSWAPIN		1		/* Influencia do NICE no swapin */	
#define	NSWAPOUT	1		/* Influencia do NICE no swapout */	
#define	INAGE		2		/* Tempo (seg) para swapin */
#define	OUTAGE		3		/* Tempo (seg) para swapout */

/*
 ******	Definições das linhas serias ****************************
 */
#define	SIO_PORT_0	0x3F8		/* COM1 */
#define	SIO_PORT_1	0x2F8		/* COM2 */
#define	SIO_PORT_2	0x3E8		/* COM3 */
#define	SIO_PORT_3	0x2E8		/* COM4 */

#define	SIO_IRQ_0	4		/* COM1 */
#define	SIO_IRQ_1	3		/* COM2 */
#define	SIO_IRQ_2	4		/* COM3 */
#define	SIO_IRQ_3	3		/* COM4 */

/*
 ******	Definições das saídas paralelas *************************
 */
#define	LP_PORT_0	0x378		/* LP0 */
#define	LP_PORT_1	0x278		/* LP1 */
#define	LP_PORT_2	0x3BC		/* LP2 */
#define	LP_PORT_3	0		/* LP3 */

#define	LP_IRQ_0	7		/* LP0 */
#define	LP_IRQ_1	0		/* LP1 */
#define	LP_IRQ_2	0		/* LP2 */
#define	LP_IRQ_3	0		/* LP3 */

/*
 ******	Definições das saídas ethernet **************************
 */
#define	ED_PORT_0	0		/* ED0 */
#define	ED_PORT_1	0		/* ED1 */
#define	ED_PORT_2	0		/* ED2 */
#define	ED_PORT_3	0		/* ED3 */

#define	ED_IRQ_0	0		/* ED0 */
#define	ED_IRQ_1	0		/* ED1 */
#define	ED_IRQ_2	0		/* ED2 */
#define	ED_IRQ_3	0		/* ED3 */

/*
 ******	Definições Diversas *************************************
 */

/*
 *	NFLUSH é o número de "buffers" que irão ser atualizados
 *	automaticamente de segundo em segundo.
 */
#define	NFLUSH	0

/*
 *	TSLICE é a constante que determina o tempo em
 *	ticks (1/HZ seg.) entre cada recalculo das prioridades
 *	do processo corrente e dos processos da runqueue
 */
#define	TSLICE	10	/* = 1/10 seg. = 100 ms */

/*
 *	INCPRI é a constante com a qual em cada "time-slice"
 *	são incrementadas as prioridades dos processos na runqueue
 */
#define	INCPRI	10

/*
 *	DECPRI é  a constante com a qual é decrementada a
 *	prioridade do processo corrente em cada "time-slice" 
 */
#define	DECPRI	10

/*
 *	TTYINC é a constante (em segundos) com a qual é
 *	incrementada o tempo de residencia no SWAP de
 *	processos que estavam aguardando entrada de teclado
 *	e foram acordados. O objetivo é melhorar a resposta
 *	para processos interativos.
 */
#define	TTYINC	16

/*
 *	NBASE e NFACTOR são parametros do NICE: No modelo atual
 *	o nice (internamente) varia de -NBASE a +NBASE, com
 *	o valor default zero. NFACTOR indica a influencia do 
 *	NICE na prioridade. As prioridades são incrementadas
 *	(ou decrementadas) do valor NFACTOR * (p_nice).
 *	O nice tambem influencia linearmente no tempo de
 *	residencia no SWAP e multiplicado por 2 no tempo 
 *	residente na memoria.
 */
#define	NBASE	20
#define	NFACTOR	10
