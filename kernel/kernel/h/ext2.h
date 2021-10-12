/*
 ****************************************************************
 *								*
 *			<sys/ext2.h>				*
 *								*
 *	Estruturas do sistema de arquivos EXT2			*
 *								*
 *	Versão	4.4.0, de 22.10.02				*
 *		4.7.0, de 09.12.04				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *		/usr/include/sys				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#define	EXT2_H			/* Para definir os protótipos */

/*
 ******	Definições globais **************************************
 */
#define	EXT2_SBNO		2			/* No. do bloco (512 B) do SUPERBLOCO */
#define	EXT2_BGNO(shift)	(shift >= 2 ? 8 : 4)	/* No. do bloco (512 B) dos BGs */

#define	EXT2_BLOCKtoBL(x)	((x) << ext2sp->s_BLOCKtoBL_SHIFT)	/* Para converter os no. de blocos */

/*
 ******	A estrutura do SUPER BLOCO ******************************
 */
typedef struct
{
	ulong	bg_block_bitmap;
	ulong	bg_inode_bitmap;
	ulong	bg_inode_table;
	ushort	bg_free_blocks_count;
	ushort	bg_free_inodes_count;
	ushort	bg_used_dirs_count;
	ushort	bg_pad;
	ulong	bg_reserved[3];

}	EXT2BG;

#define	NOEXT2BG (EXT2BG *)NULL

struct	ext2sb
{
	ulong	s_inodes_count;
	ulong	s_blocks_count;
	ulong	s_r_blocks_count;
	ulong	s_free_blocks_count;
	ulong	s_free_inodes_count;
	ulong	s_first_data_block;
	ulong	s_log_block_size;
	ulong	s_log_frag_size;
	ulong	s_blocks_per_group;
	ulong	s_frags_per_group;
	ulong	s_inodes_per_group;
	time_t	s_mtime;
	time_t	s_wtime;
	ushort	s_mnt_count;
	ushort	s_max_mnt_count;
	ushort	s_magic;
	ushort	s_state;
	ushort	s_errors;
	ushort	s_minor_rev_level;
	time_t	s_last_check;
	int	s_check_interval;
	ulong	s_creator_os;
	ulong	s_rev_level;
	ushort	s_def_resuid;
	ushort	s_def_resgid;
	ulong	s_first_ino;
	ushort	s_inode_size;
	ushort	s_block_group_nr;
	ulong	s_feature_compat;
	ulong	s_feature_incompat;
	ulong	s_feature_ro_compat;
	char	s_uuid[16];
	char	s_volume_name[16];

	/* Variáveis adicionais somente na memória */

	int	s_group_cnt;		/* Número de grupos */

	int	s_BLOCKtoBL_SHIFT;	/* Converte blocos para blocos de 512 B */

	int	s_BLOCK_SHIFT;		/* Quasi-constantes dos blocos (1KB, 2KB, 4KB) */
	int	s_BLOCK_SZ;
	int	s_BLOCK_MASK;

	int	s_INDIR_SHIFT;		/* Quasi-constantes dos blocos indiretos */
	int	s_INDIR_SZ;
	int	s_INDIR_MASK;

	int	s_INOperBLOCK_SHIFT;	/* Quasi-constantes do número de INODEs/bloco */
	int	s_INOperBLOCK_SZ;
	int	s_INOperBLOCK_MASK;

	EXT2BG	s_bg[1];		/* O Primeiro descritor de grupo */
};

#define	EXT2_SBMAGIC	0xEF53		/* No. de Controle para a montagem */

#define	s_end_member 	s_group_cnt	/* Final da parte do disco */

/*
 ******	Diretório do EXT2FS ***************************************
 */
#define	MAXNAMLEN	255	/* Tamanho máximo do nome */

struct	ext2dir
{
	ino_t	d_ino;			/* No. do Inode */
	short	d_ent_sz;		/* Tamanho da entrada */
	char	d_nm_len;		/* Comprimento do nome */
	char	d_type;			/* Tipo do arquivo */
	char	d_name[4];		/* Nome da entrada */
};

#define	EXT2DIR_SIZEOF(len)	(sizeof (EXT2DIR) + ((len - 1) & ~3))
#define	EXT2DIR_SZ_PTR(dp)	(EXT2DIR *)((int)(dp) + (dp)->d_ent_sz)
#define	EXT2DIR_LEN_PTR(dp)	(EXT2DIR *)((int)(dp) + EXT2DIR_SIZEOF ((dp)->d_nm_len))

/*
 ******	Estrutura do INODE **************************************
 */
#define	EXT2_NADDR	(12 + 3)

struct ext2dino
{
	ushort	n_mode;
	ushort	n_uid;
	off_t	n_size;
	time_t	n_atime;
	time_t	n_ctime;
	time_t	n_mtime;
	time_t	n_dtime;
	ushort	n_gid;
	ushort	n_nlink;
	ulong	n_blocks;
	ulong	n_flags;
	ulong	n_osd1[1];
	daddr_t	n_addr[EXT2_NADDR];
	ulong	n_version;
	ulong	n_file_acl;
	ulong	n_dir_acl;
	ulong	n_faddr;
	ulong	n_osd2[3];
};

#define	i_blocks	c.i_c_addr[EXT2_NADDR]	/* Repare que 15 < 19 */

#define	EXT2_ROOT_INO	2	/* No. do INODE da RAIZ */

/*
 ****** Mapa de bits ********************************************
 */
#define	EXT2_GET_MAP(bp, i)	(((char *)bp)[(i) >> 3] & (1 << ((i) & 7)))
#define	EXT2_SET_MAP(bp, i)	(((char *)bp)[(i) >> 3] |= (1 << ((i) & 7)))
#define	EXT2_CLR_MAP(bp, i)	(((char *)bp)[(i) >> 3] &= ~(1 << ((i) & 7)))
