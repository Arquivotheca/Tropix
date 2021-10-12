/*
 ****************************************************************
 *								*
 *			aic7xxx.h				*
 *								*
 *	Reconhecimento das placas SCSI				*
 *								*
 *	Versão	4.0.0, de 19.03.01				*
 *		4.0.0, de 19.03.01				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 *		Baseado no FreeBSD 4.2				*
 *								*
 ****************************************************************
 */

/*
 ******	Parâmetros sintonizáveis ********************************
 */
#define	NAHC		2
#define NTARGET		16	/* 16 alvos por unidade */

#define	NUM_TRY		6	/* Número de tentativas */

#define PL		3	/* Prioridade de interrupção */
#define splahc		spl3

#undef	AHC_TARGET_MODE

#define TRUE 1
#define FALSE 0

#define NUM_ELEMENTS(array) (sizeof(array) / sizeof(*array))

#define ALL_TARGETS_MASK 0xFFFF


/**************************** Driver Constants ********************************/
/*
 * The maximum number of supported luns.
 * Although the identify message only supports 64 luns in SPI3, you
 * can have 2^64 luns when information unit transfers are enabled.
 * The max we can do sanely given the 8bit nature of the RISC engine
 * on these chips is 256.
 */
#define AHC_NUM_LUNS 256

/*
 * The maximum number of concurrent transactions supported per driver instance.
 * Sequencer Control Blocks (SCBs) store per-transaction information.  Although
 * the space for SCBs on the host adapter varies by model, the driver will
 * page the SCBs between host and controller memory as needed.  We are limited
 * to 255 because of the 8bit nature of the RISC engine and the need to
 * reserve the value of 255 as a "No Transaction" value.
 */
#define AHC_SCB_MAX	255

/* Reset line assertion time in us */

#define AHC_BUSRESET_DELAY	250

/*
 ******	Tipo do "chip" ******************************************
 */
typedef enum
{
	AHC_NONE	= 0x0000,
	AHC_CHIPID_MASK	= 0x00FF,

	AHC_AIC7770	= 0x0001,	/* Menos sofisticado */
	AHC_AIC7850	= 0x0002,
	AHC_AIC7855	= 0x0003,
	AHC_AIC7859	= 0x0004,
	AHC_AIC7860	= 0x0005,
	AHC_AIC7870	= 0x0006,
	AHC_AIC7880	= 0x0007,
	AHC_AIC7895	= 0x0008,
	AHC_AIC7895C	= 0x0009,
	AHC_AIC7890	= 0x000A,
	AHC_AIC7896	= 0x000B,
	AHC_AIC7892	= 0x000C,
	AHC_AIC7899	= 0x000D,	/* Mais sofisticado */

	AHC_VL		= 0x0100,	/* Bus type VL */
	AHC_EISA	= 0x0200,	/* Bus type EISA */
	AHC_PCI		= 0x0400,	/* Bus type PCI */
	AHC_BUS_MASK	= 0x0F00

}	ahc_chip;

/*
 ******	Capacidades de cada "chip" ******************************
 */
