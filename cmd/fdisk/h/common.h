/*
 ****************************************************************
 *								*
 *			common.h				*
 *								*
 *	Altera a tabela de partição de discos	 		*
 *								*
 *	Versão	4.6.0, de 11.09.04				*
 *		4.6.0, de 11.09.04				*
 *								*
 *	Módulo: fdisk						*
 *		Utilitários Sofisticados			*
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
#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

extern int	exit_code;	/* Código de retorno */

extern int	vflag;		/* Verbose */

/*
 ******	Definições úteis para a tabela de partições *************
 */
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
 ******	Informação sobre os discos IDE **************************
 */
typedef struct
{
	int	h_head;		/* No. de cabeças  (REAL) */
	int	h_sect;		/* No. de setores  (REAL) */
	int	h_cyl;		/* No. de cilindros (REAL) */

	int	h_flags;	/* Indicadores (ver abaixo) */

}	HDINFO;

#define NOHDINFO (HDINFO *)0	/* Ponteiro NULO */

#define HD_INT13_EXT	0x04 	/* O disco aceita as extensões da INT 13 */

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

#define	CYLMAX(c)	((c) > 1023 ? 1023 : (c))

typedef struct
{
	int	f_reg_index;	/* No. da partição regular */
	int	f_log_index;	/* No. da partição lógica */
	PARTTB	*f_pp;		/* Ponteiro para a entrada */

}	PARTINFO;

enum	{ IDE_TYPE, SCSI_TYPE };	/* Tipo do disco */

extern char		fdisk_type;	/* Tipo: IDE ou SCSI */
extern const void	*fdisk_data;	/* Estrutura ATA ou SCSI */
extern const DISKTB	*fdisk_disktb;	/* Entrada do disco "hda" ou "hdb" */
extern const HDINFO	*fdisk_hdinfo;	/* Entrada de parâmetros do disco */
extern daddr_t		fdisk_size;	/* Tamanho do disco */
extern daddr_t		fdisk_round;	/* Para arrendondar os endereços */

extern int		fdisk_modified;	/* NÃO zero se modificada */
extern int		fdisk_debug;	/* Debug ? */
extern int		fdisk_align;	/* Aligned ? */

/*
 ****** Definições e variáveis da tabela de partições ***********
 */
#if (0)	/*******************************************************/
extern PARTTB	parttb[NPART];		/* A tabela de partições regular */
extern PARTTB	log_parttb[NPART][NLOGPART]; /* Idem, lógicas */

extern daddr_t	fdisk_log_offset[NPART][NLOGPART]; /* Deslo. das par. lógicas */
#endif	/*******************************************************/

/*
 ******	Tabela "fictícia" para os discos IDE ou  SCSI ***********
 */
extern HDINFO	generic_hdinfo;	/* Genérico */

/*
 ******	Protótipos de funções ***********************************
 */
int		geo_get_parttb_geo (const PARTTB *, HDINFO *gp, daddr_t disksz, const char *dev_nm, int);
