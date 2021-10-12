/*
 ****************************************************************
 *								*
 *			dos.h					*
 *								*
 *	Definições do formato do sistema de arquivos DOS	*
 *								*
 *	Versão	3.0.0, de 29.10.93				*
 *		3.1.8, de 22.04.99				*
 *								*
 *	Módulo: DOSMP						*
 *		Utilitários Sofisticados			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 		Inspirado no PLDOS de Norival Ribeiro Figueira	*
 * 								*
 ****************************************************************
 */


/*
 ******	Bloco de informações universais *************************
 */
typedef struct
{
	int	u_sectors_per_track;	/* No. de setores por trilha */
	int	u_bytes_per_sector;	/* No. de bytes por setor */
	int	u_n_heads;		/* No. de cabeças do disco */
	int	u_fat1_blkno;		/* Início do FAT 1 */
	int	u_end_fat1_blkno;	/* Final do FAT 1 */
	int	u_fat2_blkno;		/* Início do FAT 2 */
	int	u_end_fat2_blkno;	/* Final do FAT 2 */
	int	u_root_dir_blkno;	/* Início do diretório raiz */
	int	u_end_root_dir_blkno;	/* Final do diretório raiz */
	int	u_end_fs_blkno;		/* Final do sistema de arquivos */
	int	u_n_fats;		/* No. de FATs */
	int	u_sectors_per_fat;	/* No. de setores por FAT */
	int	u_n_clusters;		/* No. de "clusters" */
	int	u_sectors_per_cluster;	/* No. de setores por cluster */
	int	u_root_dir_entries;	/* No. de ent. do dir. raiz */
	int	u_cluster_0_blkno;	/* End. do bloco do cluster = 0 */
	int	u_bytes_per_cluster;	/* No. de bytes por cluster */
	int	u_fat_bits;		/* No. de bits da FAT (12, 16 ou 32) */
	int	u_root_cluster;		/* No. do CLUSTER da raiz */
	int	u_sb_backup;		/* Cópia do superbloco */
	int	u_fs_info;		/* Setor do bloco de Informações  */
	int	u_disk_infree;		/* No. de FATs livres (original) */
	int	u_disk_inxtfree;	/* No. da próxima livre (original) */
	int	u_infree;		/* No. de FATs livres */
	int	u_inxtfree;		/* No. da próxima livre */

}	UNI;

#define	CLUSZ	(up->u_bytes_per_cluster)	/* No. de bytes por cluster */

extern UNI	uni;

/*
 ******	Bloco 0 do disco ****************************************
 */
typedef struct
{
	char	s_reser0[3];
	char	s_id[8];		/* Identificação: "DOS  2.0" */

	char	s_bytes_per_sector[2];	/* Bytes por setor */
	char	s_sectors_per_cluster;	/* Setores por cluster */
	char	s_fat_offset[2];	/* Deslocamento da FAT */
	char	s_n_fats;		/* No. de FATs */
	char	s_root_dir_entries[2];	/* No. de ent. do dir. raiz */
	char	s_little_size[2];	/* No. de setores do dispositivo */
	char	s_media;		/* Tipo do meio */
	char	s_little_sectors_per_fat[2]; /* No. de setores por FAT */
	char	s_sectors_per_track[2];	/* No. de setores por trilha */
	char	s_n_heads[2];		/* No. de lados */
	char	s_hidden_sectors[2];	/* No. de setores ocultos */
	char	s_reser_0[2];
	char	s_big_size[4];		/* Tamanho (se >= 32 Mb) */

  union
  {
    struct	/* Continuação para FAT32 */
    {
	char	s_32_big_sectors_per_fat[4]; /* No. de setores por FAT (32) */
	char	s_32_flags[2];		/* Indicadores */
	char	s_32_version[2];	/* Versão */
	char	s_32_root_cluster[4];	/* Início do diretório raiz */
	char	s_32_fs_info[2];	/* Setor do bloco de Informações  */
	char	s_32_sb_backup[2];	/* Cópia do superbloco */

	char	s_reser_1[12];
	char	s_phys_drive;		/* Dispositivo */
	char	s_reser_2;
	char	s_signature;		/* Assinatura (0x29) */
	char	s_vol_ser_no[4];	/* No. serial */
	char	s_label[11];		/* Rótulo */
	char	s_fat_msg[5];		/* Tipo da FAT */

    }	fat_32;

    struct	/* Continuação para FAT12 / FAT16 */
    {
	char	s_phys_drive;		/* Dispositivo */
	char	s_reser_2;
	char	s_signature;		/* Assinatura (0x29) */
	char	s_vol_ser_no[4];	/* No. serial */
	char	s_label[11];		/* Rótulo */
	char	s_fat_msg[5];		/* Tipo da FAT */

    }	fat_16;

  }	u;

	char	s_boot[512-87-2];
	char	s_boot_valid[2];	/* Indica o o boot está válido */

}	DOSSB;

