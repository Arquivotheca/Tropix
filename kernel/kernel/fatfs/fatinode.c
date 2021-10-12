/*
 ****************************************************************
 *								*
 *			fatinode.c				*
 *								*
 *	Gerência da FAT						*
 *								*
 *	Versão	4.2.0, de 19.12.01				*
 *		4.8.0, de 03.09.05				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2005 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/inode.h"
#include "../h/bhead.h"
#include "../h/sysdirent.h"
#include "../h/fat.h"
#include "../h/stat.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis globais e constantes				*
 ****************************************************************
 */

/*
 ******	Dias acumulativos para anos regulares e bissextos *******
 */
static const int	regular_days_vec[] =
{
	0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365, 1000000
};

static const int	leap_days_vec[] =
{
	0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366, 1000000
};

/*
 ******	Protótipos de funções ***********************************
 */
int		fat_put_fat_value_in_one_fat (const SB *, int, int, off_t, int);
time_t		fat_get_time (int fat_time, int fat_date);
void		fat_put_time (time_t tempo, ushort *fat_time, ushort *fat_date);

/*
 ****************************************************************
 *	Leitura e Conversão do INODE da FAT			*
 ****************************************************************
 */
int
fat_read_inode (INODE *ip)
{
	const FATDIR	*dp;
	SB		*sp = ip->i_sb;
	FATSB		*fatsp = sp->sb_ptr;
	BHEAD		*bp;

	/*
	 *	Cria "na mão" o INODE do diretório RAIZ
	 */
	if (ip->i_ino == FAT_ROOT_INO)
	{
		ip->i_mode = S_IFDIR|S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH;

	   /***	ip->i_nlink	= ...;	***/

		ip->i_uid = sp->sb_uid;
		ip->i_gid = sp->sb_gid;

#if (0)	/*******************************************************/
		if (sp->sb_mntent.me_flags & SB_USER)
		{
			ip->i_uid = sp->sb_uid;
			ip->i_gid = sp->sb_gid;
		}
		else
		{
			ip->i_uid = 0;
			ip->i_gid = 0;
		}
#endif	/*******************************************************/

		ip->i_rdev	= NODEV;

		ip->i_size	= 0;

		ip->i_first_clusno = fatsp->s_root_cluster;

		ip->i_atime	=
		ip->i_mtime	=
		ip->i_ctime	= fatsp->s_inode_time;

		ip->i_par_ino		= FAT_ROOT_INO;

	   /***	ip->i_clusvec_sz	= 0;	***/
	   /***	ip->i_clusvec_len	= 0;	***/
		ip->i_clusvec		= NULL;

		ip->i_fsp = &fstab[FS_FAT];

		fat_get_nlink_dir (ip);

		return (0);
	}

	/*
	 *	Le o bloco correspondente
	 */
	bp = bread (ip->i_dev, FAT_INO_TO_BLKNO (ip->i_ino), 0);

	if (bp->b_flags & B_ERROR)
		{ block_put (bp); return (-1); }

	dp = (FATDIR *)(bp->b_addr + FAT_INO_TO_OFFSET (ip->i_ino));

	/*
	 *	Processo o modo e "nlink"
	 */
	switch (dp->d_mode & FAT_IFMT)
	{
	    case FAT_REG:
		ip->i_mode  = S_IFREG|S_IXUSR;
		ip->i_nlink = 1;
		break;

	    case FAT_DIR:
		ip->i_mode  = S_IFDIR|S_IXUSR|S_IXGRP|S_IXOTH;
	   /***	ip->i_nlink = ...;	***/
		break;

	    default:
		printf ("%g: Lendo tipo 0x%02X inválido do INODE %d da FAT\n", dp->d_mode, ip->i_ino);
		block_put (bp);
		return (-1);
	}

	ip->i_mode |= S_IRUSR|S_IRGRP|S_IROTH;

	if ((dp->d_mode & FAT_RO) == 0)
		ip->i_mode |= S_IWUSR;

	/*
	 *	Retira outros campos
	 */
	ip->i_uid = sp->sb_uid;
	ip->i_gid = sp->sb_gid;

#if (0)	/*******************************************************/
	if (sp->sb_mntent.me_flags & SB_USER)
	{
		ip->i_uid = sp->sb_uid;
		ip->i_gid = sp->sb_gid;
	}
	else
	{
		ip->i_uid = 0;
		ip->i_gid = 0;
	}
#endif	/*******************************************************/

	ip->i_rdev	= NODEV;

	ip->i_size = FAT_GET_LONG (dp->d_size);

	/*
	 *	Obtém o número do "cluster"
	 */
	ip->i_first_clusno = fat_get_cluster (fatsp, dp);

	/*
	 *	Retira os tempos
	 */
	ip->i_atime = fat_get_time (0,		 dp->d_adate);
	ip->i_mtime = fat_get_time (dp->d_mtime, dp->d_mdate);
	ip->i_ctime = fat_get_time (dp->d_ctime, dp->d_cdate);

	/*
	 *	Epílogo
	 */
   /***	ip->i_clusvec_sz  = 0;	***/
   /***	ip->i_clusvec_len = 0;	***/
	ip->i_clusvec	  = NULL;

	ip->i_fsp = &fstab[FS_FAT];

	block_put (bp);

	/*
	 *	Calcula o "nlink"
	 */
	if (S_ISDIR (ip->i_mode))
		fat_get_nlink_dir (ip);

	return (0);

}	/* end fat_read_inode */

