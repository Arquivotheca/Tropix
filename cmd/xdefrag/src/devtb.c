/*
 ****************************************************************
 *								*
 *			devtb.c					*
 *								*
 *	Obtém a tabela de dispositivos				*
 *								*
 *	Versão	4.3.0, de 09.10.02				*
 *		4.5.0, de 07.09.03				*
 *								*
 *	Módulo: xdefrag						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/syscall.h>
#include <sys/disktb.h>
#include <sys/kcntl.h>
#include <sys/t1.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stat.h>
#include <ustat.h>
#include <errno.h>

#include "../h/xdefrag.h"

/*
 ******	Protótipos de funções ***********************************
 */
int		is_good_dev (const DISKTB *pp);

/*
 ****************************************************************
 *	Obtém a tabela de dispositivos				*
 ****************************************************************
 */
void
get_devtb (void)
{
	int		n;
	const DISKTB	*disktb, *pp;
	DEVTB		*dp;

	/*
	 *	Desaloca os recursos antigos
	 */
	if (devtb != NODEVTB)
		free (devtb);

	/*
	 *	Obtém a tabela de disco
	 */
	if ((n = kcntl (K_GET_DEV_TB, NODEV, NODISKTB)) < 0)
		error ("$*Não consegui obter o tamanho da tabela de discos");

	disktb = alloca (n * sizeof (DISKTB));

	if (kcntl (K_GET_DEV_TB, NODEV, disktb) < 0)
		error ("$*Não consegui obter a tabela de discos");

	/*
	 *	Em primeiro lugar, conta o número de candidatos
	 */
	for (n = 0, pp = disktb; pp->p_name[0]; pp++)
	{
		if (is_good_dev (pp))
			n++;
	}

	/*
	 *	Aloca a nova tabela
	 */
	if ((devtb = dp = malloc (n * sizeof (DEVTB))) == NODEVTB)
		error (NOSTR);

	memsetl (dp, 0, n * sizeof (DEVTB) / sizeof (long));

	NDEV = n;

	/*
	 *	Preenche a tabela
	 */
	for (pp = disktb; pp->p_name[0]; pp++)
	{
		if (!is_good_dev (pp))
			continue;

		memmove (&dp->d_disktb, pp, sizeof (DISKTB)); dp++;
	}

	/*
	 *	Obtém a primeira versão dos estados dos dispositivos
	 */
	for (dp = devtb; n > 0; dp++, n--)
		examine_device (dp);

}	/* end get_devtb */

/*
 ****************************************************************
 *	Le o SuperBloco do sistema de arquivos T1		*
 ****************************************************************
 */
void
examine_device (DEVTB *dp)
{
	int		fd;
	T1SB		t1sb;
	char		path[16] = "/dev/";
	char		*cp_point = path + 5;
	STAT		s;
	USTAT		u;

	/*
	 *	Prólogo
	 */
	strcpy (cp_point, dp->d_disktb.p_name);

	if (stat (path, &s) < 0)
		{ dp->d_state = FS_NOSTAT; return; }

	if (ustat (s.st_rdev, &u) >= 0)
		{ strcpy (dp->d_fname, u.f_fname); dp->d_state = FS_MOUNTED; return; }

	/*
	 *	Tenta abrir o dispositivo
	 */
	if   ((fd = open (path, O_RDWR)) >= 0)
	{
		dp->d_state = FS_OK;
	}
	elif (errno != EROFS || (fd = open (path, O_RDONLY)) < 0)
	{
		dp->d_state = FS_NOTREADY; return;
	}
	else
	{
		dp->d_state = FS_RONLY;
	}

	/*
	 *	Le o superbloco
	 */
	lseek (fd, BL4TOBY (T1_SBNO), L_SET);

	if   (read (fd, &t1sb, sizeof (T1SB)) != sizeof (T1SB))
	{
		dp->d_state = FS_NOREAD;
	}
	elif (t1sb.s_magic != T1_SBMAGIC)
	{
		dp->d_state = FS_NOTT1;
	}
	else
	{
		dp->d_disktb.p_size = t1sb.s_fs_sz * (t1sb.s_bl_sz >> BLSHIFT);

		strcpy (dp->d_fname, t1sb.s_fname);
	}

	close (fd);

}	/* end examine_device */

/*
 ****************************************************************
 *	Verifica se é uma entrada interessante			*
 ****************************************************************
 */
int
is_good_dev (const DISKTB *pp)
{
#undef	RAMD
#ifdef	RAMD
	if (strncmp (pp->p_name, "ramd", 4) == 0)
		return (1);
#endif	RAMD

	if (pp->p_name[0] != 'h' && pp->p_name[0] != 's' && pp->p_name[0] != 'u')
		return (0);

	if (pp->p_name[1] != 'd')
		return (0);

	if (pp->p_name[2] == '\0')
		return (0);

	if (pp->p_name[3] == '\0')
		return (0);

	if (pp->p_type != 0 && pp->p_type != 0xA9)
		return (0);

	return (1);

}	/* end is_good_dev */

/*
 ****************************************************************
 *	Converte blocos em MB					*
 ****************************************************************
 */
const char *
cv_blocks_to_MB (long bl_cnt)
{
	static char	area[32];

	bl_cnt += (MBSZ/BLSZ/(2 * 100)); /* Arredonda 0.005 MB */

	sprintf
	(	area,
		"%d.%02d",
		(unsigned)bl_cnt >> (MBSHIFT-BLSHIFT),
		(100 * ((unsigned)bl_cnt & (MBSZ/BLSZ - 1))) >> (MBSHIFT-BLSHIFT)
	);

	return (area);

}	/* end cv_blocks_to_KB_MB_or_GB */
