/*
 ****************************************************************
 *								*
 *			v7bio.c					*
 *								*
 *	Funções para leitura do sistema de arquivos V7		*
 *								*
 *	Versão	3.0.0, de 18.07.94				*
 *		4.6.0, de 21.10.04				*
 *								*
 *	Módulo: Boot2						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <common.h>
#include <sync.h>

#include <v7.h>
#include <stat.h>
#include <disktb.h>
#include <bhead.h>
#include <devmajor.h>

#include "../h/common.h"
#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Definições globais					*
 ****************************************************************
 */
#define	NINDIR	10
#define	NIDs	10			/* No. de componentes do caminho */

entry char	path_vec[NIDs*16];	/* Nomes já separados */

/*
 ****************************************************************
 *	Procura o Arquivo Desejado				*
 ****************************************************************
 */
V7DINO *
v7_iname (const DISKTB *pp, const char *path)
{
	ino_t		inode;
	long		bno;
	daddr_t		daddr;
	V7DIR		*dp;
	V7DINO		*ip;
	char		*cp, *cp1;
	char		buf[BLSZ];

	/*
	 *	converte o caminho (para facilitar a comparação)
	 */
	for (cp = &path_vec[0]; cp < &path_vec[NIDs * 16]; *cp++ = '\0')
		/* vazio */;

	for (cp = cp1 = path_vec; *path; path++)
	{
		if   (*path != '/')
			*cp++ = *path;
		elif (cp != cp1)
			cp = (cp1 += 16);
	}

	/*
	 *	Percorre os diretórios, procurando o arquivo pedido
	 */
	cp = path_vec; 	inode = V7_ROOT_INO;

	while (bno = 0, ip = v7_iget (pp, inode), *cp)
	{
		if (vflag)
			printf ("\ninode = %d, mode = %4X\n", inode, ip->n_mode);
		do
		{
			if ((daddr = daddrvec[bno++]) == 0)
			{
				printf ("\nArquivo inexistente\n");
				return (NOV7DINO);
			}

			bread (pp, daddr, buf, BLSZ);

			for (dp = (V7DIR *)buf; dp < (V7DIR *)&buf[BLSZ]; dp++)
			{
				if (dp->d_ino == 0)
					continue;

				if (dp->d_name[0] == '\0')
					continue;

				if (vflag)
					printf ("%16s", dp->d_name);

				if (ideq (cp, dp->d_name))
					break;
			}
		}
		while (dp >= (V7DIR *)&buf[BLSZ]);

		if (vflag)
			printf ("\n");

#ifdef	LITTLE_ENDIAN
		inode = short_endian_cv (dp->d_ino);
#else
		inode = dp->d_ino;
#endif	LITTLE_ENDIAN

		cp += 16;

	}	/* end while */

	/*
	 *	Já sai da malha com o INODE do arquivo desejado
	 */
	if (vflag)
		printf ("\ninode = %d, mode = %4X\n", inode, ip->n_mode);

	return (ip);

}	/* end v7_iname */

/*
 ****************************************************************
 *	Obtém um INODE de um arquivo				*
 ****************************************************************
 */
V7DINO *
v7_iget (const DISKTB *pp, ino_t inode)
{
	V7DINO		*ip;
	daddr_t		daddr_11, *dp;
	char		*pt, *pa;
	int		i;
	char		area[BLSZ];
	static V7DINO	dino;

	bread (pp, V7_INTODA (inode), area, BLSZ);

	ip = (V7DINO *)area + V7_INTODO (inode);

	/*
	 *	Inicialmente, converte os 13 endereços (ordem natural)
	 */
	for
	(	pt = &ip->n_addr[39], pa = (char *)&daddrvec[13];
		pt > &ip->n_addr[0];
		/* vazio */
	)
	{
		*--pa = *--pt;
		*--pa = *--pt;
		*--pa = *--pt;
		*--pa = '\0';
	}

	/*
	 *	Le o bloco simplesmente indireto e NINDIR - 1 duplamente
	 *	indiretos.
	 *	Isto suporta até (10 + NINDIR * 128) * 512 = 645 Kb de KERNEL
	 */
	daddr_11 = long_endian_cv (daddrvec[11]);

	if (daddrvec[10])
	{
		bread (pp, long_endian_cv (daddrvec[10]), &daddrvec[10], BLSZ); /* 10 */

		if (daddr_11)
		{
			bread (pp, daddr_11, &daddrvec[10+128], BLSZ);	/* 11 */

			for (i = NINDIR - 2; i >= 0; i--) /* NINDIR - 1 duplamente indiretos */
			{
				daddr_11 = long_endian_cv (daddrvec[10+128+i]);

				if (daddr_11 == 0)
					continue;

				bread (pp, daddr_11, &daddrvec[10+128+128*i], BLSZ);
			}
		}
	}

#ifdef	LITTLE_ENDIAN
	/*
	 *	Agora converte tudo para "big-endian", se for o caso
	 */
	for (dp = &daddrvec[0]; dp[0] != 0; dp++)
		dp[0] = long_endian_cv (dp[0]);

	dino.n_mode  = short_endian_cv (ip->n_mode);
	dino.n_nlink = short_endian_cv (ip->n_nlink);
	dino.n_uid   = short_endian_cv (ip->n_uid);
	dino.n_gid   = short_endian_cv (ip->n_gid);
	dino.n_size  = long_endian_cv  (ip->n_size);
	dino.n_atime = long_endian_cv  (ip->n_atime);
	dino.n_mtime = long_endian_cv  (ip->n_mtime);
	dino.n_ctime = long_endian_cv  (ip->n_ctime);
#endif	LITTLE_ENDIAN

	return (&dino);

}	/* end v7_iget */