typedef enum
{
	AHC_FENONE	= 0x00000,
	AHC_ULTRA	= 0x00001,	/* Supports 20MHz Transfers */
	AHC_ULTRA2	= 0x00002,	/* Supports 40MHz Transfers */
	AHC_WIDE	= 0x00004,	/* Wide Channel */
	AHC_TWIN	= 0x00008,	/* Twin Channel */
	AHC_MORE_SRAM	= 0x00010,	/* 80 bytes instead of 64 */
	AHC_CMD_CHAN	= 0x00020,	/* Has a Command DMA Channel */
	AHC_QUEUE_REGS	= 0x00040,	/* Has Queue management registers */
	AHC_SG_PRELOAD	= 0x00080,	/* Can perform auto-SG preload */
	AHC_SPIOCAP	= 0x00100,	/* Has a Serial Port I/O Cap Register */
	AHC_MULTI_TID	= 0x00200,	/* Has bitmask of TIDs for select-in */
	AHC_HS_MAILBOX	= 0x00400,	/* Has HS_MAILBOX register */
	AHC_DT		= 0x00800,	/* Double Transition transfers */
	AHC_NEW_TERMCTL	= 0x01000,	/* Newer termination scheme */
	AHC_MULTI_FUNC	= 0x02000,	/* Multi-Function Twin Channel Device */
	AHC_LARGE_SCBS	= 0x04000,	/* 64byte SCBs */
	AHC_AUTORATE	= 0x08000,	/* Automatic update of SCSIRATE/OFFSET */
	AHC_AUTOPAUSE	= 0x10000,	/* Automatic pause on register access */
	AHC_TARGETMODE	= 0x20000,	/* Has tested target mode support */
	AHC_MULTIROLE	= 0x40000,	/* Space for two roles at a time */

	AHC_AIC7770_FE	= AHC_FENONE,
	AHC_AIC7850_FE	= AHC_SPIOCAP | AHC_AUTOPAUSE | AHC_TARGETMODE,
	AHC_AIC7855_FE	= AHC_AIC7850_FE,
	AHC_AIC7860_FE	= AHC_AIC7850_FE | AHC_ULTRA,
	AHC_AIC7870_FE	= AHC_TARGETMODE,
	AHC_AIC7880_FE	= AHC_AIC7870_FE | AHC_ULTRA,

	/*
	 *	Although we have space for both the initiator and target roles on
	 *	ULTRA2 chips, we currently disable the initiator role to allow
	 *	multi-scsi-id target mode configurations.  We can only respond on
	 *	the same SCSI ID as our initiator role if we allow initiator
	 *	operation. At some point, we should add a configuration knob to
	 *	allow both roles to be loaded.
	 */
	AHC_AIC7890_FE	= AHC_MORE_SRAM | AHC_CMD_CHAN | AHC_ULTRA2 | AHC_QUEUE_REGS | AHC_SG_PRELOAD |
		AHC_MULTI_TID | AHC_HS_MAILBOX | AHC_NEW_TERMCTL | AHC_LARGE_SCBS | AHC_TARGETMODE,

	AHC_AIC7892_FE	= AHC_AIC7890_FE | AHC_DT | AHC_AUTORATE | AHC_AUTOPAUSE,

	AHC_AIC7895_FE	= AHC_AIC7880_FE | AHC_MORE_SRAM | AHC_AUTOPAUSE | AHC_CMD_CHAN | AHC_MULTI_FUNC |
		AHC_LARGE_SCBS,

	AHC_AIC7895C_FE	= AHC_AIC7895_FE | AHC_MULTI_TID,
	AHC_AIC7896_FE	= AHC_AIC7890_FE | AHC_MULTI_FUNC,
	AHC_AIC7899_FE	= AHC_AIC7892_FE | AHC_MULTI_FUNC

}	ahc_feature;

/*
 ******	Erros a compensar ***************************************
 */
typedef enum
{
	AHC_BUGNONE = 0x00,

	/*
	 *	On all chips prior to the U2 product line, the WIDEODD S/G segment
	 *	feature does not work during scsi->HostBus transfers.
	 */
	AHC_TMODE_WIDEODD_BUG = 0x01,

	/*
	 *	On the aic7890/91 Rev 0 chips, the autoflush feature does not
	 *	work.  A manual flush of the DMA FIFO is required.
	 */
	AHC_AUTOFLUSH_BUG = 0x02,

	/*
	 *	On many chips, cacheline streaming does not work.
	 */
	AHC_CACHETHEN_BUG = 0x04,

	/*
	 *	On the aic7896/97 chips, cacheline streaming must be enabled.
	 */
	AHC_CACHETHEN_DIS_BUG = 0x08,

	/*
	 *	PCI 2.1 Retry failure on non-empty data fifo.
	 */
	AHC_PCI_2_1_RETRY_BUG = 0x10,

	/*
	 *	Controller does not handle cacheline residuals properly on S/G
	 *	segments if PCI MWI instructions are allowed.
	 */
	AHC_PCI_MWI_BUG = 0x20,

	/*
	 *	An SCB upload using the SCB channel's auto array entry copy
	 *	feature may corrupt data.  This appears to only occur on 66MHz
	 *	systems.
	 */
	AHC_SCBCHAN_UPLOAD_BUG = 0x40

}	ahc_bug;

/*
 ******	Configuração ********************************************
 */
