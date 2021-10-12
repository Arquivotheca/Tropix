/*
 ****************************************************************
 *								*
 *			tok.c					*
 *								*
 *	General Archiver -- Utilitario para Fitas/Disquettes	*
 *								*
 *	Versão	1.1.0, de 11.01.88				*
 *		4.8.0, de 19.07.05				*
 *								*
 *	Módulo: GAR						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2005 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>

#include <stdio.h>
#include <stdlib.h>
#include <stat.h>
#include <string.h>

#include "../h/common.h"
#include "../h/gar.h"
#include "../h/tar.h"
#include "../h/cpio.h"

/*
 ******	Variáveis globais ***************************************
 */
entry int	col_version;

/*
 ****************************************************************
 *	Identifica o arquivo (GAR, CPIO, TAR)			*
 ****************************************************************
 */
TYPE
typeid (void)
{
	CPHEAD		*cp;
	THEAD		*tp;
	GVOL		*gv;
	int		crc;

	/*
	 *	Verifica se é GAR.
	 */
	gv = (GVOL *)b->buf;

	if (strncmp (gv->g_magic, garvolume, GARMSZ) == 0)
	{
		date		= atoi (gv->g_date);
		vol		= atoi (gv->g_vol);
		col_version 	= atoi (gv->g_version + 7);
		crc		= atoi (gv->g_vcrc);

		if (crc != crc16 (gv, GVOLCRCSZ) )
			error ("Erro de CRC no volume GAR");

		if (vol != 0)
		{
			error ("Este volume não é o começo da coleção GAR");
			fprintf (stderr, "\tContinua? (n): ");
			if (askyesno () <= 0)
				quit (1);
			garheader_search ();
			return (GAR);
		}

		col_area_avail_sz = col_area_sz - sizeof (GVOL);
		totalbytes = sizeof (GVOL);

		return (GAR);
	}

	col_area_avail_sz = col_area_sz;

	/*
	 *	Verifica se é CPIO.
	 */
	cp = (CPHEAD *)b->buf;

	if (cp->c_magic == CPIOMAGIC)
		return (CPIO);

	if (cp->c_magic == CPIORMAGIC)
		return (CPIOR);

	/*
	 *	Verifica se é TAR.
	 */
	tp = (THEAD *)b->buf;

	if (otoi (tp->t_chksum) == tchecksum (tp) )
		return (TAR);

	/*
	 *	Modo de PANICO - tenta analisar o interior da coleção
	 */
	if (Tflag)
	{
		error ("Coleção não identificada");
		fprintf (stderr, "\tTenta achar o início de um arquivo TAR? (n): ");
		if (askyesno () <= 0)
			quit (1);
	
		tarheader_search ();
	
		vol = 0;
	
		return (TAR);
	}

	/*
	 *	Tenta achar GAR
	 */
	error ("Coleção não identificada");
	fprintf (stderr, "\tTenta achar o início de um arquivo GAR? (n): ");
	if (askyesno () <= 0)
		quit (1);

	garheader_search ();

	vol = 0;

	return (GAR);

}	/* end typeid */ 

/*
 ****************************************************************
 *	Procura o começo de um arquivo GAR (modo de PANICO)	*
 ****************************************************************
 */
void
garheader_search (void)
{
	char		area[GARMSZ];

	col_area_avail_sz = col_area_sz;

	for (EVER)
	{
		bread (&area[0], 1);
		if (area[0] != garheader[0])
			continue;
		bread (&area[1], GARMSZ-1);
		bback (GARMSZ, 0 /* read only */);
		if (strncmp (area, garheader, GARMSZ) == 0)
		{
			/* Lembrar do CRC */
			return;
		}
		bread (NOSTR, 1);	/* Pula o "G" não aceito */

	}	/* end for (EVER) */

}	/* end garheader_search */

/*
 ****************************************************************
 *	Procura o começo de um arquivo TAR (modo de PANICO)	*
 ****************************************************************
 */
void
tarheader_search (void)
{
	char		*cp;
	THEAD		thead;

	col_area_avail_sz = col_area_sz;

	for (EVER)
	{
		bread (&thead, sizeof (THEAD));

		if (otoi (thead.t_chksum) != tchecksum (&thead))
			continue;

		for (cp = thead.t_size; cp < &thead.t_size[11]; cp++)
		{
			if (*cp != ' ' && (*cp < '0' || *cp > '7'))
				continue;
		}

		bback (sizeof (THEAD), 0 /* read only */);
		return;

	}	/* end for (EVER) */

}	/* end garheader_search */

/*
 ****************************************************************
 *	Conversão do Formato TAR para o interno			* 
 ****************************************************************
 */