/*
 ****************************************************************
 *	Conversão e Escrita do INODE da FAT			*
 ****************************************************************
 */
void
fat_write_inode (INODE *ip)
{
	BHEAD		*bp;
	FATDIR		*dp;
	ushort		dummy_time;

	/*
	 *	Verifica se não houve modificação no INODE
	 *	ou entao ele está montado "Read only"
	 */
	if ((ip->i_flags & ICHG) == 0)
		return;

	inode_dirty_dec (ip);

	if (ip->i_sb->sb_mntent.me_flags & SB_RONLY)
		return;

	/*
	 *	O INODE do diretório RAIZ não existe no disco
	 */
	if (ip->i_ino == FAT_ROOT_INO)
		return;

	/*
	 *	Verifica se o INODE foi liberado: será realizado por "fat_clr_dir"
	 */
	if (ip->i_mode == 0)
		return;

	/*
	 *	Le o bloco correspondente
	 */
	bp = bread (ip->i_dev, FAT_INO_TO_BLKNO (ip->i_ino), 0);

	if (bp->b_flags & B_ERROR)
		{ block_put (bp); return; }

	dp = (FATDIR *)(bp->b_addr + FAT_INO_TO_OFFSET (ip->i_ino));

	/*
	 *	Atualiza o modo e tamanho
	 */
	switch (ip->i_mode & S_IFMT)
	{
	    case S_IFREG:
		dp->d_mode = FAT_REG;
		FAT_PUT_LONG (ip->i_size, dp->d_size);
		break;

	    case S_IFDIR:
		dp->d_mode = FAT_DIR;
		FAT_PUT_LONG (0, dp->d_size);
		break;

	    default:
		printf ("%g: Escrevendo tipo 0x%04X inválido do INODE %d da FAT\n", ip->i_mode, ip->i_ino);
		block_put (bp);
		return;
	}

	if ((ip->i_mode & (S_IWUSR|S_IWGRP|S_IWOTH)) == 0)
		dp->d_mode |= FAT_RO;

	dp->d_mode |= FAT_MODIF;

	/*
	 *	Atualiza o número do "cluster" inicial
	 */
	FAT_PUT_CLUSTER (ip->i_first_clusno, dp);

	/*
	 *	Atualiza os tempos
	 */
	fat_put_time (ip->i_ctime, &dp->d_ctime, &dp->d_cdate);
	fat_put_time (ip->i_atime, &dummy_time,  &dp->d_adate);
	fat_put_time (ip->i_mtime, &dp->d_mtime, &dp->d_mdate);

	dp->d_ccenti = 0;

	bdwrite (bp);

}	/* end fat_write_inode */

