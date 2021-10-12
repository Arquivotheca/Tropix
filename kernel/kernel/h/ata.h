/*
 ****************************************************************
 *								*
 *			ata.h					*
 *								*
 *	Definições acerca de dispositivos ATA			*
 *								*
 *	Versão	4.0.0, de 18.07.01				*
 *		4.9.0, de 26.04.07				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2007 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#define	ATA_H

/*
 ****************************************************************
 *	Registradores e bits dos controladores			*
 ****************************************************************
 */
#define NATAC				16	/* Número máximo de canais */
#define NATAT				2	/* Cada canal comporta, no máximo, 2 dispositivos */

#define PL				3	/* Prioridade de interrupção */
#define	PATA_IRQ_BASE			14	/* IRQs 14 e 15 para PATA */

#define ATA_PRIMARY			0x1F0	/* Port do canal primário */
#define ATA_SECONDARY			0x170	/* Port do canal secundário */
#define ATA_ALTOFFSET			0x206	/* Deslocamento do registrador ATA_ALTSTAT */

#define	ATA_BMIOSIZE			8	/* Deslocamento do port do DMA para o canal secundário */

/*
 ******	Deslocamentos virtuais dos Registradores ****************
 */
enum
{
	ATA_DATA,		/* RW) data */
	ATA_FEATURE,		/* W) feature */
	ATA_COUNT,		/* W) sector count */
	ATA_SECTOR,		/* RW) sector # */
	ATA_CYL_LSB,		/* RW) cylinder# LSB */
	ATA_CYL_MSB,		/* RW) cylinder# MSB */
	ATA_DRIVE,		/* W) Sector/Drive/Head */
	ATA_COMMAND,		/* W) command */
	ATA_ERROR,		/* R) error */
	ATA_IREASON,		/* R) interrupt reason */
	ATA_STATUS,		/* R) status */

	ATA_ALTSTAT,		/* R) alternate status */
	ATA_CONTROL,		/* W) control */

	ATA_SSTATUS,		/* SATA status */
	ATA_SERROR,		/* SATA error */
	ATA_SCONTROL,		/* SATA control */
	ATA_SACTIVE,		/* SATA active */

	ATA_BMCMD_PORT,		/* DMA - controle */
	ATA_BMDEVSPEC_0,	/* DMA */
	ATA_BMSTAT_PORT,	/* DMA - status */
	ATA_BMDEVSPEC_1,	/* DMA */
	ATA_BMDTP_PORT,		/* DMA - endereço da tabela */

	ATA_IDX_ADDR,		/* ??? */
	ATA_IDX_DATA,		/* ??? */

	ATA_AHCI,

	ATA_MAX_RES
};

/*
 ******	Bits de "ATA_ERROR" *************************************
 */
#define	ATA_E_ILI		0x01	/* illegal length */
#define	ATA_E_NM		0x02	/* no media */
#define	ATA_E_ABORT		0x04	/* command aborted */
#define	ATA_E_MCR		0x08	/* media change request */
#define	ATA_E_IDNF		0x10	/* ID not found */
#define	ATA_E_MC		0x20	/* media changed */
#define	ATA_E_UNC		0x40	/* uncorrectable data */
#define	ATA_E_ICRC		0x80	/* UDMA crc error */

/*
 ******	Bits de "ATA_FEATURES" **********************************
 */
#define	ATA_F_DMA		0x01	/* enable DMA */
#define	ATA_F_OVL		0x02	/* enable overlap */

/*
 ******	Bits de "ATA_INTERRUPT REASON" **************************
 */
#define	ATA_I_CMD		0x01	/* cmd (1) | data (0) */
#define	ATA_I_IN		0x02	/* read (1) | write (0) */
#define	ATA_I_RELEASE		0x04	/* released bus (1) */
#define	ATA_I_TAGMASK		0xF8	/* tag mask */

/*
 ******	Bits de "ATA_DRIVE" *************************************
 */
#define ATA_MASTER		0x00	/* Seleciona o dispositivo mestre */
#define ATA_SLAVE		0x10	/* Seleciona o dispositivo escravo */
#define	ATA_D_LBA		0x40	/* use LBA addressing */
#define	ATA_D_IBM		0xA0	/* 512 byte sectors, ECC */

/*
 ******	Bits de "ATA_COMMAND" ***********************************
 */
#define	ATA_C_NOP		0x00	/* NOP command */
#define ATA_C_F_FLUSHQUEUE	0x00	/* flush queued cmd's */
#define ATA_C_F_AUTOPOLL	0x01	/* start autopoll function */
#define	ATA_C_ATAPI_RESET	0x08	/* reset ATAPI device */
#define	ATA_C_READ		0x20	/* read command */
#define	ATA_C_READ48		0x24	/* read command */
#define	ATA_C_READ_DMA48	0x25	/* read w/DMA command */
#define	ATA_C_READ_DMA_QUEUED48 0x26	/* read w/DMA QUEUED command */
#define	ATA_C_READ_MUL48	0x29	/* read multi command */
#define	ATA_C_WRITE		0x30	/* write command */
#define	ATA_C_WRITE48		0x34	/* write command */
#define	ATA_C_WRITE_DMA48	0x35	/* write w/DMA command */
#define	ATA_C_WRITE_DMA_QUEUED48 0x36	/* write w/DMA QUEUED command */
#define	ATA_C_WRITE_MUL48	0x39	/* write multi command */
#define	ATA_C_PACKET_CMD	0xA0	/* packet command */
#define	ATA_C_ATAPI_IDENTIFY	0xA1	/* get ATAPI params*/
#define	ATA_C_SERVICE		0xA2	/* service command */
#define	ATA_C_READ_MUL		0xC4	/* read multi command */
#define	ATA_C_WRITE_MUL		0xC5	/* write multi command */
#define	ATA_C_SET_MULTI		0xC6	/* set multi size command */
#define	ATA_C_READ_DMA_QUEUED	0xC7	/* read w/DMA QUEUED command */
#define	ATA_C_READ_DMA		0xC8	/* read w/DMA command */
#define	ATA_C_WRITE_DMA		0xCA	/* write w/DMA command */
#define	ATA_C_WRITE_DMA_QUEUED	0xCC	/* write w/DMA QUEUED command */
#define	ATA_C_SLEEP		0xE6	/* sleep command */
#define	ATA_C_FLUSHCACHE	0xE7	/* flush cache to disk */
#define	ATA_C_FLUSHCACHE48	0xEA	/* flush cache to disk */
#define	ATA_C_ATA_IDENTIFY	0xEC	/* get ATA params */
#define	ATA_C_SETFEATURES	0xEF	/* features command */
#define		    ATA_C_F_SETXFER	0x03	/* set transfer mode */
#define		    ATA_C_F_ENAB_WCACHE 0x02	/* enable write cache */
#define		    ATA_C_F_DIS_WCACHE	0x82	/* disable write cache */
#define		    ATA_C_F_ENAB_RCACHE 0xAA	/* enable readahead cache */
#define		    ATA_C_F_DIS_RCACHE	0x55	/* disable readahead cache */
#define		    ATA_C_F_ENAB_RELIRQ 0x5D	/* enable release interrupt */
#define		    ATA_C_F_DIS_RELIRQ	0xDD	/* disable release interrupt */
#define		    ATA_C_F_ENAB_SRVIRQ 0x5E	/* enable service interrupt */
#define		    ATA_C_F_DIS_SRVIRQ	0xDE	/* disable service interrupt */

