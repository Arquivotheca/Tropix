/*
 ****************************************************************
 *								*
 *			fd.c					*
 *								*
 *	Driver de unidades de disquetes				*
 *								*
 *	Versão	3.0.0, de 06.10.94				*
 *		4.5.0, de 17.06.03				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/cpu.h"
#include "../h/frame.h"
#include "../h/disktb.h"
#include "../h/bhead.h"
#include "../h/devhead.h"
#include "../h/intr.h"
#include "../h/kfile.h"
#include "../h/inode.h"
#include "../h/ioctl.h"
#include "../h/timeout.h"
#include "../h/signal.h"
#include "../h/uproc.h"
#include "../h/uerror.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define NFD		2	/* Por enquanto 2 unidades */
#define FD_IRQ		6	/* No. do IRQ do disquete */
#define FD_PL		3	/* Prioridade de interrupção */
#define FD_DMA		2	/* No. do DMA */
#define splfd		spl3	/* Função de prioridade de interrupção */

#define	LOOP_CTE	100000000 /* Número de tentativas para malhas */
#define	NUM_TRY		10	/* Número de tentativas em geral */
#define	PROBE_TRY	3	/* Número de tentativas para reconhecimento */
#define	SPEEDUPTIME	(1 * scb.y_hz)	/* Tempo para aceleração do motor */
#define	RETAINTIME	(3 * scb.y_hz)	/* Tempo para manter o motor ligado */
#define	TIMEOUTTIME	(10 * scb.y_hz)	/* Tempo para esperar RW */

/*
 ****** Portas do controlador do disquete ***********************
 */
#define FD_STATUS 	0x3F4
#define FD_DATA		0x3F5
#define FD_DOR 		0x3F2	/* Registro digital de saída */
#define FD_DIR 		0x3F7	/* Registro digital de entrada (leitura) */
#define FD_DCR 		0x3F7	/* Registro de contrôle (escrita) */

/*
 ****** Indicadores do registro de estado ***********************
 */
#define STATUS_BUSYMASK	0x0F	/* Máscara de "ocupado" */
#define STATUS_BUSY	0x10	/* DCR ocupado */
#define STATUS_DMA	0x20	/* Modo de DMA */
#define STATUS_DIR	0x40	/* Sentido de informação do DCR */
#define STATUS_READY	0x80	/* Registro de dados ocupado */

/*
 ****** Comandos ************************************************
 */
#define FD_RECALIBRATE	0x07	/* Volte para o cilindro 0 */
#define FD_SEEK		0x0F	/* Vá para um cilindro dado */
#define FD_READ		0xE6	/* Leitura com MT e MFM */
#define FD_WRITE	0xC5	/* Escrita com MT e MFM */
#define FD_SENSED	0x04	/* Obtenha o estado da unidade */
#define FD_SENSEI	0x08	/* Obtenha o estado de interrupção */
#define FD_SPECIFY	0x03	/* Especifica os parâmetros */
#define FD_FORMAT	0x4D	/* Formata uma trilha */
#define FD_VERSION	0x10	/* Obtem o código da versão */
#define FD_CONFIGURE	0x13	/* Configura operação FIFO */
#define FD_PERPENDICULAR 0x12	/* Leitura/escrita perpendicular */

/*
 ****** Bits de DOR *********************************************
 */
#define DOR_SEL		0x03	/* Unidade selecionada */
#define DOR_RESET	0x04	/* Reset */
#define DOR_DMA_ENABLE	0x08	/* Habilita o DMA */
#define DOR_MOTOR	0x10	/* Liga o motor (10 a 80) */

/*
 ****** Bits de FD_ST0 ******************************************
 */
#define ST0_DS		0x03	/* Unidade */
#define ST0_HA		0x04	/* Cabeça */
#define ST0_NR		0x08	/* Não pronta */
#define ST0_ECE		0x10	/* Erro de equipamento */
#define ST0_SE		0x20	/* Final de "seek" */
#define ST0_INTR	0xC0	/* Máscara do código de interrupção */

const char	ST0_bits[] = 	"\x08" "invalid"
				"\x07" "abnormal"
				"\x06" "seek_end"
				"\x05" "drive_check"
				"\x04" "drive_ready"
				"\x03" "top_head";

/*
 ****** Bits de FD_ST1 ******************************************
 */
#define ST1_MAM		0x01	/* Faltando a marca de endereço */
#define ST1_WP		0x02	/* Proteção de escrita */
#define ST1_ND		0x04	/* Ilegível */
#define ST1_OR		0x10	/* "OverRun" */
#define ST1_CRC		0x20	/* Erro de CRC no dado ou endereço */
#define ST1_EOC		0x80	/* Final do cilindro */

#if (0)	/*************************************/
NE7_ST1BITS	"\010end_of_cyl\006bad_crc\005data_overrun\003sec_not_fnd\002write_protect\001no_am"
#endif	/*************************************/
/*
 ****** Bits de FD_ST2 ******************************************
 */
#define ST2_MAM		0x01		/* Faltando a marca de endereço */
#define ST2_BC		0x02		/* Cilindro inválido */
#define ST2_SNS		0x04		/* "Scan" não satisfeito */
#define ST2_SEH		0x08		/* "Scan" igual */
#define ST2_WC		0x10		/* Cilindro errado */
#define ST2_CRC		0x20		/* Erro de CRC no dado */
#define ST2_CM		0x40		/* Marca de controle removida */

#if (0)	/*************************************/
NE7_ST2BITS	"\007ctrl_mrk\006bad_crc\005wrong_cyl\004scn_eq\003scn_not_fnd\002bad_cyl\001no_dam"
#endif	/*************************************/
/*
 ****** Bits de FD_ST3 ******************************************
 */
#define ST3_HA		0x04		/* Cabeça (endereço) */
#define ST3_TZ		0x10		/* Sinal de trilha 0 */
#define ST3_WP		0x40		/* Proteção de escrita */

#if (0)	/*************************************/
NE7_ST3BITS	"\010fault\007write_protect\006drdy\005tk0\004two_side\003side_sel\002"
#endif	/*************************************/

