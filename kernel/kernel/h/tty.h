/*
 ****************************************************************
 *								*
 *			<sys/tty.h>				*
 *								*
 *	Definições das estruturas dos terminais			*
 *								*
 *	Versão	3.0.0, de 07.09.94				*
 *		4.9.0, de 14.06.06				*
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

#define	TTY_H			/* Para definir os protótipos */

/*
 ******	Estruturas da CLIST *************************************
 */
struct	chead
{
	LOCK	c_lock;		/* Semaforo da CLIST */
	short	c_count;	/* No. de Caracteres */
	char	*c_cf;		/* Ponteiro para o primeiro caracter */
	char	*c_cl;		/* Ponteiro para o carac. seg. ao ultimo */
};

struct	cblk
{
	CBLK	*c_next;	/* bloco seguinte */
	char	c_info[CLBSZ];	/* os caracteres */
};

/*
 ******	Estrutura para o IOCTL **********************************
 */
#define	NCC	15		/* No. de Caracteres de Controle */
#define	NWC	4		/* No. de caracteres da janela */

struct	termio
{
	ushort	c_iflag;	/* Modos de Entrada */
	ushort	c_oflag;	/* Modos de Saida */
	ushort	c_cflag;	/* Modos de Controle */
	ushort	c_lflag;	/* Modos locais */
	char	c_line;		/* Disciplina da Linha */
	char	c_cc[NCC];	/* Caracteres de Controle */
	char	c_name[16];	/* Nome da Linha */
	char	c_wc[4];	/* Controle da janela */
	long	c_res[3];	/* Reservado para uso Futuro */
};

#define	VINTR	0		/* Interrupção (^C) */
#define	VQUIT	1		/* Quit (FS) */
#define	VERASE	2		/* Erase (DEL) */
#define	VKILL	3		/* Kill (^U) */
#define	VEOF	4		/* End of File (^D) */
#define	VMIN	5		/* No. MIN de car. para entrada RAW */
#define	VEOL	6		/* End of Line (Não usado) */
#define	VTIME	7		/* Tempo minimo para entrada RAW */
#define	VRES	8		/* (Reservado para uso futuro) */
#define	SWTCH	9		/* (Não Identificado) */
#define	VPAGE	10		/* Tamanho da Pagina */
#define	VAERASE	11		/* Outro Erase (BS) */
#define	VRETYPE	12		/* Retype (^R) */
#define	VWORD	13		/* Apaga Palavra (^W) */
#define	VCOLOR	14		/* Cor dos caracteres */

#define	VNLINE	0		/* No. de linhas */
#define	VNCOL	1		/* No. de colunas */

/*
 ******	Equivalencias para simplificar o uso ********************
 */
#define	t_iflag	 t_termio.c_iflag	/* Modos de Entrada */
#define	t_oflag	 t_termio.c_oflag	/* Modos de Saida */
#define	t_cflag	 t_termio.c_cflag	/* Modos de Controle */
#define	t_lflag	 t_termio.c_lflag	/* Modos locais */

#define	t_intr	 t_termio.c_cc[VINTR]	/* Interrupção (^C) */
#define	t_quit	 t_termio.c_cc[VQUIT]	/* Quit (FS) */
#define	t_erase	 t_termio.c_cc[VERASE]	/* Erase (DEL) */
#define	t_kill	 t_termio.c_cc[VKILL]	/* Kill (^U) */
#define	t_eof	 t_termio.c_cc[VEOF]	/* End of File (^D) */
#define	t_min	 t_termio.c_cc[VMIN]	/* No. minimo da car. para RAW */
#define	t_eol	 t_termio.c_cc[VEOL]	/* End of Line (Não usado) */
#define	t_time	 t_termio.c_cc[VTIME]	/* Tempo de Espera no modo RAW */
#define	t_res	 t_termio.c_cc[VRES]	/* (Reservado para uso futuro) */
#define	t_swtch	 t_termio.c_cc[SWTCH]	/* (Não Identificado) */
#define	t_page	 t_termio.c_cc[VPAGE]	/* Tamanho da Pagina */
#define	t_aerase t_termio.c_cc[VAERASE]	/* Outro Erase (BS) */
#define	t_retype t_termio.c_cc[VRETYPE]	/* Retype (^R) */
#define	t_word	 t_termio.c_cc[VWORD]	/* Apaga Palavra (^W) */
#define	t_color	 t_termio.c_cc[VCOLOR]	/* A cor dos caracteres na tela */