/*
 ******	Bits de "ATA_STATUS" ************************************
 */
#define	ATA_S_ERROR		0x01	/* error */
#define	ATA_S_INDEX		0x02	/* index */
#define	ATA_S_CORR		0x04	/* data corrected */
#define	ATA_S_DRQ		0x08	/* data request */
#define	ATA_S_DSC		0x10	/* drive seek completed */
#define	ATA_S_SERVICE		0x10	/* drive needs service */
#define	ATA_S_DWF		0x20	/* drive write fault */
#define	ATA_S_DMA		0x20	/* DMA ready */
#define	ATA_S_READY		0x40	/* drive ready */
#define	ATA_S_BUSY		0x80	/* busy */

/*
 ******	Bits de "ATA_CONTROL" ***********************************
 */
#define	ATA_A_IDS		0x02	/* disable interrupts */
#define	ATA_A_RESET		0x04	/* RESET controller */
#define	ATA_A_4BIT		0x08	/* 4 head bits */
#define ATA_A_HOB		0x80    /* High Order Byte enable */

/*
 ******	Bits de "ATA_BMCMD" *************************************
 */
#define		ATA_BMCMD_START_STOP	0x01
#define		ATA_BMCMD_WRITE_READ	0x08

/*
 ******	Bits de "ATA_BMSTAT" ************************************
 */
#define		ATA_BMSTAT_ACTIVE	0x01
#define		ATA_BMSTAT_ERROR	0x02
#define		ATA_BMSTAT_INTERRUPT	0x04
#define		ATA_BMSTAT_MASK		0x07
#define		ATA_BMSTAT_DMA_MASTER	0x20
#define		ATA_BMSTAT_DMA_SLAVE	0x40
#define		ATA_BMSTAT_DMA_SIMPLEX	0x80

/*
 ******	Bits de "ATA_SSTATUS" ***********************************
 */
#define         ATA_SS_DET_MASK         0x0000000f
#define         ATA_SS_DET_NO_DEVICE    0x00000000
#define         ATA_SS_DET_DEV_PRESENT  0x00000001
#define         ATA_SS_DET_PHY_ONLINE   0x00000003
#define         ATA_SS_DET_PHY_OFFLINE  0x00000004

#define         ATA_SS_SPD_MASK         0x000000f0
#define         ATA_SS_SPD_NO_SPEED     0x00000000
#define         ATA_SS_SPD_GEN1         0x00000010
#define         ATA_SS_SPD_GEN2         0x00000020

#define         ATA_SS_IPM_MASK         0x00000f00
#define         ATA_SS_IPM_NO_DEVICE    0x00000000
#define         ATA_SS_IPM_ACTIVE       0x00000100
#define         ATA_SS_IPM_PARTIAL      0x00000200
#define         ATA_SS_IPM_SLUMBER      0x00000600

#define         ATA_SS_CONWELL_MASK \
		    (ATA_SS_DET_MASK|ATA_SS_SPD_MASK|ATA_SS_IPM_MASK)
#define         ATA_SS_CONWELL_GEN1 \
		    (ATA_SS_DET_PHY_ONLINE|ATA_SS_SPD_GEN1|ATA_SS_IPM_ACTIVE)
#define         ATA_SS_CONWELL_GEN2 \
		    (ATA_SS_DET_PHY_ONLINE|ATA_SS_SPD_GEN2|ATA_SS_IPM_ACTIVE)

/*
 ******	Bits de "ATA_SERROR" ************************************
 */
#define         ATA_SE_DATA_CORRECTED   0x00000001
#define         ATA_SE_COMM_CORRECTED   0x00000002
#define         ATA_SE_DATA_ERR         0x00000100
#define         ATA_SE_COMM_ERR         0x00000200
#define         ATA_SE_PROT_ERR         0x00000400
#define         ATA_SE_HOST_ERR         0x00000800
#define         ATA_SE_PHY_CHANGED      0x00010000
#define         ATA_SE_PHY_IERROR       0x00020000
#define         ATA_SE_COMM_WAKE        0x00040000
#define         ATA_SE_DECODE_ERR       0x00080000
#define         ATA_SE_PARITY_ERR       0x00100000
#define         ATA_SE_CRC_ERR          0x00200000
#define         ATA_SE_HANDSHAKE_ERR    0x00400000
#define         ATA_SE_LINKSEQ_ERR      0x00800000
#define         ATA_SE_TRANSPORT_ERR    0x01000000
#define         ATA_SE_UNKNOWN_FIS      0x02000000