#define NE7_SPEC_1(srt, hut) (((16 - (srt)) << 4) | (((hut) / 16)))
#define NE7_SPEC_2(hlt, nd)  (((hlt) & 0xFE) | ((nd) & 1))

/*
 ******	Tipos de controladores **********************************
 */
enum { FD_NE765, FD_I82077, FD_NE72065, FD_UNKNOWN };

/*
 ******	Tabela de tipos de disquetes ****************************
 */
typedef struct
{
	char	*t_nm;		/* Nome da entrada */
	int	t_sect;		/* Setores por trilha */
	int	t_gap;		/* Espaço entre setores */
	int	t_cyl;		/* No. total de cilindros */
	int	t_size;		/* No. total de setores */
	int	t_stepcyl;	/* Log (2) da rel. cilindro geométrico/lógico */
	int	t_trans;	/* Código da velocidade de transferencia */
	int	t_spec;		/* Velocidade de "seek" e cabeças */
	int	t_format_gap;	/* Espaço entre setores usado na formatação */
	int	t_interleave;	/* No. de setores a pular na formatação */

}	FD_TYPE;

#define	NO_FD_TYPE	(FD_TYPE *)NULL

#define	NHEAD	2	/* Todos tipos têm 2 cabeças */
#define	SECSIZE	2	/* Todos tipos têm setores de 512 bytes */
#define	DATALEN	0xFF	/* Todos tipos têm o mesmo tamanho de dados */

const FD_TYPE fd_types_3[] =
{
/*     sect     gap    cyls     size stepcyl  trans     spec  fmt_gap inter. */

#if (0)	/*******************************************************/
	 "1.72 MB HD em dispositivo de 3½",

	21,	0x04,	82,	3444,	0,	0,	0xCF,	0x0C,	2,
#endif	/*******************************************************/

	 "1.44 MB HD em dispositivo de 3½",

	18,	0x1B,	80,	2880,	0,	0,	0xCF,	0x6C,	1,

	 "720 KB em dispositivo de 3½",

	9,	0x2A,	80,	1440,	0,	2,	0xDF,	0x50,	1,

	""			/* Final da tabela */
};

/* A entradas de 3.5" são para o caso de CMOS errada */

const FD_TYPE fd_types_5[] =
{
/*     sect     gap    cyls     size stepcyl  trans     spec  fmt_gap inter. */

	 "1.72 MB HD em dispositivo de 3½",

	21,	0x04,	82,	3444,	0,	0,	0xCF,	0x0C,	2,

	 "1.44 MB HD em dispositivo de 3½",

	18,	0x1B,	80,	2880,	0,	0,	0xCF,	0x6C,	1,

	 "1.2 MB HD em dispositivo de 5¼",

	15,	0x1B,	80,	2400,	0,	0,	0xDF,	0x54,	1,

	 "360 KB em dispositivo de 5¼",

	9,	0x23,	40,	720,	1,	1,	0xDF,	0x50,	1,

	""			/* Final da tabela */
};

entry FD_TYPE fd_format_types[2];	/* Usado para a formatação */

/*
 ****** Estado das várias unidades ******************************
 */
typedef struct
{
	const FD_TYPE	*s_fd_table;	/* Tabela de 3½ ou 5¼ */
	const FD_TYPE	*s_fd_type;	/* Tipo do disquete montado */

	int		s_geo_cyl_now;	/* Cilindro geométrico no momento */

	char		s_probe;	/* Testando qual o tipo do disquete */
	char		s_write_open;	/* Abrindo para escrita */
	char		s_format;	/* Operação de formatação */

	BHEAD		*s_bp;		/* Bloco em processamento */

	daddr_t		s_blkno;	/* No. do bloco em processamento */
	int		s_bcount;	/* No. de bytes */
	void		*s_phaddr;	/* Endereço físico */

	int		s_sector;	/* Setor desejado */
	int		s_head;		/* Cabeça desejada */
	int		s_log_cyl;	/* Cilindro desejado */
	int		s_geo_cyl;	/* Cilindro geométrico desejado */

	int		s_num_try;	/* Número de tentativas */

}	UNIT_STATUS;

entry UNIT_STATUS	target_status[NFD];

/*
 ******	Estado das unidades *************************************
 */
typedef enum
{
	IDLE,		/* Nada fazendo */
	RESET,		/* Liguei o motor e dando um "reset" */
	RECAL,		/* Recalibrando */
	RECAL_ERROR,	/* Deve refazer o recalibração */
	SEEK,		/* Fazendo Seek */
	SEEK_ERROR,	/* Deve refazer o Seek */
	RW,		/* Lendo/escrevendo */
	RW_ERROR	/* Deve refazer a leitura/escrita */

}	STATE;

/*
 ****** Variáveis globais ***************************************
 */
entry LOCK	fdbusy;		/* Disquete ocupado */
entry STATE	fdstate = IDLE;	/* Estado atual */
entry int	fdtarget = 0;	/* Unidade em atividade no momento */
entry int	fdreset = -1;	/* Última unidade em que deu RESET */
entry int	fdspec = -1;	/* Última especificação dada */
entry int	fdtrans = -1;	/* Última taxa de transmissão dada */
entry int	fd_ctl_type;	/* Tipo do controlador */

entry BHEAD	fdbhead;	/* Para o open */

entry DEVHEAD	fdtab;		/* Cabeca da lista de dp's e do major */
entry DEVHEAD	fdutab[NFD];	/* Cabeças das listas de pedidos das unidade */

entry BHEAD	rfdbuf;		/* Para as operações "raw" */

entry char	fdarea[BLSZ];	/* Área para "probe" e casos de DMA inacessíveis */

entry int	fdtypes;	/* Tipos das unidades (para o "kcntl") */

entry TIMEOUT	*fd_timeout_handle;	/* Para o "reset" */
entry TIMEOUT	*fd_motor_off_handle;	/* Para o "reset" */
entry TIMEOUT	*fdstart_handle;	/* Para o "recal" */

/*
 *	A variável "DOR_value" contém a informação corrente
 *	se o motor está ligado e a unidade selecionada
 */
