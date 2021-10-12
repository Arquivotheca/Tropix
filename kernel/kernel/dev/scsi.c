/*
 ****************************************************************
 *								*
 *			scsi.c					*
 *								*
 *	Funções usadas pelos dispositivos SCSI			*
 *								*
 *	Versão	3.0.5, de 19.02.97				*
 *		4.5.0, de 23.12.03				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/map.h"

#include "../h/disktb.h"
#include "../h/kfile.h"
#include "../h/bhead.h"
#include "../h/scsi.h"
#include "../h/ioctl.h"
#include "../h/signal.h"
#include "../h/uproc.h"
#include "../h/uerror.h"

#include "../h/cdio.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ******	Macro para ZIP e JAZ ************************************
 */
#define	IS_ZIP_OR_JAZ(sp)				\
	(streq ((sp)->scsi_product, "ZIP 100") ||	\
	 streq ((sp)->scsi_product, "JAZ 1GB"))

/*
 ****************************************************************
 *	Função de "open"					*
 ****************************************************************
 */
int
scsi_open (SCSI *sp, dev_t dev, int oflag)
{
	BHEAD		*bp;
	DISKTB		*up;
	const char	*cp;
	int		i, old_disksz;
	char		status;

	/*
	 *	Prólogo
	 */
	if (sp->scsi_is_present < 0)
		{ u.u_error = ENXIO; return (-1); }

	if (sp->scsi_nopen > 0)
		return (0);

	/*
	 *	Coloca o nome do disco
	 */
	if (sp->scsi_dev_nm == NOSTR)
	{
		if ((up = disktb_get_first_entry (dev)) != NODISKTB)
			sp->scsi_dev_nm = up->p_name;
		else
			sp->scsi_dev_nm = "???";
	}

	/*
	 *	Obtém um BHEAD
	 */
	bp = block_free_get (BLSZ);

	bp->b_flags	= B_STAT|B_READ;
	bp->b_phdev	= dev;
   /***	bp->b_phblkno	= 0; ***/
   	bp->b_base_count = 255;
   	bp->b_error	= 0;

   /***	EVENTCLEAR (&bp->b_done); ***/

	if (sp->scsi_is_present > 0)
		goto check_if_is_ready;

	/*
	 *	Executa um comando INQUIRY para a unidade
	 */
   /***	memclr (bp->b_cmd_txt, sizeof (bp->b_cmd_txt)); ***/
	bp->b_cmd_txt[0] = SCSI_CMD_INQUIRY;
	bp->b_cmd_txt[4] = 255;		/* Comprimento da área */

	bp->b_cmd_len	 = 6;

	if ((*sp->scsi_cmd) (bp) < 0)
		{ u.u_error = EOLINE; goto bad; }

	/*
	 *	Obtém as características do dispositivo
	 */
	cp = bp->b_base_addr;

   /***	sp->scsi_is_disk = 0;		***/
   /***	sp->scsi_is_tape = 0;		***/
   /***	sp->scsi_read_only = 0;		***/
	sp->scsi_removable = (cp[1] & 0x80) ? 1 : 0;
   /***	sp->scsi_company[0] = '\0';	***/

	sp->scsi_blsz	 = BLSZ;	/* Em princípio blocos de 512 bytes */
	sp->scsi_blshift = BLSHIFT;

	switch (cp[0])
	{
	    case 0:	/* Disco normal */
	    case 4:	/* Disco de escrita única */
		sp->scsi_is_disk++;
		break;

	    case 1:	/* Fita */
		sp->scsi_is_tape++;
		break;

	    case 5:	/* Disco somente de leitura (CDROM) */
		sp->scsi_is_disk++;
		sp->scsi_read_only++;
		break;

	    case 127:
		printf ("%s: Unidade lógica NÃO presente\n", sp->scsi_dev_nm);
		u.u_error = ENXIO; goto bad;

	    default:
		printf ("%s: Tipo de unidade desconhecida: %d\n", sp->scsi_dev_nm, cp[0]);
		u.u_error = ENXIO; goto bad;

	}	/* end switch */

	/*
	 *	Obtém a marca, produto, ...
	 */
	if (cp[4] >= 31)
	{
		scsi_str_conv (sp->scsi_company,  cp + 8,  sizeof (sp->scsi_company));
		scsi_str_conv (sp->scsi_product,  cp + 16, sizeof (sp->scsi_product));
		scsi_str_conv (sp->scsi_revision, cp + 32, sizeof (sp->scsi_revision));
	}

	if (scb.y_tagenable)
	{
		if (cp[7] & 0x02)
			sp->scsi_tagenable++;
	}

	/*
	 *	Fala sobre o dispositivo presente
	 */
#ifdef	VERBOSE
	printf ("%s:", sp->scsi_dev_nm);

#if (0)	/*******************************************************/
	if (sp->scsi_company[0] != '\0')
	{
#endif	/*******************************************************/
		printf
		(	" <%s, %s, %s>",
			sp->scsi_product, sp->scsi_company, sp->scsi_revision
		);
#if (0)	/*******************************************************/
	}
#endif	/*******************************************************/

	if (sp->scsi_read_only)
		printf (" readonly");

	if (sp->scsi_removable)
		printf (" removable");
	else
		printf (" winchester");

	if (sp->scsi_is_disk)
		printf (" disk");

	if (sp->scsi_is_tape)
		printf (" tape");

	if (sp->scsi_tagenable)
		printf (" (tag enable)");

	putchar ('\n');
#endif	VERBOSE

	sp->scsi_is_present = 1;

	/*
	 *	Verifica se a unidade está ONLINE
	 */
    check_if_is_ready:
	bp->b_flags  = B_STAT|B_READ;
   	bp->b_base_count = 0;
   	bp->b_error  = 0;

	EVENTCLEAR (&bp->b_done);

	memclr (bp->b_cmd_txt, sizeof (bp->b_cmd_txt));
   /***	bp->b_cmd_txt[0] = SCSI_CMD_TEST_UNIT_READY; ***/

	bp->b_cmd_len	 = 6;

	if ((*sp->scsi_cmd) (bp) < 0)
		{ u.u_error = EOLINE; goto bad; }

	/*
	 *	Executa um (ou mais) comandos READ CAPACITY para a unidade
	 */
	old_disksz = sp->scsi_disksz;

	for (i = 0; /* abaixo */; i++)
	{
		if (i >= 10)
			{ u.u_error = EOLINE; goto bad; }

		bp->b_flags  = B_STAT|B_READ;
		bp->b_error  = 0;
		bp->b_base_count = 2 * sizeof (long);

		EVENTCLEAR (&bp->b_done);

	   /***	memclr (bp->b_cmd_txt, sizeof (bp->b_cmd_txt)); ***/
		bp->b_cmd_txt[0] = SCSI_CMD_READ_CAPACITY;

		bp->b_cmd_len	 = 10;

		if ((*sp->scsi_cmd) (bp) < 0)
			{ u.u_error = EOLINE; goto bad; }

		cp = bp->b_base_addr;

		sp->scsi_disksz	= long_endian_cv (*(long *)&cp[0]) + 1;
		sp->scsi_blsz	= long_endian_cv (*(long *)&cp[4]);

		switch (sp->scsi_blsz)
		{
		    case 1 * BLSZ:
			sp->scsi_blshift = BLSHIFT + 0;
			break;

		    case 2 * BLSZ:
			sp->scsi_blshift = BLSHIFT + 1;
			break;

		    case 4 * BLSZ:
			sp->scsi_blshift = BLSHIFT + 2;
			break;

		    case 8 * BLSZ:
			sp->scsi_blshift = BLSHIFT + 3;
			break;

		    case CD_RAW_BLSZ:
			sp->scsi_blshift = BLSHIFT + 2;
			break;

		    default:
			continue;

		}	/* end (switch) */

		break;

	}	/* end for (...) */

	/*
	 *	Consulta a proteção, se for o caso, ...
	 */
	if (IS_ZIP_OR_JAZ (sp))
	{
		bp->b_flags  = B_STAT|B_READ;
	   	bp->b_error  = 0;
		bp->b_base_count = 128;

		EVENTCLEAR (&bp->b_done);

		bp->b_cmd_txt[0] = SCSI_CMD_GET_PROTECTION;
		bp->b_cmd_txt[1] = 0;
		bp->b_cmd_txt[2] = 2;
		bp->b_cmd_txt[3] = 0;
		bp->b_cmd_txt[4] = 128;	/* Comprimento da área */
		bp->b_cmd_txt[5] = 0;

		bp->b_cmd_len	 = 6;

		if ((*sp->scsi_cmd) (bp) < 0)
			{ u.u_error = EOLINE; goto bad; }

		cp = bp->b_base_addr; status = cp[21] & 0x0F;

		if (oflag & O_WRITE && status != 0 && (status & 0x08) == 0)
			{ u.u_error = EROFS; goto bad; }
	}

	/*
	 *	Sucesso
	 */
#ifdef	VERBOSE
	if (sp->scsi_removable || old_disksz == 0) printf
	(
		"%s: %d blocos de %d bytes (%d MB)\n",
		sp->scsi_dev_nm, sp->scsi_disksz, sp->scsi_blsz,
		(((unsigned)sp->scsi_disksz >> KBSHIFT) * sp->scsi_blsz) >> KBSHIFT
	);
#endif	VERBOSE

	/*
	 *	Verifica se há tabela de partições ou não
	 *
	 *	Por convenção, se o tamanho for "BL4SZ / BLSZ", há tabela de partições
	 */
	up = &disktb[MINOR (dev)];

	if   (up->p_size == BL4SZ / BLSZ)
	{
		if (disktb_open_entries (dev, sp) < 0)	/* Lê a tabela de partições */
			goto bad;
	}
	elif (up->p_size == 0)
	{
#if (0)	/*******************************************************/
		up->p_size	= sp->scsi_disksz * (sp->scsi_blsz >> BLSHIFT);
#endif	/*******************************************************/
		up->p_size	= sp->scsi_disksz << (sp->scsi_blshift - BLSHIFT);
		up->p_blshift	= sp->scsi_blshift;
	}

#if (0)	/*******************************************************/
	/*
	 *	Atualiza a DISKTB		(PROVISÓRIO)
	 */
	if   (IS_ZIP_OR_JAZ (sp))
	{
		if (disktb_open_entries (dev, sp) < 0)
			goto bad;
	}
	elif (MAJOR (dev) == 8 /**  UD_MAJOR **/)	/* PROVISÓRIO */
	{
		if (disktb_open_entries (dev, sp) < 0)
			goto bad;
	}
	elif (sp->scsi_is_disk /*** && sp->scsi_read_only ***/)	/* CDROM */
	{
		up = &disktb[MINOR (dev)];
		up->p_size = sp->scsi_disksz * (sp->scsi_blsz >> BLSHIFT);
	}
#endif	/*******************************************************/

	bp->b_dev = bp->b_phdev = NODEV; block_put (bp);
	return (0);

	/*
	 *	Algum erro
	 */
    bad:
	bp->b_dev = bp->b_phdev = NODEV; block_put (bp);
	return (-1);

}	/* end scsi_open */

