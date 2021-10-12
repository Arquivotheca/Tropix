/*
 ****************************************************************
 *								*
 *			<sys/fat.h>				*
 *								*
 *	Estruturas do sistema de arquivos FAT			*
 *								*
 *	Versão	4.2.0, de 18.12.01				*
 *		4.8.0, de 02.09.05				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *		/usr/include/sys				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2005 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#define	FAT_H			/* Para definir os protótipos */

/*
 ******	A estrutura do SUPER BLOCO FAT **************************
 */
enum { FAT_FREE_CLUSTER_SZ = 64 };

struct	fatsb
{
	int	s_sectors_per_track;	/* No. de setores por trilha */
	int	s_bytes_per_sector;	/* No. de bytes por setor */
	int	s_n_heads;		/* No. de cabeças do disco */

	int	s_fat1_blkno;		/* Início do FAT 1 */
	int	s_end_fat1_blkno;	/* Final do FAT 1 */
	int	s_fat2_blkno;		/* Início do FAT 2 */
	int	s_end_fat2_blkno;	/* Final do FAT 2 */

	int	s_root_dir_blkno;	/* Início do diretório raiz */
	int	s_end_root_dir_blkno;	/* Final do diretório raiz */
	int	s_end_fs_blkno;		/* Final do sistema de arquivos */

	int	s_n_fats;		/* No. de FATs */
	int	s_sectors_per_fat;	/* No. de setores por FAT */

	int	s_n_clusters;		/* No. de "clusters" */
	int	s_sectors_per_cluster;	/* No. de setores por cluster */

	int	s_root_dir_entries;	/* No. de ent. do dir. raiz */
	int	s_cluster_0_blkno;	/* End. do bloco do cluster = 0 */

	int	s_CLUSTER_SZ;		/* No. de bytes por cluster */
	int	s_CLUSTER_SHIFT;
	int	s_CLUSTER_MASK;

	int	s_fat_bits;		/* No. de bits da FAT (12, 16 ou 32) */
	int	s_root_cluster;		/* No. do CLUSTER da raiz */
	int	s_sb_backup;		/* Cópia do superbloco */
	int	s_fs_info;		/* Setor do bloco de Informações  */

	int	s_disk_infree;		/* No. de FATs livres (original) */
	int	s_disk_inxtfree;	/* No. da próxima livre (original) */
	int	s_infree;		/* No. de FATs livres */
	int	s_inxtfree;		/* No. da próxima livre */

	int	s_BAD_BEGIN;		/* 0xFF1, 0xFFF1 ou 0xFFFFFF1 */
	int	s_EOF_BEGIN;		/* 0xFF8, 0xFFF8 ou 0xFFFFFF8 */
	int	s_STD_EOF;		/* 0xFFF, 0xFFFF ou 0xFFFFFFF */

	LOCK	s_free_cluster_lock;	/* Variáveis para a lista livre de "cluster" */
	int	s_free_cluster_index;
	int	*s_free_cluster_ptr;
	int	*s_free_cluster_end;
	int	s_free_cluster_vector[FAT_FREE_CLUSTER_SZ];

	time_t	s_inode_time;		/* Tempo simulado para os INODEs */
};

/*
 ******	Bloco 0 do disco ****************************************
 */