entry int	DOR_value = DOR_RESET | DOR_DMA_ENABLE;

/*
 ****** Protótipos de funções ***********************************
 */
int		fd_enable_fifo (void);
int		fdstrategy (BHEAD *);
void		timeout_fdstart (void);
void		fdstart (void);
void		fd_timeout (void);
void		fd_motor_off (void);
int		fd_read_fdc_byte (void);
int		fd_write_fdc_byte (int byte);
void		fdint (struct intr_frame);

/*
 ****************************************************************
 *   Verifica se a unidade existe e prepara a interrupção	*
 ****************************************************************
 */
int
fdattach (int major)
{
	int		target, cab;
	const FD_TYPE	*fd_table;
	char		byte, has_fifo = 0;
	const char	*str;

	/*
	 *	Obtém os tipos dos disquetes
	 */
	if ((byte = read_cmos (0x10)) == 0)
		return (-1);

	for (cab = 0, target = 0; target < NFD; target++)
	{
		if ((byte & 0xF0) == 0)		/* 0 == unidade inexistente */
			continue;
#ifdef	ATTACH_MSG
		if (!cab)
			{ printf ("\nUnidade(s) de disquete: "); cab++; }
		else
			printf (", ");

		printf ("fd%d = ", target);
#endif	ATTACH_MSG

		if (target == 0)		/* Particular para NFD == 2 */
			fdtypes  |= ((byte & 0xF0) >> 4);
		else
			fdtypes  |= ((byte & 0xF0) << 4);

		switch (byte & 0xF0)
		{
		    case 0x10:			/* 360 Kb */
#ifdef	ATTACH_MSG
			printf ("360 Kb");
#endif	ATTACH_MSG
			fd_table = fd_types_5;
			break;

		    case 0x20:			/* 1.2 MB */
#ifdef	ATTACH_MSG
			printf ("1.2 MB");
#endif	ATTACH_MSG
			fd_table = fd_types_5;
			break;

		    case 0x30:			/* 720 Kb */
#ifdef	ATTACH_MSG
			printf ("720 Kb");
#endif	ATTACH_MSG
			fd_table = fd_types_3;
			break;

		    case 0x40:			/* 1.44 MB */
#ifdef	ATTACH_MSG
			printf ("1.44 MB");
#endif	ATTACH_MSG
			fd_table = fd_types_3;
			break;

		    default:			/* ???? */
#ifdef	ATTACH_MSG
			printf ("????");
#endif	ATTACH_MSG
		   	fd_table = NO_FD_TYPE;
			break;
		}

		target_status[target].s_fd_table = fd_table;

		byte <<= 4;

	}	/* end for (unidades de disquete) */

#ifdef	ATTACH_MSG
	if (cab)
		printf ("\n\n");
#endif	ATTACH_MSG

	/*
	 *	Dá um RESET
	 */
	DOR_value = DOR_RESET | DOR_DMA_ENABLE;

	write_port (DOR_value & ~(DOR_RESET|DOR_DMA_ENABLE), FD_DOR);

	DELAY (100);

	write_port (DOR_value & ~DOR_DMA_ENABLE, FD_DOR);

	DELAY (100);

	write_port (DOR_value, FD_DOR);

	/*
	 *	Verifica se aceita o primeiro comando
	 */
	if
	(	fd_write_fdc_byte (FD_SPECIFY) < 0 ||
		fd_write_fdc_byte (NE7_SPEC_1 (3, 240)) < 0 ||
		fd_write_fdc_byte (NE7_SPEC_2 (2, 0)) < 0
	)
		return (-1);

	/*
	 *	Tenta descobrir o tipo de controlador
	 */
	if (fd_write_fdc_byte (FD_VERSION) < 0)
		return (-1);

	switch (fd_read_fdc_byte ())
	{
	    case 0x80:
		fd_ctl_type = FD_NE765;
		str = "NE765";
		break;

	    case 0x81:
		fd_ctl_type = FD_I82077;
		str = "I82077";
		break;

	    case 0x90:
		fd_ctl_type = FD_NE72065;
		str = "NE72065";
		break;

	    default:
		fd_ctl_type = FD_UNKNOWN;
		str = "desconhecido"; 
		break;
	}

	/*
	 *	Tenta ligar o FIFO
	 */
	switch (fd_ctl_type)
	{
	    case FD_I82077:
	    case FD_NE72065:
		if (fd_enable_fifo () >= 0)
			has_fifo++;
	}

	printf
	(	"fd:  [0: 0x%3X, %s, %d, %d, FIFO = %d]\n",
		FD_DOR, str, FD_IRQ, FD_DMA,
		has_fifo ? 8 : 0
	);

	/*
	 *	Prepara a interrupção
	 */
	if (set_dev_irq (FD_IRQ, FD_PL, 0, fdint) < 0)
		return (-1);

	fdtab.v_flags |= V_DMA_24;

	return (0);

}	/* end fdattach */

/*
 ****************************************************************
 *	Tenta ligar o FIFO					*
 ****************************************************************
 */
int
fd_enable_fifo (void)
{
	int	i, status;

	/*
	 *	Dá o comando de configuração
	 */
	if (fd_write_fdc_byte (FD_CONFIGURE) < 0)
		return (-1);

	/*
	 *	Espera para ver se o comando é válido
	 */
	for (i = LOOP_CTE; /* abaixo */; i--)
	{
		if (i < 0)
			return (-1);

		status = read_port (FD_STATUS) & (STATUS_DIR|STATUS_READY);

		if (status == STATUS_READY)
			break;

		if (status == (STATUS_DIR|STATUS_READY))
			return (-1);
	}

	/*
	 *	Liga o FIFO
	 */
	if
	(	fd_write_fdc_byte (FD_CONFIGURE) < 0 ||
		fd_write_fdc_byte (0) < 0 ||
		fd_write_fdc_byte (7) < 0 ||	/* Na verdade 8 - 1 */
		fd_write_fdc_byte (0) < 0
	)
		return (-1);

	return (0);

}	/* fd_enable_fifo */

/*
 ****************************************************************
 *	Função de "open"					*
 ****************************************************************
 */