/*
 ******	Bits de "ATA_SCONTROL" **********************************
 */
#define         ATA_SC_DET_MASK         0x0000000f
#define         ATA_SC_DET_IDLE         0x00000000
#define         ATA_SC_DET_RESET        0x00000001
#define         ATA_SC_DET_DISABLE      0x00000004

#define         ATA_SC_SPD_MASK         0x000000f0
#define         ATA_SC_SPD_NO_SPEED     0x00000000
#define         ATA_SC_SPD_SPEED_GEN1   0x00000010
#define         ATA_SC_SPD_SPEED_GEN2   0x00000020

#define         ATA_SC_IPM_MASK         0x00000f00
#define         ATA_SC_IPM_NONE         0x00000000
#define         ATA_SC_IPM_DIS_PARTIAL  0x00000100
#define         ATA_SC_IPM_DIS_SLUMBER  0x00000200

/*
 ******	Definições diversas *************************************
 */
enum
{
	ATA_OP_FINISHED,			/* A operação terminou */
	ATA_OP_CONTINUES,			/* A operação continua, e   há dados transferidos */
	ATA_OP_CONTINUES_NO_DATA		/* A operação continua, não há dados transferidos */
};

#define	ATA_IMMEDIATE			0x01	/* Não deve aguardar ATA_S_READY */

#define	ATA_ATA_MASTER			0x01
#define	ATA_ATA_SLAVE			0x02
#define	ATA_ATAPI_MASTER		0x04
#define	ATA_ATAPI_SLAVE			0x08

/*
 ******	Modos de Transferência **********************************
 */
#define ATA_MODE_MASK			0x0F
#define ATA_DMA_MASK			0xF0

#define	ATA_PIO				0x00
#define ATA_PIO_MAX			0x0F

#define ATA_PIO0			0x08
#define ATA_PIO1			0x09
#define ATA_PIO2			0x0A
#define ATA_PIO3			0x0B
#define ATA_PIO4			0x0C

#define ATA_DMA				0x10
#define ATA_DMA_MAX			0x4F

#define	ATA_WDMA			0x20
#define	ATA_WDMA0			0x20
#define	ATA_WDMA1			0x21
#define	ATA_WDMA2			0x22

#define	ATA_UDMA			0x40
#define	ATA_UDMA0			0x40
#define	ATA_UDMA1			0x41
#define	ATA_UDMA2			0x42
#define	ATA_UDMA3			0x43
#define	ATA_UDMA4			0x44
#define	ATA_UDMA5			0x45
#define	ATA_UDMA6			0x46
#define ATA_SA150			0x47
#define ATA_SA300			0x48

#define ATA_USB				0x80
#define ATA_USB1			0x81
#define ATA_USB2			0x82

/*
 ******	ATAPI  **************************************************
 */
/* ATAPI misc defines */
#define ATAPI_MAGIC_LSB			0x14
#define ATAPI_MAGIC_MSB			0xEB
#define ATAPI_P_READ			(ATA_S_DRQ | ATA_I_IN)
#define ATAPI_P_WRITE			(ATA_S_DRQ)
#define ATAPI_P_CMDOUT			(ATA_S_DRQ | ATA_I_CMD)
#define ATAPI_P_DONEDRQ			(ATA_S_DRQ | ATA_I_CMD | ATA_I_IN)
#define ATAPI_P_DONE			(ATA_I_CMD | ATA_I_IN)
#define ATAPI_P_ABORT			0

/* error register bits */
#define ATAPI_E_MASK			0x0F	/* error mask */
#define ATAPI_E_ILI			0x01	/* illegal length indication */
#define ATAPI_E_EOM			0x02	/* end of media detected */
#define ATAPI_E_ABRT			0x04	/* command aborted */
#define ATAPI_E_MCR			0x08	/* media change requested */
#define ATAPI_SK_MASK			0xF0	/* sense key mask */
#define ATAPI_SK_NO_SENSE		0x00	/* no specific sense key info */
#define ATAPI_SK_RECOVERED_ERROR	0x10	/* command OK, data recovered */
#define ATAPI_SK_NOT_READY		0x20	/* no access to drive */
#define ATAPI_SK_MEDIUM_ERROR		0x30	/* non-recovered data error */
#define ATAPI_SK_HARDWARE_ERROR		0x40	/* non-recoverable HW failure */
#define ATAPI_SK_ILLEGAL_REQUEST	0x50	/* invalid command param(s) */
#define ATAPI_SK_UNIT_ATTENTION		0x60	/* media changed */
#define ATAPI_SK_DATA_PROTECT		0x70	/* write protect */
#define ATAPI_SK_BLANK_CHECK		0x80	/* blank check */
#define ATAPI_SK_VENDOR_SPECIFIC	0x90	/* vendor specific skey */
#define ATAPI_SK_COPY_ABORTED		0xA0	/* copy aborted */
#define ATAPI_SK_ABORTED_COMMAND	0xB0	/* command aborted, try again */
#define ATAPI_SK_EQUAL			0xC0	/* equal */
#define ATAPI_SK_VOLUME_OVERFLOW	0xD0	/* volume overflow */
#define ATAPI_SK_MISCOMPARE		0xE0	/* data dont match the medium */
#define ATAPI_SK_RESERVED		0xF0