typedef enum
{
	AHC_FNONE		= 0x00000,
	AHC_PAGESCBS		= 0x00001,	/* Enable SCB paging */
	AHC_USEDEFAULTS		= 0x00004,	/* Se não tiver SEEPROM ou BIOS */
	AHC_SEQUENCER_DEBUG	= 0x00008,
	AHC_SHARED_SRAM		= 0x00010,
	AHC_LARGE_SEEPROM	= 0x00020,	/* Uses C56_66 not C46 */
	AHC_RESET_BUS_A		= 0x00040,
	AHC_RESET_BUS_B		= 0x00080,
	AHC_EXTENDED_TRANS_A	= 0x00100,
	AHC_EXTENDED_TRANS_B	= 0x00200,
	AHC_TERM_ENB_A		= 0x00400,
	AHC_TERM_ENB_B		= 0x00800,
	AHC_INITIATORROLE	= 0x01000,	/* Allow initiator operations on this controller */
	AHC_TARGETROLE		= 0x02000,	/* Allow target operations on this controller */
	AHC_NEWEEPROM_FMT	= 0x04000,
	AHC_RESOURCE_SHORTAGE	= 0x08000,
	AHC_TQINFIFO_BLOCKED	= 0x10000,	/* Blocked waiting for ATIOs */
	AHC_INT50_SPEEDFLEX	= 0x20000,	/* Internal 50pin connector sits behind an aic3860 */
	AHC_SCB_BTT		= 0x40000,	/* Busy targets table stored in SCB space rather than SRAM */
	AHC_BIOS_ENABLED	= 0x80000

}	ahc_flag;

/*
 ******	Informação durante a identificação **********************
 */
struct ahc_probe_config
{
	char            channel;
	char            channel_b;
	ahc_chip        chip;
	ahc_feature     features;
	ahc_bug         bugs;
	ahc_flag        flags;
};

/************************* Hardware  SCB Definition ***************************/

/*
 *	The driver keeps up to MAX_SCB scb structures per card in memory.  The SCB
 *	consists of a "hardware SCB" mirroring the fields availible on the card
 *	and additional information the kernel stores for each transaction.
 *
 *	To minimize space utilization, a portion of the hardware scb stores
 *	different data during different portions of a SCSI transaction.
 *	As initialized by the host driver for the initiator role, this area
 *	contains the SCSI cdb (or a pointer to the  cdb) to be executed.  After
 *	the cdb has been presented to the target, this area serves to store
 *	residual transfer information and the SCSI status byte.
 *	For the target role, the contents of this area do not change, but
 *	still serve a different purpose than for the initiator role.  See
 *	struct target_data for details.
 */

/*
 *	Status information embedded in the shared poriton of
 *	an SCB after passing the cdb to the target.  The kernel
 *	driver will only read this data for transactions that
 *	complete abnormally (non-zero status byte).
 */
struct status_pkt
{
	ulong	residual_datacnt;	/* Residual in the current S/G seg */
	ulong	residual_sg_ptr;	/* The next S/G for this transfer */
	uchar	scsi_status;		/* Standard SCSI status byte */
};

/*
 *	Target mode version of the shared data SCB segment
 */
struct target_data
{
	uchar	target_phases;		/* Bitmap of phases to execute */
	uchar	data_phase;		/* Data-In or Data-Out */
	uchar	scsi_status;		/* SCSI status to give to initiator */
	uchar	initiator_tag;		/* Initiator's transaction tag */
};