int
tartok (KHEAD *kh)
{
	THEAD		thead;
	THEAD		*th;

	th = &thead;
	bread (th, sizeof (THEAD) );

	if (th->t_name[0] == '\0')
		return (-1);

	kh->k_dev	= NODEV;
	kh->k_ino	= NOINO;
	kh->k_mode	= otoi (th->t_mode);
	kh->k_nlink	= NULL; /* Desconhecido */ 
	kh->k_uid	= otoi (th->t_uid);
	kh->k_gid	= otoi (th->t_gid);
	kh->k_rdev	= NODEV;
	kh->k_size	= otoi (th->t_size);
	kh->k_atime	=
	kh->k_mtime	= 
	kh->k_ctime	= otoi (th->t_mtime);
	kh->k_linkflag	= th->t_typeflag == '1' ? '*' : ' ';
	strcpy (kh->k_name,	th->t_name);
	strcpy (kh->k_linkname,	th->t_linkname);

	kh->k_mode &= ~S_IFMT;

	switch (th->t_typeflag)
	{
	    case 0:
	    case '0':
	    default:
		kh->k_mode |= S_IFREG;
		break;

	    case '3':
		kh->k_mode |= S_IFCHR;
		kh->k_rdev = MAKEDEV (otoi (th->t_devmajor), otoi (th->t_devminor));
		break;

	    case '4':
		kh->k_mode |= S_IFBLK;
		kh->k_rdev = MAKEDEV (otoi (th->t_devmajor), otoi (th->t_devminor));
		break;

	    case '5':
		kh->k_mode |= S_IFDIR;
		break;

	    case '6':
		kh->k_mode |= S_IFIFO;
		break;
	}

	if (otoi (th->t_chksum) != tchecksum (th) )
	{
		error ("Erro de checksum no header TAR");
		fprintf (stderr, "\tContinua? (n): ");
		if (askyesno () <= 0)
			quit (1);
	}

	return (0);

}	/* end tartok */

/*
 ****************************************************************
 *	Conversão do Formato CPIO para o interno		* 
 ****************************************************************
 */
int
cpiotok (KHEAD *kh)
{
	CPHEAD		chead;
	CPHEAD		*ch;
	char		*cp;
	char		c;

	ch = &chead;
	bread (ch, CPHEADSZ);

	/*
	 *	Processa o formato de bytes invertidos
	 */
	if (type == CPIOR)
	{
		for (cp = (char *)ch; cp < (char *)ch + CPHEADSZ; cp += 2)
		{
			c = cp[0]; 
			cp[0] = cp[1];
			cp[1] = c;
		}
	}

	if (ch->c_magic != CPIOMAGIC)
		error ("$Erro no número mágico de um header CPIO");

	kh->k_dev	= ch->c_dev;
	kh->k_ino	= ch->c_ino;
	kh->k_mode	= ch->c_mode;
	kh->k_nlink	= ch->c_nlink; 
	kh->k_uid	= ch->c_uid;
	kh->k_gid	= ch->c_gid;
	kh->k_rdev	= ch->c_rdev;
	kh->k_size	= *(off_t *)ch->c_size;
	kh->k_atime	=
	kh->k_mtime	= 
	kh->k_ctime	= ch->c_mtime;
	kh->k_linkflag	= ' ';

	bread (kh->k_name, WDROUND (ch->c_namesz) );

	if (streq (kh->k_name, CPIOTRAILER))
		return (-1);
	else
		return (0);

}	/* end cpiotok */

/*
 ****************************************************************
 *	Conversão do Formato GAR para o interno			* 
 ****************************************************************
 */