#if (0)	/*******************************************************/
/* ATAPI commands */
#define ATAPI_TEST_UNIT_READY		0x00	/* check if device is ready */
#define ATAPI_REZERO			0x01	/* rewind */
#define ATAPI_REQUEST_SENSE		0x03	/* get sense data */
#define ATAPI_FORMAT			0x04	/* format unit */
#define ATAPI_READ			0x08	/* read data */
#define ATAPI_WRITE			0x0A	/* write data */
#define ATAPI_WEOF			0x10	/* write filemark */
#define	    WF_WRITE				0x01
#define ATAPI_SPACE			0x11	/* space command */
#define	    SP_FM				0x01
#define	    SP_EOD				0x03
#define ATAPI_MODE_SELECT		0x15	/* mode select */
#define ATAPI_ERASE			0x19	/* erase */
#define ATAPI_MODE_SENSE		0x1A	/* mode sense */
#define ATAPI_START_STOP		0x1B	/* start/stop unit */
#define	    SS_LOAD				0x01
#define	    SS_RETENSION			0x02
#define	    SS_EJECT				0x04
#define ATAPI_PREVENT_ALLOW		0x1E	/* media removal */
#define ATAPI_READ_CAPACITY		0x25	/* get volume capacity */
#define ATAPI_READ_BIG			0x28	/* read data */
#define ATAPI_WRITE_BIG			0x2A	/* write data */
#define ATAPI_LOCATE			0x2B	/* locate to position */
#define ATAPI_READ_POSITION		0x34	/* read position */
#define ATAPI_SYNCHRONIZE_CACHE		0x35	/* flush buf, close channel */
#define ATAPI_WRITE_BUFFER		0x3B	/* write device buffer */
#define ATAPI_READ_BUFFER		0x3C	/* read device buffer */
#define ATAPI_READ_SUBCHANNEL		0x42	/* get subchannel info */
#define ATAPI_READ_TOC			0x43	/* get table of contents */
#define ATAPI_PLAY_10			0x45	/* play by lba */
#define ATAPI_PLAY_MSF			0x47	/* play by MSF address */
#define ATAPI_PLAY_TRACK		0x48	/* play by track number */
#define ATAPI_PAUSE			0x4B	/* pause audio operation */
#define ATAPI_READ_DISK_INFO		0x51	/* get disk info structure */
#define ATAPI_READ_TRACK_INFO		0x52	/* get track info structure */
#define ATAPI_RESERVE_TRACK		0x53	/* reserve track */
#define ATAPI_SEND_OPC_INFO		0x54	/* send OPC structurek */
#define ATAPI_MODE_SELECT_BIG		0x55	/* set device parameters */
#define ATAPI_REPAIR_TRACK		0x58	/* repair track */
#define ATAPI_READ_MASTER_CUE		0x59	/* read master CUE info */
#define ATAPI_MODE_SENSE_BIG		0x5A	/* get device parameters */
#define ATAPI_CLOSE_TRACK		0x5B	/* close track/session */
#define ATAPI_READ_BUFFER_CAPACITY	0x5C	/* get buffer capicity */
#define ATAPI_SEND_CUE_SHEET		0x5D	/* send CUE sheet */
#define ATAPI_BLANK			0xa1	/* blank the media */
#define ATAPI_SEND_KEY			0xa3	/* send DVD key structure */
#define ATAPI_REPORT_KEY		0xa4	/* get DVD key structure */
#define ATAPI_PLAY_12			0xa5	/* play by lba */
#define ATAPI_LOAD_UNLOAD		0xa6	/* changer control command */
#define ATAPI_READ_STRUCTURE		0xaD	/* get DVD structure */
#define ATAPI_PLAY_CD			0xb4	/* universal play command */
#define ATAPI_SET_SPEED			0xBB	/* set drive speed */
#define ATAPI_MECH_STATUS		0xBD	/* get changer status */
#define ATAPI_READ_CD			0xBE	/* read data */
#define ATAPI_POLL_DSC			0xFF	/* poll DSC status bit */
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Parâmetros das Unidades ATA/ATAPI			*
 ****************************************************************
 */
/*
 ******	Valores para "packet_size" ******************************
 */
#define ATAPI_PSIZE_12          0       /* 12 bytes */
#define ATAPI_PSIZE_16          1       /* 16 bytes */

/*
 ******	Valores para "drq_type" *********************************
 */
#define ATAPI_DRQT_MPROC        0       /* cpu    3 ms delay */
#define ATAPI_DRQT_INTR         1       /* intr  10 ms delay */
#define ATAPI_DRQT_ACCEL        2       /* accel 50 us delay */

/*
 ******	Valores para "type" *************************************
 */
#define ATAPI_TYPE_DIRECT       0       /* disk/floppy */
#define ATAPI_TYPE_TAPE         1       /* streaming tape */
#define ATAPI_TYPE_CDROM        5       /* CD-ROM device */
#define ATAPI_TYPE_OPTICAL      7       /* optical disk */

/*
 ******	Valores para "atavalid" *********************************
 */
#define	ATA_FLAG_54_58		1		/* words 54-58 valid */
#define	ATA_FLAG_64_70		2		/* words 64-70 valid */
#define	ATA_FLAG_88		4		/* word 88 valid */

/*
 ****************************************************************
 *	Chipsets						*
 ****************************************************************
 */
/*
 ******	Valores para INTEL **************************************
 */
