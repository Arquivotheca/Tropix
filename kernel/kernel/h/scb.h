/*
 ****************************************************************
 *								*
 *			<sys/scb.h>				*
 *								*
 *	System Control Block: 					*
 *	    Contem os parâmetros atuais do sistema		*
 *								*
 *	Versão	3.0.0, de 01.09.94				*
 *		4.8.0, de 08.09.05				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *		/usr/include/sys				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2005 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#define	SCB_H			/* Para definir os protótipos */

/*
 ******	A estrutura *********************************************
 */
struct	scb
{
	/*
	 *	Informações sobre as CPUs
	 */
	long	y_cputype;	/* Tipo da CPU (386, 486 ou 586) */
	int	y_ncpu;		/* No. de CPUs ativas */
	long	y_initpid;	/* Pid do INIT */

	/*
	 *	Tabela de processos
	 */
	UPROCV	*y_uproc;	/* Início da tabela */
	UPROCV	*y_lastuproc;	/* Última entrada usada */
	UPROCV	*y_enduproc;	/* Final da tabela */
	int	y_nproc;	/* No. de entradas */

	/*
	 *	Tabela de HASH dos processos
	 */
	PHASHTB	*y_phash;	/* Início da tabela */
	PHASHTB	*y_endphash;	/* Final da tabela */
	int	y_nphash;	/* No. de entradas */
	int	y_phmask;	/* Máscara para o cálculo do hash */

	/*
	 *	Tabela de INODEs
	 */
	INODE	*y_inode;	/* Começo da tabela */
	INODE	*y_endinode;	/* Final da tabela */
	int	y_ninode;	/* No. de entradas */

	/*
	 *	Tabela de HASH dos INODEs
	 */
	IHASHTB	*y_ihash;	/* Início da tabela */
	IHASHTB	*y_endihash;	/* Final da tabela */
	int	y_nihash;	/* No. de entradas */

	/*
	 *	Tabela de BUFHEADs e BUFFERs 
	 */
	BHEAD	*y_bhead;	/* Início dos BHEADs */
	BHEAD	*y_endbhead;	/* Final  dos BHEADs */
	char	*y_buf0;	/* Início do buffers 0 */
	char	*y_endbuf0;	/* Final do buffers 0 */
	char	*y_buf1;	/* Início do buffers 1 */
	char	*y_endbuf1;	/* Final do buffers 1 */
	int	y_nbuf;		/* No. de BUFFERs    */

	/*
	 *	Tabela de HASH dos BHEADs
	 */
	BHASHTB	*y_bhash;	/* Início da tabela */
	BHASHTB	*y_endbhash;	/* Final da tabela */
	int	y_nbhash;	/* No. de entradas */

	/*
	 *	Tabela de KFILEs
	 */
	KFILE	*y_kfile;	/* Começo da tabela */
	KFILE	*y_endkfile;	/* Final da tabela */
	int	y_nkfile;	/* No. de entradas */

	/*
	 *	Tabela de CLISTs
	 */
	CBLK	*y_clist;	/* Começo da tabela */
	CBLK	*y_endclist;	/* Final da tabela */
	int	y_nclist;	/* No. de entradas */

	/*
	 *	Tabela de alocação da memória interna & disco (swap)
	 */
	MAP	*y_map;		/* Início da MAP */
	MAP	*y_endmap;	/* Final  da MAP */
	int	y_nmap;		/* No. de entradas */

	/*
	 *	Tabela de HASH dos símbolos das bibliotecas compartilhadas
	 */
	void	*y_shlib_hash;	/* Início da SHLIB_HASH */
	void	*y_end_shlib_hash; /* Final da SHLIB_HASH */
	int	y_n_shlib_hash;	/* No. de entrada da SHLIB_HASH */

	/*
	 *	Tabela de TIMEOUT
	 */
	TIMEOUT	*y_timeout;	/* Início da tabela */ 
	TIMEOUT	*y_endtimeout;	/* Final da tabela */
	int	y_ntout;	/* No. de entradas por CPU */

