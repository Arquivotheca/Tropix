/*
 ****************************************************************
 *								*
 *			<sys/cdfs.h>				*
 *								*
 *	Definições referentes ao Sistema de Arquivos ISO9660	*
 *								*
 *	Versão	4.0.0, de 31.01.02				*
 *		4.2.0, de 18.03.02				*
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

#define	CDFS_H			/* Para definir os protótipos */

/*
 ****************************************************************
 *	Superbloco para o FS ISO9660				*
 ****************************************************************
 */
#define	NOCDSB		(CDSB *)0

typedef struct
{
	int	s_blsz;			/* Tamanho do bloco */
	int	s_size;			/* Tamanho do FS em blocos */
	int	s_root_first_block;	/* Primeiro bloco do diretório raiz */
	int	s_high_sierra;
	int	s_joliet_level;		/* Não NULO se Joliet => UNICODE */
	int	s_rock_ridge;		/* Não NULO se Rock Ridge */
	int	s_rock_ridge_skip0;	/* A pular se Rock Ridge: Raiz */
	int	s_rock_ridge_skip;	/* A pular se Rock Ridge: Outros diretórios */

}	CDSB;

/*
 ******	Formato da Entrada do Diretório *************************
 */
#define CDDIRSZ		(sizeof (CDDIR) - 1)	/* 33 */

typedef struct
{
	char	d_len;				/* Tamanho da entrada */
	char	d_ext_attr_len;			/* Tam. dos atributos */
	char	d_first_block[8];		/* Bloco inicial */
	char	d_size[8];			/* Tamanho do arquivo */
	char	d_date[7];			/* Data */
	char	d_flags;			/* Indicadores */
	char	d_file_unit_size;
	char	d_interleave;
	char	d_volume_sequence_number[4];
	char	d_name_len;			/* Tamanho do nome */
	char	d_name[1];			/* Início do nome */

}	CDDIR;

#define NOCDDIR	(CDDIR *)NULL

#define	ISO_HIDDEN	1
#define	ISO_IFMT	2
#define	    ISO_REG	0
#define	    ISO_DIR	2

/*
 ******	Estrutura do Bloco de Identificação *********************
 */
#define	ISO_BLSHIFT		11
#define ISO_BLSZ		(1 << ISO_BLSHIFT)
#define	ISO_BLMASK		(ISO_BLSZ - 1)

#define	BYTOISOBL(x)		(((x) + ISO_BLSZ - 1) >> ISO_BLSHIFT)

#define	ISO_SBNO		16

#define ISO_VD_PRIMARY		1
#define ISO_VD_SUPPLEMENTARY	2
#define ISO_VD_END		255

#define ISO_STANDARD_ID		"CD001"
#define ISO_ECMA_ID		"CDW01"

#define ISO_SIERRA_ID		"CDROM"

typedef struct
{
	char	cd_type;		/* Tipo de disco */
	char	cd_id[5];		/* Identificação de disco */
	char	cd_version;

	char	cd_joliet_flags;

	char	cd_system_id[32];
	char	cd_volume_id[32];

	char	cd_unused2[8];

	char	cd_volume_space_size[8];

	char	cd_joliet_escape[32];

	char	cd_volume_set_size[4];
	char	cd_volume_sequence_number[4];
	char	cd_logical_block_size[4];

	char	cd_path_table_size[8];
	char	cd_type_l_path_table[4];
	char	cd_opt_type_l_path_table[4];
	char	cd_type_m_path_table[4];
	char	cd_opt_type_m_path_table[4];

	CDDIR	cd_root_dir;

	char	cd_volume_set_id[128];
	char	cd_publisher_id[128];
	char	cd_preparer_id[128];
	char	cd_application_id[128];
	char	cd_copyright_file_id[37];
	char	cd_abstract_file_id[37];
	char	cd_bibliographic_file_id[37];

	char	cd_creation_date[17];
	char	cd_modification_date[17];
	char	cd_expiration_date[17];
	char	cd_effective_date[17];

	char	cd_file_structure_version;
	char	cd_unused4;

	char	cd_application_data[512];
	char	cd_unused5[653];

}	CDVOL;

/*
 ******	Atributos ***********************************************
 */
typedef	struct
{
	char	a_owner[4];
	char	a_group[4];
	char	a_perm[2];
	char	a_ctime[17];
	char	a_mtime[17];
	char	a_xtime[17];
	char	a_ftime[17];
	char	a_recfmt;
	char	a_recattr;
	char	a_reclen[4];
	char	a_system_id[32];
	char	a_system_use[64];
	char	a_version;
	char	a_len_esc;
	char	a_reserved[64];
	char	a_len_au[4];

}	ISOATTR;