#define ATA_INTEL_ID            0x8086
#define ATA_I960RM		0x09628086
#define ATA_I82371FB            0x12308086
#define ATA_I82371SB            0x70108086
#define ATA_I82371AB            0x71118086
#define ATA_I82443MX            0x71998086
#define ATA_I82451NX            0x84ca8086
#define ATA_I82372FB            0x76018086
#define ATA_I82801AB            0x24218086
#define ATA_I82801AA            0x24118086
#define ATA_I82801BA            0x244a8086
#define ATA_I82801BA_1          0x244b8086
#define ATA_I82801CA            0x248a8086
#define ATA_I82801CA_1          0x248b8086
#define ATA_I82801DB            0x24cb8086
#define ATA_I82801DB_1          0x24ca8086
#define ATA_I82801EB            0x24db8086
#define ATA_I82801EB_S1         0x24d18086
#define ATA_I82801EB_R1         0x24df8086
#define ATA_I6300ESB            0x25a28086
#define ATA_I6300ESB_S1         0x25a38086
#define ATA_I6300ESB_R1         0x25b08086
#define ATA_I63XXESB2           0x269e8086
#define ATA_I63XXESB2_S1        0x26808086
#define ATA_I63XXESB2_S2        0x26818086
#define ATA_I63XXESB2_R1        0x26828086
#define ATA_I63XXESB2_R2        0x26838086
#define ATA_I82801FB            0x266f8086
#define ATA_I82801FB_S1         0x26518086
#define ATA_I82801FB_R1         0x26528086
#define ATA_I82801FBM           0x26538086
#define ATA_I82801GB            0x27df8086
#define ATA_I82801GB_S1         0x27c08086
#define ATA_I82801GB_AH         0x27c18086
#define ATA_I82801GB_R1         0x27c38086
#define ATA_I82801GBM_S1        0x27c48086
#define ATA_I82801GBM_AH        0x27c58086
#define ATA_I82801GBM_R1        0x27c68086
#define ATA_I82801HB_S1         0x28208086
#define ATA_I82801HB_AH6        0x28218086
#define ATA_I82801HB_R1         0x28228086
#define ATA_I82801HB_AH4        0x28248086
#define ATA_I82801HB_S2         0x28258086
#define ATA_I82801HBM_S1        0x28298086
#define ATA_I82801HBM_S2        0x282a8086
#define ATA_I31244		0x32008086
#define ATA_I82801IB_S1         0x29208086
#define ATA_I82801IB_AH2        0x29218086
#define ATA_I82801IB_AH6        0x29228086
#define ATA_I82801IB_AH4        0x29238086
#define ATA_I82801IB_S2         0x29268086

/*
 ******	Valores para VIA ****************************************
 */
#define ATA_VIA_ID		0x1106
#define ATA_VIA82C571           0x05711106
#define ATA_VIA82C586           0x05861106
#define ATA_VIA82C596           0x05961106
#define ATA_VIA82C686           0x06861106
#define ATA_VIA8231             0x82311106
#define ATA_VIA8233             0x30741106
#define ATA_VIA8233A            0x31471106
#define ATA_VIA8233C            0x31091106
#define ATA_VIA8235             0x31771106
#define ATA_VIA8237             0x32271106
#define ATA_VIA8237A            0x05911106
#define ATA_VIA8251             0x33491106
#define ATA_VIA8361             0x31121106
#define ATA_VIA8363             0x03051106
#define ATA_VIA8371             0x03911106
#define ATA_VIA8662             0x31021106
#define ATA_VIA6410             0x31641106
#define ATA_VIA6420             0x31491106
#define ATA_VIA6421             0x32491106

/*
 ******	Valores para Sis ****************************************
 */
#define ATA_SIS_ID		0x1039
#define ATA_SISSOUTH            0x00081039
#define ATA_SIS5511             0x55111039
#define ATA_SIS5513             0x55131039
#define ATA_SIS5517             0x55171039
#define ATA_SIS5518             0x55181039
#define ATA_SIS5571             0x55711039
#define ATA_SIS5591             0x55911039
#define ATA_SIS5596             0x55961039
#define ATA_SIS5597             0x55971039
#define ATA_SIS5598             0x55981039
#define ATA_SIS5600             0x56001039
#define ATA_SIS530		0x05301039
#define ATA_SIS540		0x05401039
#define ATA_SIS550		0x05501039
#define ATA_SIS620		0x06201039
#define ATA_SIS630		0x06301039
#define ATA_SIS635		0x06351039
#define ATA_SIS633		0x06331039
#define ATA_SIS640		0x06401039
#define ATA_SIS645		0x06451039
#define ATA_SIS646		0x06461039
#define ATA_SIS648		0x06481039
#define ATA_SIS650		0x06501039
#define ATA_SIS651		0x06511039
#define ATA_SIS652		0x06521039
#define ATA_SIS655		0x06551039
#define ATA_SIS658		0x06581039
#define ATA_SIS661		0x06611039
#define ATA_SIS730		0x07301039
#define ATA_SIS733		0x07331039
#define ATA_SIS735		0x07351039
#define ATA_SIS740		0x07401039
#define ATA_SIS745		0x07451039
#define ATA_SIS746		0x07461039
#define ATA_SIS748		0x07481039
#define ATA_SIS750		0x07501039
#define ATA_SIS751		0x07511039
#define ATA_SIS752		0x07521039
#define ATA_SIS755		0x07551039
#define ATA_SIS961		0x09611039
#define ATA_SIS962		0x09621039
#define ATA_SIS963		0x09631039
#define ATA_SIS964		0x09641039
#define ATA_SIS965		0x09651039
#define ATA_SIS180		0x01801039
#define ATA_SIS181		0x01811039
#define ATA_SIS182		0x01821039

/*
 ******	Valores para Jmicron ************************************
 */
#define ATA_JMICRON_ID		0x197B
#define ATA_JMB360		0x2360197B
#define ATA_JMB361		0x2361197B
#define ATA_JMB363		0x2363197B
#define ATA_JMB365		0x2365197B
#define ATA_JMB366		0x2366197B
#define ATA_JMB368		0x2368197B

typedef struct
{
	ulong		ch_id;		/* Identificação: (DEV, VENDOR) */
	char		ch_rev;		/* Revisão */
	int		ch_cfg1;	/* Indicadores */
	int		ch_cfg2;	/* Mais indicadores */
	char		ch_max_dma;	/* DMA máximo suportado */
	char		*ch_nm;		/* Nome do chipset */

}	ATA_CHIP_ID;

/*
 ****** Valores para "ch_cfg1" **********************************
 */
#define	AHCI		1

#define VIA33           0
#define VIA66           1
#define VIA100          2
#define VIA133          3
#define AMDNVIDIA       4

#define AMDCABLE        0x0001
#define AMDBUG          0x0002
#define NVIDIA          0x0004
#define NV4             0x0010
#define NVQ             0x0020
#define VIACLK          0x0100
#define VIABUG          0x0200
#define VIABAR          0x0400
#define VIAAHCI         0x0800