extern DOSSB	sb;			/* O Super bloco */

/*
 ******	Equivalencias convenientes ******************************
 */
#define	s_big_sectors_per_fat	u.fat_32.s_32_big_sectors_per_fat
#define	s_flags			u.fat_32.s_32_flags
#define	s_version		u.fat_32.s_32_version
#define	s_root_cluster		u.fat_32.s_32_root_cluster
#define	s_fs_info		u.fat_32.s_32_fs_info
#define	s_sb_backup		u.fat_32.s_32_sb_backup

#define	s_32_phys_drive		u.fat_32.s_phys_drive
#define	s_32_signature		u.fat_32.s_signature
#define	s_32_vol_ser_no		u.fat_32.s_vol_ser_no
#define	s_32_label		u.fat_32.s_label
#define	s_32_fat_msg		u.fat_32.s_fat_msg

#define	s_16_phys_drive		u.fat_16.s_phys_drive
#define	s_16_signature		u.fat_16.s_signature
#define	s_16_vol_ser_no		u.fat_16.s_vol_ser_no
#define	s_16_label		u.fat_16.s_label
#define	s_16_fat_msg		u.fat_16.s_fat_msg

/*
 ******	Bloco de informação da FAT32 ****************************
 */
typedef struct
{
	char	fs_isig1[4];		/* Assinatura 1 */
	char	fs_ifill1[480];
	char	fs_isig2[4];		/* Assinatura 2 */
	char	fs_infree[4];		/* No. de FATs livres */
	char	fs_inxtfree[4];		/* No. da próxima livre */
	char	fs_ifill2[12];
	char	fs_isig3[4];		/* Assinatura 3 */

	char	fs_ifill3[508];
	char	fs_isig4[4];		/* Assinatura 4 */

}	FSINFO;

/*
 ******	Entrada de diretório do DOS *****************************
 */
typedef	struct
{
	char	d_name[8];		/* Nome do arquivo */
	char	d_ext[3];		/* Extensão do nome */
	char	d_mode;			/* Atributos do arquivo */

	char	d_reser;		/* Ainda não usado */
	char	d_ccenti;		/* Centésimos de segundo da criação */
	char	d_ctime[2];		/* Tempo de criação */
	char	d_cdate[2];		/* Data de criação */
	char	d_adate[2];		/* Data de acesso */
	char	d_high_cluster[2];	/* Parte alta do CLUSTER */

	char	d_mtime[2];		/* Tempo de atualização */
	char	d_mdate[2];		/* Data de atualização */
	char	d_low_cluster[2];	/* Parte baixa do CLUSTER */
	char	d_size[4];		/* Tamanho do arquivo */

}	DOSDIR;

#define NODOSDIR	(DOSDIR *)0

/*
 ******	Estrutura de STAT e LFN diretório DOS *******************
 */
#define LFN_SZ		256

typedef	struct
{
	DOSDIR	z_d;		/* O conteúdo do diretório */

	/* Estado para localizar a entrada (de informação) do diretório */

	int	z_cluster;	/* O Número do CLUSTER (32 bits) */
	int	z_blkno;	/* O Número do bloco do diretório */
	int	z_offset;	/* O deslocamento do dir. dentro do bloco */

	/* Estado para localizar o início das entradas LFN do diretório */

	int	z_lfn_clusno;	/* O número do CLUSTER do diretório PAI */
	int	z_lfn_blkno;	/* O Número do bloco do diretório */
	int	z_lfn_end_blkno; /* O Final do cluster */
	int	z_lfn_offset;	/* O deslocamento do dir. dentro do bloco */
	int	z_lfn_entries;	/* No. de entradas do diretório */

	/* Estado para compor o LFN */

	char	z_lfn_area[LFN_SZ]; /* Área para compor o nome longo (LFN) */
	char	*z_lfn_nm;	/* Ponteiro para a área acima */
	int	z_lfn_index;	/* Para conferir a ordem */
	int	z_lfn_checksum;	/* Para conferir o "checksum" */

}	DOSSTAT;

#define NODOSSTAT	(DOSSTAT *)0

