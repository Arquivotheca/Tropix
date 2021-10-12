/*
 ****************************************************************
 *								*
 *			<sys/scsi.h>				*
 *								*
 *	Estruturas comuns da gerência de dispositivos SCSI	*
 *								*
 *	Versão	3.0.5, de 23.02.97				*
 *		4.3.0, de 31.08.02				*
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

#define	SCSI_H

/*
 ****************************************************************
 *	Informações sobre um dispositivo SCSI			*
 ****************************************************************
 */
#define	NOSCSI		(SCSI *)NULL

typedef struct scsi	SCSI;

/*
 ******	Estrutura SCSI utilizada pelo KERNEL ********************
 */
struct scsi
{
	char		*scsi_dev_nm;		/* Nome do dispositivo */

	char		scsi_is_present;	/* > 0: presente, < 0: ausente, == 0: não sabe */
	char		scsi_is_atapi;		/* É um dispositivo ATAPI */
	char		scsi_is_disk;		/* Indica se é um disco */
	char		scsi_is_tape;		/* Indica se é uma fita */

	char		scsi_read_only;		/* Indica se é READ-ONLY */
	char		scsi_removable;		/* Indica se o meio é removível */
	char		scsi_tagenable;		/* Permite tag queueing */
	char		scsi_syncrate;		/* Velocidade síncrona */

	char		scsi_medium_changed;	/* Foi trocado o meio */
	char		scsi_company[9];	/* Fabricante do dispositivo */
	char		scsi_product[17];	/* Nome do produto */
	char		scsi_revision[5];	/* No. da versão */

	int		scsi_disksz;		/* No. de blocos */
	int		scsi_blsz;		/* Tamanho do bloco */
	int		scsi_blshift;		/* Log (2) do tamanho acima */

	int		scsi_nopen;		/* No. de OPENs */

	/*
	 *	A função para executar um comando
	 */
	int		(*scsi_cmd) (BHEAD *);

	/*
	 *	Parte adicional para o BOOT
	 */
#ifdef	BOOT
	int		scsi_bios_head;		/* Nos. de cabeças, setores e cilindros */
	int		scsi_bios_sect;		/* (de fantasia) */
	int		scsi_bios_cyl;

	int		scsi_target;		/* O número do dispositivo */
	void		*scsi_disktb;		/* Ponteiro para DISKTB */

	char		scsi_int13_ext;		/* Aceita as extensões da INT 13 */
	char		scsi_no_parttb;		/* Não possui tabela de partições */
#endif

};

/*
 ****************************************************************
 *	Comandos SCSI						*
 ****************************************************************
 */
#define SCSI_CMD_TEST_UNIT_READY	0x00    /* check if the device is ready */
#define SCSI_CMD_REZERO_UNIT		0x01    /* reinit device */
#define SCSI_CMD_REQUEST_SENSE		0x03    /* get sense data */
#define	SCSI_CMD_GET_PROTECTION		0x06	/* get protection */
#define	SCSI_CMD_SET_PROTECTION		0x0C	/* set protection */
#define	SCSI_CMD_INQUIRY		0x12	/* get parameters */
#define SCSI_CMD_START_STOP		0x1B    /* start/stop the media */
#define SCSI_CMD_PREVENT_ALLOW		0x1E    /* prevent/allow media removal */
#define SCSI_CMD_READ_CAPACITY		0x25    /* get volume capacity */
#define SCSI_CMD_READ_BIG		0x28    /* read data */
#define SCSI_CMD_WRITE_BIG		0x2A    /* write data */
#define SCSI_CMD_SYNCHRONIZE_CACHE	0x35    /* flush write buf, close write chan */
#define SCSI_CMD_READ_SUBCHANNEL	0x42    /* get subchannel info */
#define SCSI_CMD_READ_TOC		0x43    /* get table of contents */
#define SCSI_CMD_PLAY_MSF		0x47    /* play by MSF address */
#define SCSI_CMD_PLAY_TRACK		0x48    /* play by track number */
#define SCSI_CMD_PAUSE_RESUME		0x4B    /* stop/start audio operation */
#define SCSI_CMD_READ_TRACK_INFO	0x52    /* get track information structure */
#define SCSI_CMD_MODE_SELECT		0x55    /* set device parameters */
#define SCSI_CMD_MODE_SENSE		0x5A    /* get device parameters */
#define SCSI_CMD_CLOSE_TRACK		0x5B    /* close track/session */
#define SCSI_CMD_PLAY_BIG		0xA5    /* play by logical block address */
#define SCSI_CMD_LOAD_UNLOAD		0xA6    /* changer control command */
#define SCSI_CMD_PLAY_CD		0xB4    /* universal play command */
#define SCSI_CMD_MECH_STATUS		0xBD    /* get changer mechanism status */
#define SCSI_CMD_READ_ATAPI_CD		0xBE    /* read data (ATAPI) */
#define SCSI_CMD_READ_CD		0xD8    /* read data */

/*
 ****************************************************************
 *	Código do estado					*
 ****************************************************************
 */