#define SIIMEMIO        1
#define SIIINTR         0x01
#define SIISETCLK       0x02
#define SIIBUG          0x04
#define SII4CH          0x08

#define SIS_SOUTH       1
#define SISSATA         2
#define SIS133NEW       3
#define SIS133OLD       4
#define SIS100NEW       5
#define SIS100OLD       6
#define SIS66           7
#define SIS33           8

/*
 ****** Estrutura retornada pelo comando IDENTIFY ***************
 */
#define	NOATAPARAM	(ATA_PARAM *)0

typedef struct
{
/*000*/	ushort		packet_size	:2;	/* packet command size */

    	ushort		incomplete	:1;
    	ushort		:2;
    	ushort		drq_type	:2;	/* DRQ type */

    	ushort		removable	:1;	/* device is removable */
    	ushort		type		:5;	/* device type */

    	ushort		:2;
    	ushort		cmd_protocol	:1;	/* command protocol */

/*001*/	ushort		cylinders;		/* # of cylinders */
	ushort		reserved2;

/*003*/	ushort		heads;			/* # heads */
	ushort		obsolete4;
	ushort		obsolete5;

/*006*/	ushort		sectors;		/* # sectors/track */
/*007*/	ushort		vendor7[3];
/*010*/	char		serial[20];		/* serial number */
	ushort		retired20;
	ushort		retired21;
	ushort		obsolete22;

/*023*/	char		revision[8];		/* firmware revision */
/*027*/	char		model[40];		/* model name */

/*047*/	ushort		sectors_intr;		/* sectors per interrupt */

/*048*/	ushort		usedmovsd;		/* double word read/write? */

/*049*/	ushort		retired49:8;
	ushort		support_dma	:1;	/* DMA supported */
	ushort		support_lba	:1;	/* LBA supported */
	ushort		disable_iordy	:1;	/* IORDY may be disabled */
	ushort		support_iordy	:1;	/* IORDY supported */
	ushort		softreset	:1;	/* needs softreset when busy */
	ushort		stdby_ovlap	:1;	/* standby/overlap supported */
	ushort		support_queueing:1;	/* supports queuing overlap */
	ushort		support_idma	:1;	/* interleaved DMA supported */

/*050*/	ushort		device_stdby_min:1;
	ushort		:13;
	ushort		capability_one:1;
	ushort		capability_zero:1;

/*051*/	ushort		vendor51:8;
	ushort		retired_piomode:8;	/* PIO modes 0-2 */

/*052*/	ushort		vendor52:8;
	ushort		retired_dmamode:8;	/* DMA modes, not ATA-3 */

/*053*/	ushort		atavalid;		/* fields valid */

	ushort		obsolete54[5];

/*059*/	ushort		multi_count:8;
	ushort		multi_valid:1;
	ushort		:7;

/*060*/	ulong		lba_size;
	ushort		obsolete62;

/*063*/	ushort		mwdmamodes;		/* multiword DMA modes */ 
/*064*/	ushort		apiomodes;		/* advanced PIO modes */ 

/*065*/	ushort		mwdmamin;		/* min. M/W DMA time/word ns */
/*066*/	ushort		mwdmarec;		/* rec. M/W DMA time ns */
/*067*/	ushort		pioblind;		/* min. PIO cycle w/o flow */
/*068*/	ushort		pioiordy;		/* min. PIO cycle IORDY flow */
	ushort		reserved69;
	ushort		reserved70;
/*071*/	ushort		rlsovlap;		/* rel time (us) for overlap */
/*072*/	ushort		rlsservice;		/* rel time (us) for service */
	ushort		reserved73;
	ushort		reserved74;

/*075*/	ushort		queuelen:5;
	ushort		:11;

/*076*/	ushort		satacapabilities;
	ushort		reserved77;
/*078*/	ushort		satasupport;
/*079*/	ushort		sataenabled;
/*080*/	ushort		version_major;
/*081*/	ushort		version_minor;

	struct
	{
/*082/085*/	ushort	smart:1;
		ushort	security:1;
		ushort	removable:1;
		ushort	power_mngt:1;
		ushort	packet:1;
		ushort	write_cache:1;
		ushort	look_ahead:1;
		ushort	release_irq:1;
		ushort	service_irq:1;
		ushort	reset:1;
		ushort	protected:1;
		ushort	:1;
		ushort	write_buffer:1;
		ushort	read_buffer:1;
		ushort	nop:1;
		ushort	:1;

/*083/086*/	ushort	microcode:1;
		ushort	queued:1;
		ushort	cfa:1;
		ushort	apm:1;
		ushort	notify:1;
		ushort	standby:1;
		ushort	spinup:1;
		ushort	:1;
		ushort	max_security:1;
		ushort	auto_acoustic:1;
		ushort	address48:1;
		ushort	config_overlay:1;
		ushort	flush_cache:1;
		ushort	flush_cache48:1;
		ushort	support_one:1;
		ushort	support_zero:1;

/*084/087*/	ushort	smart_error_log:1;
		ushort	smart_self_test:1;
		ushort	media_serial_no:1;
		ushort	media_card_pass:1;
		ushort	streaming:1;
		ushort	logging:1;
		ushort	:8;
		ushort	extended_one:1;
		ushort	extended_zero:1;

	}	support, enabled;

/*088*/	ushort		udmamodes;		/* UltraDMA modes */
/*089*/	ushort		erase_time;
/*090*/	ushort		enhanced_erase_time;
/*091*/	ushort		apm_value;
/*092*/	ushort		master_passwd_revision;

/*093*/	ushort		hwres_master	:8;
	ushort		hwres_slave	:5;
	ushort		hwres_cblid	:1;
	ushort		hwres_valid	:2;

/*094*/	ushort		current_acoustic:8;
	ushort		vendor_acoustic:8;

/*095*/	ushort		stream_min_req_size;
/*096*/	ushort		stream_transfer_time;
/*097*/	ushort		stream_access_latency;
/*098*/	ulong		stream_granularity;
/*100*/	ushort		lba_size48_1;
	ushort		lba_size48_2;
	ushort		lba_size48_3;
	ushort		lba_size48_4;
	ushort		reserved104[23];
/*127*/	ushort		removable_status;
/*128*/	ushort		security_status;
	ushort		reserved129[31];
/*160*/	ushort		cfa_powermode1;
	ushort		reserved161[14];
/*176*/	ushort		media_serial[30];
	ushort		reserved206[49];
/*255*/	ushort		integrity;

}	ATA_PARAM;