int
fdopen (dev_t dev, int oflag)
{
	UNIT_STATUS	*usp;
	const FD_TYPE	*ft;
	DISKTB		*up;
	int		target;
	int		probe_try = PROBE_TRY;

	/*
	 *	Prólogo
	 */
	if ((up = disktb_get_entry (dev)) == NODISKTB)
		return (-1);

	if ((unsigned)(target = up->p_target) >= NFD)
		{ u.u_error = ENXIO; return (-1); }

	usp = &target_status[target];

	if (usp->s_fd_table == NO_FD_TYPE)
		{ u.u_error = ENXIO; return (-1); }

	/*
	 *	Verifica o "O_LOCK"
	 */
	if (up->p_lock || (oflag & O_LOCK) && up->p_nopen)
		{ u.u_error = EBUSY; return (-1); }

	/*
	 *	Examina a abertura exclusiva
	 */
	SLEEPLOCK (&fdbhead.b_lock, PBLKIO);

	if (oflag & O_FORMAT)
	{
		usp->s_format++;
	 	usp->s_fd_type = usp->s_fd_table; /* Just in case ... */
		goto good;
	}

	usp->s_format = 0;

	if (up->p_nopen)
		goto good;

	/*
	 *	Tenta descobrir o tipo de disquete ("probe")
	 */
	usp->s_probe++;
	usp->s_write_open = (oflag & O_WRITE);

	fdbhead.b_flags   = B_READ;
   	fdbhead.b_phdev   = dev;
   /***	fdbhead.b_phblkno = ...; ***/
	fdbhead.b_base_count  = BLSZ;
	fdbhead.b_addr    = fdarea;

    retry:
 	for (usp->s_fd_type = usp->s_fd_table; /* abaixo */; usp->s_fd_type++)
	{
	 	ft = usp->s_fd_type;

		if (ft->t_nm[0] == '\0')
		{
			if (--probe_try > 0)
				goto retry;

			printf
			(	"fd%d: Não consegui reconhecer o disquete "
				"(possivelmente desformatado)\n",
				target
			);

			u.u_error = ENODEV;

			goto fatal_error;
		}
#ifdef	MSG
		if (CSWT (22))
			printf ("fd%d: Tentando %s\n", target, ft->t_nm);
#endif	MSG
		up->p_size = ft->t_size;
		up->p_head = NHEAD;
		up->p_sect = ft->t_sect;
		up->p_cyl  = ft->t_cyl;

	   	fdbhead.b_phblkno = ft->t_sect - 1;

		EVENTCLEAR (&fdbhead.b_done);

		fdbhead.b_flags &= ~B_ERROR;

		fdstrategy (&fdbhead);

		EVENTWAIT (&fdbhead.b_done, PBLKIO);

		if ((fdbhead.b_flags & B_ERROR) == 0)
			break;

		switch (fdbhead.b_error)
		{
		    case ETIMEOUT:
		    case EROFS:
			u.u_error = fdbhead.b_error;
			goto fatal_error;
		}

	}	/* Tentando vários tipos */

	/*
	 *	Reconheceu
	 */
#ifdef	MSG
	if (CSWT (22))
		printf ("fd%d: O tipo montado é de %s\n", target, ft->t_nm);
#endif	MSG

    good:
	up->p_nopen++;

	if (oflag & O_LOCK)
		up->p_lock = 1;

	SLEEPFREE (&fdbhead.b_lock);

	usp->s_probe = 0;

	u.u_error = NOERR;
	return (0);

	/*
	 *	Teve algum erro
	 */
    fatal_error:
	up->p_size = 0;
	up->p_head = 0;
	up->p_sect = 0;
	up->p_cyl  = 0;

#ifdef	MSG
	if (CSWT (22))
		printf ("fd%d: Não reconheceu, u_error = %d\n", target, u.u_error);
#endif	MSG

	SLEEPFREE (&fdbhead.b_lock);

	usp->s_probe = 0;

	return (-1);

}	/* end fdopen */

/*
 ****************************************************************
 *	Função de close						*
 ****************************************************************
 */
int
fdclose (dev_t dev, int flag)
{
	UNIT_STATUS	*usp;
	DISKTB		*up;
	int		target;

	/*
	 *	Prólogo
	 */
	up = &disktb[MINOR (dev)]; target = up->p_target;
	usp = &target_status[target];

#ifdef	MSG
	if (CSWT (22))
		printf ("fd%d: Close, flag = %d\n", target, flag);
#endif	MSG

	/*
	 *	Atualiza os contadores
	 */
	if (--up->p_nopen <= 0)
	{
		up->p_lock = 0;

		up->p_type = 0;
		up->p_size = 0;

		up->p_head = 0;
		up->p_sect = 0;
		up->p_cyl  = 0;
	}

	return (0);

}	/* end fdclose */

/*
 ****************************************************************
 *	Executa uma operação de entrada/saida			*
 ****************************************************************
 */
int
fdstrategy (BHEAD *bp)
{
	DEVHEAD		*dp;
	const DISKTB	*up;
	UNIT_STATUS	*usp;
	int		target;
	daddr_t		bn;

	/*
	 *	verifica a validade do pedido
	 */
	up = &disktb[MINOR (bp->b_phdev)]; target = up->p_target;
	usp = &target_status[target];

	if ((unsigned)target >= NFD)
		{ bp->b_error = ENXIO; goto bad; }

	if ((bn = bp->b_phblkno) < 0)
		{ bp->b_error = ENXIO; goto bad; }

	if (!usp->s_format && bn + BYTOBL (bp->b_base_count) > up->p_size)
		{ bp->b_error = ENXIO; goto bad; }

	/*
	 *	coloca o pedido na fila e inicia a operação
	 */
	bp->b_cylin = bn;	/* Os números de blocos são pequenos */

	dp = &fdutab[target];

#if (0)	/*******************************************************/
	splfd ();
#endif	/*******************************************************/
	splx (irq_pl_vec[FD_IRQ]);

	insdsort (dp, bp);

	if (TAS (&fdbusy) >= 0)
		fdstart ();

	spl0 ();

	return (0);

	/*
	 *	Houve algum erro
	 */
    bad:
	bp->b_flags |= B_ERROR;
	EVENTDONE (&bp->b_done);
	return (-1);

}	/* end fdstrategy */