/*
 ******	Macros **************************************************
 */
#define	ISO_GET_SHORT(p)	(*(short *)(p))
#define	ISO_GET_LONG(p)		(*(long  *)(p))

#define	ISOBL_TO_BL(x)		((x) << (ISO_BLSHIFT - BLSHIFT))
#define	ISO_MAKE_INO(bl,of)	0

#define	i_first_block		i_addr[0]

/*
 ****** Rock Ridge Extension ************************************
 */
typedef struct
{
	char	rr_type[2];	/* Tipo da entrada */
	char	rr_len;		/* Tamanho da entrada */
	char	rr_version;	/* Versão da entrada */

  union
  {
    struct
    {
	/*
	 *	Tipo "continuation": "CE"
	 */
	char	rr_u_block[8];
	char	rr_u_offset[8];
	char	rr_u_len[8];

    }	ce;

    struct
    {
	/*
	 *	Tipo "offset": "SP"
	 */
	char	rr_u_check[2];
	char	rr_u_skip;

    }	sp;

    struct
    {
	/*
	 *	Tipo "Extensions Reference": "ER"
	 */
	char	rr_u_len_id;
	char	rr_u_len_des;
	char	rr_u_len_src;
	char	rr_u_version;
	char	rr_u_ext_id[1];

    }	er;

    struct
    {
	/*
	 *	Tipo "Symbolic Link": "SL"
	 */
	char	rr_u_flags;
	char	rr_u_component[1];

    }	sl;

    struct
    {
	/*
	 *	Tipo "alt_name": "NM"
	 */
	char	rr_u_flags;
	char	rr_u_name[1];

    }	nm;

    struct
    {
	/*
	 *	Tipo "child_link": "CL"
	 */
	char	rr_u_child_dir[8];

    }	cl;

    struct
    {
	/*
	 *	Tipo "parent_link": "PL"
	 */
	char	rr_u_parent_dir[8];

    }	pl;

    struct
    {
	/*
	 *	Tipo "File Attributes": "PX"
	 */
	char	rr_u_mode[8];
	char	rr_u_nlink[8];
	char	rr_u_uid[8];
	char	rr_u_gid[8];

    }	px;

    struct
    {
	/*
	 *	Tipo "Device Numbers": "PN"
	 */
	char	rr_u_high[8];
	char	rr_u_low[8];

    }	pn;

    struct
    {
	/*
	 *	Tipo "Time Stamp": "TF"
	 */
	char	rr_u_time_flag;
	char	rr_u_time[7];

    }	tf;

  }	u;

}	RR;

/*
 ******	Definições úteis ****************************************
 */
#define	NORR		(RR *)NULL

#define	RR_HEADER_SZ	4	/* Os campos comuns */

#define	rr_con_block	u.ce.rr_u_block
#define	rr_con_offset	u.ce.rr_u_offset
#define	rr_con_len	u.ce.rr_u_len
#define	rr_check	u.sp.rr_u_check
#define	rr_skip		u.sp.rr_u_skip
#define	rr_er_len_id	u.er.rr_u_len_id
#define	rr_er_version	u.er.rr_u_version
#define	rr_er_id	u.er.rr_u_ext_id
#define	rr_sl_flags	u.sl.rr_u_flags
#define	rr_component	u.sl.rr_u_component
#define	rr_nm_flags	u.nm.rr_u_flags
#define	rr_name		u.nm.rr_u_name
#define	rr_child_dir	u.cl.rr_u_child_dir
#define	rr_parent_dir	u.pl.rr_u_parent_dir
#define	rr_mode		u.px.rr_u_mode
#define	rr_nlink	u.px.rr_u_nlink
#define	rr_high		u.pn.rr_u_high
#define	rr_low		u.pn.rr_u_low
#define	rr_uid		u.px.rr_u_uid
#define	rr_gid		u.px.rr_u_gid
#define	rr_time_flag	u.tf.rr_u_time_flag
#define	rr_time		u.tf.rr_u_time

/*
 ******	As diversas missões da análise **************************
 */
enum
{
	RR_NO_MISSION,
	RR_START_MISSION,	RR_NAME_MISSION,	RR_STAT_MISSION,
	RR_PARENT_MISSION,	RR_SYMLINK_MISSION
};

/*
 ******	Protótipos de funções ***********************************
 */
extern int	cd_rr_fields_analysis (const CDDIR *, const SB *, int mission, void *vp);