struct hardware_scb
{
	 /*0*/ union
	{
		/*
		 *	If the cdb is 12 bytes or less, we embed it directly in
		 *	the SCB.  For longer cdbs, we embed the address of the cdb
		 *	payload as seen by the chip and a DMA is used to pull it
		 *	in.
		 */
		uchar			cdb[12];
		ulong			cdb_ptr;
		struct status_pkt	status;
		struct target_data	tdata;

	}	shared_data;

/*
 *	A word about residuals.
 *	The scb is presented to the sequencer with the dataptr and datacnt
 *	fields initialized to the contents of the first S/G element to
 *	transfer.  The sgptr field is initialized to the bus address for
 *	the S/G element that follows the first in the in core S/G array
 *	or'ed with the SG_FULL_RESID flag.  Sgptr may point to an invalid
 *	S/G entry for this transfer (single S/G element transfer with the
 *	first elements address and length preloaded in the dataptr/datacnt
 *	fields).  If no transfer is to occur, sgptr is set to SG_LIST_NULL.
 *	The SG_FULL_RESID flag ensures that the residual will be correctly
 *	noted even if no data transfers occur.  Once the data phase is entered,
 *	the residual sgptr and datacnt are loaded from the sgptr and the
 *	datacnt fields.  After each S/G element's dataptr and length are
 *	loaded into the hardware, the residual sgptr is advanced.  After
 *	each S/G element is expired, its datacnt field is checked to see
 *	if the LAST_SEG flag is set.  If so, SG_LIST_NULL is set in the
 *	residual sg ptr and the transfer is considered complete.  If the
 *	sequencer determines that there is a residual in the tranfer, it
 *	will set the SG_RESID_VALID flag in sgptr and dma the scb back into
 *	host memory.  To sumarize:
 *
 *	    Sequencer:
 *		o A residual has occurred if SG_FULL_RESID is set in sgptr,
 *		  or residual_sgptr does not have SG_LIST_NULL set.
 *
 *		o We are transfering the last segment if residual_datacnt has
 *		  the SG_LAST_SEG flag set.
 *
 *	   Host:
 *		o A residual has occurred if a completed scb has the
 *		  SG_RESID_VALID flag set.
 *
 *		o residual_sgptr and sgptr refer to the "next" sg entry
 *		  and so may point beyond the last valid sg entry for the
 *		  transfer.
 */
	/*12*/	ulong	dataptr;
	/*16*/	ulong	datacnt;	
	 /*20*/	ulong	sgptr;

#define SG_PTR_MASK	0xFFFFFFF8

	 /*24*/ uchar	control;	/* See SCB_CONTROL in aic7xxx.reg for details */
	 /*25*/ uchar	scsiid;		/* what to load in the SCSIID register */
	 /*26*/ uchar	lun;
	 /*27*/ uchar	tag;		/* Index of our kernel SCB array. Also the tag for tagged I/O */
	 /*28*/ uchar	cdb_len;
	 /*29*/ uchar	scsirate;	/* Value for SCSIRATE register */
	 /*30*/ uchar	scsioffset;	/* Value for SCSIOFFSET register */
	 /*31*/ uchar	next;		/* Used for threading SCBs in the "Waiting
					 * for Selection" and "Disconnected SCB"
					 * lists down in the sequencer. */
	 /*32*/ uchar	cdb32[32];	/* CDB storage for cdbs of size
					 * 13->32.  We store them here
					 * because hardware scbs are
					 * allocated from DMA safe memory so
					 * we are guaranteed the controller
					 * can access this data. */
};

/*
 ************************ Kernel SCB Definitions ******************************
 *
 *	Some fields of the SCB are OS dependent.  Here we collect the
 *	definitions for elements that all OS platforms need to include
 *	in there SCB definition.
 *
 *	Definition of a scatter/gather element as transfered to the controller.
 *	The aic7xxx chips only support a 24bit length.  We use the top byte of
 *	the length to store additional address bits and a flag to indicate
 *	that a given segment terminates the transfer.  This gives us an
 *	addressable range of 512GB on machines with 64bit PCI or with chips
 *	that can support dual address cycles on 32bit PCI busses.
 */
struct ahc_dma_seg
{
	ulong        addr;
	ulong        len;
};

#define	AHC_DMA_LAST_SEG	0x80000000
#define	AHC_SG_HIGH_ADDR_MASK	0x7F000000
#define	AHC_SG_LEN_MASK		0x00FFFFFF

/*
 ******	Estado corrente do SCB **********************************
 */
typedef enum
{
	SCB_FREE		= 0x0000,
	SCB_OTHERTCL_TIMEOUT	= 0x0002,	/* Another device was active during
						 * the first timeout for this SCB so
						 * we gave ourselves an additional
						 * timeout period in case it was
						 * hogging the bus */
	SCB_DEVICE_RESET	= 0x0004,
	SCB_SENSE		= 0x0008,
	SCB_CDB32_PTR		= 0x0010,
	SCB_RECOVERY_SCB	= 0x0040,
	SCB_NEGOTIATE		= 0x0080,
	SCB_QUEUE_FULL		= 0x0100,	/* Erro de fila cheia (tag) */
	SCB_DATA_OVERRUN	= 0x0200,	/* Não suportou a velocidade */
	SCB_ABORT		= 0x1000,
	SCB_UNTAGGEDQ		= 0x2000,
	SCB_ACTIVE		= 0x4000,
	SCB_TARGET_IMMEDIATE	= 0x8000

}	scb_flag;

