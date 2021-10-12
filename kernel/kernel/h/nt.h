/*
 ****************************************************************
 *								*
 *			<sys/nt.h>				*
 *								*
 *	Estruturas do sistema de arquivos NTFS			*
 *								*
 *	Versão	4.6.0, de 19.05.04				*
 *		4.6.0, de 19.05.04				*
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

#define	NT_H			/* Para definir os protótipos */

/*
 ******	Definições Globais **************************************
 */
#define	NT_SBNO			0

#define	NTFS_BBID		"NTFS    "
#define	NTFS_BBIDLEN		8

#define NTFS_AF_INRUN		0x00000001
#define NTFS_MAXATTRNAME	255

#define	NTFS_SYSNODESNUM	16

/*
 ******	Definições da compressão de atributos *******************
 */
#define NTFS_COMPRESS_CLUSTER_SHIFT	4
#define NTFS_COMPRESS_CLUSTER_SIZE	(1 << NTFS_COMPRESS_CLUSTER_SHIFT)
#define NTFS_COMPRESS_CLUSTER_MASK	(NTFS_COMPRESS_CLUSTER_SIZE - 1)

#define NTFS_COMPBLOCK_SIZE		(4 * KBSZ)

enum
{
	NTFS_MFTINO		= 0,
	NTFS_VOLUMEINO		= 3,
	NTFS_ATTRDEFINO		= 4,
	NTFS_ROOTINO		= 5,
	NTFS_BITMAPINO		= 6,
	NTFS_BOOTINO		= 7,
	NTFS_BADCLUSINO		= 8,
	NTFS_UPCASEINO		= 10,
	NTFS_PSEUDO_MFTINO	= 15		/* Deselegância para evitar INO == 0 */
};

#define NTFS_FFLAG_RDONLY	0x00000001
#define NTFS_FFLAG_HIDDEN	0x00000002
#define NTFS_FFLAG_SYSTEM	0x00000004
#define NTFS_FFLAG_ARCHIVE	0x00000020
#define NTFS_FFLAG_COMPRESSED	0x00000800
#define NTFS_FFLAG_DIR		0x10000000

/*
 ****** A estrutura do SUPER BLOCO ******************************
 */
struct ntsb
{
	int		s_bytes_per_sector;
	int		s_sectors_per_cluster;
	int		s_CLUSTER_SHIFT;
	int		s_CLUSTER_SZ;
	int		s_CLUSTER_MASK;
	long		s_total_sectors;
	long		s_free_clusters;
	int		s_MFT_cluster_no;
	int		s_MFT_mirror_cluster_no;
	int		s_MFT_record_size;
	int 		s_COMPRESS_BLOCK_SIZE;

	INODE		*s_mft_inode;
};

/*
 ****** O tipo "long long" simplificado *************************
 */
typedef union
{
	long long	value_quad;
	long		value[2];

}	long_long;

/*
 ****** A estrutura do BLOCO de BOOT ****************************
 */
struct ntboot
{
	char		s_reser_1[3];		/* Instrução "jmp" */
	char		s_sysid[8];		/* "NTFS    " */
	char		s_bps[2];		/* Bytes por setor */
	char		s_spc;			/* Setores por Cluster */
	char		s_reser_2[7];
	char		s_media;		/* Descritor do meio (0xF8) */
	char		s_reser_3[2];
	ushort		s_spt;			/* Setores por trilha */
	ushort		s_heads;		/* Número de cabeças */
	char		s_reser_4[12];
	long_long	s_spv_quad;		/* No. total de setores */
	long_long	s_mftcn_quad;		/* No. do Cluster da $MFT */
	long_long	s_mftmirrcn_quad;	/* No. do Cluster da cópia da $MFT */
	signed char	s_mftrecsz;		/* No. de Clusters por registro */
	char		s_reser_5[3];
	long		s_ibsz;			/* No. de Clusters por "Index Block" */
	long long	s_volsn;		/* No. de série do Volume */
	ulong		s_checksum;		/* Checksum */
};