#define	t_nline	 t_termio.c_wc[VNLINE]	/* No. de linhas */
#define	t_ncol	 t_termio.c_wc[VNCOL]	/* No. de colunas */

/*
 ******	Estrutura "tty" *****************************************
 */
struct	tty
{
	LOCK	t_olock;	/* Lock para uso exclusivo da saida */
	LOCK	t_obusy;	/* Semaforo de Saida em ação */
	EVENT	t_inqnempty;	/* Evento Lista de Entrada não vazia */
	EVENT	t_outqnfull;	/* Evento Lista de Saida não cheia */

	EVENT	t_outqempty;	/* Evento Lista de Saida vazia */
	EVENT	t_outqstart;	/* Evento Lista de Saida esperando ^Q */
	EVENT	t_outqnempty;	/* Evento Lista de Saida não vazia (para pty) */
	char	t_attention_set; /* Execute o procedimento de attention */

	CHEAD	t_inq;		/* Lista de Entrada (raw ou cooked) */
	CHEAD	t_outq;		/* Lista de Saida */
	void	(*t_oproc) (TTY *, int); /* Rotina que inicia a saida */
	void	*t_addr;	/* Endereco do Dispositivo */

	dev_t	t_dev;		/* Codigo do Dispositivo */
	ushort	t_state;	/* Estado Interno da Linha */
	short	t_irq;		/* No. do IRQ */
	long	t_tgrp;		/* No. do grupo de Terminais */

	UPROC	*t_uproc;	/* Processo dono da linha (ATTENTION) */
	int	t_index;	/* Índice do descritor da linha (ATTENTION) */

	char	t_cpu;		/* CPU capaz de manipular esta linha */
	char	t_delimcnt;	/* No. de Delimitadores na t_inq */
	char	t_col;		/* Coluna de entrada */
	char	t_wcol;		/* Ultima coluna escrita pelo prog */

	char	t_lno;		/* No. da linha */
	char	t_canal;	/* Canal fisico */
	char	t_vterm;	/* Terminal virtual */
	char	t_escchar;	/* Caractere de escape de acento */

	char	*t_cvtb;	/* Endereco da tabela de conversão */
	TERMIO	t_termio;	/* Estrutura termio desta linha */
};

/*
 ******	Caracteres de controle default **************************
 */
#define	CERASE	0x7F	/* ^*  Delete */
#define	CAERASE	0x08	/* ^H: Backspace */
#define CBELL	0x07	/* ^G: Campainha */
#define	CEOT	0x04	/* ^D: Final de Arquivo */
#define	CESC	0x1B	/* \e: Escape */
#define	CKILL	0x15	/* ^U: Remove toda linha  */
#define	CQUIT	0x18	/* ^X: Quit */
#define CINTR	0x03	/* ^C: Cancela o Programa */
#define	CSTOP	0x13	/* ^S: Para a saida */
#define	CSTART	0x11	/* ^Q: Reinicia a Saida */
#define CRTYPE	0x12	/* ^R: Repete a Linha */
#define	CWORD	0x17	/* ^W: Apaga uma WORD */
#define	CFF	0x0C	/* ^L: Form-Feed */

/*
 ******	Valores para "c_iflag": Modos de entrada ****************
 */
#define	IGNBRK	0x0001	/* Ignore a Condição de BREAK */
#define	BRKINT	0x0002	/* Sinalize uma interrupção no caso de BREAK */
#define	IGNPAR	0x0004	/* Ignorar Caracteres com erro de Paridade */
#define	PARMRK	0x0008	/* Marcar Erros de Paridade */
#define	INPCK	0x0010	/* Habilitar Verificação de Paridade na Entrada */
#define	ISTRIP	0x0020	/* Mascara o Oitavo Bit */
#define	INLCR	0x0040	/* Mapeie NL em CR na entrada */
#define	IGNCR	0x0080	/* Ignore CR's */
#define	ICRNL	0x0100	/* Mapeie CR em NL */
#define	IUCLC	0x0200	/* Mapeie Maiusculas em minusculas */
#define	IXON	0x0400	/* Habilite XON/XOFF na Saida */
#define	IXANY	0x0800	/* Qualquer caracter recomeca a saida */
#define	IXOFF	0x1000	/* Habilite XON/XOFF na Entrada */


/*
 ******	Valores para "c_oflag": Modos de saída ******************
 */