/*
 ******	SCB (software) ******************************************
 *
 *	Cuidado: Deve ter tamanho múltiplo de 8
 */
struct ahc_scb
{
	struct ahc_dma_seg	sg_list[2];		/* Para o DMA (ainda NÃO devidamente usado) */
	int			sg_count;

	struct hardware_scb	*hscb;

	struct ahc_scb		*free_next;		/* Encadeamento da lista livre */

	BHEAD			*bp;

	struct ahc_softc	*ahc_softc;

	int			flags;

	struct scsi_sense_data	sense_area;		/* Para obter o "sense" */

}	/* end struct ahc_scb */;

/*
 ******	Parâmetros de transferência *****************************
 */
#define AHC_TRANS_CUR		0x01	/* Modify current neogtiation status */
#define AHC_TRANS_ACTIVE	0x03	/* Assume this target is on the bus */
#define AHC_TRANS_GOAL		0x04	/* Modify negotiation goal */
#define AHC_TRANS_USER		0x08	/* Modify user negotiation settings */

/*
 *	Transfer Negotiation Information
 */
struct ahc_transinfo
{
	uchar	protocol_version;	/* SCSI Revision level */
	uchar	transport_version;	/* SPI Revision level */
	uchar	width;			/* Bus width */
	uchar	period;			/* Sync rate factor */
	uchar	offset;			/* Sync offset */
	uchar	ppr_options;		/* Parallel Protocol Request options */
};

/*
 *	Per-initiator current, goal and user transfer negotiation information
 */
struct ahc_tinfo_trio
{
	uchar			scsirate;	/* Computed value for SCSIRATE reg */
	struct ahc_transinfo	current;
	struct ahc_transinfo	goal;
	struct ahc_transinfo	user;
};

/*
 ******	Serial EEPROM Format ************************************
 */
struct seeprom_config
{
	 /* Per SCSI ID Configuration Flags */

	ushort	device_flags[16];	/* words 0-15 */

#define		CFXFER		0x0007	/* synchronous transfer rate */
#define		CFSYNCH		0x0008	/* enable synchronous transfer */
#define		CFDISC		0x0010	/* enable disconnection */
#define		CFWIDEB		0x0020	/* wide bus device */
#define		CFSYNCHISULTRA	0x0040	/* CFSYNCH is an ultra offset (2940AU) */
#define		CFSYNCSINGLE	0x0080	/* Single-Transition signalling */
#define		CFSTART		0x0100	/* send start unit SCSI command */
#define		CFINCBIOS	0x0200	/* include in BIOS scan */
#define		CFRNFOUND	0x0400	/* report even if not found */
#define		CFMULTILUNDEV	0x0800	/* Probe multiple luns in BIOS scan */
#define		CFWBCACHEENB	0x4000	/* Enable W-Behind Cache on disks */
#define		CFWBCACHENOP	0xC000	/* Don't touch W-Behind Cache */

	/* BIOS Control Bits */

	ushort	bios_control;	/* word 16 */

#define		CFSUPREM	0x0001	/* support all removeable drives */
#define		CFSUPREMB	0x0002	/* support removeable boot drives */
#define		CFBIOSEN	0x0004	/* BIOS enabled */
#define		CFSM2DRV	0x0010	/* support more than two drives */
#define		CF284XEXTEND	0x0020	/* extended translation (284x cards) */
#define		CFSTPWLEVEL	0x0010	/* Termination level control */
#define		CFEXTEND	0x0080	/* extended translation enabled */
#define		CFSCAMEN	0x0100	/* SCAM enable */

	/* Host Adapter Control Bits */