#define	s_spv_low	s_spv_quad.value[0]
#define	s_spv_high	s_spv_quad.value[1]

#define	s_mftcn_low	s_mftcn_quad.value[0]
#define	s_mftmirrcn_low	s_mftmirrcn_quad.value[0]

/*
 ******	Alocado nos endereços ***********************************
 */
#define	i_attr_list		i_addr[0]	/* NÃO precisa de SPINLOCK */

#define	i_last_entryno		i_addr[1]
#define	i_last_type		i_addr[2]
#define	i_last_offset		i_addr[3]
#define	i_last_dir_blkno	i_addr[4]
#define	i_total_entrynos	i_addr[5]

/*
 ******	Estrutura do INODE **************************************
 */
#define	NTFS_FILEMAGIC	(0x454C4946)	/* "FILE" */
#define	NTFS_INDXMAGIC	(0x58444E49)	/* "INDX" */

#define	NTFS_FRFLAG_DIR	0x0002

struct fixuphdr
{
	ulong		fh_magic;
	ushort		fh_foff;
	ushort		fh_fnum;
};

struct filerec
{
	struct fixuphdr fr_fixup;

	char		fr_reserved[8];

	ushort		fr_seqnum;	/* Sequence number */
	ushort		fr_nlink;
	ushort		fr_attroff;	/* offset to attributes */
	ushort		fr_flags;	/* 1-nonresident attr, 2-directory */

	ulong		fr_size;	/* hdr + attributes */
	ulong		fr_allocated;	/* allocated length of record */

	long long	fr_mainrec;	/* main record */
	ushort		fr_attrnum;	/* maximum attr number + 1 ??? */
};

/*
 ******	Cabeçalho do atributo ***********************************
 */
struct attrhdr
{
	ulong		a_type;
	ulong		a_reclen;
	char		a_flag;
	char		a_nm_len;
	char		a_nameoff;
	char		reserved1;
	char		a_compression;
	char		reserved2;
	ushort		a_index;
};

/*
 ******	Tipos do atributos **************************************
 */
#define NTFS_A_STD		0x10
#define NTFS_A_ATTRLIST		0x20
#define NTFS_A_NAME		0x30
#define NTFS_A_VOLUMENAME	0x60
#define NTFS_A_DATA		0x80
#define	NTFS_A_INDXROOT		0x90
#define	NTFS_A_INDX		0xA0
#define NTFS_A_INDXBITMAP	0xB0

/*
 ******	Estrutura de um atributo ********************************
 */
struct attr
{
	struct attrhdr	a_hdr;

	union
	{
		struct
		{
			ushort		a_datalen_long;
			ushort		reserved1;
			ushort		a_dataoff;
			ushort		a_indexed;

		} 	a_S_r;

		struct
		{
			long_long	a_vcnstart_quad;
			long_long	a_vcnend_quad;
			ushort		a_dataoff;
			ushort		a_compressalg;
			ulong		reserved1;
			long_long	a_allocated_quad;
			long_long	a_datalen_quad;
			long_long	a_initialized_quad;

		}	a_S_nr;

	}	a_S;
};

#define a_r	a_S.a_S_r
#define a_nr	a_S.a_S_nr

#define	a_vcnstart	a_vcnstart_quad.value[0]
#define	a_vcnend	a_vcnend_quad.value[0]
#define	a_allocated	a_allocated_quad.value[0]
#define	a_datalen	a_datalen_quad.value[0]
#define	a_initialized	a_initialized_quad.value[0]

/*
 ******	Outras estruturas ***************************************
 */
typedef struct
{
	long long	t_create;
	long long	t_write;
	long long	t_mftwrite;
	long long	t_access;

}	ntfs_times_t;

struct attr_std
{
	ntfs_times_t	n_times;
	long_long	n_mode;
	long_long	n_unknown;
};

