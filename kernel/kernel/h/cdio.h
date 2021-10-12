/*
 ****************************************************************
 *								*
 *			cdio.h					*
 *								*
 *	Definições e Estruturas para tratamento de CDROMs	*
 *								*
 *	Versão	3.2.3, de 29.02.00				*
 *		4.3.0, de 29.06.02				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

/*
 ****************************************************************
 *	Estrutura da TOC (Table of Contents)			*
 ****************************************************************
 */
#define	CD_RAW_BLSZ	2352		/* Tamanho real do bloco */

#define	MAXTRK		((BLSZ - sizeof (TOC_HEADER)) / sizeof (TOC_ENTRY) - 1)

typedef struct
{
	ushort		len;
	uchar		starting_track;
	uchar		ending_track;

}	TOC_HEADER;

union msf_lba
{
	struct
	{
		uchar   unused;
		uchar   minute;
		uchar   second;
		uchar   frame;

	}	msf;

	int		lba;
	uchar		addr[4];
};

typedef struct
{
	char		reserved1;

	uint		control:4;
	uint		addr_type:4;

	char		track;

	char		reserved2;

	union msf_lba   addr;

}	TOC_ENTRY;

typedef struct
{
	TOC_HEADER	hdr;
	TOC_ENTRY	tab[MAXTRK + 1];

}	TOC;

/*
 ****************************************************************
 *	Diversas estruturas relacionadas a "sub-channel"	*
 ****************************************************************
 */
struct cd_sub_channel_header
{
	char		reserved;
	uchar		audio_status;
	uchar		data_len[2];
};

/*
 *	Valores para "audio_status"
 */
#define CD_AS_AUDIO_INVALID        0x00
#define CD_AS_PLAY_IN_PROGRESS     0x11
#define CD_AS_PLAY_PAUSED          0x12
#define CD_AS_PLAY_COMPLETED       0x13
#define CD_AS_PLAY_ERROR           0x14
#define CD_AS_NO_STATUS            0x15

struct cd_sub_channel_position_data
{
	uchar		data_format;
	uint		control:4;
	uint		addr_type:4;
	uchar		track_number;
	uchar		index_number;
	union msf_lba   absaddr;
	union msf_lba   reladdr;
};

struct cd_sub_channel_media_catalog
{
	uchar		data_format;
	uint:8;
	uint:8;
	uint:8;
	uint:7;
	uint		mc_valid:1;
	uchar		mc_number[15];
};

struct cd_sub_channel_track_info
{
	uchar		data_format;
	uint:8;
	uchar		track_number;
	uint:8;
	uint:7;
	uint		ti_valid:1;
	uchar		ti_number[15];
};

struct cd_sub_channel_info
{
	struct cd_sub_channel_header header;

	union
	{
		struct cd_sub_channel_position_data	position;
		struct cd_sub_channel_media_catalog	media_catalog;
		struct cd_sub_channel_track_info	track_info;

	}	what;
};

/*
 ****************************************************************
 *	Códigos de IOCTL para o Driver				*
 ****************************************************************
 */
#define	CD_READ_TOC		(('C' << 8) | 0)
#define	CD_PLAY_BLOCKS		(('C' << 8) | 1)
#define CD_READ_AUDIO		(('C' << 8) | 2)
#define	CD_START_STOP		(('C' << 8) | 3)
#define	CD_LOCK_UNLOCK		(('C' << 8) | 4)
#define	CD_PAUSE_RESUME		(('C' << 8) | 5)
#define CD_READ_SUBCHANNEL	(('C' << 8) | 6)

/*
 ****************************************************************
 *	Estrutura para o comando READ TOC			*
 ****************************************************************
 */
struct ioc_read_toc
{
	void		*addr;
	int		len;
};

/*
 ****************************************************************
 *	Estrutura para os comandos PLAY				*
 ****************************************************************
 */
struct ioc_play_track
{
	uchar		start_track;
	uchar		start_index;
	uchar		end_track;
	uchar		end_index;
};

struct ioc_play_blocks
{
	int		blk;
	int             len;
};

struct ioc_play_msf
{
	uchar		start_m;
	uchar		start_s;
	uchar		start_f;
	uchar		end_m;
	uchar		end_s;
	uchar		end_f;
};

/*
 ****************************************************************
 *	Estrutura para o comando READ SUBCHANNEL		*
 ****************************************************************
 */
#define CD_LBA_FORMAT		1
#define CD_MSF_FORMAT		2

#define CD_SUBQ_DATA		0
#define CD_CURRENT_POSITION	1
#define CD_MEDIA_CATALOG	2
#define CD_TRACK_INFO		3

struct ioc_read_subchannel
{
	uchar		address_format;
	uchar		data_format;
	uchar		track;
	int		data_len;
	struct cd_sub_channel_info *data;
};

struct subchan
{
	uchar		void0;
	uchar		audio_status;
	ushort		data_length;
	uchar		data_format;
	uchar		control;
	uchar		track;
	uchar		indx;
	ulong		abslba;
	ulong		rellba;

};

/*
 ****************************************************************
 *	Estrutura para o comando READ AUDIO			*
 ****************************************************************
 */
struct ioc_read_audio
{
	int		lba;
	int             nframes;
	void		*buf;
};
