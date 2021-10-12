/*
 ****************************************************************
 *								*
 *			extern.h				*
 *								*
 *	Declaração das váriáveis externas			*
 *								*
 *	Versão	3.0.0, de 30.08.94				*
 *		4.9.0, de 10.05.07				*
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
 ******	Variáveis externas globais ******************************
 */
extern char		etext;			/* Final do texto */
extern char		end;			/* Final do BSS (Inicio da SYMTB) */
extern int		video_line;		/* Para controlar a tela cheia */
extern char		libt_nm[];		/* Nome da biblioteca "libt.o" */
extern int		nmount;			/* Número de dispositivos montados */
extern const char	lowertoupper[];		/* Tabela de conversão ISO */
extern const char	uppertolower[];		/* Tabela de conversão ISO */
extern const char	* const sys_errlist[];	/* Lista das mensagens de erro */
extern ulong		pl_mask[];		/* Máscara dos níveis para interrupções */
extern int		irq_pl_vec[];		/* Níveis definitivos dos IRQs */
extern char		CSW;			/* Código da depuração (para ASM) */
extern const char	*file_code_name_vec[];	/* Códigos dos tipos de sistema de arquivos */
extern const int	sys_nerr;		/* O número de mensagens de erro */
extern char		*domain_name;		/* Nome do domain */
extern const char	video_8x16_tb[];	/* A Fonte de caracteres 8x16 */
extern int		idle_intr_active;	/* Ativa as "lâmpadas" */

extern char		*dmesg_area;		/* Área de mensagens */
extern char		*dmesg_ptr;
extern const char	*dmesg_end;

extern int		usb_busy;		/* Para verificar o uso */
extern int		usb_active;		/* Controladoras ativas */

enum SCREEN_SAVER_STATE { SCREEN_SAVER_IDLE, SCREEN_SAVER_ACTIVE };	/* Estado do protetor de tela */

extern enum SCREEN_SAVER_STATE	screen_saver_state;	/* Estado do protetor de tela */

/*
 ******	Variáveis definidas no módulo "boot.s" ******************
 */
extern char		bootcode[];		/* Endereço do código do BOOT */
extern int		bcodesz;		/* Tamanho do código do BOOT */

/*
 ******	Variáveis definidas no módulo "dispat.c" ****************
 */
extern short		bestpri;		/* Prioridade da cabeça da "corerdylist" */

/*
 ******	Variáveis definidas no módulo "kinit.c" *****************
 */
extern char		fpu_present;		/* Inicializado no fpu.s */
extern int		tick_utime;		/* Tempo de um tick em microsegundos */
extern int		step_utime_16;		/* Tempo de uma contagem do PIT << 16 */
extern int		PIT_init;		/* Valor inicial do contador do PIT */

/*
 ******	Variáveis definidas no módulo "main.c" ******************
 */
extern char		preemption_flag[];	 /* Habilita troca de proc. em modo SUP */

/*
 ******	Variáveis definidas no módulo "clock.c" *****************
 */
extern int		hz;			/* Contador de interrupções do relógio */
extern time_t		time;			/* Tempo em segundos desde o BIG BANG */
extern time_t		uptime;			/* Tempo em segundos desde o BOOT */
extern int		PIT_val_proc;		/* Valor do PIT próximo ao final do */
						/* processamento da interrupção do relógio */

/*
 ******	Variáveis definidas no módulo "con.c" *******************
 */
extern const char	iso_to_pc_tb[];

/*
 ******	Variáveis definidas no módulo "boot.s" *******************
 */
extern char		init_nm[];		/* Nome do INIT */

/*
 ******	Variáveis definidas no módulo "excep.s" *****************
 */
extern void		syscall_vector ();

/*
 ******	Variáveis definidas no módulo "start.s" *****************
 */
extern void		*end_base_bss;		/* Final da área baixa alocada */
extern void		*end_ext_bss;		/* Final da área alta alocada */
extern int		phys_mem;		/* Memória física total (KB) */
extern int		cputype;		/* Tipo da CPU */
#ifdef	UPROC_H
extern UPROC		*uproc_0;		/* Endereço da PPDA do proc 0 */
#endif	UPROC_H

/*
 ******	Variáveis utilizando definições de "bcb.h" **************
 */
#ifdef	BCB_H
extern BCB		bcb;			/* A estrutura herdada do BOOT */
#endif	BCB_H

#if (0)	/*******************************************************/
/*
 ******	Variáveis utilizando definições de "bhead.h" ************
 */
#ifdef	BHEAD_H
extern BHEAD		*bclock;		/* Ponteiro do algoritmo do CLOCK */
#endif	BHEAD_H
#endif	/*******************************************************/

/*
 ******	Variáveis utilizando definições de "disktb.h" ***********
 */
#ifdef	DISKTB_H
extern DISKTB		*disktb;		/* A tabela de discos */
extern DISKTB		*next_disktb;		/* A próxima entrada vaga da tabela */
extern DISKTB		*end_disktb;		/* O final da tabela de discos */
#endif	DISKTB_H

/*
 ******	Variáveis utilizando definições de "inode.h" ************
 */
#ifdef	INODE_H
extern INODE		*rootdir;		/* Diretorio da RAIZ Universal */
extern INODE		*iclock;		/* INODE do Algoritmo do CLOCK */
extern	const FSTAB	fstab[];		/* Sistemas de arquivos */
#endif	INODE_H

/*
 ******	Variáveis utilizando definições de "iotab.h" **********
 */
