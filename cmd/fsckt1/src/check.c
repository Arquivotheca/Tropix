/*
 ****************************************************************
 *								*
 *			check.c					*
 *								*
 *	Verifica um sistema de arquivos TROPIX T1		*
 *								*
 *	Versão	4.3.0, de 09.07.02				*
 *		4.8.0, de 17.07.05				*
 *								*
 *	Módulo: fsckt1						*
 *		Utilitários Especiais				*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2005 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/sync.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/disktb.h>
#include <sys/kcntl.h>
#include <sys/mntent.h>
#include <sys/t1.h>
#include <sys/v7.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <ustat.h>
#include <setjmp.h>
#include <errno.h>

#include "../h/fsckt1.h"

/*
 ******	Variáveis globais ***************************************
 */
entry DISKTB		dev_disktb;

/*
 ****************************************************************
 *	Verifica um sistema de arquivos				*
 ****************************************************************
 */
void
check (const char *name)
{
	T1SB		*sp = (T1SB *)sb.b_area;

	/*
	 *	Antes de mais nada, libera a memória de todas as tabelas
	 */
	free_all ();

	/*
	 *	Agora, analisa o dispositivo dado
	 */
	if (get_dev (name) != CONTINUE)
		return;

	printf
	(	"*** Analisando \"%s\" (T1, %s, %s): %s\n",
		fs_nm, sp->s_fname,
		dev_disktb.p_name[0] ? dev_disktb.p_name : sp->s_fpack,
		cv_blocks_to_KB_MB_or_GB (sp->s_fs_sz)
	);

	if (dev_is_root)
		printf ("*** O dispositivo é a raiz do sistema\n");

	if (read_only && fflag >= 0)
		printf ("*** Sistema montado ou protegido para escritas: NÃO será modificado\n");

	if (phase1 () != CONTINUE)
		return;

	if (phase2 () != CONTINUE)
		return;

	if (phase3 () != CONTINUE)
		return;

	if (phase4 () != CONTINUE)
		return;

	if (phase5 () != CONTINUE)
		return;

	if (phase6 () != CONTINUE)
		return;

	/*
	 *	Imprime os totais
	 */
	printf
	(	"%d arquivos, %d blocos, %d usados (%s %%)\n",
		sp->s_busy_ino, sp->s_fs_sz, sp->s_busy_bl, 
		editpercent (getpercent (sp->s_busy_bl, sp->s_fs_sz))
	);

	/*
	 *	Epílogo
	 */
	bflush (&sb);
	bflush (&inoblk);
	bflush (&fileblk);

	if (fs_modified)
	{
		printf
		(	"\n****** O sistema de arquivos "
			"foi modificado ******\n"
		);

		if (dev_is_root)
		{
			printf
			(	"\n****** Recarregue o Sistema - "
				" Não dê \"sync\" ******\n"
			);

#if (0)	/*******************************************************/
			for (EVER)
				/* vazio */;
#endif	/*******************************************************/
		}
	}


}	/* end check */

/*
 ****************************************************************
 *	Analisa o dispositivo					*
 ****************************************************************
 */