/*
 ****************************************************************
 *	Função de "close"					*
 ****************************************************************
 */
void
scsi_close (SCSI *sp, dev_t dev)
{
	if (IS_ZIP_OR_JAZ (sp))
	{
		disktb_close_entry (dev);
	}
	elif (sp->scsi_is_disk && sp->scsi_read_only)	/* CDROM */
	{
		DISKTB		*up;

		up = &disktb[MINOR (dev)];
		up->p_size = 0;
	}

}	/* end scsi_close */

/*
 ****************************************************************
 *	Rotina de IOCTL						*
 ****************************************************************
 */
int
scsi_ctl (SCSI *sp, dev_t dev, int cmd, int arg)
{
	BHEAD		*bp;
	char		*cp;
	int		status, len;

	/*
	 *	Verifica qual o comando
	 */
	switch (cmd)
	{
	    case DKISADISK:
		if (sp->scsi_is_disk)
			return (0);
		break;

	    case ZIP_IS_ZIP:
		if (IS_ZIP_OR_JAZ (sp))
			return (0);

		break;

	    case ZIP_GET_PROTECTION:
		if (!IS_ZIP_OR_JAZ (sp))
			{ u.u_error = EINVAL; return (-1); }

		bp = block_free_get (BLSZ);

		bp->b_flags	= B_STAT|B_READ;
		bp->b_phdev	= dev;
	   /***	bp->b_phblkno	= 0; ***/
	   	bp->b_base_count = 128;
	   	bp->b_error	= 0;

	   /***	EVENTCLEAR (&bp->b_done); ***/

		bp->b_cmd_txt[0] = SCSI_CMD_GET_PROTECTION;
		bp->b_cmd_txt[2] = 2;
		bp->b_cmd_txt[4] = 128;	/* Comprimento da área */

		bp->b_cmd_len	 = 6;

		if ((*sp->scsi_cmd) (bp) < 0)
			{ u.u_error = EOLINE; goto bad; }

		cp = bp->b_base_addr; status = cp[21] & 0x0F;

		bp->b_dev = bp->b_phdev = NODEV; block_put (bp);
		return (status);

	    case ZIP_SET_PROTECTION:
	    {
		struct	{ int mode; char pwd[80]; }	mode_pwd;

		if (!IS_ZIP_OR_JAZ (sp))
			{ u.u_error = EINVAL; return (-1); }

		if (superuser () < 0)
			return (-1);

		if (sp->scsi_nopen > 1)
			{ u.u_error = EBUSY; return (-1); }

		if (unimove (&mode_pwd, (void *)arg, sizeof (mode_pwd), US) < 0)
			{ u.u_error = EFAULT; return (-1); }

		mode_pwd.pwd[sizeof (mode_pwd.pwd) - 1] = '\0';
		len = strlen (mode_pwd.pwd);

		bp = block_free_get (BLSZ);

		bp->b_flags	= B_STAT|B_WRITE;
		bp->b_phdev	= dev;
	   /***	bp->b_phblkno	= 0; ***/
	   	bp->b_base_count = len;
	   	bp->b_error	= 0;

		memmove (bp->b_base_addr, mode_pwd.pwd, sizeof (mode_pwd.pwd));

	   /***	EVENTCLEAR (&bp->b_done); ***/

		bp->b_cmd_txt[0] = SCSI_CMD_SET_PROTECTION;
		bp->b_cmd_txt[1] = mode_pwd.mode;
		bp->b_cmd_txt[4] = len;		/* Comprimento da senha */

		bp->b_cmd_len	 = 6;

		if ((*sp->scsi_cmd) (bp) < 0)
			{ u.u_error = EINVAL; goto bad; }

		bp->b_dev = bp->b_phdev = NODEV; block_put (bp);
		return (0);
	    }

	    case CD_START_STOP:
	    case ZIP_START_STOP:
		if (superuser () < 0)
			return (-1);

		if (sp->scsi_nopen > 1)
			{ u.u_error = EBUSY; return (-1); }

		bp = block_free_get (BLSZ);

		bp->b_flags	= B_STAT|B_READ;
		bp->b_phdev	= dev;
	   /***	bp->b_phblkno	= 0; ***/
	   	bp->b_base_count = 0;
	   	bp->b_error	= 0;

	   /***	EVENTCLEAR (&bp->b_done); ***/

		bp->b_cmd_txt[0] = SCSI_CMD_START_STOP;
		bp->b_cmd_txt[4] = arg;		/* 1: Start, 0: Stop */

		bp->b_cmd_len = 6;

		if ((*sp->scsi_cmd) (bp) < 0)
			{ u.u_error = EINVAL; goto bad; }

		bp->b_dev = bp->b_phdev = NODEV; block_put (bp);
		return (0);

	    case CD_LOCK_UNLOCK:
	    case ZIP_LOCK_UNLOCK:
		if (superuser () < 0)
			return (-1);

		if (sp->scsi_nopen > 1)
			{ u.u_error = EBUSY; return (-1); }

		bp = block_free_get (BLSZ);

		bp->b_flags	= B_STAT|B_READ;
		bp->b_phdev	= dev;
	   /***	bp->b_phblkno	= 0; ***/
	   	bp->b_base_count = 0;
	   	bp->b_error	= 0;

	   /***	EVENTCLEAR (&bp->b_done); ***/

		bp->b_cmd_txt[0] = SCSI_CMD_PREVENT_ALLOW;
		bp->b_cmd_txt[4] = arg;

		bp->b_cmd_len = 6;

		if ((*sp->scsi_cmd) (bp) < 0)
			{ u.u_error = EINVAL; goto bad; }

		bp->b_dev = bp->b_phdev = NODEV; block_put (bp);
		return (0);

	    case CD_READ_TOC:
	    {
		TOC		*tp;
		TOC_ENTRY	*ep;
		int		ntracks;

		/*
		 *	Lê o cabeçalho da TOC
		 */
		bp = block_free_get (BLSZ);

		bp->b_flags	= B_STAT|B_READ;
		bp->b_phdev	= dev;
	   /***	bp->b_phblkno	= 0;	***/
	   	bp->b_base_count = sizeof (TOC_HEADER);
	   	bp->b_error	= 0;

	   /***	EVENTCLEAR (&bp->b_done); ***/

		bp->b_cmd_txt[0] = SCSI_CMD_READ_TOC;
		bp->b_cmd_txt[7] = sizeof (TOC_HEADER) >> 8;
		bp->b_cmd_txt[8] = sizeof (TOC_HEADER);

		bp->b_cmd_len = 10;

		if ((*sp->scsi_cmd) (bp) < 0)
			{ u.u_error = EINVAL; goto bad; }

		tp = (TOC *)bp->b_base_addr;

		/*
		 *	Descobre o número de trilhas
		 */
		ntracks = tp->hdr.ending_track - tp->hdr.starting_track + 1;

		if (ntracks <= 0 || ntracks > MAXTRK)
		{
			printf
			(	"%s: número excessivo de trilhas: %d\n",
				sp->scsi_dev_nm, ntracks
			);

			u.u_error = EIO;
			goto bad;
		}

		/*
		 *	Agora, lê a TOC na íntegra
		 */
		len = sizeof (TOC_HEADER) + ntracks * sizeof (TOC_ENTRY);

		bp->b_flags	= B_STAT|B_READ;
	   	bp->b_base_count = len;
	   	bp->b_error	= 0;

	   	EVENTCLEAR (&bp->b_done);

		bp->b_cmd_txt[0] = SCSI_CMD_READ_TOC;
		bp->b_cmd_txt[7] = len >> 8;
		bp->b_cmd_txt[8] = len;

		bp->b_cmd_len = 10;

		if ((*sp->scsi_cmd) (bp) < 0)
			{ u.u_error = EINVAL; goto bad; }

		/*
		 *	Converte BIG para LITTLE ENDIAN
		 */
		tp->hdr.len = short_endian_cv (tp->hdr.len);

		for (ep = &tp->tab[0]; ep < &tp->tab[ntracks]; ep++)
		{
			if (ep->addr_type == CD_LBA_FORMAT)
				ep->addr.lba = long_endian_cv (ep->addr.lba);
		}

		/*
		 *	Coloca a sentinela ao final
		 */
	   /***	ep = &tp->tab[ntracks];	***/

		ep->control	= (ep - 1)->control;
		ep->addr_type	= CD_LBA_FORMAT;
		ep->track	= 170;
		ep->addr.lba	= sp->scsi_disksz;

		/* Verifica se a área é do usuário/supervisor */

		if (arg < SYS_ADDR)
		{
			if (unimove ((TOC *)arg, tp, sizeof (TOC), SU) < 0)
				{ u.u_error = EFAULT; goto bad; }
		}
		else
		{
			memmove ((TOC *)arg, tp, sizeof (TOC));
		}

		bp->b_dev = bp->b_phdev = NODEV; block_put (bp);

		return (ntracks);
	    }

	    case CD_PLAY_BLOCKS:
	    {
		struct ioc_play_blocks	play, *ap = &play;

		if (unimove (ap, (struct ioc_play_blocks *)arg, sizeof (play), US) < 0)
			{ u.u_error = EFAULT; return (-1); }

		if
		(	ap->blk < 0 || ap->len <= 0 ||
			ap->blk >= sp->scsi_disksz ||
			ap->blk + ap->len > sp->scsi_disksz
		)
			break;

		bp = block_free_get (BLSZ);

		bp->b_flags	= B_STAT|B_READ;
		bp->b_phdev	= dev;
	   /***	bp->b_phblkno	= 0;	***/
	   	bp->b_base_count = 0;
	   	bp->b_error	= 0;

	   /***	EVENTCLEAR (&bp->b_done); ***/

		bp->b_cmd_txt[0]	   = SCSI_CMD_PLAY_BIG;
		*(long *)&bp->b_cmd_txt[2] = long_endian_cv (ap->blk);
		*(long *)&bp->b_cmd_txt[6] = long_endian_cv (ap->len);

		bp->b_cmd_len = 12;

		if ((*sp->scsi_cmd) (bp) < 0)
			{ u.u_error = EINVAL; goto bad; }

		bp->b_dev = bp->b_phdev = NODEV; block_put (bp);
		return (0);
	    }

	    case CD_PAUSE_RESUME:
		bp = block_free_get (BLSZ);

		bp->b_flags	= B_STAT|B_READ;
		bp->b_phdev	= dev;
	   /***	bp->b_phblkno	= 0;	***/
	   	bp->b_base_count = 0;
	   	bp->b_error	= 0;

	   /***	EVENTCLEAR (&bp->b_done);	***/

		bp->b_cmd_txt[0] = SCSI_CMD_PAUSE_RESUME;
		bp->b_cmd_txt[8] = arg;	/* 0: Pause; 1: Resume */

		bp->b_cmd_len = 10;

		if ((*sp->scsi_cmd) (bp) < 0)
			{ u.u_error = EINVAL; goto bad; }

		bp->b_dev = bp->b_phdev = NODEV; block_put (bp);
		return (0);

	    case CD_READ_SUBCHANNEL:
		bp = block_free_get (BLSZ);

		bp->b_flags	= B_STAT|B_READ;
		bp->b_phdev	= dev;
	   /***	bp->b_phblkno	= 0;	***/
	   	bp->b_base_count = sizeof (struct cd_sub_channel_info);
	   	bp->b_error	= 0;

	   /***	EVENTCLEAR (&bp->b_done); ***/

		bp->b_cmd_txt[0] = SCSI_CMD_READ_SUBCHANNEL;
		bp->b_cmd_txt[2] = 0x40;
		bp->b_cmd_txt[3] = CD_CURRENT_POSITION;
		bp->b_cmd_txt[7] = sizeof (struct cd_sub_channel_info) >> 8;
		bp->b_cmd_txt[8] = sizeof (struct cd_sub_channel_info);

		bp->b_cmd_len = 10;

		if ((*sp->scsi_cmd) (bp) < 0)
			{ u.u_error = EINVAL; goto bad; }

		if
		(	unimove
			(	(struct cd_sub_channel_info *)arg,
				bp->b_base_addr,
				sizeof (struct cd_sub_channel_info),
				SU
			) < 0
		)
			{ u.u_error = EFAULT; goto bad; }

		bp->b_dev = bp->b_phdev = NODEV; block_put (bp);
		return (0);

	    case CD_READ_AUDIO:
	    {
		struct ioc_read_audio	raudio;
		int			count;
		void			*ph_addr;
		static BHEAD		bh;

		if (unimove (&raudio, (struct ioc_read_audio *)arg, sizeof (raudio), US) < 0)
			{ u.u_error = EFAULT; return (-1); }

		if (raudio.nframes < 0 || raudio.lba < 0)
			{ u.u_error = EINVAL; return (-1); }

		/*
		 *	Obtém o endereço físico correspondente ao buffer do
		 *	usuário
		 */
		count   = raudio.nframes * CD_RAW_BLSZ;
		ph_addr = user_area_to_phys_area (raudio.buf, count);

		if (ph_addr == NOVOID)
			{ u.u_error = EFAULT; return (-1); }

		/*
		 *	Preenche um "bhead" com os dados da operação
		 */
		bp		= &bh;

		SLEEPLOCK (&bp->b_lock, PBLKIO-1);

		bp->b_flags	= B_STAT|B_READ;
		bp->b_phdev	= bp->b_dev     = dev;
		bp->b_blkno	= bp->b_phblkno = raudio.lba;
		bp->b_addr	=
		bp->b_base_addr	= (void *)PHYS_TO_VIRT_ADDR (ph_addr);
	   	bp->b_base_count = count;
		bp->b_residual	= 0;
	   	bp->b_error	= 0;

		EVENTCLEAR (&bp->b_done);

		/*
		 *	Usa o comando de extração apropriado
		 */
		if (sp->scsi_is_atapi)
		{
			bp->b_cmd_txt[0] = SCSI_CMD_READ_ATAPI_CD;
			bp->b_cmd_txt[1] = 4;
			*(long *)&bp->b_cmd_txt[2] = long_endian_cv (raudio.lba);
			bp->b_cmd_txt[6] = 0;
			bp->b_cmd_txt[7] = 0;
			bp->b_cmd_txt[8] = raudio.nframes;
			bp->b_cmd_txt[9] = 0xF0;
		}
		else
		{
			bp->b_cmd_txt[0] = SCSI_CMD_READ_CD;
			*(long *)&bp->b_cmd_txt[2] = long_endian_cv (raudio.lba);
			*(long *)&bp->b_cmd_txt[6] = long_endian_cv (raudio.nframes);
		}

		bp->b_cmd_len = 12;

	   /***	u.u_flags |= SLOCK; ***/

		if ((*sp->scsi_cmd) (bp) < 0)
			{ u.u_error = EINVAL; return (-1); }

	   /***	u.u_flags &= ~SLOCK; ***/

		SLEEPFREE (&bp->b_lock);

		return (count);
	    }
	}

	u.u_error = EINVAL;
  	return (-1);

	/*
	 *	Saída de erro para algumas condições
	 */
    bad:
	bp->b_dev = bp->b_phdev = NODEV; block_put (bp);
  	return (-1);

}	/* end scsi_ctl */