#define	OPOST	0x0001	/* Processe a Saida (Saida Cooked) */
#define	OLCUC	0x0002	/* Mapeie Minusculo em Maiusculo */
#define	ONLCR	0x0004	/* Mapeie NL em CR-NL */
#define	OCRNL	0x0008	/* Mapeie CR em NL */
#define	ONOCR	0x0010	/* Evite CR na Coluna 0 */
#define	ONLRET	0x0020	/* NL executa a função de CR */
#define	OFILL	0x0040	/* Use caracteres de enchimento para atrasos */
#define	OFDEL	0x0080	/* O enchimento é DEL, em caso contrario NULL */
#define	NLDLY	0x0100	/* Atrasos do NL: */
#define	 NL0	0x0000	/*	Tipo 0 */ 
#define	 NL1	0x0100	/*	Tipo 1 */ 
#define	CRDLY	0x0600	/* Atrasos do CR: */
#define	 CR0	0x0000	/* 	Tipo 0 */
#define	 CR1	0x0200	/* 	Tipo 1 */
#define	 CR2	0x0400	/* 	Tipo 2 */
#define	 CR3	0x0600	/* 	Tipo 3 */
#define	TABDLY	0x1800	/* Atrasos do TAB: */ 
#define	 TAB0	0x0000	/* 	Tipo 0 */
#define	 TAB1	0x0800	/* 	Tipo 1 */
#define	 TAB2	0x1000	/* 	Tipo 2 */
#define	 TAB3	0x1800	/* 	Tipo 3: Converta em brancos */
#define	BSDLY	0x2000	/* Atrasos do BS: */ 
#define	 BS0	0x0000	/*	Tipo 0 */ 
#define	 BS1	0x2000	/*	Tipo 1 */ 
#define	VTDLY	0x4000	/* Atraso do VT: */ 
#define	 VT0	0x0000	/* 	Tipo 0 */
#define	 VT1	0x4000	/* 	Tipo 1 */
#define	FFDLY	0x8000	/* Atraso do FF: */ 
#define	 FF0	0x0000	/*	Tipo 0 */
#define	 FF1	0x8000	/*	Tipo 1 */

/*
 ******	Valores para "c_cflag": Modos de controle ***************
 */
#define	CBAUD	 0x000F	/* Velocidade da Linha: */
#define	 B0	 0x0000	/*	Desconecte */ 
#define	 B110	 0x0001	/*	  110 Baud */
#define	 B134	 0x0002	/*	  134.5 Baud */
#define	 B150	 0x0003	/*	  150 Baud */
#define	 B200	 0x0004	/*	  200 Baud */
#define	 B300	 0x0005	/*	  300 Baud */
#define	 B600	 0x0006	/*	  600 Baud */
#define	 B1200	 0x0007	/*	 1200 Baud */
#define	 B1800	 0x0008	/*	 1800 Baud */
#define	 B2400	 0x0009	/*	 2400 Baud */
#define	 B4800	 0x000A	/*	 4800 Baud */
#define	 B9600	 0x000B	/*	 9600 Baud */
#define	 B19200	 0x000C	/*	19200 Baud */
#define	 B38400	 0x000D	/*	38400 Baud */
#define	 B57600	 0x000E	/*	57600 Baud */
#define	 B115200 0x000F	/*     115200 Baud */
#define	CSIZE	 0x0030	/* Tamanho do Caracter: */
#define	 CS5	 0x0000	/*	5 Bits */ 
#define	 CS6	 0x0010	/*	6 Bits */ 
#define	 CS7	 0x0020	/*	7 Bits */ 
#define	 CS8	 0x0030	/*	8 Bits */ 
#define	CSTOPB	 0x0040	/* Envie 2 Bits de Stop, em caso contrario 1 */ 
#define	CREAD	 0x0080	/* Habilite a Leitura */ 
#define	PARENB	 0x0100	/* Habilite a Paridade */ 
#define	PARODD	 0x0200	/* Paridade Impar, em caso contrario par */ 
#define	HUPCL	 0x0400	/* Desconecte no ultimo close */
#define	CLOCAL	 0x0800	/* Linha local, em caso contrario discada */
#define	LOBLK	 0x1000	/* Block layer Output */

/*
 ******	Valores para "c_lflag": Modos locais ********************
 */
