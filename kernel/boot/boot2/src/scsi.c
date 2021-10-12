/*
 ****************************************************************
 *								*
 *			scsi.c					*
 *								*
 *	Funções usadas pelos dispositivos SCSI			*
 *								*
 *	Versão	3.0.6, de 04.03.97				*
 *		4.6.0, de 08.05.04				*
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
#include <bcb.h>
#include <disktb.h>
#include <bhead.h>
#include <scsi.h>
#include <cdio.h>

#include "../h/common.h"
#include "../h/extern.h"
#include "../h/proto.h"

/*
 ******	Protótipos de funções ***********************************
 */
extern char	*ahc_edit_syncrate (int);

/*
 ****************************************************************
 *	Função de "open"					*
 ****************************************************************
 */
int
scsi_attach (SCSI *sp)
{
	const char	*cp;
	int		i;
	const DISK_INFO	*dp;
	char		area[256];
	BHEAD		bhead, *bp = &bhead;


	/*
	 *	Executa um comando INQUIRY para a unidade
	 */
	bp->b_ptr	= sp;
	bp->b_disktb	= sp->scsi_disktb;

	bp->b_flags	= B_READ|B_STAT;
	bp->b_blkno	= 0;
	bp->b_addr	= area;
	bp->b_base_count = 255;

	bp->b_cmd_txt[0] = SCSI_CMD_INQUIRY;
	bp->b_cmd_txt[1] = 0;
	bp->b_cmd_txt[2] = 0;
	bp->b_cmd_txt[3] = 0;
	bp->b_cmd_txt[4] = 255;	/* Comprimento da área */
	bp->b_cmd_txt[5] = 0;

	bp->b_cmd_len    = 6;

	if ((*sp->scsi_cmd) (bp) < 0)
		return (-1);

	/*
	 *	Obtém as características do dispositivo
	 */
	cp = bp->b_addr;

   /***	sp->scsi_is_disk = 0;		***/
   /***	sp->scsi_is_tape = 0;		***/
   /***	sp->scsi_read_only = 0;		***/
	sp->scsi_removable = (cp[1] & 0x80) ? 1 : 0;
   /***	sp->scsi_company[0] = '\0';	***/

	sp->scsi_blsz = BLSZ;	/* Em princípio blocos de 512 bytes */

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
		return (-1);

	    default:
		printf ("%s: Tipo de unidade desconhecida: %d\n", sp->scsi_dev_nm, cp[0]);
		return (-1);

	}	/* end switch */

	sp->scsi_is_present = 1;

	/*
	 *	Obtém a marca, produto, ...
	 */
	if (cp[4] >= 31)
	{
		scsi_str_conv (sp->scsi_company,  cp + 8,  sizeof (sp->scsi_company));
		scsi_str_conv (sp->scsi_product,  cp + 16, sizeof (sp->scsi_product));
		scsi_str_conv (sp->scsi_revision, cp + 32, sizeof (sp->scsi_revision));
	}

	if (cp[7] & 0x02)
		sp->scsi_tagenable++;

	/*
	 *	Fala sobre o dispositivo presente
	 */
#define	VERBOSE
#ifdef	VERBOSE
	printf ("%s:", sp->scsi_dev_nm);

	if (sp->scsi_company[0] != '\0')
	{
		printf
		(	" <%s, %s, %s>",
			sp->scsi_product, sp->scsi_company, sp->scsi_revision
		);
	}

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

	printf ("\n");