/*
 ****************************************************************
 *	Inicia uma operação em uma unidade (para "timeout")	*
 ****************************************************************
 */
void
timeout_fdstart (void)
{
#if (0)	/*******************************************************/
	splfd ();
#endif	/*******************************************************/
	splx (irq_pl_vec[FD_IRQ]);

	fdstart ();

	spl0 ();

}	/* end timeout_fdstart */

/*
 ****************************************************************
 *	Inicia uma operação em uma unidade			*
 ****************************************************************
 */
void
fdstart (void)
{
	DEVHEAD		*dp;
	BHEAD		*bp;
	UNIT_STATUS	*usp;
	const FD_TYPE	*ft;
	daddr_t		block;
	int		target;
	int		i, rw;

	/*
	 *	Pequena inicialização
	 */
	target = fdtarget; usp = &target_status[target];

	if (fdstart_handle)
	{
		toutreset (fdstart_handle, timeout_fdstart, 0);
		fdstart_handle = NOTIMEOUT;
	}

	/*
	 *	Verifica o estado e o que fazer
	 */
	switch (fdstate)
	{
		/*
		 *	Não tem nenhuma operação em andamento
		 */
	    case IDLE:
		for (i = 0; /* abaixo */; i++)
		{
			if (i >= NFD)
				{ CLEAR (&fdbusy); return; }

			dp = &fdutab[fdtarget];

			SPINLOCK (&dp->v_lock);

			if ((bp = dp->v_first) != NOBHEAD)
				break;

			SPINFREE (&dp->v_lock);

			if (++fdtarget >= NFD)
				fdtarget = 0;

		}	/* end for (pelas unidades) */

		/*
		 *	Achou algo para fazer
		 */
		if (fd_motor_off_handle)
		{
			toutreset (fd_motor_off_handle, fd_motor_off, 0);
			fd_motor_off_handle = NOTIMEOUT;
		}

		if (fd_timeout_handle)	/* Em todos os casos ... */
		{
			toutreset (fd_timeout_handle, fd_timeout, 0);
			fd_timeout_handle = NOTIMEOUT;
		}

		target = fdtarget;	/* Unidade seguinte */

		SPINFREE (&dp->v_lock);

		usp = &target_status[target];

		usp->s_bp      = bp;
		usp->s_blkno   = bp->b_phblkno;
		usp->s_bcount  = bp->b_base_count;
		usp->s_phaddr  = (void *)VIRT_TO_PHYS_ADDR (bp->b_base_addr);
		usp->s_num_try = usp->s_probe ? 2 : NUM_TRY;
#ifdef	MSG
		if (CSWT (22))
		{
			printf
			(	"fd%d: Iniciando: bp = %P, %c, blkno = %d, bcount = %d\n",
				target, bp, (bp->b_flags & B_READ) ? 'R' : 'W',
				usp->s_blkno, usp->s_bcount
			);
		}
#endif	MSG
		/*
		 *	Verifica se o motor está ligado e a unidade selecionada
		 */
		if
		(	(DOR_value & (DOR_MOTOR << target)) == 0 ||
			fdreset != target ||
			(DOR_value & DOR_SEL) != target ||
			usp->s_probe
		)
		{
#ifdef	MSG
			if (CSWT (22))
				printf ("fd%d: RESET\n", target);
#endif	MSG
			DOR_value = DOR_RESET | DOR_DMA_ENABLE | (DOR_MOTOR << target) | target;

			write_port (DOR_value & ~(DOR_RESET|DOR_DMA_ENABLE), FD_DOR);

			DELAY (100);

			write_port (DOR_value & ~DOR_DMA_ENABLE, FD_DOR);

			DELAY (100);

			write_port (DOR_value, FD_DOR);

			fdreset = target;
			fdspec = -1; fdtrans = -1;

		  	fdstate = RESET;
			return;
		}
		else	/* Tudo pronto */
		{
			goto case_SEEK;
		}

		/*
		 *	Inicia a operação de recalibração
		 */
	    case RECAL:
#if (0)	/*******************************************************/
		usp->s_num_try  = usp->s_probe ? 2 : NUM_TRY;
#endif	/*******************************************************/
	    case RECAL_ERROR:
#ifdef	MSG
		if (CSWT (22))
			printf ("fd%d: RECAL\n", target);
#endif	MSG
		fd_write_fdc_byte (FD_RECALIBRATE);
		fd_write_fdc_byte (target);

	   	fdstate = RECAL;
		return;

		/*
		 *	Inicia a operação de SEEK
		 */
	    case SEEK:
	    case_SEEK:
		block = usp->s_blkno;
		ft    = usp->s_fd_type;

		usp->s_sector	= block % ft->t_sect + 1;
		block		= block / ft->t_sect;
		usp->s_head	= block % NHEAD;
		usp->s_log_cyl	= block / NHEAD;
		usp->s_geo_cyl	= usp->s_log_cyl << ft->t_stepcyl;
#if (0)	/*******************************************************/
		usp->s_num_try  = usp->s_probe ? 1 : NUM_TRY;
#endif	/*******************************************************/

		/*
		 *	Verifica se precisa de SEEK
		 */
		if (usp->s_geo_cyl_now != usp->s_geo_cyl)
		{
	    case SEEK_ERROR:
#ifdef	MSG
			if (CSWT (22))
			{
				printf
				(	"fd%d: SEEK: cyl = %d, geo_cyl = %d\n",
					target, usp->s_log_cyl, usp->s_geo_cyl
				);
			}
#endif	MSG
			fd_write_fdc_byte (FD_SEEK);
			fd_write_fdc_byte (target);
			fd_write_fdc_byte (usp->s_geo_cyl);

		   	fdstate  = SEEK;
			return;
		}

	   /*** goto case_RW; ***/

		/*
		 *	Começa a operação de RW
		 */
	    case RW:
		if (fdspec != usp->s_fd_type->t_spec)
		{
			fd_write_fdc_byte (FD_SPECIFY);
			fd_write_fdc_byte (usp->s_fd_type->t_spec);
			fd_write_fdc_byte (2);

			fdspec = usp->s_fd_type->t_spec;
		}

		if (fdtrans != usp->s_fd_type->t_trans)
		{
			write_port (usp->s_fd_type->t_trans, FD_DCR);
			fdtrans = usp->s_fd_type->t_trans;
		}

#if (0)	/*******************************************************/
		usp->s_num_try = usp->s_probe ? 1 : NUM_TRY;
#endif	/*******************************************************/
	    case RW_ERROR:
		if (usp->s_format)
			goto format;

		rw = usp->s_bp->b_flags & (B_READ|B_WRITE);
		ft = usp->s_fd_type;

		uni_dma_setup (rw, usp->s_phaddr, BLSZ, FD_DMA);
#ifdef	MSG
		if (CSWT (22))
		{
			printf
			(	"fd%d: RW: sector = %d, head = %d, cyl = %d, addr = %P\n",
				target, usp->s_sector, usp->s_head, usp->s_log_cyl, usp->s_phaddr
			);
		}
#endif	MSG

		fd_write_fdc_byte (rw == B_WRITE ? FD_WRITE : FD_READ);
		fd_write_fdc_byte (usp->s_head << 2 | target);
		fd_write_fdc_byte (usp->s_log_cyl);
		fd_write_fdc_byte (usp->s_head);
		fd_write_fdc_byte (usp->s_sector);
		fd_write_fdc_byte (SECSIZE);
		fd_write_fdc_byte (ft->t_sect);
		fd_write_fdc_byte (ft->t_gap);
		fd_write_fdc_byte (DATALEN);

	   	fdstate = RW;

		if (fd_timeout_handle)	/* Em todos os casos ... */
			toutreset (fd_timeout_handle, fd_timeout, 0);

		fd_timeout_handle = toutset (TIMEOUTTIME, fd_timeout, 0);
		return;

		/*
		 *	Formata uma trilha
		 */
	    format:
		ft = usp->s_fd_type;

		/* A estrutura IMAGE tem 4 bytes */

		uni_dma_setup (B_WRITE, usp->s_phaddr, ft->t_sect * 4, FD_DMA);
#ifdef	MSG
		if (CSWT (22))
		{
			printf
			(	"fd%d: FORMAT: sector = %d, head = %d, cyl = %d, addr = %P\n",
				target, usp->s_sector, usp->s_head, usp->s_log_cyl, usp->s_phaddr
			);
		}
#endif	MSG

		fd_write_fdc_byte (FD_FORMAT);
		fd_write_fdc_byte (usp->s_head << 2 | target);
		fd_write_fdc_byte (SECSIZE);
		fd_write_fdc_byte (ft->t_sect);
		fd_write_fdc_byte (ft->t_format_gap);
		fd_write_fdc_byte (0xF6);	/* Fill byte */

	   	fdstate = RW;

		if (fd_timeout_handle)	/* Em todos os casos ... */
			toutreset (fd_timeout_handle, fd_timeout, 0);

		fd_timeout_handle = toutset (TIMEOUTTIME, fd_timeout, 0);
		return;

	    default:
		printf ("fdstart: Estado inválido: %d; ", fdstate);
		return;

	}	/* end switch */

}	/* end fdstart */