	ushort	adapter_control;	/* word 17 */

#define		CFAUTOTERM	0x0001	/* Perform Auto termination */
#define		CFULTRAEN	0x0002	/* Ultra SCSI speed enable */
#define		CF284XSELTO     0x0003	/* Selection timeout (284x cards) */
#define		CF284XFIFO      0x000C	/* FIFO Threshold (284x cards) */
#define		CFSTERM		0x0004	/* SCSI low byte termination */
#define		CFWSTERM	0x0008	/* SCSI high byte termination */
#define		CFSPARITY	0x0010	/* SCSI parity */
#define		CF284XSTERM     0x0020	/* SCSI low byte term (284x cards) */
#define		CFMULTILUN	0x0020	/* SCSI low byte term (284x cards) */
#define		CFRESETB	0x0040	/* reset SCSI bus at boot */
#define		CFCLUSTERENB	0x0080	/* Cluster Enable */
#define		CFCHNLBPRIMARY	0x0100	/* aic7895 probe B channel first */
#define		CFSEAUTOTERM	0x0400	/* Ultra2 Perform secondary Auto Term */
#define		CFSELOWTERM	0x0800	/* Ultra2 secondary low term */
#define		CFSEHIGHTERM	0x1000	/* Ultra2 secondary high term */
#define		CFDOMAINVAL	0x4000	/* Perform Domain Validation */

	/* Bus Release Time, Host Adapter ID */

	ushort	brtime_id;	/* word 18 */

#define		CFSCSIID	0x000F	/* host adapter SCSI ID */
#define		CFBRTIME	0xFF00	/* bus release time */

	/* Maximum targets */

	ushort	max_targets;	/* word 19 */

#define		CFMAXTARG	0x00FF	/* maximum targets */
#define		CFBOOTLUN	0x0F00	/* Lun to boot from */
#define		CFBOOTID	0xF000	/* Target to boot from */

	ushort	res_1[10];	/* words 20-29 */
	ushort	signature;	/* Signature == 0x250 */

#define		CFSIGNATURE	0x250

	ushort	checksum;	/* word 31 */
};

/*
 ******	Mensagens ***********************************************
 */
typedef enum
{
	MSG_TYPE_NONE,
	MSG_TYPE_INITIATOR_MSGOUT,
	MSG_TYPE_INITIATOR_MSGIN,
	MSG_TYPE_TARGET_MSGOUT,
	MSG_TYPE_TARGET_MSGIN

}	ahc_msg_type;

typedef enum
{
	MSGLOOP_IN_PROG,
	MSGLOOP_MSGCOMPLETE,
	MSGLOOP_TERMINATED

}	msg_loop_stat;

/*
 ******	Dados do controlador ************************************
 */
typedef struct ahcinfo
{
	SCSI			info_scsi;	/* A estrutura comum SCSI */

	LOCK			info_busy;	/* Unidade ocupado */
	DEVHEAD			info_utab;	/* Cabeças das listas de pedidos do alvo */

	struct ahc_tinfo_trio	transinfo;	/* Parâmetros de transferência */

	const char		*info_rate;	/* Velocidade (em MB/s) */

	int			info_max_tag;	/* Número máximo de pedidos simultâneos */
	int			info_cur_op;	/* Número corrente de pedidos */
	int			info_inc_op;	/* Número de pedidos faltando para um incremento */

}	AHCINFO;

#define	AHC_MAX_TAG	64	/* Valor inicial para "info"max_tag" */
#define	AHC_INC_OP	1024	/* Após este número de escritas, incrementa de 1 */

struct ahc_softc
{
	int			port_base;			/* Porta de entrada/saída */
	int			irq;

	struct ahc_scb		*free_scbs;			/* Lista de SCBs livres */

	struct ahc_scb		*scbindex[AHC_SCB_MAX + 1];	/* Mapping from tag to SCB */
	struct hardware_scb	*hscbs;				/* Array of hardware SCBs */
	struct ahc_scb		*scbarray;			/* Array of kernel SCBs */

	uchar			numscbs;			/* No. de SCBs alocados */
	uchar			maxhscbs;			/* Number of SCBs on the card */

	struct ahc_scb		*next_queued_scb;

	/*
	 *	Platform specific device information
	 */
	PCIDATA			*dev_softc;

	/*
	 *	Target mode related state kept on a per enabled lun basis. Targets
	 *	that are not enabled will have null entries. As an initiator, we
	 *	keep one target entry for our initiator ID to store our sync/wide
	 *	transfer settings.
	 */
	ushort			ultraenb;		/* Using ultra sync rate  */
	ushort			discenable;		/* Disconnection allowed  */
	ushort			tagenable;		/* Tagged Queuing allowed */

	ushort			user_discenable;	/* Disconnection allowed  */
	ushort			user_tagenable;		/* Tagged Queuing allowed */