#endif	VERBOSE

	/*
	 *	Tenta "adivinhar" o tamanho do bloco
	 */
	sp->scsi_blshift = BLSHIFT;

	if (sp->scsi_removable && sp->scsi_read_only)
		sp->scsi_blshift = BLSHIFT + 2;

	/*
	 *	Verifica se a unidade está ONLINE
	 */
	bp->b_flags	= B_READ|B_STAT;
	bp->b_blkno	= 0;
	bp->b_addr	= NOVOID;
	bp->b_base_count = 0;

	memset (bp->b_cmd_txt, 0, 6);
   /***	bp->b_cmd_txt[0] = SCSI_CMD_TEST_UNIT_READY; ***/
	bp->b_cmd_len	 = 6;

	if ((*sp->scsi_cmd) (bp) < 0)
		return (-1);

	sp->scsi_nopen++;

	/*
	 *	Executa um (ou mais) comandos READ CAPACITY para a unidade
	 */
	for (i = 0; /* abaixo */; i++)
	{
		if (i >= 10)
		{
			printf ("scsi_attach: NÃO consegui obter o tamanho do bloco de \"%s\"\n", sp->scsi_dev_nm);
			break;
		}

		bp->b_flags	= B_READ|B_STAT;
		bp->b_blkno	= 0;
		bp->b_addr	= area;
		bp->b_base_count = 2 * sizeof (long);

		memset (bp->b_cmd_txt, 0, 10);
		bp->b_cmd_txt[0] = SCSI_CMD_READ_CAPACITY;
		bp->b_cmd_len	 = 10;

		if ((*sp->scsi_cmd) (bp) < 0)
			return (-1);

		cp = area;

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
	}

	/*
	 *	Coleta as informações da BIOS
	 */
	dp = disk_info_table_ptr;

	if (!sp->scsi_removable && dp->info_present)
	{
		daddr_t		CYLSZ;

		sp->scsi_bios_head = dp->info_nhead;
		sp->scsi_bios_sect = dp->info_nsect;

		CYLSZ = sp->scsi_bios_head * sp->scsi_bios_sect;
	   	sp->scsi_bios_cyl = (sp->scsi_disksz + CYLSZ - 1) / CYLSZ;

		sp->scsi_int13_ext = dp->info_int13ext;

		disk_info_table_ptr++;
	}

	/*
	 *	Sucesso
	 */
#ifdef	VERBOSE
	printf
	(	"%s: %d blocos de %d bytes (%d MB)",
		sp->scsi_dev_nm, sp->scsi_disksz, sp->scsi_blsz,
		(((unsigned)sp->scsi_disksz >> KBSHIFT) * sp->scsi_blsz) >> KBSHIFT
	);

	if (sp->scsi_bios_head) printf
	(	", bios = (%d, %d, %d, %s)",
		sp->scsi_bios_head, sp->scsi_bios_sect, sp->scsi_bios_cyl,
		sp->scsi_int13_ext ? "L" : "G"
	);

	printf ("\n");
#endif	VERBOSE

	return (0);

}	/* end scsi_attach */

/*
 ****************************************************************
 *	Ordena os dispositivos SCSI				*
 ****************************************************************
 */
void
scsi_sort (void)
{
	SCSI		*new_scsi_ptr[MAX_TARGETs + 1];
	SCSI		**spp, **new_spp;
	SCSI		*sp, *new_sp;
	DISKTB		*dp;
	char		letter = 'a';

	/*
	 *	Insere inicialmente os discos rígidos não removíveis
	 */
	for (spp = scsi_ptr, new_spp = new_scsi_ptr; (sp = *spp) != NOSCSI; spp++)
	{
		if (!sp->scsi_is_disk || sp->scsi_read_only || sp->scsi_removable)
			continue;

		*new_spp++ = sp;
	}

	/*
	 *	Insere agora os demais dispositivos
	 */
	for (spp = scsi_ptr; (sp = *spp) != NOSCSI; spp++)
	{
		if (sp->scsi_is_disk && !sp->scsi_read_only && !sp->scsi_removable)
			continue;

		*new_spp++ = sp;
	}

	*new_spp = NOSCSI;

	/*
	 *	Copia de volta e dá os novos nomes
	 */
	for (spp = scsi_ptr, new_spp = new_scsi_ptr; (new_sp = *new_spp) != NOSCSI; new_spp++)
	{
		dp = (DISKTB *)new_sp->scsi_disktb;

		dp->p_name[2]		= letter;
		new_sp->scsi_dev_nm[2]	= letter++;

		*spp++ = new_sp;
	}

	*spp = NOSCSI;

}	/* end scsi_sort */

/*
 ****************************************************************
 *	Função de "open"					*
 ****************************************************************
 */
