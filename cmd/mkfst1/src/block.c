/*
 ****************************************************************
 *								*
 *			block.c					*
 *								*
 *	Gerência de blocos do dispositivo			*
 *								*
 *	Versão	4.3.0, de 04.07.02				*
 *		4.4.0, de 02.11.02				*
 *								*
 *	Módulo: mkfst1						*
 *		Utilitários especiais				*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/sync.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/t1.h>
#include <sys/disktb.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "../h/mkfst1.h"

/*
 ****************************************************************
 *	Identifica o nome do "boot1"				*
 ****************************************************************
 */
const char *
get_boot1_nm (void)
{
	const char	*cp;

	/*
	 *	Em primeiro lugar, identifica o tipo de dispositivo
	 */
	cp = dev_disktb.p_name;

	if (cp[0] == '\0')
		return (NOSTR);

	/*
	 *	Acha o nome do "boot1"
	 */
	if (strncmp (cp, "ramd", 4) == 0)
	{
		return ("/etc/boot/t1.g.boot1");
	}
	if (cp[1] != 'd')
	{
		return (NOSTR);
	}
	elif (cp[0] == 'h' || cp[0] == 's' || cp[0] == 'm')
	{
		if (dev_disktb.p_flags & DISK_INT13_EXT)
			return ("/etc/boot/t1.l.boot1");
		else
			return ("/etc/boot/t1.g.boot1");
	}
	elif (cp[0] != 'f')
	{
		return (NOSTR);
	}
	else switch (dev_disktb.p_size)
	{
	    case 2880:		/* Disquete 3½ 1440 KB */
		return ("/etc/boot/t1.d.boot1");

	    default:
		return (NOSTR);
	}

}	/* end get_boot1_nm */

/*
 ****************************************************************
 *	Escreve o bloco do "boot"				*
 ****************************************************************
 */
void
write_boot_block (const char *boot1_nm)
{
	int		fd;
	STAT		s;
	char		area[BL4SZ];

	if ((fd = open (boot1_nm, O_RDONLY)) < 0)
		{ error ("*Não consegui abrir \"%s\"", boot1_nm); goto bad1; }

	if (fstat (fd, &s) < 0)
		{ error ("*Não consegui obter o estado de \"%s\"", boot1_nm); goto bad0; }

	if (!S_ISREG (s.st_mode))
		{ error ("O arquivo \"%s\" não é regular", boot1_nm); goto bad0; }

	if (s.st_size > BL4SZ)
	{
		error ("O arquivo \"%s\" tem mais de " XSTR (BL4SZ) " bytes", boot1_nm);
		goto bad0;
	}

	memset (area, 0, BL4SZ);

	if (read (fd, area, s.st_size) != s.st_size)
		{ error ("*Não consegui ler \"%s\"", boot1_nm); goto bad0; }

	fs_write (T1_BOOTNO, &area);

	close (fd); return;

	/*
	 *	Em caso de erro, ...
	 */
   bad0:
	close (fd);
   bad1:
	error ("Bloco de BOOT inalterado");

}	/* end write_boot_block */

/*
 ****************************************************************
 *	Le um bloco do dispositivo				*
 ****************************************************************
 */
void
fs_read (daddr_t bno, void *area)
{
	llseek (fs_fd, (loff_t)bno << BL4SHIFT, NULL, L_SET);

	if (read (fs_fd, area, BL4SZ) != BL4SZ)
		error ("$*Erro de leitura: bloco \"%d\"", bno);

}	/* end fs_read */

/*
 ****************************************************************
 *	Escreve um Bloco no Dispositivo				*
 ****************************************************************
 */
void
fs_write (daddr_t bno, const void *area)
{
	llseek (fs_fd, (loff_t)bno << BL4SHIFT, NULL, L_SET);

	if (write (fs_fd, area, BL4SZ) != BL4SZ)
		error ("$*Erro de escrita: bloco \"%d\"", bno);

}	/* end fs_write */

/*
 ****************************************************************
 *	Zera um Bloco do Dispositivo				*
 ****************************************************************
 */
void
fs_clear (daddr_t bno)
{
	char		area[BL4SZ];

	memsetl (area, 0, sizeof (area) / sizeof (long));

	fs_write (bno, area);

}	/* end fs_clear */
