/*
 ****************************************************************
 *								*
 *			common.h				*
 *								*
 *	Definições comuns a todos os módulos			*
 *								*
 *	Versão	3.0.0, de 28.07.94				*
 *		4.6.0, de 22.04.04				*
 *								*
 *	Módulo: Boot2						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define	MAKEDEV(x,y)	(dev_t)((x) << 8 | (y))
#define	BLtoMB(x)	((unsigned)(x) >> 11)

#define	MBSHIFT		20

/*
 *** Número máximo de alvos para todos os controladores SCSI ****
 */
#define	MAX_TARGETs	26	/* Lembrar das letras 'a' a 'z'! */

/*
 ****** Uma entrada da tabela de partições **********************
 */
typedef struct
{
	char	head;		/* Cabeça */
	char	sect;		/* Setor */
	char	cyl;		/* Cilindro */

}	TRIO;

typedef struct			/* Tabela de partições */
{
	char	pt_active;	/* Entrada ativa se 0x80 */
	TRIO	pt_start;	/* Trio inicial */
	char	pt_type;	/* Tipo do sistema de arquivos */
	TRIO	pt_end;		/* Trio final */
	long	pt_offset;	/* Deslocamento (blocos) */
	long	pt_size;	/* Tamanho (blocos) */

}	PARTTB;

#define	NOTRIO	 (TRIO *)0	/* Ponteiro NULO */
#define NOPARTTB (PARTTB *)0	/* Ponteiro NULO */

#define CYLMAX(c)	((c) > 1023 ? 1023 : (c)) 

/*
 ******	Definições úteis para a tabela de partições *************
 */
enum { fdisk_parttb_orig_sect = 0 }; /* Endereços da tabelas de partições regular */

#define	NPART	 4		/* Número total de partições reg. */
#define	NLOGPART ('z'-'a'+ 1)	/* Número máximo de partições lógicas */

#define PARTB_OFF (512 - 2 - 4 * 16)	/* Início da tabela no setor */

#define MAGIC_OFF (512 - 2)	/* Endereço do "0xAA55" */

/*
 ******	Códigos de algumas partições ****************************
 */
#define	DOS_EXT_G	0x05	/* Código da partição estendida DOS (Geo) */
#define	DOS_EXT_L	0x0F	/* Código da partição estendida DOS (Linear) */
#define	TROPIX_EXT	0xAE	/* Código da partição estendida TROPIX */

#define	TROPIX_V7	0xA8	/* Código da partição TROPIX V7 */
#define	TROPIX_T1	0xA9	/* Código da partição TROPIX T1 */

#define	DOS_FAT16_G	0x06	/* Código da partição FAT16 DOS (Geo) */
#define	DOS_FAT16_L	0x0E	/* Código da partição FAT16 DOS (Linear) */

#define	DOS_FAT32_G	0x0B	/* Código da partição FAT32 DOS (Geo) */
#define	DOS_FAT32_L	0x0C	/* Código da partição FAT32 DOS (Linear) */

#define	IS_EXT(t)	(t == DOS_EXT_G || t == DOS_EXT_L || t == TROPIX_EXT)
#define	IS_DOS_L(t)	(t == DOS_EXT_L || t == DOS_FAT16_L || t == DOS_FAT32_L)
#define	IS_DOS_G(t)	(t == DOS_EXT_G || t == DOS_FAT16_G || t == DOS_FAT32_G)

/*
 ******	Tabela de nomes de partições ****************************
 */
typedef struct
{
	int	pt_type;	/* Tipo do sistema de arquivos */
	char	*pt_nm;		/* Nome do sistema de arquivos */

}	PARTNM;

/*
 ******	Tabela de informações dos discos (do 16 bits) ***********
 */
typedef struct
{
	char	info_present;	/* O disco está presente */
	char	info_int13ext;	/* Aceita as extensões da INT 13 */
	char	info_nsect;	/* Número de setores */
	char	info_nhead;	/* Número de cabeças */

}	DISK_INFO;

extern DISK_INFO	disk_info_table[];
extern DISK_INFO	*disk_info_table_ptr;

#if (0)	/*******************************************************/
/*
 ******	Estrutura retornada pela INT 13 4B **********************
 */
typedef struct
{
	ulong	b_status;

	uchar	b_size;
	uchar	b_media;
	uchar	b_drive;
	uchar	b_ctrl_index;

	ulong	b_lba;

	ushort	b_drive_spec;
	ushort	b_area_seg;
	ushort	b_load_seg;
	ushort	b_sector_cnt;

	uchar	b_res[4];

}	BOOTINFO;

extern BOOTINFO			boot_info;
#endif	/*******************************************************/

/*
 ******	Tamanhos particulares de ZIP 100 MB *********************
 */
#define	ZIP_REG_CAPACITY	196608
#define	ZIP_FLOPPY_CAPACITY	196576