int
get_dev (const char *dev_nm)
{
	STAT		dev_s;
	USTAT		u;
	T1SB		*sp;
	int		block_map_sz, inode_hash_tb_sz;

	/*
	 *	Obtém o estado do dispositivo
	 */
	if (stat (dev_nm, &dev_s) < 0)
		{ error ("*Não consegui obter o estado de \"%s\"", dev_nm); goto bad0; }

	fs_nm = dev_nm; fs_dev = dev_s.st_rdev;

	/*
	 *	Verifica se foi dado um diretório
	 */
	if (S_ISDIR (dev_s.st_mode))
	{
		FILE		*fp;
		int		code;
		char		fstab_dev_nm[64];
		char		fstab_dir_nm[64];
		MNTENT		mntent;

		/*
		 *	Procura no arquivo FSTAB
		 */
		if ((fp = fopen (fstab_nm, "r")) == NOFILE)
			error ("$*Não consegui abrir \"%s\"", fstab_nm);

		for (EVER)
		{
			if ((code = getmntent (fp, fstab_dev_nm, fstab_dir_nm, &mntent)) == EOF)
				error ("$Não achei \"%s\" em \"%s\"", dev_nm, fstab_nm);

			if (code == 0)
				continue;

			if (streq (fstab_dir_nm, dev_nm))
				break;
		}

		fclose (fp);

		fs_nm = dev_nm = fstab_dev_nm;

		if (stat (dev_nm, &dev_s) < 0)
			{ error ("*Não consegui obter o estado de \"%s\"", dev_nm); goto bad0; }

		fs_dev = dev_s.st_rdev;
	}

	/*
	 *	Analisa o tipo do dispositivo
	 */
	switch (dev_s.st_mode & S_IFMT)
	{
	    case S_IFBLK:
	    case S_IFCHR:
	    case S_IFREG:
		break;

	    default:
		error ("O arquivo \"%s\" não é regular nem especial", dev_nm);
	}

	/*
	 *	Verifica se o dispositivo é o ROOT
	 */
	dev_is_root = (dev_s.st_rdev == root_dev);

	/*
	 *	Analisa se pode atualizar o sistema
	 */
	if (fflag < 0 || !dev_is_root && ustat (fs_dev, &u) >= 0)
		read_only = 1;
	else
		read_only = 0;

	/*
	 *	Abre o Sistema
	 */
	if ((dev_fd = open (fs_nm, read_only ? O_RDONLY : O_RDWR)) < 0)
	{
		if (errno != EROFS || (dev_fd = open (fs_nm, O_RDONLY)) < 0)
			{ error ("*Não consegui abrir \"%s\"", fs_nm); goto bad0; }

		read_only = 1;
	}

	if (isatty (dev_fd))
		{ error ("O dispositivo \"%s\" é um terminal!!!", fs_nm); goto bad0; }

	/*
	 *	obtém a entrada da partição do disco
	 */
	dev_disktb.p_name[0] = '\0';

	if (!S_ISREG (dev_s.st_mode) && kcntl (K_GET_DEV_TB, dev_s.st_rdev, &dev_disktb) < 0)
		error ("CUIDADO: Não consegui obter a tabela de \"%s\"", fs_nm);

	/*
	 *	Obtém o superbloco do sistema
	 */
	sp = bget (&sb, T1_SBNO);

	if (sp->s_magic != T1_SBMAGIC)
	{
		const V7SB	*v7sp;
		char		*area;

		/* Verifica se é por acaso V7 */

		sp = bget (&sb, 0); v7sp = (V7SB *)((int)sp + BLTOBY (V7_SBNO));

		if (long_endian_cv (v7sp->s_magic) != V7_SBMAGIC)
		{
			error ("O dispositivo \"%s\" não contém um sistema de arquivos", fs_nm);
			goto bad0;
		}

		area = alloca (120);

		strcpy (area, "fsckv7 ");

		if   (fflag < 0)
			strcat (area, "-n ");
		elif (fflag > 0)
			strcat (area, "-y ");

		strcat (area, dev_nm);

		system (area);

		return (SKIP);
	}

	if (dev_disktb.p_name[0] != '\0' && T1_BL4toBL (sp->s_fs_sz) > dev_disktb.p_size)
	{
		error
		(	"O tamanho do sistema de arquivos é maior do que a partição: %d :: %d",
			T1_BL4toBL (sp->s_fs_sz), dev_disktb.p_size
		);
	}

	/*
	 *	Aloca espaço para o mapa de BLOCOs
	 */
	block_map_sz = MAP_SZ (sp->s_fs_sz);

	if ((disk_block_map = malloc (block_map_sz)) == NOSTR)
		goto bad1;

	if ((mem_block_map = malloc (block_map_sz)) == NOSTR)
		goto bad1;

	/*
	 *	Prepara a tabela HASH dos INODEs
	 */
	inode_hash_tb_sz = INO_HASH_SZ * sizeof (INO_HASH *);

	if ((inode_hash_tb = malloc (inode_hash_tb_sz)) == NOVOID)
		goto bad1;

	/*
	 *	Zera todos os mapas de bits e a tabela HASH
	 */
	memsetl (mem_block_map, 0, block_map_sz / sizeof (long));
	memsetl (inode_hash_tb, 0, inode_hash_tb_sz / sizeof (long));

	return (CONTINUE);

	/*
	 *	Em caso de erro, ...
	 */
    bad1:
	error (NOSTR);	/* Memória esgotada */
    bad0:
	return (SKIP);

}	/* end get_dev */