	/*
	 *	Tabela de LOCKF
	 */
	LOCKF	*y_lockf;	/* Início da tabela */ 
	LOCKF	*y_endlockf;	/* Final da tabela */
	int	y_nlockf;	/* No. de entradas */

	/*
	 *	Tabela de UEVENTLs
	 */
	UEVENTL	*y_ueventl;	/* Começo da tabela */
	UEVENTL	*y_endueventl;	/* Final da tabela */
	int	y_nueventl;	/* No. de entradas */

	/*
	 *	Tabela de UEVENTs
	 */
	UEVENTG	*y_ueventg;	/* Começo da tabela */
	UEVENTG	*y_endueventg;	/* Final da tabela */
	int	y_nueventg;	/* No. de entradas */

	/*
	 *	Tabela de USEMALs
	 */
	USEMAL	*y_usemal;	/* Começo da tabela */
	USEMAL	*y_endusemal;	/* Final da tabela */
	int	y_nusemal;	/* No. de entradas */

	/*
	 *	Tabela de USEMAGs
	 */
	USEMAG	*y_usemag;	/* Começo da tabela */
	USEMAG	*y_endusemag;	/* Final da tabela */
	int	y_nusemag;	/* No. de entradas */

	/*
	 *	Tabela de REGIONLs
	 */
	REGIONL	*y_regionl;	/* Começo da tabela */
	REGIONL	*y_endregionl;	/* Final da tabela */
	int	y_nregionl;	/* No. de entradas */

	/*
	 *	Tabela de REGIONGs
	 */
	REGIONG	*y_regiong;	/* Começo da tabela */
	REGIONG	*y_endregiong;	/* Final da tabela */
	int	y_nregiong;	/* No. de entradas */

	/*
	 *	Tabela do DOMAIN NAME SYSTEM
	 */
	DNS	*y_dns;		/* Começo da tabela */
	DNS	*y_end_dns;	/* Final da tabela */
	int	y_n_dns;	/* No. de entradas */

	/*
	 *	Tabela de Roteamento da INTERNET
	 */
	ROUTE	*y_route;	/* Começo da tabela */
	ROUTE	*y_end_route;	/* Final da tabela */
	int	y_n_route;	/* No. de entradas */

	/*
	 *	Tabela de endereços ETHERNET
	 */
	ETHER	*y_ether;	/* Começo da tabela */
	ETHER	*y_end_ether;	/* Final da tabela */
	int	y_n_ether;	/* No. de entradas */
	ETHER	*y_ether_first;	/* Começo da lista */

	/*
	 *	Tabela de blocos para datagramas da INTERNET
	 */
	ITBLOCK	*y_itblock;	/* Começo da tabela */
	ITBLOCK	*y_end_itblock;	/* Final da tabela */
	int	y_n_itblock;	/* No. de entradas */

	/*
	 *	ENDPOINTs RAW da INTERNET
	 */
	RAW_EP	*y_raw_ep;	/* Começo da tabela */
	RAW_EP	*y_end_raw_ep;	/* Final da tabela */
	int	y_n_raw_ep;	/* No. de entradas */

	/*
	 *	ENDPOINTs UDP da INTERNET
	 */
	UDP_EP	*y_udp_ep;	/* Começo da tabela */
	UDP_EP	*y_end_udp_ep;	/* Final da tabela */
	int	y_n_udp_ep;	/* No. de entradas */

	/*
	 *	ENDPOINTs TCP da INTERNET
	 */
	TCP_EP	*y_tcp_ep;	/* Começo da tabela */
	TCP_EP	*y_end_tcp_ep;	/* Final da tabela */
	int	y_n_tcp_ep;	/* No. de entradas */

	/*
	 *	Tabela de Pseudo Terminais
	 */
	PTY	*y_pty;		/* Começo da tabela */
	PTY	*y_end_pty;	/* Final da tabela */
	int	y_npty;		/* No. de entradas */