	/*
	 *	The black hole device responsible for handling requests for
	 *	disabled luns on enabled targets.
	 */
	struct tmode_lstate	*black_hole;

	/*
	 *	Device instance currently on the bus awaiting a continue TIO for a
	 *	command that was not given the disconnect priveledge.
	 */
	struct tmode_lstate	*pending_device;

	/* Card characteristics */

	int			chip;
	int			features;
	int			bugs;
	int			flags;

	/* Values to store in the SEQCTL register for pause and unpause */

	uchar			unpause, pause;

	/* Command Queues */

	uchar			qoutfifonext, qinfifonext, *qoutfifo, *qinfifo;

	/* Critical Section Data */

	struct cs		*critical_sections;
	unsigned		num_critical_sections;

	/* Channel Names ('A', 'B', etc.) */

	char			channel, channel_b;

	/* Initiator Bus ID */

	uchar			our_id;

	/* Targets that need negotiation messages */

	ushort			targ_msg_req;

	/*
	 *	Target incoming command FIFO
	 */
	struct target_cmd	*targetcmds;
	uchar			tqinfifonext;

	/*
	 * Incoming and outgoing message handling.
	 */
	uchar			send_msg_perror;
	int			msg_type;
	uchar			msgout_buf[12];		/* Message we are sending */
	uchar			msgin_buf[12];		/* Message we are receiving */
	unsigned		msgout_len;		/* Length of message to send */
	unsigned		msgout_index;		/* Current index in msgout */
	unsigned		msgin_index;		/* Current index in msgin */

	/* PCI cacheline size. */

	unsigned		pci_cachesize;

	/* Per-Unit descriptive information */

	char			name[8];
	int			unit;

	/* Cabeçalho de área de E/S para operações não estruturadas */

	BHEAD			ahc_raw_buf;		/* Para as operações "raw" */

	AHCINFO			ahc_info[NTARGET];	/* Dados de cada "alvo" */

	int			ahc_busy_scb;
};

/*
 ******	Recuperação de erros ************************************
 */
typedef enum
{
	SEARCH_COMPLETE,
	SEARCH_COUNT,
	SEARCH_REMOVE

}	ahc_search_action;

/*
 ******	Acesso ao dispositivo ***********************************
 */
#define ahc_inb(ahc, port)			read_port  (ahc->port_base + port)
#define ahc_outb(ahc, port, value)		write_port (value, ahc->port_base + port)
#define ahc_outsb(ahc, port, area, count)	write_port_string_byte (ahc->port_base + port, area, count)

/*
 ******	Primitivas do sistema operacional ***********************
 */
#define ahc_delay DELAY

/*
 *	x
 */
#define	CAM_LUN_WILDCARD (~0)
#define	CAM_TARGET_WILDCARD (~0)

/*
 ******	Tabela de velocidades síncronas *************************
 */
struct ahc_syncrate
{
	unsigned	sxfr_u2;	/* Value of the SXFR parameter for Ultra2+ Chips */
	unsigned	sxfr;		/* Value of the SXFR parameter for <= Ultra Chips */

#define	ULTRA_SXFR	0x100		/* Rate Requires Ultra Mode set */
#define	ST_SXFR		0x010		/* Rate Single Transition Only */
#define	DT_SXFR		0x040		/* Rate Double Transition Only */

	uchar		period;		/* Period to send to SCSI target */
	const char	*rate;
	const char	*double_rate;
};

/* Índices para a nossa tabela de velocidades de transferências síncronas */

#define AHC_SYNCRATE_DT		0
#define AHC_SYNCRATE_ULTRA2	1
#define AHC_SYNCRATE_ULTRA	3
#define AHC_SYNCRATE_FAST	6

/*
 ******	Protótipos de funções ***********************************
 */
struct ahc_scb		*ahc_get_scb (struct ahc_softc *ahc);
int			ahc_init (struct ahc_softc * ahc);
void			pause_sequencer (struct ahc_softc *ahc);
void			unpause_sequencer (struct ahc_softc *ahc);
void			ahc_intr ();

/*
 ******	Protótipos de funções ***********************************
 */