/*
 ****************************************************************
 *	Cria um INODE do sistema de arquivos FAT		*
 ****************************************************************
 */
INODE *
fat_make_inode (IOREQ *iop, DIRENT *dep, int mode)
{
	INODE		*ip;
	const FATSB	*sp = iop->io_ip->i_sb->sb_ptr;
	ino_t		ino;
	int		ifmt;

	/*
	 *	Para o sistema FAT, só podemos ter REG e DIR
	 */
	if   ((mode & IFMT) == 0)
		mode |= IFREG;
	elif ((ifmt = mode & IFMT) != IFREG && ifmt != IFDIR)
		{ u.u_error = EINVAL; iput (iop->io_ip); return (NOINODE); }

	/*
	 *	Escreve a entrada no diretório pai, e obtém um INODE
	 */
	if ((ino = fat_lfn_dir_put (iop->io_ip, dep)) == -1)
		{ iput (iop->io_ip); return (NOINODE); }

	if ((ip = iget (iop->io_ip->i_dev, ino, 0)) == NOINODE)
		{ iput (iop->io_ip); return (NOINODE); }

	iput (iop->io_ip);

	/*
	 *	Prepara o INODE
	 */
	ip->i_mode = mode & ~u.u_cmask;
	ip->i_nlink = 1;
	ip->i_uid = u.u_euid;
	ip->i_gid = u.u_egid;

	ip->i_atime = time;
	ip->i_mtime = time;
	ip->i_ctime = time;

	inode_dirty_inc (ip);

	ip->i_first_clusno	= sp->s_STD_EOF;

	ip->i_par_ino		= iop->io_ip->i_ino;

   /***	ip->i_clusvec_sz	= 0;	***/
   /***	ip->i_clusvec_len	= 0;	***/
	ip->i_clusvec		= NULL;

	ip->i_fsp = &fstab[FS_FAT];

	return (ip);

}	/* end fat_make_inode */

/*
 ****************************************************************
 *	Libera todos os "clusters" de um arquivo		*
 ****************************************************************
 */
void
fat_trunc_inode (INODE *ip)
{
	SB		*unisp = ip->i_sb;
	FATSB		*sp = unisp->sb_ptr;
	int		ifmt;
	clusno_t	log_clusno, clusno, next_clusno;

	/*
	 *	Somente trunca arquivos de certo tipo
	 */
	if ((ifmt = ip->i_mode & IFMT) != IFREG && ifmt != IFDIR)
		return;

	if ((clusno = ip->i_first_clusno) == 0 || C_ISEOF (clusno))
		return;

	log_clusno = 0;

	SLEEPLOCK (&sp->s_free_cluster_lock, PINODE);

	for (EVER)
	{
		if (C_ISBAD_OR_EOF (clusno))
			break;

		if (clusno < 2 || clusno >= sp->s_n_clusters + 2)
			{ printf ("%g: No. %d de \"cluster\" inválido, dev = %v\n", clusno, ip->i_dev); break; }

		next_clusno = fat_cluster_read_map (ip, ++log_clusno);

		fat_put_fat_value (unisp, C_FREE, clusno);

		if (sp->s_free_cluster_ptr > sp->s_free_cluster_vector)
			*--sp->s_free_cluster_ptr = clusno;

		sp->s_infree++; unisp->sb_sbmod = 1;

		clusno = next_clusno;

	}	/* end for (EVER) */

	SLEEPFREE (&sp->s_free_cluster_lock);

	/*
	 *	Libera o vetor de CLUSTERs
	 */
	if (ip->i_clusvec != NULL)
		free_byte ((void *)ip->i_clusvec);

   /***	ip->i_clusvec_sz   = 0;	***/
   /***	ip->i_clusvec_len  = 0;	***/
	ip->i_clusvec	   = NULL;

	/*
	 *	Epilogo
	 */
	ip->i_first_clusno = C_STD_EOF;
	ip->i_size	   = 0;

	ip->i_mtime	   = time;
	inode_dirty_inc (ip);

}	/* end fat_trunc_inode */