#define	SCSI_STATUS_OK			0x00
#define	SCSI_STATUS_CHECK_COND		0x02
#define	SCSI_STATUS_COND_MET		0x04
#define	SCSI_STATUS_BUSY		0x08
#define SCSI_STATUS_INTERMED		0x10
#define SCSI_STATUS_INTERMED_COND_MET	0x14
#define SCSI_STATUS_RESERV_CONFLICT	0x18
#define SCSI_STATUS_CMD_TERMINATED	0x22
#define SCSI_STATUS_QUEUE_FULL		0x28

/*
 ****************************************************************
 *	Estrutura de retorno do pedido de "sense_data"		*
 ****************************************************************
 */
struct scsi_sense_data
{
	uchar		error_code;
	uchar		segment;
	uchar		flags;
	uchar		info[4];
	uchar		extra_len;
	uchar		cmd_spec_info[4];
	uchar		add_sense_code;
	uchar		add_sense_code_qual;
	uchar		fru;
	uchar		sense_key_spec[3];
	uchar		extra_bytes[14];
};

/*
 ****************************************************************
 *	Códigos das mensagens SCSI				*
 ****************************************************************
 */

/* Mensagens (1 byte) */	     /* I/T (M)andatory or (O)ptional */

#define MSG_CMDCOMPLETE		0x00 /* M/M */
#define MSG_TASK_COMPLETE	0x00 /* M/M */ /* SPI3 Terminology */
#define MSG_EXTENDED		0x01 /* O/O */
#define MSG_SAVEDATAPOINTER	0x02 /* O/O */
#define MSG_RESTOREPOINTERS	0x03 /* O/O */
#define MSG_DISCONNECT		0x04 /* O/O */
#define MSG_INITIATOR_DET_ERR	0x05 /* M/M */
#define MSG_ABORT		0x06 /* O/M */
#define MSG_ABORT_TASK_SET	0x06 /* O/M */ /* SPI3 Terminology */
#define MSG_MESSAGE_REJECT	0x07 /* M/M */
#define MSG_NOOP		0x08 /* M/M */
#define MSG_PARITY_ERROR	0x09 /* M/M */
#define MSG_LINK_CMD_COMPLETE	0x0A /* O/O */
#define MSG_LINK_CMD_COMPLETEF	0x0B /* O/O */
#define MSG_BUS_DEV_RESET	0x0C /* O/M */
#define MSG_TARGET_RESET	0x0C /* O/M */ /* SPI3 Terminology */
#define MSG_ABORT_TAG		0x0D /* O/O */
#define MSG_ABORT_TASK		0x0D /* O/O */ /* SPI3 Terminology */
#define MSG_CLEAR_QUEUE		0x0E /* O/O */
#define MSG_CLEAR_TASK_SET	0x0E /* O/O */ /* SPI3 Terminology */
#define MSG_INIT_RECOVERY	0x0F /* O/O */ /* Deprecated in SPI3 */
#define MSG_REL_RECOVERY	0x10 /* O/O */ /* Deprecated in SPI3 */
#define MSG_TERM_IO_PROC	0x11 /* O/O */ /* Deprecated in SPI3 */
#define MSG_CLEAR_ACA		0x16 /* O/O */ /* SPI3 */
#define MSG_LOGICAL_UNIT_RESET	0x17 /* O/O */ /* SPI3 */
#define MSG_QAS_REQUEST		0x55 /* O/O */ /* SPI3 */

/* Mensagens (2 bytes) */

#define MSG_SIMPLE_Q_TAG	0x20 /* O/O */
#define MSG_SIMPLE_TASK		0x20 /* O/O */ /* SPI3 Terminology */
#define MSG_HEAD_OF_Q_TAG	0x21 /* O/O */
#define MSG_HEAD_OF_QUEUE_TASK	0x21 /* O/O */ /* SPI3 Terminology */
#define MSG_ORDERED_Q_TAG	0x22 /* O/O */
#define MSG_ORDERED_TASK	0x22 /* O/O */ /* SPI3 Terminology */
#define MSG_IGN_WIDE_RESIDUE	0x23 /* O/O */
#define MSG_ACA_TASK		0x24 /* 0/0 */ /* SPI3 */

/* Mensagem de Identificação */		     /* M/M */	

#define MSG_IDENTIFYFLAG	0x80 
#define MSG_IDENTIFY_DISCFLAG	0x40 
#define MSG_IDENTIFY(lun, disc)	(((disc) ? 0xc0 : MSG_IDENTIFYFLAG) | (lun))
#define MSG_ISIDENTIFY(m)	((m) & MSG_IDENTIFYFLAG)
#define MSG_IDENTIFY_LUNMASK	0x3F 

/* Mensagens estendidas (código e comprimento) */

#define MSG_EXT_SDTR		0x01
#define MSG_EXT_SDTR_LEN	0x03

#define MSG_EXT_WDTR		0x03
#define MSG_EXT_WDTR_LEN	0x02
#define MSG_EXT_WDTR_BUS_8_BIT	0x00
#define MSG_EXT_WDTR_BUS_16_BIT	0x01
#define MSG_EXT_WDTR_BUS_32_BIT	0x02 /* Deprecated in SPI3 */

#define MSG_EXT_PPR		0x04 /* SPI3 */
#define MSG_EXT_PPR_LEN		0x06
#define MSG_EXT_PPR_QAS_REQ	0x04
#define MSG_EXT_PPR_DT_REQ	0x02
#define MSG_EXT_PPR_IU_REQ	0x01