struct ahc_scb	*ahc_get_scb(struct ahc_softc *ahc);
void		ahc_queue_scb(struct ahc_softc *ahc, struct ahc_scb *scbp);
int		ahc_check_residual (struct ahc_scb *scbp);
int		ahc_calc_residual (struct ahc_softc *ahc, struct ahc_scb *scbp);
void		ahc_done (struct ahc_softc *ahc, struct ahc_scb *scbp);
void		ahc_put_scb(struct ahc_softc *ahc, struct ahc_scb *scbp);
int		ahc_rem_wscb (struct ahc_softc *ahc, int scbpos, int prev);
void		ahc_release_untagged_queues (struct ahc_softc *ahc);
void		ahc_add_curscb_to_free_list (struct ahc_softc *ahc);
void		ahc_update_target_msg_request (struct ahc_softc *, int target, int force, int paused);
void		ahc_setup_initiator_msgout (struct ahc_softc *, int target, struct ahc_scb *);
void		ahc_handle_message_phase (struct ahc_softc *ahc);
const char	*ahc_get_dev_nm_by_scb (struct ahc_softc *ahc, struct ahc_scb *scbp);
const char	*ahc_get_dev_nm (struct ahc_softc *ahc, int target);
int		ahc_qinfifo_count (struct ahc_softc *ahc);
void		ahc_qinfifo_requeue_tail (struct ahc_softc *ahc, struct ahc_scb *scbp);
void		ahc_qinfifo_requeue (struct ahc_softc *ahc, struct ahc_scb *prev_scb, struct ahc_scb *scbp);
void		ahc_build_transfer_msg (struct ahc_softc *ahc, int target);
int		ahc_parse_msg (struct ahc_softc *ahc, int target);
int		ahc_handle_msg_reject (struct ahc_softc *ahc, int target);
const struct ahc_syncrate	*ahc_devlimited_syncrate (struct ahc_softc *ahc, struct ahc_tinfo_trio * tinfo,
			 uint * period, uint * ppr_options);
void		ahc_construct_sdtr (struct ahc_softc *, const AHCINFO *, uint, uint);
void		ahc_construct_wdtr (struct ahc_softc *, const AHCINFO *, uint bus_width);
void		ahc_construct_ppr (struct ahc_softc *, const AHCINFO *, uint, uint, uint, uint);
void		ahc_busy_tcl (struct ahc_softc *ahc, uint tcl, uint scbid);
void		ahc_clear_msg_state (struct ahc_softc *ahc);
int			ahc_init_scbdata (struct ahc_softc *ahc);
uint		ahc_index_busy_tcl(struct ahc_softc *ahc, int target, int lun, int unbusy);
void			ahc_loadseq (struct ahc_softc *ahc);
void			ahc_download_instr (struct ahc_softc *ahc, uint instrptr, uchar * dconsts);
int		ahc_match_scb (struct ahc_softc *, struct ahc_scb *, int, int, uint);
void		ahc_clear_intstat (struct ahc_softc *ahc);
void		ahc_clear_critical_section (struct ahc_softc *ahc);
int		ahc_search_qinfifo (struct ahc_softc *, int, int, uint, ulong, ahc_search_action);
const struct ahc_syncrate	*ahc_find_syncrate (struct ahc_softc *, unsigned *, unsigned *, unsigned);
void		ahc_set_transaction_tag (struct ahc_scb *scbp, int enabled, uint type);
void		ahc_set_transaction_status (struct ahc_scb *scbp, ulong status);
uint		ahc_find_period (struct ahc_softc *ahc, uint scsirate, uint maxsync);
extern void	restart_sequencer (struct ahc_softc *ahc);
extern void	ahc_run_qoutfifo (struct ahc_softc *ahc);
extern void	ahc_handle_seqint (struct ahc_softc *ahc, uint intstat);
extern void	ahc_search_wainting_selection_list (struct ahc_softc *ahc);

/*
 ******	Protótipos de funções ***********************************
 */
int		ahc_cmd (BHEAD *bp);
int		ahc_strategy (BHEAD *bp);
void		ahc_start (struct ahc_softc *ahc, int target, struct ahc_scb *scbp);


void		XPT_SET_TRAN_SETTINGS (struct ahc_softc *ahc, int target);

/*
 ******	Variáveis externas **************************************
 */
extern struct ahc_softc			*ahc_softc_vec[NAHC];
extern const struct ahc_syncrate	ahc_syncrates[];
