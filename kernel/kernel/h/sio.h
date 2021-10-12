/*
 ****************************************************************
 *								*
 *			<sys/sio.h>				*
 *								*
 *	Definições para linhas seriais com NS16450 ou NS16550	*
 *								*
 *	Versão	3.0.0, de 12.02.95				*
 *		4.4.0, de 30.11.02				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *		/usr/include/sys				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#define	SIO_H			/* Para definir os protótipos */

/*
 ******	Definições globais **************************************
 */
#define PL	4		/* Prioridade de interrupção */
#define splsio	spl4		/* Função de prioridade de interrupção */

#define	HASFIFO	0x8000		/* Tem FIFO: Usado em t_state */

/*
 ******	Registros do dispositivo ********************************
 */
#define SIO_DATA_PORT		(port + 0)	/* com_data */
#define SIO_DIV_LATCH_LOW_PORT	(port + 0)	/* com_dlbl */
#define SIO_DIV_LATCH_HIGH_PORT	(port + 1)	/* com_dlbh */
#define SIO_INT_ENABLE_REG_PORT	(port + 1)	/* com_ier */
#define SIO_INT_ID_PORT		(port + 2)	/* com_iir */
#define SIO_FIFO_CTL_PORT	(port + 2)	/* com_fifo */
#define SIO_LINE_CTL_REG_PORT	(port + 3)	/* com_cfcr */
#define SIO_MODEM_CTL_PORT	(port + 4)	/* com_mcr */
#define SIO_LINE_STATUS_PORT	(port + 5)	/* com_lsr */
#define SIO_MODEM_STATUS_PORT	(port + 6)	/* com_msr */

/*
 ******	 Interrupt enable register ******************************
 */
#define	IER_ERXRDY	0x01	/* Data available interrupt */
#define	IER_ETXRDY	0x02	/* Transm. holding reg. interrupt */
#define	IER_ERLS	0x04	/* Line status interrupt */
#define	IER_EMSC	0x08	/* Modem status change interrupt */

/*
 ******	Interrupt identification register ***********************
 */
#define	IIR_IMASK	0x0F	/* Bits de identificação da interrrupção */
#define	IIR_RXTOUT	0x0C	/* Data available + 16550 timeout */
#define	IIR_RLS		0x06	/* Line status change */
#define	IIR_RXRDY	0x04	/* Data available */
#define	IIR_TXRDY	0x02	/* Transmiter reg. empty */
#define	IIR_NOPEND	0x01	/* No interrupt pending */
#define	IIR_MLSC	0x00	/* Modem status change */
#define	IIR_FIFO_MASK	0xC0	/* set if FIFOs are enabled */

/*
 ******	Fifo control register ***********************************
 */
#define	FIFO_ENABLE	0x01	/* Enable clear XMT & RCV FIFO queue */
#define	FIFO_RCV_RST	0x02	/* Clear RCV FIFO */
#define	FIFO_XMT_RST	0x04	/* Clear XMT FIFO */
#define	FIFO_DMA_MODE	0x08	/* Change RYDs pins from mode 0 to 1 */
#define	FIFO_TRIGGER_1	0x00	/* Trigger levels for RCV FIFO intr */
#define	FIFO_TRIGGER_4	0x40
#define	FIFO_TRIGGER_8	0x80
#define	FIFO_TRIGGER_14	0xC0

/*
 ******	Character format control register ***********************
 */
#define	CFCR_DLAB	0x80	/* Seleciona DIV_LATCH_PORT */
#define	CFCR_SBREAK	0x40	/* Force spacing break state */
#define	CFCR_PONE	0x20	/* Enable parity */
#define	CFCR_PEVEN	0x10	/* Paridade par */
#define	CFCR_PODD	0x00	/* Paridade ímpar */
#define	CFCR_PENAB	0x08	/* Liga a paridade */
#define	CFCR_STOPB	0x04	/* 1.5 ou 2 bits de STOP */
#define	CFCR_8BITS	0x03	/* Tamanho do caractere */
#define	CFCR_7BITS	0x02
#define	CFCR_6BITS	0x01
#define	CFCR_5BITS	0x00

/*
 ******	Modem control register **********************************
 */
#define	MCR_LOOPBACK	0x10	/* Modo interno */
#define	MCR_IENABLE	0x08	/* OUT2 */
#define	MCR_DRS		0x04	/* OUT1 */
#define	MCR_RTS		0x02	/* Sinal RTS */
#define	MCR_DTR		0x01	/* Sinal DTR */

/*
 ******	Line status register ************************************
 */
#define	LSR_RCV_FIFO	0x80	/* 16550: Erros na fila do FIFO */
#define	LSR_TSRE	0x40	/* TSRE (trans. shift reg.) vazio */
#define	LSR_TXRDY	0x20	/* THRE (trans. holding reg.) vazio */
#define	LSR_BI		0x10	/* Break interrupt */
#define	LSR_FE		0x08	/* Framing error */
#define	LSR_PE		0x04	/* Parity error */
#define	LSR_OE		0x02	/* Overrun error */
#define	LSR_RXRDY	0x01	/* Data error */
#define	LSR_RCV_MASK	0x1F	/* OUtório de dado presente */

/*
 ******	Modem status register ***********************************
 */
#define	MSR_DCD		0x80	/* Para loopback test */
#define	MSR_RI		0x40	/* Ring */
#define	MSR_DSR		0x20	/* DSR */
#define	MSR_CTS		0x10	/* CTS */
#define	MSR_DDCD	0x08	/* Delta DCD */
#define	MSR_TERI	0x04	/* Ring indicator changed */
#define	MSR_DDSR	0x02	/* Delta DSR */
#define	MSR_DCTS	0x01	/* Delta CTS */

/*
 ******	Tabela de conversão das velocidades *********************
 */
#define	COMBRD(x)	(1843200 / (16 * (x)))

extern const int sio_speed_cvtb[];

/*
 ******	Variáveis globais ***************************************
 */
extern char	sio_lock[];		/* Para o "O_LOCK" */

extern char	sio_nopen[];		/* Para evitar conflito com "slip" */

/*
 ******	Tabela configuração das unidades ************************
 */
typedef struct
{
	char	sio_unit_alive;	/* A unidade existe */
	char	sio_has_fifo;	/* A unidade tem FIFO */
	char	sio_fifo_level;	/* Nível do FIFO em uso */

}	SIO_CONFIG;

extern SIO_CONFIG sio_config[];
