/*
 ****************************************************************
 *								*
 *			iname.c					*
 *								*
 *	Decodifica um caminho completo, retornando um INODE	*
 *								*
 *	Versão	3.0.0, de 24.11.94				*
 *		4.2.0, de 20.04.02				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/inode.h"
#include "../h/bhead.h"
#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/sysdirent.h"
#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Decodifica um caminho					*
 ****************************************************************
 */
INODE *
iname (const char *path, int (*func) (const char *), int mission, IOREQ *iop, DIRENT *dep)
{
	INODE		*ip, *pip;
	ino_t		ino;
	const char	*str;
	char 		*cp, *sym_ptr;
	char		c;
	dev_t		dev;
	int		level = (mission & INAME_LEVEL_MASK), new_mission;

	/*
	 *	Decodifica um caminho completo, devolvendo um INODE trancado
	 *
	 *	"func" =  função que fornece os caracteres do nome ("getschar" ou "getuchar")
	 *
	 *	"mission" = ISEARCH, LSEARCH, ICREATE ou IDELETE
	 *
	 *	Se IGIVEN de "mission" estiver ligado, o INODE (trancado) já é dado em "iop->io_ip"
	 */
	str = path; u.u_error = NOERR;

	/*
	 *	Comeca da RAIZ ou do diretório corrente,
	 *	conforme o primeiro caracter do caminho
	 */
	if ((c = (*func) (str++)) == '\0')
	{
#if (0)	/*******************************************************/
		printf ("%g: Caminho Vazio\n");
#endif	/*******************************************************/

#if (0)	/*******************************************************/
		if (CSWT (8))
#endif	/*******************************************************/
			{ u.u_error = ENOENT; return (NOINODE); }
	}

	/*
	 *	Procura o INODE inicial
	 */
	if   (mission & IGIVEN)
	{
		ip = iop->io_ip; goto nextname;
#if (0)	/*******************************************************/
		ip = iop->io_ip; goto begin;
#endif	/*******************************************************/
	}
	elif (c == '/')
	{
		if ((ip = u.u_rdir) == NOINODE)
			ip = rootdir;

		iop->io_ip = ip;
	}
	else
	{
		iop->io_ip = ip = u.u_cdir;
	}

	SLEEPLOCK (&ip->i_lock, PINODE); iinc (ip);
	
	/*
	 *	Pula os "/"s restantes
	 */
	while (c == '/')
		c = (*func) (str++);

#if (0)	/*******************************************************/
	/*
	 *	A RAIZ e o diretório corrente já existem e não podem ser apagados
	 */
    begin:
	if (c == '\0' && mission & (ICREATE|IDELETE))
		u.u_error = ENOENT;
#endif	/*******************************************************/

	/*
	 *	Neste ponto "ip" contem o último componente processado
	 */
    nextname:
	if (u.u_error)
		goto bad;

	if (c == '\0')
		return (ip);

	/*
	 *	Compõe o próximo componente do nome
	 */
	cp = &dep->d_name[0];

	while (c != '/' && c != '\0' && u.u_error == NOERR)
	{
		if (cp < &dep->d_name[MAXNAMLEN])
			*cp++ = c;

		c = (*func) (str++);
	}

	*cp = '\0';

	dep->d_namlen = cp - &dep->d_name[0];

	strscpy (u.u_name, dep->d_name, sizeof (u.u_name));

	while (c == '/')
		c = (*func) (str++);

	/*
	 *	Evita sair da subarvores limitada pelo "chroot" pelo ".."
	 */
	if (ip == u.u_rdir && streq (dep->d_name, ".."))
		goto nextname;

	/*
	 *	O INODE deve ser de um diretório, e poder ser consultado.
	 *	Neste diretório o nome coletado acima deve ser encontrado
	 */
	if ((ip->i_mode & IFMT) != IFDIR)
		u.u_error = ENOTDIR;

	kaccess (ip, IEXEC);

	if (u.u_error)
		goto bad;

	/*
	 *	Busca no diretório
	 */
    procname:
	if ((*ip->i_fsp->fs_search_dir) (iop, dep, mission) <= 0)
	{
		if (u.u_error)
			goto bad;

		/*
		 *	Não foi encontrada a entrada desejada.
		 */
		if ((mission & ICREATE) && c == '\0')
		{
			if (kaccess (ip, IWRITE) < 0)
				goto bad;

		   /***	iop->io_ip	= ip;		***/
		   /***	iop->io_dev	= ip->i_dev;	***/
			iop->io_dev	= NODEV;	/* Para testar  PROVISÓRIO */

			/*
			 *	Retorna com o diretório "iop->io_ip" trancado
			 */
			return (NOINODE);
		}

		u.u_error = ENOENT;
		goto bad;
	}

	if ((mission & IDELETE) && c == '\0')
	{
		if (kaccess (ip, IWRITE) < 0)
			goto bad;

		/*
		 *	O arquivo vai ser apagado. Repare
		 *	que é retornado o INODE trancado do pai!
		 */
		return (ip);
	}

	/*
	 *	Verifica se o arquivo corrente é a RAIZ
	 *	de um FS montado e o nome seguinte é ".."
	 *
	 *	(O pai da RAIZ é a RAIZ)
	 */
	if (ip->i_ino == dep->d_ino && streq (dep->d_name, ".."))
	{
		if (ip == rootdir)
			goto nextname;

		pip = ip->i_sb->sb_inode; iput (ip);	

		SLEEPLOCK (&pip->i_lock, PINODE); iinc (ip = pip);

		iop->io_ip = ip;

		goto procname;
	}

	/*
	 *	Não se trata da RAIZ
	 */
	dev = ip->i_dev; ino = ip->i_ino;	/* Guarda o pai */

	iput (ip);

	if ((ip = iget (dev, dep->d_ino, 0)) == NOINODE)
		return (NOINODE);

	iop->io_ip = ip;

	/*
	 *	Guarda o INO e DEV do pai
	 */
	if (!IS_DOT_or_DOTDOT (dep->d_name))
	{
		if (ip->i_par_ino == NOINO)
			ip->i_par_ino = (ip->i_dev == dev) ? ino : ip->i_sb->sb_root_ino;

#if (0)	/*******************************************************/
		if (ip->i_par_dev == NODEV)
			ip->i_par_dev = dev;
#endif	/*******************************************************/

		if (ip->i_nm == NOSTR && (ip->i_nm = malloc_byte (dep->d_namlen + 2)) != NOSTR)
			{ ip->i_nm[0] = dep->d_namlen; strcpy (ip->i_nm + 1, dep->d_name); }
	}

	/*
	 *	Trata-se de um elo simbólico
	 */
	if ((ip->i_mode & IFMT) != IFLNK)
		goto nextname;

	if ((mission & LSEARCH) && c == '\0')
		return (ip);

	if (level > 5)
		{ u.u_error = ELOOP; goto bad; }

	if ((sym_ptr = malloc_byte (ip->i_size + 1)) == NOVOID)
		{ u.u_error = ENOMEM; goto bad; }

   /***	iop->io_ip	= ...;	***/
	iop->io_area	= sym_ptr;
	iop->io_count	= ip->i_size + 1;
	iop->io_cpflag	= SS;

	if ((*ip->i_fsp->fs_read_symlink) (iop) < 0)
		{ free_byte (sym_ptr); goto bad; }

	iput (ip);	/* Termina com o elo simbólico */

	/*
	 *	Prepara a nova missão: verifica se o elo simbólico é relativo
	 */
	if (c == 0)
		new_mission = (mission & ~IGIVEN);
	else
		new_mission = ISEARCH | level;

	if (sym_ptr[0] != '/')
	{
		if ((ip = iget (dev, ino, 0)) == NOINODE)	/* Volta ao pai do elo simbólico */
			{ free_byte (sym_ptr); return (NOINODE); }

		iop->io_ip = ip;

		new_mission |= IGIVEN;
	}

	if ((ip = iname (sym_ptr, getschar, new_mission + 1, iop, dep)) == NOINODE)
		{ free_byte (sym_ptr); return (NOINODE); }

	iop->io_ip = ip;

	free_byte (sym_ptr);

	goto nextname;

	/*
	 *	Houve algum erro
	 */
    bad:
	iput (ip);
	return (NOINODE);

}	/* end iname */