struct attr_name
{
	ulong		n_pnumber;	/* Parent ntnode */
	ulong		reserved;
	ntfs_times_t	n_times;
	long long	n_size;
	long long	n_attrsz;
	long long	n_flag;
	char		n_nm_len;
	char		n_nametype;
	ushort		n_name[1];
};

#define NTFS_IRFLAG_INDXALLOC	0x00000001

struct attr_indexroot
{
	ulong		ir_unkn1;	/* always 0x30 */
	ulong		ir_unkn2;	/* always 0x1 */
	ulong		ir_size;	/* ??? */
	ulong		ir_unkn3;	/* number of cluster */
	ulong		ir_unkn4;	/* always 0x10 */
	ulong		ir_datalen;	/* sizeof simething */
	ulong		ir_allocated;	/* same as above */
	ushort		ir_flag;	/* ?? always 1 */
	ushort		ir_unkn7;
};

/*
 ******	Atributo na memória *************************************
 */
struct ntvattr
{
	struct ntvattr	*va_next;
	ulong		va_vflag;

	ulong		va_flag;
	ulong		va_type;
	char		va_nm_len;
	char		va_name[NTFS_MAXATTRNAME];

	ulong		va_compression;
	ulong		va_compressalg;
	ulong		va_datalen;
	ulong		va_allocated;
	long		va_vcnstart;
	long		va_vcnend;
	ushort		va_index;

	union
	{
		struct
		{
			clusno_t	*cn;
			clusno_t	*cl;
			int		cnt;

		}	vrun;

		void			*datap;
		struct attr_name 	*name;
		struct attr_indexroot 	*iroot;
		struct attr_indexalloc 	*ialloc;

	}	va_d;
};

#define	va_vruncn	va_d.vrun.cn
#define va_vruncl	va_d.vrun.cl
#define va_vruncnt	va_d.vrun.cnt
#define va_datap	va_d.datap
#define va_a_name	va_d.name
#define va_a_iroot	va_d.iroot
#define va_a_ialloc	va_d.ialloc

/*
 ******	Mais estruturas *****************************************
 */
#define	NTFS_CLUSTER_to_BYTE(cn)	((cn) << ntsp->s_CLUSTER_SHIFT)

struct attr_attrlist
{
	int		al_type;		/* Attribute type */
	ushort		reclen;			/* length of this entry */
	char		al_nm_len;		/* Attribute name len */
	char		al_nameoff;		/* Name offset from entry start */
	long_long 	al_vcnstart_quad;	/* VCN number */
	int		al_inumber;		/* Parent ntnode */
	int		reserved;
	ushort		al_index;		/* Attribute index in MFT record */
	ushort		al_name[1];		/* Name */
};

#define	al_vcnstart	al_vcnstart_quad.value[0]

struct attr_indexalloc
{
	struct fixuphdr ia_fixup;
	long long	unknown1;
	long long	ia_bufcn;
	ushort		ia_hdrsize;
	ushort		unknown2;
	ulong		ia_inuse;
	ulong		ia_allocated;
};

#define	NTFS_IEFLAG_SUBNODE	0x00000001
#define	NTFS_IEFLAG_LAST	0x00000002

#define NTFS_MAXFILENAME	255

typedef ushort		wchar;

struct attr_indexentry
{
	ulong		ie_number;
	ulong		unknown1;
	ushort		reclen;
	ushort		ie_size;
	ulong		ie_flag;/* 1 - has subnodes, 2 - last */
	ulong		ie_fpnumber;
	ulong		unknown2;
	ntfs_times_t	ie_ftimes;
	long long	ie_fallocated;
	long long	ie_fsize;
	long_long	ie_fflag;
	uchar		ie_fnm_len;
	uchar		ie_fnametype;
	wchar		ie_fname[NTFS_MAXFILENAME];

	/* long long	ie_bufcn;	 buffer with subnodes */
};

#define ie_fflag_low	ie_fflag.value[0]