int
gartok (KHEAD *kh)
{
	char		c;
	int		namesz;
	char		crc[8];
	GHEAD		ghead;
	OGHEAD		oghead;

	/*
	 *	Parte dependente da versão
	 */
	if (col_version < 3)
	{
		bread (&oghead, sizeof (OGHEAD));

		if (strncmp (oghead.g_magic, gartrailer, GARMSZ) == 0)
			return (-1);

		if (strncmp (oghead.g_magic, garheader, GARMSZ) != 0)
			error ("$Erro na cadeia mágica de um header GAR: \"%P\"", *(long *)oghead.g_magic);

		if (atoi (oghead.g_crc) != crc16 (&oghead, OGHEADCRCSZ) )
			error ("Erro no CRC de um header GAR");

		kh->k_dev	= atoi (oghead.g_dev);
		kh->k_ino	= atoi (oghead.g_ino);
		kh->k_mode	= atoi (oghead.g_mode);
		kh->k_nlink	= atoi (oghead.g_nlink); 
		kh->k_uid	= atoi (oghead.g_uid);
		kh->k_gid	= atoi (oghead.g_gid);
		kh->k_rdev	= atoi (oghead.g_rdev);
		kh->k_size	= atoi (oghead.g_size);
		kh->k_atime	= atoi (oghead.g_atime);
		kh->k_mtime	= atoi (oghead.g_mtime); 
		kh->k_ctime	= atoi (oghead.g_ctime);
		kh->k_linkflag	= oghead.g_linkflag[1];

		if (oghead.g_origsz[11] != ' ') 
		{
			kh->k_origsz	= atoi (oghead.g_origsz);
			kh->k_bits	= atoi (oghead.g_bits);
			kh->k_compcrc	= oghead.g_compcrc[1] == ' ' ? 1 : 0;
			c		= oghead.g_method[1];
			kh->k_variant	= oghead.g_method[2];
		}
		else
		{
			c = 0;
			kh->k_origsz	= 0;
		}

		namesz = atoi (oghead.g_namesz);
	}
	else
	{
		bread (&ghead, sizeof (GHEAD));

		if (strncmp (ghead.g_magic, gartrailer, GARMSZ) == 0)
			return (-1);

		if (strncmp (ghead.g_magic, garheader, GARMSZ) != 0)
			error ("$Erro na cadeia mágica de um header GAR: \"%P\"", *(long *)ghead.g_magic);

		if (atoi (ghead.g_crc) != crc16 (&ghead, GHEADCRCSZ) )
			error ("Erro no CRC de um header GAR");

		kh->k_dev	= atoi (ghead.g_dev);
		kh->k_ino	= atoi (ghead.g_ino);
		kh->k_mode	= atoi (ghead.g_mode);
		kh->k_nlink	= atoi (ghead.g_nlink); 
		kh->k_uid	= atoi (ghead.g_uid);
		kh->k_gid	= atoi (ghead.g_gid);
		kh->k_rdev	= atoi (ghead.g_rdev);
		kh->k_size	= atoi (ghead.g_size);
		kh->k_atime	= atoi (ghead.g_atime);
		kh->k_mtime	= atoi (ghead.g_mtime); 
		kh->k_ctime	= atoi (ghead.g_ctime);
		kh->k_linkflag	= ghead.g_linkflag[1];

		if (ghead.g_origsz[11] != ' ') 
		{
			kh->k_origsz	= atoi (ghead.g_origsz);
			kh->k_bits	= atoi (ghead.g_bits);
			kh->k_compcrc	= ghead.g_compcrc[1] == ' ' ? 1 : 0;
			c		= ghead.g_method[1];
			kh->k_variant	= ghead.g_method[2];
		}
		else
		{
			c = 0;
			kh->k_origsz	= 0;
		}

		namesz = atoi (ghead.g_namesz);
	}

	/*
	 *	Parte comum
	 */
	if (c == ' ')
		c = 'C';

	if (c != 'C' && c != 'D' && c != 0)
		error ("Método de compressão desconhecido: '%c'", c);

	kh->k_method	= c;

	if (c == 'D' && kh->k_variant != '0')
		error ("Versão de deflação desconhecido: '%c'", kh->k_variant);

	bread (kh->k_name, namesz);
	kh->k_name[namesz] = '\0';

	bread (crc, sizeof (crc) );

	if (atoi (crc) != crc16 (kh->k_name, namesz)) 
		error ("Erro no CRC de um nome GAR");

	/*
	 *	Verifica se é um elo simbólico
	 */
	if (kh->k_linkflag == ' ')
	{
		if (S_ISLNK (kh->k_mode))
		{
			bread (kh->k_linkname, kh->k_size);
			kh->k_linkname[kh->k_size] = '\0';

			bread (crc, sizeof (crc) );

			if (atoi (crc) != crc16 (kh->k_linkname, kh->k_size)) 
				error ("Erro no CRC de um elo simbólico GAR");
		}

		return (0);
	}

	/*
	 *	Processa o nome do elo físico
	 */
	if (col_version < 3)
	{
		bread (&oghead, sizeof (OGHEAD) );

		if (strncmp (oghead.g_magic, garlink, GARMSZ) != 0)
			error ("$Erro na cadeia mágica de um elo físico GAR");

		if (atoi (oghead.g_crc) != crc16 (&oghead, OGHEADCRCSZ) )
			error ("Erro no CRC de um elo físico GAR");

		namesz = atoi (oghead.g_namesz);
	}
	else
	{
		bread (&ghead, sizeof (GHEAD) );

		if (strncmp (ghead.g_magic, garlink, GARMSZ) != 0)
			error ("$Erro na cadeia mágica de um elo físico GAR");

		if (atoi (ghead.g_crc) != crc16 (&ghead, GHEADCRCSZ) )
			error ("Erro no CRC de um elo físico GAR");

		namesz = atoi (ghead.g_namesz);
	}

	bread (kh->k_linkname, namesz);
	kh->k_linkname[namesz] = '\0';

	bread (crc, sizeof (crc) );

	if (atoi (crc) != crc16 (kh->k_linkname, namesz)) 
		error ("Erro no CRC de um nome de elo físico GAR");
		
	return (0);

}	/* end gartok */