/*
 ****************************************************************
 *	Interrupção						*
 ****************************************************************
 */
void
fdint (struct intr_frame frame)
{
	UNIT_STATUS	*usp;
	DEVHEAD		*dp;
	BHEAD		*bp;
	int		target;
	STATE		next_state;
	int		status[7];
	int		i;

	/*
	 *	Pequena inicialização
	 */
	target = fdtarget; usp = &target_status[target]; bp  = usp->s_bp;

	/*
	 *	Desliga o TIMEOUT
	 */
	if (fd_timeout_handle)
	{
		toutreset (fd_timeout_handle, fd_timeout, 0);
		fd_timeout_handle = NOTIMEOUT;
	}

	/*
	 *	Analisa o estado em que veio a interrupção
	 */
	switch (fdstate)
	{
		/*
		 ******	Interrupção no estado RESET *************
		 */
	    case RESET:
#ifdef	MSG
		if (CSWT (22))
			printf ("INTR RESET\n");
#endif	MSG
		/* Verifica se pode escrever no disquete */

		if (usp->s_probe && usp->s_write_open)
		{
			fd_write_fdc_byte (FD_SENSED);
			fd_write_fdc_byte (target);
		
			if (fd_read_fdc_byte () & ST3_WP)
			{
				bp->b_error = EROFS;
				bp->b_flags |= B_ERROR;
				goto done;
			}
		}

		fdstart_handle = toutset (SPEEDUPTIME, timeout_fdstart, 0);
	  	fdstate = RECAL;
		return;

		/*
		 ******	Interrupção no estado RECAL *************
		 */
	    case RECAL:
#ifdef	MSG
		if (CSWT (22))
			printf ("INTR RECAL\n");
#endif	MSG

		fd_write_fdc_byte (FD_SENSEI);

		for (i = 0; i < 7; i++)
			status[i] = fd_read_fdc_byte ();

		if ((status[0] & ST0_INTR) != 0 || status[1] != 0)
		{
#ifdef	MSG
			if (CSWT (22) || !usp->s_probe && usp->s_num_try != NUM_TRY)
#else
			if (!usp->s_probe && usp->s_num_try != NUM_TRY)
#endif	MSG
			{
				printf
				(	"fd%d: Erro no RECALIBRATE (ST0 = 0x%b)\n"
					"cil = %d\n",
					target, status[0], ST0_bits, status[1]
				);
			}

			if (--usp->s_num_try > 0)
				{ fdstate = RECAL_ERROR; break; }

			goto fatal_error;
		}
		else	/* Não teve erro */
		{
			usp->s_geo_cyl_now = 0;

			fdstate = SEEK;
			break;
		}

		/*
		 ******	Interrupção no estado SEEK **************
		 */
	    case SEEK:
#ifdef	MSG
		if (CSWT (22))
			printf ("INTR SEEK\n");
#endif	MSG
		fd_write_fdc_byte (FD_SENSEI);

		for (i = 0; i < 7; i++)
			status[i] = fd_read_fdc_byte ();

		if ((status[0] & 0xF8) != 0x20 || status[1] != usp->s_geo_cyl)
		{
			usp->s_geo_cyl_now = status[1];
#ifdef	MSG
			if (CSWT (22) || !usp->s_probe && usp->s_num_try != NUM_TRY)
#else
			if (!usp->s_probe && usp->s_num_try != NUM_TRY)
#endif	MSG
			{
				printf
				(	"fd%d: Erro no SEEK (ST0 = 0x%b), "
					"cil. pedido = %d, cil. = %d\n",
					target, status[0], ST0_bits,
					usp->s_geo_cyl, status[1]
				);
			}

			if (--usp->s_num_try > 0)
				{ fdstate = SEEK_ERROR; break; }

			goto fatal_error;
		}
		else	/* Não teve erro */
		{
			usp->s_geo_cyl_now = usp->s_geo_cyl;

			fdstate = RW;
			break;
		}

		/*
		 ******	Interrupção no estado RW ****************
		 */
	    case RW:
#ifdef	MSG
		if (CSWT (22))
			printf ("INTR RW\n");
#endif	MSG
#if (0)	/*******************************************************/
fd_write_fdc_byte (FD_SENSEI);
#endif	/*******************************************************/

		for (i = 0; i < 7; i++)
			status[i] = fd_read_fdc_byte ();

		switch ((status[0] & ST0_INTR) >> 6)
		{
			/*
			 *	Erro ocorrido durante a execução do comando
			 */
		    case 1:
			next_state = RW_ERROR;

			if   (usp->s_probe)
			{
#ifdef	MSG
				if (CSWT (22))
					printf ("fd%d: Erro de RW\n", target);
#endif	MSG
#if (0)	/*******************************************************/
				if (--usp->s_num_try > 0)
				   	{ fdstate = RW_ERROR; break; }

				goto fatal_error;
#endif	/*******************************************************/
			}
			elif (status[1] & ST1_WP)
			{
				printf ("fd%d: Proteção de escrita\n", target);

				bp->b_error = EROFS;
				bp->b_flags |= B_ERROR;

				goto done;
			}
			elif (status[1] & ST1_OR)
			{
#ifdef	MSG
				if (CSWT (22) || usp->s_num_try <= NUM_TRY/2)
#else
				if (usp->s_num_try <= NUM_TRY/2)
#endif	MSG
				{
					printf ("fd%d: \"Over/underrun\"\n", target);
					usp->s_num_try++;	/* Tenta para sempre */
				}
			}
			elif (status[0] & ST0_ECE)
			{
				printf ("fd%d: Falha na recalibração\n", target);
				next_state = RECAL_ERROR;
			}
			elif (status[2] & ST2_CRC)
			{
				printf
				(	"fd%d: Erro no CRC dos dados, bloco %d\n",
					target, usp->s_blkno
				);
			}
			elif (status[1] & ST1_CRC)
			{
				printf
				(	"fd%d: Erro de CRC, bloco %d\n",
					target, usp->s_blkno
				);
			}
			elif ((status[1] & (ST1_MAM|ST1_ND)) || (status[2] & ST2_MAM))
			{
#ifdef	MSG
				if (CSWT (22) || usp->s_num_try != NUM_TRY)
#else
				if (usp->s_num_try != NUM_TRY)
#endif	MSG
					printf
					(	"fd%d: Setor não encontrado, bloco %d\n",
						target, usp->s_blkno
					);

				next_state = RECAL_ERROR;

#if (0)	/*******************************************************/
				if (--usp->s_num_try > 0)
					{ fdstate = SEEK_ERROR; break; }

				goto fatal_error;
#endif	/*******************************************************/
			}
			elif (status[2] & ST2_WC)
			{
				printf ("fd%d: Estou no cilindro errado\n", target);
				next_state = RECAL_ERROR;
			}
			elif (status[2] & ST2_BC)
			{
				printf ("fd%d: Cilindro defeituoso\n", target);
			}
			else
			{
				printf
				(	"fd%d: Erro desconhecido. Os valores de status[0..3] são: "
					"0x%02X 0x%02X 0x%02X 0x%02X\n",
					target, status[0], status[1], status[2], status[3]
				);
			}

			if (--usp->s_num_try > 0)
			   	{ fdstate = next_state; break; }

			/*
			 *	Erro irrecuperável
			 */
		   fatal_error:
			bp->b_error = EIO;
			bp->b_flags |= B_ERROR;
			goto done;

			/*
			 *	Foi dado um comando inválido
			 */
		    case 2:
			printf ("fd%d: Comando inválido de controlador\n", target);
			goto fatal_error;

			/*
			 *	Término anormal de "polling"
			 */
		    case 3:
			printf ("fd%d: Terminação anormal de \"polling\"\n", target);
			goto fatal_error;

			/*
			 *	Sucesso!
			 */
	       /*** case 0: ***/
		    default:
			if (usp->s_format)
				goto done;
#undef	DEBUG
#ifdef	DEBUG
			if ((hz & 0x1E) == 0x10)
			{
				printf
				(	"fd%d: Simulando erro, bloco %d\n",
					target, usp->s_blkno
				);

				if (--usp->s_num_try > 0)
					{ fdstate = RECAL_ERROR; break; }

				goto fatal_error;
			}
#endif	DEBUG
			usp->s_blkno  += 1;
			usp->s_bcount -= BLSZ;
			usp->s_phaddr += BLSZ;

			if (usp->s_bcount > 0)
			{
			   	fdstate = SEEK;
				break;
			}

			/*
			 *	Terminou esta operação de RW
			 */
		    done:
			dp = &fdutab[target];

			SPINLOCK (&dp->v_lock);

			bp = remdsort (dp);

			SPINFREE (&dp->v_lock);

			bp->b_residual = 0;

			EVENTDONE (&bp->b_done);

			fd_motor_off_handle = toutset (RETAINTIME, fd_motor_off, 0);

			fdstate = IDLE;

			break;

		}	/* end switch (ST0) */

		break;

		/*
		 ******	Interrupção no estado inválido **********
		 */
	    default:
		if (CSWT (22))
		{
			printf
			(	"fd%d: Interrupção inesperada (estado %d)\n",
				target, fdstate
			);
		}

		fd_write_fdc_byte (FD_SENSEI);

		for (i = 0; i < 7; i++)
			status[i] = fd_read_fdc_byte ();

		return;

	}	/* end switch (fdstate) */

	/*
	 *	Executa o pedido seguinte ou re-executa o pedido atual
	 */
	fdstart ();

}	/* end fdint */