/*
 ****************************************************************
 *	Complemento aos dados de um pedido			*
 ****************************************************************
 */
#define	NOATASCB	(ATA_SCB *)NULL

typedef struct atascb	ATA_SCB;

struct atascb
{
	int		scb_step;		/* Fase do pedido */
	int		scb_flags;		/* Indicadores (os mesmos de b_flags em BHEAD) */
	int		scb_max_pio_sz;		/* No. máximo de bytes que pode ser transferido */
	int		scb_result;		/* Resultado da operação */

	/*
	 *	Dados do pedido total
	 */
	daddr_t		scb_blkno;		/* No. de bloco */
	void		*scb_area;		/* Endereço da área */
	int		scb_count;		/* No. total de bytes a transferir */

	/*
	 *	Dados do fragmento corrente
	 */
	int		scb_bytes_requested;	/* No. de bytes requisitados  */
	int		scb_bytes_transferred;	/* No. de bytes efetivamente transferidos */

	/*
	 *	Para os dispositivos ATAPI apenas
	 */
	char		*scb_cmd;		/* Comando SCSI corrente */

	struct scsi_sense_data	scb_sense;	/* Resultado do REQUEST SENSE */

#if (0)	/********************************************************/
	/*
	 *	Campos comuns
	 */
	ATA_SCB		*scb_next;	/* Próximo SCB livre */
#endif	/********************************************************/
};

/*
 ****************************************************************
 *	Dados de um Dispositivo					*
 ****************************************************************
 */
#define	NOATACONTROLLER		(ATA_CHANNEL *)NULL
#define	NOATADEVICE		(ATA_DEVICE *)NULL

#define	ATA_DEVNO		adp->ad_channel->ch_index, adp->ad_devno

typedef struct ata_channel	ATA_CHANNEL;
typedef struct ata_device	ATA_DEVICE;

/*
 ******	Valores para "ad_type" **********************************
 */
enum
{
	ATA_HD		= 1,		/* Disco Rígido */
	ATAPI_CDROM,			/* CDROM ATAPI */
	ATAPI_ZIP			/* ZIP 100 ATAPI */
};

/*
 ******	Valores para "ad_flags" *********************************
 */
enum
{
	HD_CHS_USED	= 0x01,		/* O dispositivo deve usar o método geométrico */
	ATAPI_DEV	= 0x02,		/* O dispositivo é ATAPI */
	HD_INT13_EXT	= 0x04		/* O dispositivo aceita as extensões da INT 13 */
};

/*
 ******	Informações sobre um dispositivo alvo ********************
 */
struct ata_device
{
	ATA_CHANNEL	*ad_channel;	/* Ponteiro para a estrutura do canal */
	int		ad_devno;	/* Posição no canal (0 = mestre, 1 = escravo) */

	char		ad_dev_nm[4];	/* Nome do dispositivo */

	int		ad_type;	/* Tipo de periférico (ver acima) */
	int		ad_flags;	/* Indicadores (ver acima) */

	char		ad_error;	/* Último erro */
	char		ad_status;	/* Último estado */

	int		ad_head;	/* No. de cabeças   (REAL) */
	int		ad_sect;	/* No. de setores   (REAL) */
	int		ad_cyl;		/* No. de cilindros (REAL) */

	int		ad_max_pio_sz;	/* No. máx. de bytes que pode ser transferido por PIO de uma vez */

	int		ad_blsz;	/* Tamanho de um setor (em bytes) */
	int		ad_blshift;	/* Log (2) do tamanho acima */

	daddr_t		ad_disksz;	/* No. total de blocos */

	ATA_PARAM	ad_param;	/* Parâmetros (obtidos com IDENTIFY) */

	/*
	 *	PIO de 16 ou 32 bits
	 */
	void		(*ad_read_port)  (int, void *, int);
	void		(*ad_write_port) (int, const void *, int);
	int		ad_pio_shift;

	/*
	 *	Somente para dispositivos ATAPI
	 */
	int		ad_cmdsz;	/* Tamanho do comando SCSI (em shorts) */
	SCSI		*ad_scsi;	/* Parâmetros do SCSI */

#ifdef	BOOT
	/*
	 *	Parte utilizada apenas pelo BOOT2
	 */
	int		ad_bios_head;	/* No. de cabeças   (BIOS) */
	int		ad_bios_sect;	/* No. de setores   (BIOS) */
	int		ad_bios_cyl;	/* No. de cilindros (BIOS) */

	const void	*ad_disktb;	/* Ponteiro para DISKTB */
#else
	/*
	 *	Parte utilizada apenas pelo KERNEL
	 */
	int		ad_pio_mode;	/* PIO mode */
	int		ad_wdma_mode;	/* Multi-word DMA mode */
	int		ad_udma_mode;	/* Ultra-DMA mode */

	int		ad_transfer_mode;

	BHEAD		*ad_bp;		/* Pedido corrente */
	DEVHEAD		ad_queue;	/* Cabeça da lista de pedidos */
#endif
};

/*
 ****************************************************************
 *	Dados de um Controlador					*
 ****************************************************************
 */