	/*
	 *	Tabela do EXPORT
	 */
	EXPORT	*y_export;	/* Começo da tabela */
	EXPORT	*y_end_export;	/* Final da tabela */
	int	y_n_export;	/* No. de entradas */

	/*
	 *	Reserva para novas tabelas
	 */
	int	y_tb_res[8*3];	/* Para cada TB 3 ints */

	/*
	 *	Começo e final das tabelas
	 */
	void	*y_tb0;		/* Início das tabelas 0 */
	void	*y_endtb0;	/* Final  das tabelas 0 */
	void	*y_tb1;		/* Início das tabelas 1 */
	void	*y_endtb1;	/* Final  das tabelas 1 */

	/*
	 *	Definição dos dispositivos
	 */
	dev_t	y_rootdev;	/* Dispositivo do ROOT */
	dev_t	y_pipedev;	/* Dispositivo do PIPE */
	dev_t	y_swapdev;	/* Dispositivo do SWAP */

	char	y_sh_nm[32];	/* Nome da "sh" */

	/*
	 *	Variáveis globais do SWAP
	 */
	daddr_t	y_swaplow;	/* Bloco Inicial do SWAP */
	int	y_nswap;	/* Tamanho em Blocos do SWAP */

	/*
	 *	No. de unidades físicas
	 */
	int	y_nblkdev;	/* No. de BLK */
	int	y_nchrdev;	/* No. de CHR */

	/*
	 *	Superblocos
	 */
	V7SB	*y_rootsb;	/* Início da Lista de SBs */
	int	y_nmount;	/* No. maximo de MOUNTs */

	/*
	 *	Memória física
	 */
	void	*y_endpmem;	/* Final da memória física */

	/*
	 *	Começo e final da memória para os processos (PÁGINAS)
	 */
	pg_t	y_ucore0;	/* Início da Memória 0 */
	pg_t	y_enducore0;	/* Final  da Memória 0 */
	pg_t	y_ucore1;	/* Início da Memória 1 */
	pg_t	y_enducore1;	/* Final  da Memória 1 */

	/*
	 *	Começo e final dos discos em memória
	 */
	void	*y_ramd0;	/* Início do disco */
	void	*y_endramd0;	/* Final  do disco */
	long	y_ramd0sz;	/* No. de bytes do RAM disk */

	void	*y_ramd1;	/* Início do disco */
	void	*y_endramd1;	/* Final  do disco */
	long	y_ramd1sz;	/* No. de bytes do RAM disk */

	/*
	 *	Parametros de tempo
	 */
	int	y_hz;		/* Freqüência (ticks por segundo) */

	/*
	 *	Parâmetros do SCHEDULER
	 */
	int	y_tslice;	/* Time-slice em ticks do relógio */
	int	y_incpri;	/* Incremento da prioridade a cada tick */
	int	y_decpri;	/* Decremento da prioridade a cada tick */
	int	y_ttyinc;	/* Incremento para entrada de TTY */
	int	y_nfactor;	/* Influência do NICE */
	int	y_reser1[4];

	/*
	 *	Parâmetros dos processos do usuário
	 */
	int	y_maxppu;	/* No. máximo de proc. por usuário */
	pg_t	y_maxpsz;	/* Tamanho máximo de um Proc. de usuário */
	int	y_umask;	/* Mascara de criação de arquivos */
	long	y_stksz;	/* Tamanho inicial da stack (bytes) */
	long	y_stkincr;	/* Incremento da stack (bytes) */
	int	y_ncexec;	/* No. max. de chars. do Exec */

	/*
	 *	Outros parametros
	 */
	int	y_pgsz;		/* Tamanho (em bytes) da PG */
	void	*y_idle_pattern; /* Tabela de padrão para IDLE */
	void	*y_intr_pattern; /* Tabela de padrão para INTR */
	char	y_preemption_mask[8];	/* Troca de proc. em modo SUP */