#define	ISIG	0x0001	/* Habilite Sinais */
#define	ICANON	0x0002	/* Processe a Entrada (entrada cooked) */
#define	XCASE	0x0004	/* Processamento de Maiu/Min com '\' */
#define	ECHO	0x0008	/* Habilite o eco */ 
#define	ECHOE	0x0010	/* Eçõe "erase" como BS-SP-BS */
#define	ECHOK	0x0020	/* Eçõe NL apos "kill" */ 
#define	ECHONL	0x0040	/* Eçõe NL */
#define	NOFLSH	0x0080	/* Deshabilite "flush" apos inter. ou quit */ 

#define	ISOKEY	0x0200	/* O Teclado gera caracteres ISO */
#define CODE	0x0C00	/* Codigo de Caracteres: */
#define	 ISO	0x0000	/* 	ISO */
#define	 ASCII	0x0400	/* 	ASCII */
#define	 USER1	0x0800	/* 	USER1 */
#define	 USER2	0x0C00	/* 	USER2 */
#define	CNTRLX	0x1000	/* Caracteres de controle na forma '^X' */
#define	VIDEO	0x2000	/* O terminal é de Video */
#define	ICOMM	0x8000	/* Modo de comunicações na entrada */

/*
 ******	Estado interno da linha *********************************
 */
#define	ISOPEN	   0x0001  /* Device is open */
#define	TTYSTOP	   0x0002  /* Output stopped by ctl-s */
#define	ESCAPE	   0x0004  /* acabou de ser lido um '\' */
#define	PAGING	   0x0008  /* Chegou no final de uma Pagina */
#define ONEINIT	   0x0010  /* Inicialize apenas uma vez */

/*
 ******	Definições diversas *************************************
 */
#define	SETEB	0x7F	/* Largura de 7 Bits */
#define	OITOB	0xFF	/* Largura de 8 Bits */
#define	DELIM	0xFF	/* Delimitador na inq */

/*
 ******	Mini-estrutura para o pseudo terminal *******************
 */
struct pty
{
	TTY	pty;			/* Estrutura TTY do pseudo */

	char	ptyc_nopen;		/* Contadores de OPENs */
	char	ptys_nopen;

	LOCK	ptyc_lock;		/* Para o "O_LOCK" */
	LOCK	ptys_lock;

	UPROC	*uproc;			/* Processo dono da linha (Attention) */
	schar	index;			/* Índice do descritor da linha (Attention) */
	char	attention_set;		/* Attention armada */
};

/*
 ****** Resultado da chamada ao sistema "getpty" ****************
 */
typedef struct	ptyio
{
	int	t_fd_client;		/* O descritor do lado cliente */
	int	t_fd_server;		/* O descritor do lado servidor */
	char	t_nm_client[16];	/* O nome do dispositivo cliente */
	char	t_nm_server[16];	/* O nome do dispositivo servidor */

}	PTYIO;

/*
 ******	Comandos IOCTL ******************************************
 */
#define	TCGETS			(('T'<< 8) | 0)
#define	TCSETS			(('T'<< 8) | 1)
#define	TCSETAW			(('T'<< 8) | 2)
#define	TCSETAF			(('T'<< 8) | 3)
#define	TCSBRK			(('T'<< 8) | 4)
#define	TCXONC			(('T'<< 8) | 5)
#define	TCFLSH			(('T'<< 8) | 6)
#define	TCLOCK			(('T'<< 8) | 7)
#define	TCFREE			(('T'<< 8) | 8)

#define	TCNREAD			(('T'<< 8) | 10)
#define	TCISATTY		(('T'<< 8) | 11)
#define	TCGETCTS		(('T'<< 8) | 12)
#define	TCSETNR			(('T'<< 8) | 13)
#define	TCSETRTS		(('T'<< 8) | 14)
#define	TCRESETRTS		(('T'<< 8) | 15)
#define	TCINTERNET		(('T'<< 8) | 16)
#define	TCFIFOSZ		(('T'<< 8) | 17)
#define	TC_IS_ETHERNET		(('T'<< 8) | 18)
#define	TC_GET_ETHADDR		(('T'<< 8) | 19)
#define	TCPPPINT		(('T'<< 8) | 20)
#define	TC_KERNEL_PTY_ON	(('T'<< 8) | 21)
#define	TC_KERNEL_PTY_OFF	(('T'<< 8) | 22)
#define	TC_RESTORE_VGA		(('T'<< 8) | 23)

/*
 ******	Macros **************************************************
 */
#define	ttyecho(c, tp)		if (lflag & ECHO)	\
					ttyout (c, tp)