#ifdef	PCI_H
typedef struct
{
	int			ac_index;
	int			ac_pci_name_written;

	const char		*ac_chip_nm;
	const ATA_CHIP_ID	*ac_idp;
	char			*ac_pci_name;

	int			ac_nchannels;
	int			ac_legacy;

	int			(*ac_chipinit) (PCIDATA *);
	int			(*ac_reset) (PCIDATA *, ATA_CHANNEL *);
	int			(*ac_allocate) (PCIDATA *, ATA_CHANNEL *);
	void			(*ac_setmode) (PCIDATA *, ATA_DEVICE *, int);

}	ATA_PCI_CTLR;
#endif

/*
 ****************************************************************
 *	Dados de um Canal					*
 ****************************************************************
 */
/*
 ******	Valores de "ch_flags" ***********************************
 */
enum
{
	ATA_NO_SLAVE		= 0x01,
	ATA_USE_16BIT           = 0x02,
	ATA_ATAPI_DMA_RO        = 0x04,
	ATA_NO_48BIT_DMA        = 0x08,
	ATA_ALWAYS_DMASTAT      = 0x10,
	ATA_LEGACY		= 0x20
};

#ifdef	BOOT
struct ata_channel
{
	ATA_PCI_CTLR	*ch_ctlrp;
	PCIDATA		*ch_pci;

	char		ch_flags;		/* Indicadores (ver acima) */
	char		ch_ndevs;		/* Número de dispositivos anexados */

	ATA_DEVICE	*ch_dev[NATAT];		/* Os dispositivos */

	int		ch_unit;		/* Posição do canal no controlador (0, 1, 2, 3) */
	int		ch_index;		/* Índice deste canal em "ata_channel" (0, ..., NATAC-1) */

	int		ch_ports[ATA_MAX_RES];	/* Portas */

	int		(*ch_status) (ATA_CHANNEL *);
};
#else
struct ata_channel
{
	ATA_PCI_CTLR	*ch_ctlrp;
	PCIDATA		*ch_pci;

	LOCK		ch_busy;		/* Canal ocupado */

	char		ch_flags;		/* Indicadores (ver acima) */
	char		ch_ndevs;		/* Número de dispositivos anexados */

	ATA_DEVICE	*ch_dev[NATAT];		/* Os dispositivos */

	int		ch_unit;		/* Posição do canal no controlador (0, 1, 2, 3) */
	int		ch_index;		/* Índice deste canal em "ata_channel" (0, ..., NATAC-1) */
	int		ch_irq;			/* No. do IRQ */

	int		ch_ports[ATA_MAX_RES];	/* Portas */

	int		(*ch_status) (ATA_CHANNEL *);
	int		(*ch_begin_transaction) (ATA_DEVICE *, ATA_SCB *);
	int		(*ch_end_transaction) (ATA_DEVICE *, ATA_SCB *);

	ATA_DEVICE	*ch_active_dev;		/* Dispositivo ativo no momento */
	int		ch_target;		/* O dispositivo ativo no momento */

	void		*ch_dma_area;		/* Área para programação do DMA */

	ATA_SCB		*ch_scb;		/* Lista de SCBs disponíveis */
};
#endif	BOOT

/*
 ****************************************************************
 *	Protótipos de Funções					*
 ****************************************************************
 */
void			pci_ata_attach_2 (void);
int			atastrategy (BHEAD *);
int			ata_begin_transaction (ATA_DEVICE *, ATA_SCB *);
int			ata_end_transaction (ATA_DEVICE *, ATA_SCB *);

void			ata_print_dev_info (const ATA_DEVICE *);
int			ata_old_find_devices (ATA_CHANNEL *);
int			ata_new_find_devices (ATA_CHANNEL *);
int			ata_find_lba48_equivalent (int);
int			ata_command (ATA_DEVICE *, int, daddr_t, int, int, int);
int			ata_wait (ATA_DEVICE *, int);

#ifdef	PCI_H
void			ata_create_channels (PCIDATA *, ATA_PCI_CTLR *, int, int, int, int);
int			ata_legacy (PCIDATA *);
int			ata_pci_allocate (PCIDATA *, ATA_CHANNEL *);
#endif

int			ata_check_80pin (ATA_DEVICE *, int);
void			ata_default_registers (ATA_CHANNEL *);

#ifdef	BOOT
int			atapi_start (ATA_DEVICE *, ATA_SCB *);
#else
void			atapi_start (ATA_DEVICE *);
#endif

int			atapi_internal_cmd (BHEAD *);
int			atapi_command (ATA_DEVICE *, ATA_SCB *);
int			atapi_next_phase (ATA_DEVICE *, ATA_SCB *);

#ifdef	PCI_H
void			ata_sata_setmode (PCIDATA *, ATA_DEVICE *, int);
void			ata_pata_dma_init (PCIDATA *, ATA_DEVICE *);
#endif

void			ata_dma_reset (ATA_CHANNEL *);
char			*ata_dmamode_to_str (int);
int			ata_limit_mode (ATA_DEVICE *, int, int);
int			ata_dma_load (ATA_DEVICE *, ATA_SCB *);
void			ata_dma_start (ATA_CHANNEL *, int);
int			ata_dma_stop (ATA_DEVICE *);
int			ata_mode2idx (int);


#ifdef	PCI_H
int			ata_generic_reset (PCIDATA *, ATA_CHANNEL *);

const ATA_CHIP_ID	*ata_intel_ident (PCIDATA *, ulong);
int			ata_intel_chipinit (PCIDATA *);

const ATA_CHIP_ID	*ata_jmicron_ident (PCIDATA *, ulong);
int			ata_jmicron_chipinit (PCIDATA *);

const ATA_CHIP_ID	*ata_via_ident (PCIDATA *, ulong);
int			ata_via_chipinit (PCIDATA *);

const ATA_CHIP_ID	*ata_sis_ident (PCIDATA *, ulong);
int			ata_sis_chipinit (PCIDATA *);

int			ata_ahci_chipinit (PCIDATA *);
#endif

#ifdef	BOOT
int			atapi_process_phases (ATA_DEVICE *, ATA_SCB *);
#endif