int
scsi_open (SCSI *sp)
{
	BHEAD		bhead, *bp = &bhead;

	/*
	 *	Verifica se a unidade existe
	 */
	if (!sp->scsi_is_present)
	{
		printf ("%s: Unidade inválida\n", sp->scsi_dev_nm);
		return (-1);
	}

	if (sp->scsi_nopen)
		return (0);

	/*
	 *	Verifica se a unidade está ONLINE
	 */
	bp->b_ptr	= sp;
	bp->b_disktb	= sp->scsi_disktb;

	bp->b_flags	= B_READ|B_STAT;
   	bp->b_blkno	= 0;
   	bp->b_addr	= NOVOID;
   	bp->b_base_count = 0;

	memset (bp->b_cmd_txt, 0, 6);
   /***	bp->b_cmd_txt[0] = SCSI_CMD_TEST_UNIT_READY; ***/
	bp->b_cmd_len    = 6;

	if ((*sp->scsi_cmd) (bp) < 0)
		return (-1);

	sp->scsi_nopen++;

	return (0);

}	/* end scsi_open */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
scsi_ctl (SCSI *sp, const DISKTB *up, int cmd, int arg)
{
	BHEAD		bhead, *bp = &bhead;
	int		len;

#if (0)	/*******************************************************/
	BHEAD		*bp;
	char		*cp;
	int		status, len;
#endif	/*******************************************************/

	/*
	 *	Verifica qual o comando
	 */
	switch (cmd)
	{
	    case CD_READ_TOC:
	    {
		TOC		*tp;
		TOC_ENTRY	*ep;
		int		ntracks;
		char		area[2048];

		/*
		 *	Lê o cabeçalho da TOC
		 */
		memclr (bp, sizeof (BHEAD));

		bp->b_ptr	 = sp;
		bp->b_disktb	 = sp->scsi_disktb;

		bp->b_flags	 = B_READ|B_STAT;
	   /***	bp->b_blkno	 = 0;			***/
		bp->b_addr	 = area;
	   	bp->b_base_count = sizeof (TOC_HEADER);

		bp->b_cmd_txt[0] = SCSI_CMD_READ_TOC;
		bp->b_cmd_txt[7] = sizeof (TOC_HEADER) >> 8;
		bp->b_cmd_txt[8] = sizeof (TOC_HEADER);

		bp->b_cmd_len    = 10;

		if ((*sp->scsi_cmd) (bp) < 0)
			return (-1);

		/*
		 *	Descobre o número de trilhas
		 */
		tp = (TOC *)area;

		ntracks = tp->hdr.ending_track - tp->hdr.starting_track + 1;

		if (ntracks <= 0 || ntracks > MAXTRK)
		{
			printf ("%s: número excessivo de trilhas: %d\n", sp->scsi_dev_nm, ntracks);
			return (-1);
		}
#undef	DEBUG
#ifdef	DEBUG
		printf ("Trilhas = %d\n", ntracks);
#endif	DEBUG
		/*
		 *	Agora, lê a TOC na íntegra
		 */
		len = sizeof (TOC_HEADER) + ntracks * sizeof (TOC_ENTRY);

	   /***	bp->b_flags	 = B_STAT|B_READ;	***/
	   /***	bp->b_blkno	 = 0;			***/
	   /***	bp->b_addr	 = area;		***/
	   	bp->b_base_count = len;

		bp->b_cmd_txt[0] = SCSI_CMD_READ_TOC;
		bp->b_cmd_txt[7] = len >> 8;
		bp->b_cmd_txt[8] = len;

	   /***	bp->b_cmd_len = 10;			***/

		if ((*sp->scsi_cmd) (bp) < 0)
			return (-1);

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

		/*
		 *	Copia a TOC
		 */
		memmove ((TOC *)arg, tp, sizeof (TOC));

		return (ntracks);
	    }

	}	/* end switch */

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

	if (sense != 6)		/* Não imprime o "meio trocado" */
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
	   /***	printf ("O meio foi trocado\n"); ***/
		return (0);	/* Deve repetir a operação */

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