/*
 ******	Abreviações cômodas *************************************
 */
#define	z_name		z_d.d_name
#define	z_ext 		z_d.d_ext
#define	z_mode 		z_d.d_mode

#define	z_ccenti	z_d.d_ccenti
#define	z_ctime		z_d.d_ctime	
#define	z_cdate		z_d.d_cdate	
#define	z_adate		z_d.d_adate	
#define	z_high_cluster	z_d.d_high_cluster	

#define	z_mtime		z_d.d_mtime
#define	z_mdate		z_d.d_mdate
#define	z_low_cluster 	z_d.d_low_cluster
#define	z_size 		z_d.d_size

/*
 ******	Macros usadas por LFN ***********************************
 */
#define	dos_lfn_init(zp)					\
	 (zp)->z_lfn_nm = (zp)->z_lfn_area + LFN_SZ - 1,	\
	*(zp)->z_lfn_nm = '\0'

#define	dos_lfn_reset(zp)					\
	 (zp)->z_lfn_nm = (zp)->z_lfn_area + LFN_SZ - 1

/*
 ******	Indicadores dos arquivos ********************************
 */
#define	Z_2		0x80		/* Não usado */
#define	Z_1		0x40		/* Não usado */
#define	Z_MODIF		0x20		/* Modificado */
#define	Z_DIR		0x10		/* Diretório */
#define	Z_VOL		0x08		/* Volume */
#define	Z_SYS		0x04		/* Sistema */
#define	Z_HIDDEN	0x02		/* Oculto */
#define	Z_RO		0x01		/* Read only */

#define	Z_IFMT		(Z_DIR|Z_VOL)	/* Tipo do arquivo */
#define	Z_REG		0		/* Arquivo regular */
#define	Z_LFN		(Z_VOL|Z_SYS|Z_HIDDEN|Z_RO) /* Nome longo */

#define	Z_ISREG(m)	(((m) & Z_IFMT) == Z_REG)
#define	Z_ISDIR(m)	(((m) & Z_IFMT) == Z_DIR)
#define	Z_ISVOL(m)	(((m) & Z_IFMT) == Z_VOL)
#define	Z_ISLFN(m)	((m) == Z_LFN)

#define	Z_EMPTY		0xE5		/* Entrada de dir. vazia */

/*
 ******	Valores utilizados por CLUSTERs na FAT ******************
 */
extern int	BAD_BEGIN;	/* 0xFF1, 0xFFF1 ou 0xFFFFFF1 */
extern int	EOF_BEGIN;	/* 0xFF8, 0xFFF8 ou 0xFFFFFF8 */
extern int	STD_EOF;	/* 0xFFF, 0xFFFF ou 0xFFFFFFF */

#define	C_FREE			0
#define	C_STD_EOF		STD_EOF
#define	C_ISFREE(c)		((c) == 0)
#define	C_ISBAD(c)		((c) >= BAD_BEGIN && (c) < EOF_BEGIN)
#define	C_ISEOF(c)		((c) >= EOF_BEGIN)
#define	C_ISBAD_OR_EOF(c)	((c) >= BAD_BEGIN)

/*
 ******	Macros de endereçamento dos blocos ***********************
 */
#define FIRST_BLK(clusno)						\
	up->u_cluster_0_blkno + (clusno) * up->u_sectors_per_cluster

#define FIRST_AND_LAST_BLK(clusno, blkno, end_blkno)			\
	blkno = up->u_cluster_0_blkno + (clusno) * up->u_sectors_per_cluster,	\
	end_blkno = blkno + up->u_sectors_per_cluster

#define ROOT_FIRST_AND_LAST_BLK(blkno, end_blkno)		\
	blkno = up->u_root_dir_blkno,				\
	end_blkno = up->u_end_root_dir_blkno

/*
 ******	Macro para processar o número do CLUSTER *****************
 */
#define GET_CLUSTER(zp)	get_cluster (zp)

#define PUT_CLUSTER(cluster, zp)				\
	PUT_SHORT (cluster,       (zp)->z_low_cluster);		\
	PUT_SHORT (cluster >> 16, (zp)->z_high_cluster)

/*
 ****** Estrutura para "dos_read" *******************************
 */
typedef struct
{
	char	f_code_conv;	/* Deve converter ISO/BIN */
	int	f_o_clusno;	/* No. do primeiro cluster */
	int	f_clusno;	/* No. do cluster atual */
	int	f_o_count;	/* No. total de de bytes do arquivo */
	int	f_count;	/* No. de bytes ainda no arquivo */
	int	f_blkno;	/* No. do bloco atual */
	int	f_end_blkno;	/* No. do bloco final */
	char	*f_area;	/* Ponteiro para o inicio do buffer */
	char	*f_ptr;		/* Ponteiro para o proximo caracter */
	char	*f_end;		/* Fim do buffer */

}	DOSFILE;