/*
 ****************************************************************
 *	Foi esgotado o tempo de RW				*
 ****************************************************************
 */
void
fd_timeout (void)
{
	UNIT_STATUS	*usp;
	DEVHEAD		*dp;
	BHEAD		*bp;
	int		target;

#ifdef	MSG
	if (CSWT (22))
		printf ("TIMEOUT RW\n");
#endif	MSG

	/*
	 *	Pequena inicialização
	 */
	fd_timeout_handle = NOTIMEOUT;

	target = fdtarget; usp = &target_status[target]; bp  = usp->s_bp;

	/*
	 *	Esvazia a fila de pedidos da unidade
	 */
	dp = &fdutab[target];

	SPINLOCK (&dp->v_lock);

	while (dp->v_first != NOBHEAD)
	{
		bp = remdsort (dp);
		bp->b_residual = 0;
		bp->b_error = ETIMEOUT;
		bp->b_flags |= B_ERROR;

		EVENTDONE (&bp->b_done);
	}

	SPINFREE (&dp->v_lock);

	/*
	 *	Desliga a unidade
	 */
	DOR_value = DOR_RESET|DOR_DMA_ENABLE;

	write_port (DOR_value, FD_DOR);

	CLEAR (&fdbusy);

	fdstate = IDLE;

}	/* end fd_timeout */