/*
 ****************************************************************
 *	Conversão de tempos de FAT para "time_t"		*
 ****************************************************************
 */
time_t
fat_get_time (int fat_time, int fat_date)
{
	const int	*days_vec;
	int		year_from_1970, month, day;
	time_t		value;

	/*
	 *	Lembrar que a FAT começa de 1980!
	 */
	if (fat_date == 0)
		return (0);

	year_from_1970 = (fat_date >> 9) + 10;

	day = year_from_1970 * 365 + ((year_from_1970 + 1) >> 2);	/* Começa com o ano */
	
	if ((unsigned)(month = ((fat_date >> 5) & 0x0F) - 1) >= 12)
		{ printf ("%g: Mês %d inválido\n", month + 1); month = 0; }
	
	days_vec = (year_from_1970 + 2) & 3 ? regular_days_vec : leap_days_vec;

	day += days_vec[month];						/* Acrescenta o mes */

	day += (fat_date & 0x1F) - 1;					/* Acrescenta o dia */

	value = day   * 24 +  (fat_time >> 11);				/* Acrescenta as horas */

	value = value * 60 + ((fat_time >> 5) & 0x3F);			/* Acrescenta os minutos */

	value = value * 60 + ((fat_time & 0x1F) << 1);			/* Acrescenta os segundos */

	return (value);

}	/* end fat_get_time */

/*
 ****************************************************************
 *	Conversão de tempos de "time_t" para FAT		*
 ****************************************************************
 */
void
fat_put_time (time_t tempo, ushort *fat_time, ushort *fat_date)
{
	const int	*days_vec;
	int		day_part, second_part;
	int		feb_29_cnt, year_from_1970, year_from_1980;
	int		day, leap_cnt, month;
	int		module, hour, minute, second;

	/*
	 *	Começa com o ano, mes e dia
	 */
	day_part	= tempo / (24 * 60 * 60);
	second_part	= tempo % (24 * 60 * 60);

	/* Constantes:	(4 * 365 + 1)		=> Dias em 4 anos		*/
	/*		(365 * 2 - 31 - 28)	=> Dias de 28.2.72 até 1.1.74	*/

	feb_29_cnt = (day_part + 365 * 2 - 31 - 28) / (4 * 365 + 1);

	year_from_1970 = (day_part - feb_29_cnt) / 365;

	leap_cnt = (year_from_1970 + 1) >> 2;

	day = day_part - (year_from_1970 * 365 + leap_cnt);

	days_vec = (year_from_1970 + 2) & 3 ? regular_days_vec : leap_days_vec;

	for (month = 1; day >= days_vec[month]; month++)
		/* vazio */;

   /***	if (month > 12)		***/
   /***		month = 1; }	***/

	day -= days_vec[month - 1];

	/*
	 *	Calcula horas, minutos e segundos
	 */
	hour	= second_part / (60 * 60);
	module	= second_part % (60 * 60);

	minute	= module / 60;
	second	= module % 60;

	/*
	 *	Compõe as variáveis FAT
	 */
	*fat_time = (hour << 11) | (minute << 5) | (second >> 1);

	if ((year_from_1980 = year_from_1970 - 10) < 0)	/* Os 10 anos inexistentes */
		year_from_1980 = 0;

	*fat_date = (year_from_1980 << 9) | (month << 5) | (day + 1);

}	/* end fat_put_time */