typedef struct
{
	char	f_reser0[3];
	char	f_id[8];		/* Identificação: "DOS  2.0" */

	char	f_bytes_per_sector[2];	/* Bytes por setor */
	char	f_sectors_per_cluster;	/* Setores por cluster */
	char	f_fat_offset[2];	/* Deslocamento da FAT */
	char	f_n_fats;		/* No. de FATs */
	char	f_root_dir_entries[2];	/* No. de ent. do dir. raiz */
	char	f_little_size[2];	/* No. de setores do dispositivo */
	char	f_media;		/* Tipo do meio */
	char	f_little_sectors_per_fat[2]; /* No. de setores por FAT */
	char	f_sectors_per_track[2];	/* No. de setores por trilha */
	char	f_n_heads[2];		/* No. de lados */
	char	f_hidden_sectors[2];	/* No. de setores ocultos */
	char	f_reser_0[2];
	char	f_big_size[4];		/* Tamanho (se >= 32 MB) */

  union
  {
    struct	/* Continuação para FAT32 */
    {
	char	f_32_big_sectors_per_fat[4]; /* No. de setores por FAT (32) */
	char	f_32_flags[2];		/* Indicadores */
	char	f_32_version[2];	/* Versão */
	char	f_32_root_cluster[4];	/* Início do diretório raiz */
	char	f_32_fs_info[2];	/* Setor do bloco de Informações  */
	char	f_32_sb_backup[2];	/* Cópia do superbloco */

	char	f_reser_1[12];
	char	f_phys_drive;		/* Dispositivo */
	char	f_reser_2;
	char	f_signature;		/* Assinatura (0x29) */
	char	f_vol_ser_no[4];	/* No. serial */
	char	f_label[11];		/* Rótulo */
	char	f_fat_msg[5];		/* Tipo da FAT */

    }	fat_32;

    struct	/* Continuação para FAT12 / FAT16 */
    {
	char	f_phys_drive;		/* Dispositivo */
	char	f_reser_2;
	char	f_signature;		/* Assinatura (0x29) */
	char	f_vol_ser_no[4];	/* No. serial */
	char	f_label[11];		/* Rótulo */
	char	f_fat_msg[5];		/* Tipo da FAT */

    }	fat_16;

  }	u;

	char	f_boot[512-87-2];
	char	f_boot_valid[2];	/* Indica o o boot está válido */

}	FATSB0;

/*
 ******	Equivalencias convenientes ******************************
 */
#define	f_big_sectors_per_fat	u.fat_32.f_32_big_sectors_per_fat
#define	f_fat_flags		u.fat_32.f_32_flags
#define	f_version		u.fat_32.f_32_version
#define	f_root_cluster		u.fat_32.f_32_root_cluster
#define	f_fs_info		u.fat_32.f_32_fs_info
#define	f_sb_backup		u.fat_32.f_32_sb_backup

#define	f_32_phys_drive		u.fat_32.f_phys_drive
#define	f_32_signature		u.fat_32.f_signature
#define	f_32_vol_ser_no		u.fat_32.f_vol_ser_no
#define	f_32_label		u.fat_32.f_label
#define	f_32_fat_msg		u.fat_32.f_fat_msg

#define	f_16_phys_drive		u.fat_16.f_phys_drive
#define	f_16_signature		u.fat_16.f_signature
#define	f_16_vol_ser_no		u.fat_16.f_vol_ser_no
#define	f_16_label		u.fat_16.f_label
#define	f_16_fat_msg		u.fat_16.f_fat_msg

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

}	FATSB1;

/*
 ******	Definições globais **************************************
 */
#define	FAT_ROOT_INO		1

#define	i_first_clusno		i_addr[0]
#define	i_clusvec_sz		i_addr[4]
#define	i_clusvec_len		i_addr[5]
#define	i_clusvec		i_addr[6]

/*
 ****** Macros para retirar/inserir valores *********************
 */
#define	FAT_GET_SHORT(cp)		(*(unsigned short *)(cp))
#define	FAT_GET_LONG(cp)		(*(unsigned long *)(cp))
#define	FAT_PUT_SHORT(n, cp)	 	(*(unsigned short *)(cp) = n)
#define	FAT_PUT_LONG(n, cp) 		(*(unsigned long *)(cp) = n)

/*
 ******	Diretório do FATFS **************************************
 */
struct	fatdir
{
	char	d_name[8];		/* Nome do arquivo */
	char	d_ext[3];		/* Extensão do nome */
	char	d_mode;			/* Atributos do arquivo */

	char	d_reser;		/* Ainda não usado */
	char	d_ccenti;		/* Centésimos de segundo da criação */
	ushort	d_ctime;		/* Tempo de criação */
	ushort	d_cdate;		/* Data de criação */
	ushort	d_adate;		/* Data de acesso */
	char	d_high_cluster[2];	/* Parte alta do CLUSTER */

	ushort	d_mtime;		/* Tempo de atualização */
	ushort	d_mdate;		/* Data de atualização */
	char	d_low_cluster[2];	/* Parte baixa do CLUSTER */
	char	d_size[4];		/* Tamanho do arquivo */
};

/*
 ******	Indicadores dos arquivos ********************************
 */