#define	dos_open(fp, zp)				\
			(fp)->f_area = alloca (BLSZ),	\
			_dos_open ((fp), (zp))

#define	dos_open_by_clus(fp, clusno, count)		\
			(fp)->f_area = alloca (BLSZ),	\
			(fp)->f_o_clusno = (clusno),	\
			(fp)->f_o_count = (count),	\
			_dos_open_by_clus (fp)

#define	BIG_SZ	0x7FFFFFFF	/* Usado para diretórios */

/*
 ******	Tipos usados por "dos_ftw" ******************************
 */
typedef enum
{
	DOS_FTW_F = 1,		/* Arquivo */ 
	DOS_FTW_D,		/* Diretório */
	DOS_FTW_NS,		/* Não foi possivel obter o estado */
	DOS_FTW_DNR		/* Não foi possivel abrir o diretório */

}	DOS_FTW_ENUM;

/*
 ******	Códigos de algumas partições ****************************
 */
#define	DOS_EXT_G	0x05	/* Código da partição estendida DOS (Geo) */
#define	DOS_EXT_L	0x0F	/* Código da partição estendida DOS (Linear) */
#define	TROPIX_EXT	0xAE	/* Código da partição estendida TROPIX */

#define	TROPIX_FS1	0xA8	/* Código da partição TROPIX FS1 */

#define	DOS_FAT12	0x01	/* Código da partição FAT12 DOS */
#define	DOS_FAT16	0x04	/* Código da partição FAT16 DOS < 32 MB */

#define	DOS_FAT16_G	0x06	/* Código da partição FAT16 DOS (Geo) */
#define	DOS_FAT16_L	0x0E	/* Código da partição FAT16 DOS (Linear) */

#define	DOS_FAT32_G	0x0B	/* Código da partição FAT32 DOS (Geo) */
#define	DOS_FAT32_L	0x0C	/* Código da partição FAT32 DOS (Linear) */

#define	IS_EXT(t)	(t == DOS_EXT_G || t == DOS_EXT_L || t == TROPIX_EXT)
#define	IS_DOS_L(t)	(t == DOS_EXT_L || t == DOS_FAT16_L || t == DOS_FAT32_L)
#define	IS_DOS_G(t)	(t == DOS_EXT_G || t == DOS_FAT16_G || t == DOS_FAT32_G)

/*
 ******	Protótipos de funções ***********************************
 */
extern int		_dos_open (DOSFILE *, const DOSSTAT *);
extern int		_dos_open_by_clus (DOSFILE *);
extern void		clr_dir_ent (DOSDIR *);
extern void		copy_dos_file_nm (DOSDIR *, const DOSDIR *);
extern int		dir_walk (int (*function) (const DOSSTAT *), int);
extern void		dos_dir_write (DOSSTAT *);
extern const char	*dos_file_nm_can (const char *, DOSDIR *);
extern int		dos_fstat (const char *, int, DOSSTAT *);
extern int		dos_ftw (const char *, int (*) (const char *, DOSSTAT *, int));
extern void		dos_lfn_add (DOSSTAT *);
extern int		dos_lfn_dir_clr (DOSSTAT *);
extern int		dos_lfn_dir_put (DOSSTAT *);
extern void		dos_lfn_get_nm (DOSSTAT *);
extern int		dos_link (const DOSSTAT *, const char *);
extern int		dos_read (DOSFILE *, void *, int);
extern int		dos_rewind (DOSFILE *);
extern int		dos_stat (const char *, DOSSTAT *);
extern int		dos_unlink (DOSSTAT *, int);
extern int		file_walk (int (*function) (void *, int), DOSDIR *);
extern void		get_cluster (DOSSTAT *);
extern int		get_dir_by_clus (int, int, DOSDIR *);
extern int		get_dir_by_name (char *, int, DOSDIR *);
extern int		get_dir_by_path (char *, DOSDIR *);
extern char		*get_dos_file_nm (const DOSDIR *, const char **);
extern time_t		get_dos_file_time (const DOSDIR *);
extern void		print_file_entry (DOSDIR *);
extern void		put_dos_file_time (time_t, DOSDIR *);
extern int		vol_search (const DOSSTAT *);