	/*
	 *	Parâmetros da atualização do "cache" dos dispositivos
	 */
	int	y_sync_time;	/* Período de atualização de SB e INODEs */
	int	y_block_time;	/* Período de atualização de blocos */
	int	y_dirty_time;	/* Tempo limite para atualizar um bloco */
	int	y_max_flush;	/* No. máximo de blocos atualizados por ciclo */

	/*
	 *	Tabela de Portas/interrupções das linhas seriais 
	 */
	ushort	y_sio_port[8];	/* No. das portas */
	char	y_sio_irq[8];	/* No. dos IRQs */

	/*
	 *	Tabela de Portas/interrupções das saídas paralelas
	 */
	ushort	y_lp_port[4];	/* No. das portas */
	char	y_lp_irq[4];	/* No. dos IRQs */

	/*
	 *	Tabela de Portas/interrupções das unidades 3Com 503
	 */
	ushort	y_ed_port[4];	/* No. das portas */
	char	y_ed_irq[4];	/* No. dos IRQs */

	/*
	 *	Estrutura UTSNAME: Informações sobre o systema
	 */
	struct	uts
	{
		char	uts_sysname[16];	/* Nome do sistema */
		char	uts_systype[16];	/* Tipo do sistema */
		char	uts_nodename[16];	/* Nome do nó */
		char	uts_version[16];	/* Versão */
		char	uts_date[16];		/* Data */
		char	uts_time[16];		/* Tempo */
		char	uts_machine[16];	/* Computador */
		char	uts_customer[16];	/* Nome do cliente */
		char	uts_depto[16];		/* Nome do departamento */
		char	uts_sysserno[16];	/* Numero de série do sistema */
		char	uts_res[4][16];		/* Reservado para uso futuro */

	}	y_uts;

	/*
	 *	Variáveis diversas
	 */
	char	y_std_boot;		/* Usa o "boot" sem alterar nada */
	char	y_fpu_enabled;		/* Unidade de ponto flutuante ativada */
	char	y_CSW;			/* Para DEBUG */
	char	y_ut_clock;		/* O relógio CMOS é UT (GMT) */

	int	y_tzmin;		/* Fuso horário, em minutos */
	int	y_screen_saver_time;	/* Tempo do protetor de tela */
	int	y_DELAY_value;		/* Para 1 micro-segundo */

	char	y_boot_verbose;		/* Escreve informações sobre o BOOT */
	char	y_ignore_LEDs;		/* Não atualiza os LEDs do teclado */
	char	y_keyboard;		/* Tipo do teclado => 0: US, 1: ABNT2 */
	char	y_tagenable;		/* Permite o uso de "tag queueing" */

	char	y_dma_enable;		/* Permite o uso de DMA em dispositivos ATA */
	char	y_n_nfs_daemon;		/* Número de "daemon"s do servidor NFS */

	char	y_reser2[2];

	int	y_dmesg_sz;		/* Tamanho da área para "dmesg" (printf) */

	/*
	 *	Tabela de controladores USB
	 */
	char	y_usb_enable[16];	/* Habilita/desabilita cada controladora */

	/*
	 *	Semi constantes do sistema
	 */
	off_t	y_SYS_ADDR;		/* Endereço virtual do início do núcleo */
	off_t	y_UPROC_ADDR;		/* Endereço virtual do início da UPROC */

	off_t	y_USER_TEXT_ADDR;	/* Endereço virtual recomendado para início do texto do usuário */
	off_t	y_USER_DATA_ADDR;	/* Endereço virtual recomendado para início do texto do usuário */

	pg_t	y_USIZE;		/* Tamanho (em pgs) da UPROC */

	int	y_NUFILE;		/* No. max. de arq. abertos por processo */

	int	y_NCON;			/* No. de consoles virtuais */
	int	y_NSIO;			/* No. de linhas seriais */

	/*
	 *	Reserva para completar 1536 bytes
	 */
	int	y_etc_res[125];
};