#define	FAT_2		0x80		/* Não usado */
#define	FAT_1		0x40		/* Não usado */
#define	FAT_MODIF	0x20		/* Modificado */
#define	FAT_DIR		0x10		/* Diretório */
#define	FAT_VOL		0x08		/* Volume */
#define	FAT_SYS		0x04		/* Sistema */
#define	FAT_HIDDEN	0x02		/* Oculto */
#define	FAT_RO		0x01		/* Read only */

#define	FAT_IFMT	(FAT_DIR|FAT_VOL)			/* Tipo do arquivo */
#define	FAT_REG		0					/* Arquivo regular */
#define	FAT_LFN		(FAT_VOL|FAT_SYS|FAT_HIDDEN|FAT_RO)	/* Nome longo */

#define	FAT_ISREG(m)	(((m) & FAT_IFMT) == FAT_REG)
#define	FAT_ISDIR(m)	(((m) & FAT_IFMT) == FAT_DIR)
#define	FAT_ISVOL(m)	(((m) & FAT_IFMT) == FAT_VOL)
#define	FAT_ISLFN(m)	((m) == FAT_LFN)

#define	FAT_EMPTY	0xE5					/* Entrada de dir. vazia */

/*
 ******	Macros de endereçamento dos blocos ***********************
 */
#define FIRST_BLK(clusno)						\
	sp->s_cluster_0_blkno + (clusno) * sp->s_sectors_per_cluster

#define FIRST_AND_LAST_BLK(clusno, blkno, end_blkno)			\
	blkno = sp->s_cluster_0_blkno + (clusno) * sp->s_sectors_per_cluster,	\
	end_blkno = blkno + sp->s_sectors_per_cluster

#define ROOT_FIRST_AND_LAST_BLK(blkno, end_blkno)		\
	blkno = sp->s_root_dir_blkno,				\
	end_blkno = sp->s_end_root_dir_blkno

/*
 ******	Macros usadas por LFN ***********************************
 */
#define	FAT_LFN_INIT(zp)					\
	 (zp)->z_lfn_nm = (zp)->z_lfn_area + LFN_SZ - 1,	\
	*(zp)->z_lfn_nm = '\0'

#define	FAT_LFN_RESET(zp)					\
	 (zp)->z_lfn_nm = (zp)->z_lfn_area + LFN_SZ - 1

#define	FAT_LFN_EMPTY(zp)					\
	 ((zp)->z_lfn_nm == (zp)->z_lfn_area + LFN_SZ - 1)

/*
 ******	Estrutura de STAT e LFN diretório FAT *******************
 */
#define FAT_PUT_CLUSTER(cluster, zp)				\
	FAT_PUT_SHORT (cluster,       (zp)->d_low_cluster);	\
	FAT_PUT_SHORT (cluster >> 16, (zp)->d_high_cluster)

/*
 ******	Estrutura de STAT e LFN diretório FAT *******************
 */
#define LFN_SZ		256

typedef	struct
{
	char	z_lfn_area[LFN_SZ]; /* Área para compor o nome longo (LFN) */
	char	*z_lfn_nm;	/* Ponteiro para a área acima */
	int	z_lfn_index;	/* Para conferir a ordem */
	int	z_lfn_checksum;	/* Para conferir o "checksum" */

}	FATLFN;

#define NOFATLFN	(FATLFN *)0

/*
 ******	Cálculo do bloco do INODE *******************************
 *
 *	Assume que há 16 entradas de diretório por bloco (de 512)
 */
#define	FAT_INO_TO_BLKNO(ino)		(((unsigned)ino) >> 4)
#define	FAT_INO_TO_OFFSET(ino)		(((ino) & 0x0F) << 5)
#define	FAT_MAKE_INO(blkno, offset)	(((blkno) << 4) | ((((unsigned)offset) >> 5) & 0x0F))

/*
 ******	Valores utilizados por CLUSTERs na FAT ******************
 */
#define	C_FREE			0
#define	C_STD_EOF		sp->s_STD_EOF
#define	C_ISFREE(c)		((c) == 0)
#define	C_ISBAD(c)		((c) >= sp->s_BAD_BEGIN && (c) < sp->s_EOF_BEGIN)
#define	C_ISEOF(c)		((c) >= sp->s_EOF_BEGIN)
#define	C_ISBAD_OR_EOF(c)	((c) >= sp->s_BAD_BEGIN)