#ifdef	IOTAB_H
extern	const BIOTAB	biotab[];		/* Dispositivo de blocos */
extern	const CIOTAB	ciotab[];		/* Dispositivo de caracteres */
#endif	IOTAB_H

/*
 ******	Variáveis utilizando definições de "ipc.h" ************
 */
#ifdef	IPC_H
extern LOCK		ueventllock;		/* Lista local de eventos */
extern LOCK		ueventglock;		/* Lista global de eventos */
extern LOCK		usemallock;		/* Lista local de semáforos */
extern LOCK		usemaglock;		/* Lista global de semáforos */
extern LOCK		regionllock;		/* Lista local de regiões */
extern LOCK		regionglock;		/* Lista global de regiões */
extern UEVENTG		*ueventgfreelist;	/* Lista livre de UEVENTG */
extern USEMAG		*usemagfreelist;	/* Lista livre de UEVENTG */
extern REGIONG		*regiongfreelist;	/* Lista livre de REGIONG */
extern int		regiong_list_count;	/* Contador de REGIONG usados */
#endif	IPC_H

/*
 ******	Variáveis utilizando definições de "itnet.h" **********
 */
#ifdef	ITNET_H
extern ITSCB		itscb;			/* O bloco de contrôle */
extern const ETHADDR	ether_broadcast_addr;
extern EVENT		dns_server_table_not_empty_event;
#endif	ITNET_H

/*
 ******	Variáveis utilizando definições de "kcntl.h" ************
 */
#ifdef	KCNTL_H
extern const PARTNM	partnm[];
#endif	KCNTL_H

/*
 ******	Variáveis utilizando definições de "mmu.h" **************
 */
#ifdef	MMU_H
extern mmu_t		*kernel_page_directory;	 /* Diretório das tabela de pgs. */
extern mmu_t		*extra_page_table;	 /* Diretório das tabela de pgs. */
#endif	MMU_H

extern char		mmu_dirty[];		/* Indica que deve recarregar a MMU */

/*
 ******	Variáveis utilizando definições de "sb.h" ***************
 */
#ifdef	SB_H
extern SB		sb_head;		/* Cabeça da lista */
#endif	SB_H

/*
 ******	Variáveis utilizando definições de "scb.h" **************
 */
#ifdef	SCB_H
extern SCB		scb;			/* O bloco de contrôle do sistema */
#endif	SCB_H

/*
 ******	Variáveis utilizando definições de "sync.h" *************
 */
#ifdef	SYNC_H
extern LOCK		coremlock;		/* Lock da Tabela da Memoria */
extern LOCK		diskmlock;		/* Lock da Tabela do Disco */
extern EVENT		every_second_event;	/* Evento de segundo em segundo */
extern LOCK		iclocklock;		/* LOCK do algoritmo do CLOCK */
extern LOCK		sblistlock;		/* LOCK para a Lista de SB's */
extern LOCK		updlock;		/* LOCK de Operação de Update */
extern LOCK		corerdylist_lock;
extern EVENT		usb_explore_event;	/* Evento de exploração do USB */
#endif	SYNC_H

/*
 ******	Variáveis utilizando definições de "sysent.h" ***********
 */
#ifdef	SYSENT_H
extern const SYSENT	sysent[];		/* Tabela de chamadas ao sistema */
#endif	SYSENT_H

/*
 ******	Variáveis utilizando definições de "seg.h" **************
 */
#ifdef	SEG_H
extern EXCEP_DC		gdt[];			/* A tabela GDT */
#endif	SEG_H

/*
 ******	Variáveis utilizando definições de "tty.h" **************
 */
#ifdef	TTY_H
extern int		TTYMAXQ;		/* Limite para parar o processo */
extern int		TTYMINQ;		/* Limite para reiniciar o processo */
extern int		TTYPANIC;		/* Limite para abandonar toda a fila */
extern int		TTYCOMMPANIC;		/* Idem, para o modo de comunicações */

extern LOCK		cfreelock;		/* Lock da Lista livre */
extern CBLK		*cfreelist;		/* Comeco da lista Livre */
extern ushort		chartb[];
extern char		isotoisotb[];		/* Tabelas de conversão */
extern char		isotoatb[];
extern char		isotou1tb[];
extern char		isotou2tb[];

extern TTY		*kernel_pty_tp;		/* Para enviar mensagens do KERNEL */
#endif	TTY_H

/*
 ******	Variáveis utilizando definições de "uproc.h" **************
 */
#ifdef	UPROC_H
extern UPROC		u;			/* A extensão da tabela de processos */
extern UPROC		*corerdylist;		/* Cabeca da corerdylist */
extern UPROCV		*uprocfreelist;		/* Lista das Entradas UPROC não usadas */
extern LOCK		uprocfreelist_lock;
#endif	UPROC_H

/*
 ******	Variáveis utilizando definições de "utsname.h" **********
 */
#ifdef	UTSNAME_H
extern UTSNAME		uts;			/* A estrutura de identificação do sistema */
#endif	UTSNAME_H

/*
 ******	Variáveis utilizando definições de "video.h" ************
 */
#ifdef	VIDEO_H
extern VIDEODATA	*video_data;		/* A tabela de video */
extern VIDEODISPLAY	*curr_video_display;
#endif	VIDEO_H

/*
 ******	Variáveis utilizando definições de "mon.h" **************
 */
#ifdef	MON_H
extern MON		mon;			/* A estrutura de monitoração */
#endif	MON_H