/*
 ****************************************************************
 *	Decodifica o código do SCSI SENSE			*
 ****************************************************************
 */
int
scsi_sense (SCSI *sp, int sense)
{
	/*
	 *	Retorna:
	 *		-1: Erro irrecuperável (very bad)
	 *		 0: Erro recuperável (bad)
	 *		 1: Sem erro (good)
	 */
	if (sense == 0)		/* Sem erro */
		return (1);

	printf ("%s: ", sp->scsi_dev_nm);

	switch (sense)
	{
	    case -1:
		printf ("NÃO consegui obter o SENSE\n");
		return (0);

	    case 1:
		printf ("Erro recuperado\n");
		return (1);

	    case 2:
		printf ("Unidade não pronta\n");
		return (-1);

	    case 3:
		printf ("Erro NÃO recuperado (meio)\n");
		return (0);

	    case 4:
		printf ("Erro NÃO recuperado (circuito)\n");
		return (0);

	    case 5:
		printf ("Pedido ilegal\n");
		return (-1);

	    case 6:
		printf ("O meio foi trocado\n");
		sp->scsi_medium_changed = 1;
		return (0);

	    case 7:
		printf ("Proteção de escrita\n");
		return (-1);

	    case 11:
		printf ("Comando abortado\n");
		return (0);

	    case 13:
		printf ("Pedido além da capacidade da unidade\n");
		return (-1);

	    default:
		printf ("Código de SENSE SCSI desconhecido: %d\n", sense);
		return (0);

	}	/* end switch */

}	/* end scsi_sense */