/*
 ****************************************************************
 *	Desliga o motor						*
 ****************************************************************
 */
void
fd_motor_off (void)
{
	fd_motor_off_handle = NOTIMEOUT;

	DOR_value = DOR_RESET|DOR_DMA_ENABLE;

	write_port (DOR_value, FD_DOR);

}	/* end fd_motor_off */

/*
 ****************************************************************
 *	Lê um byte de dados do controlador			*
 ****************************************************************
 */
int
fd_read_fdc_byte (void)
{
	int		i;
	char		status;

	/*
	 *	Tenta várias vezes
	 */
	for (i = LOOP_CTE; /* abaixo */; i--)
	{
		if (i < 0)
			{ printf ("fd_read_fdc_byte: Tempo esgotado\n"); break; }

		status = read_port (FD_STATUS) & (STATUS_DIR|STATUS_READY);

		if (status == (STATUS_DIR|STATUS_READY))
			return (read_port (FD_DATA));

		if (status == STATUS_READY)
			break;
	}

	return (-1);

}	/* end fd_read_fdc_byte */

/*
 ****************************************************************
 *	Escreve um byte de dados no controlador			*
 ****************************************************************
 */
int
fd_write_fdc_byte (int byte)
{
	int		i;
	int		r = 0;

	/*
	 *	Tenta várias vezes
	 */
	for (i = LOOP_CTE; /* abaixo */; i--)
	{
		if (i < 0)
			{ printf ("fd_write_fdc_byte: Tempo esgotado da direção\n"); r = -1; break; }

		if ((read_port (FD_STATUS) & STATUS_DIR) == 0)
			break;
	}

	for (i = LOOP_CTE; /* abaixo */; i--)
	{
		if (i < 0)
			{ printf ("fd_write_fdc_byte: Tempo esgotado da unidade pronta\n"); r = -1; break; }

		if (read_port (FD_STATUS) & STATUS_READY)
			break;
	}

	write_port (byte, FD_DATA);
	return (r);

}	/* end fd_write_fdc_byte */

/*
 ****************************************************************
 *	Leitura de modo "raw"					*
 ****************************************************************
 */
int
fdread (IOREQ *iop)
{
	if (iop->io_offset_low & BLMASK || iop->io_count & BLMASK)
		u.u_error = EINVAL;
	else
		physio (iop, fdstrategy, &rfdbuf, B_READ, 1 /* dma */);

	return (UNDEF);

}	/* end fdread */

/*
 ****************************************************************
 *	Escrita de modo "raw"					*
 ****************************************************************
 */
int
fdwrite (IOREQ *iop)
{
	if (iop->io_offset_low & BLMASK || iop->io_count & BLMASK)
		u.u_error = EINVAL;
	else
		physio (iop, fdstrategy, &rfdbuf, B_WRITE, 1 /* dma */);

	return (UNDEF);

}	/* end fdwrite */

/*
 ****************************************************************
 *	Rotina de IOCTL						*
 ****************************************************************
 */
int
fdctl (dev_t dev, int cmd, int arg, int flag)
{
	DISKTB		*up;
	UNIT_STATUS	*usp;
	int		target;

	/*
	 *	Prólogo
	 */
	up = &disktb[MINOR (dev)]; target = up->p_target; usp = &target_status[target];

	/*
	 *	Executa as funções
	 */
	switch (cmd)
	{
	    case DKISADISK:
		return (0);

	    case FD_GET_TYPE_TB:
		if (arg == 0)		/* Ponteiro NULO: No. de entradas */
			return (20);

		if (unimove ((void *)arg, usp->s_fd_table, 20 * sizeof (FD_TYPE), SU) < 0)
			{ u.u_error = EFAULT; return (-1); }

		return (0);

	    case FD_PUT_FORMAT_TYPE:
		if (unimove (&fd_format_types[target], (void *)arg, sizeof (FD_TYPE), US) < 0)
			{ u.u_error = EFAULT; return (-1); }

		usp->s_fd_type = &fd_format_types[target];

		return (0);

	}	/* end switch */

	u.u_error = EINVAL;
  	return (-1);

}	/* end fdctl */