/*
 ****************************************************************
 *	Libera todas os recursos alocados			*
 ****************************************************************
 */
void
free_all (void)
{
	INO_HASH	*ip, *next_ip, **hp;

	/*
	 *	Inicializações
	 */
	fs_modified = 0; fs_busy_bl = 0; fs_busy_ino = 0;

	sb.b_dirty	= 0; 	sb.b_bno	= NIL;
	inoblk.b_dirty	= 0; 	inoblk.b_bno	= NIL;
	fileblk.b_dirty	= 0; 	fileblk.b_bno	= NIL;

	/*
	 *	Libera os recursos
	 */
	if (dev_fd >= 0)
		{ close (dev_fd); dev_fd = -1; }

	if (mem_block_map)
		{ free (mem_block_map); mem_block_map = NOSTR; }

	if (disk_block_map)
		{ free (disk_block_map); disk_block_map = NOSTR; }

	if (dup_block_tb)
	{
		free (dup_block_tb); dup_block_tb = NODUPBLK;
		dup_block_elem = 0; dup_block_sz = 0;
	}

	if (inode_hash_tb)
	{
		for (hp = &inode_hash_tb[0]; hp < &inode_hash_tb[INO_HASH_SZ]; hp++)
		{
			for (ip = *hp; ip != NO_INO_HASH; ip = next_ip)
				{ next_ip = ip->i_next; free (ip); }
		}

		free (inode_hash_tb); inode_hash_tb = NULL;
	}

}	/* end free_all */

/*
 ****************************************************************
 *	Edita uma percentagem					*
 ****************************************************************
 */
char *
editpercent (int quoc)
{
	static char	buf[8];
	char		*sinal;

	if (quoc < 0)
		{ sinal = "-"; quoc = -quoc; }
	else
		sinal = "";


	sprintf (buf, "%s%d.%02d", sinal, quoc / 100, quoc % 100);
	
	return (buf);

}	/* end editpercent */

/*
 ****************************************************************
 *	Calcula uma percentagem					*
 ****************************************************************
 */
int
getpercent (long numer, long denom)
{
	int		quoc;

	/*
	 *	Tenta evitar overflow
	 */
	if (numer > (0x7FFFFFFF / 10000))
		quoc = numer / (denom / 10000);
	else
		quoc = 10000 * numer / denom;

	return (quoc);

}	/* end percent */

/*
 ****************************************************************
 *	Converte blocos em KB, MB ou GB				*
 ****************************************************************
 */
char *
cv_blocks_to_KB_MB_or_GB (unsigned long bl_cnt)
{
	static char	area[32];

	if   (bl_cnt < (10*MBSZ/BL4SZ))		/* Menor do que 10 MB */
	{
		sprintf
		(	area,
			"%d KB",
			bl_cnt << (BL4SHIFT - KBSHIFT)
		);
	}
	elif (bl_cnt < (GBSZ/BL4SZ))		/* De 10 MB até 1024 MB */
	{
		bl_cnt += (MBSZ/BL4SZ/(2 * 100)); /* Arredonda 0.005 MB */

		sprintf
		(	area,
			"%d.%02d MB",
			bl_cnt >> (MBSHIFT-BL4SHIFT),
			(100 * (bl_cnt & (MBSZ/BL4SZ - 1))) >> (MBSHIFT-BL4SHIFT)
		);
	}
	else	/* 1024 MB ou acima */
	{
		bl_cnt += (GBSZ/BL4SZ/(2 * 100)); /* Arredonda 0.005 GB */

		sprintf
		(	area,
			"%d.%02d GB",
			bl_cnt >> (GBSHIFT-BL4SHIFT),
			(100 * (bl_cnt & (GBSZ/BL4SZ - 1))) >> (GBSHIFT-BL4SHIFT)
		);
	}

	return (area);

}	/* end cv_blocks_to_KB_MB_or_GB */