/*
 ****************************************************************
 *	Retira os brancos e converte para maíuscula		*
 ****************************************************************
 */
char *
scsi_str_conv (char *dst, const char *src, int sz)
{
	char		c, *cp;

	/*
	 *	Retira os brancos
	 */
	memmove (dst, src, sz - 1);

	cp = dst + sz - 1; *cp = '\0';

	for (cp--; cp >= dst; cp--)
	{
		if (*cp != ' ')
			break; 
	}

	cp[1] = '\0';

	/*
	 *	Converte para maíuscula
	 */
	for (cp = dst; (c = *cp) != '\0'; cp++)
	{
		if (c >= 'a' && c <= 'z')
			*cp = c - ('a' - 'A');
	}

	return (dst);

}	/* scsi_str_conv */

/*
 ****************************************************************
 *	Retorna o nome de um comando SCSI			*
 ****************************************************************
 */
char *
scsi_cmd_name (int cmd)
{
	char	buf[80];

	switch (cmd)
	{
	    case 0x00:
		return ("TEST_UNIT_READY");
	    case 0x01:
		return ("REZERO_UNIT");
	    case 0x03:
		return ("REQUEST_SENSE");
	    case 0x04:
		return ("FORMAT_UNIT");
	    case 0x06:
		return ("GET_PROTECTION");
	    case 0x0C:
		return ("SET_PROTECTION");
	    case 0x12:
		return ("INQUIRY");
	    case 0x1B:
		return ("START_STOP");
	    case 0x1E:
		return ("PREVENT_ALLOW");
	    case 0x25:
		return ("READ_CAPACITY");
	    case 0x28:
		return ("READ_BIG");
	    case 0x2A:
		return ("WRITE_BIG");
	    case 0x35:
		return ("SYNCHRONIZE_CACHE");
	    case 0x42:
		return ("READ_SUBCHANNEL");
	    case 0x43:
		return ("READ_TOC");
	    case 0x47:
		return ("PLAY_MSF");
	    case 0x48:
		return ("PLAY_TRACK");
	    case 0x4B:
		return ("PAUSE");
	    case 0x51:
		return ("READ_DISC_INFO");
	    case 0x52:
		return ("READ_TRACK_INFO");
	    case 0x53:
		return ("RESERVE_TRACK");
	    case 0x54:
		return ("SEND_OPC_INFO");
	    case 0x55:
		return ("MODE_SELECT");
	    case 0x58:
		return ("REPAIR_TRACK");
	    case 0x59:
		return ("READ_MASTER_CUE");
	    case 0x5A:
		return ("MODE_SENSE");
	    case 0x5B:
		return ("CLOSE_TRACK/SESSION");
	    case 0x5C:
		return ("READ_BUFFER_CAPACITY");
	    case 0x5D:
		return ("SEND_CUE_SHEET");
	    case 0xA1:
		return ("BLANK_CMD");
	    case 0xA5:
		return ("PLAY_BIG");
	    case 0xB4:
		return ("PLAY_CD");
	    case 0xBD:
		return ("ATAPI_MECH_STATUS"); 
	    case 0xBE:
		return ("READ_CD");
	}

	snprintf (buf, sizeof (buf), "COMANDO DESCONHECIDO = %X", cmd);

	return (buf);

}	/* end scsi_cmd_name */
