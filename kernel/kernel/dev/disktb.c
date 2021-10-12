/*
 ****************************************************************
 *								*
 *			disktb.c				*
 *								*
 *	Atualiza a "disktb" para volumes montáveis		*
 *								*
 *	Versão	3.0.5, de 13.01.97				*
 *		4.9.0, de 27.09.06				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2006 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/bcb.h"
#include "../h/region.h"

#include "../h/bhead.h"
#include "../h/disktb.h"
#include "../h/scsi.h"
#include "../h/signal.h"
#include "../h/uproc.h"
#include "../h/uerror.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define PARTB_OFF (512 - 2 - 4 * 16)	/* Início da tabela no setor */
#define MAGIC_OFF (512 - 2)		/* Endereço do "0xAA55" */
#define	NPART		4		/* Número total de partições reg. */

#define	IS_EXT(t)	(t == PAR_DOS_EXT_G || t == PAR_DOS_EXT_L || t == PAR_TROPIX_EXT)

#define CYLMAX(c)	((c) > 1023 ? 1023 : (c))

/*
 ****** Uma entrada da tabela de partições **********************
 */
typedef struct
{
	char	head;		/* Cabeça */
	char	sect;		/* Setor */
	char	cyl;		/* Cilindro */

}	TRIO;

typedef struct			/* Tabela de partições */
{
	char	pt_active;	/* Entrada ativa se 0x80 */
	TRIO	pt_start;	/* Trio inicial */
	char	pt_type;	/* Tipo do sistema de arquivos */
	TRIO	pt_end;		/* Trio final */
	long	pt_offset;	/* Deslocamento (blocos) */
	long	pt_size;	/* Tamanho (blocos) */

}	PARTTB;

/*
 ****** Protótipos de funções ***********************************
 */
int		geo_get_parttb_geo (const char *, const char *, HDINFO *, daddr_t);
int		geo_get_std_method (const char *, HDINFO *);
int		geo_get_cyl (const PARTTB *);
int		geo_get_sec (int, ulong, const TRIO *);
int		geo_get_heu_method (const char *, HDINFO *);
int		geo_tst_geometry (const char *block0, const HDINFO *gp);
int		geo_tst_one_geometry (const PARTTB *, const HDINFO *);
int		geo_get_trio_blno (TRIO *, const HDINFO *, daddr_t);

/*
 ****************************************************************
 *	Obtém entradas na tabela "disktb"			*
 ****************************************************************
 */
DISKTB *
disktb_attach_entries (int part_letter, int major, int unit, int target) 
{
	DISKTB		*up;
	DISKTB		*begin_up;
	int		minor;
	char		letter;

	/*
	 *	Função simples, supondo 2 entradas, uma
	 *	para o disco todo e uma para uma partição
	 */

	/*
	 *	Em primeiro lugar, procura a última entrada "sd...."
	 */
	for (up = next_disktb - 1; /* abaixo */; up--)
	{
		if (up < disktb)
			{ letter = 'a'; break; }

		if (up->p_name[0] == 's' && up->p_name[1] == 'd')
			{ letter = up->p_name[2] + 1; break; }
	}

	/*
	 *	Verifica se há espaço na "disktb"
	 */
	begin_up = up = next_disktb; minor = up - disktb;

	if (up + 2 >= end_disktb)
	{
		printf
		(	"sd%c: Tabela de Discos/Partições cheia\n",
			letter
		);
		u.u_error = ENXIO; return (NODISKTB);
	}

	/*
	 *	Entrada para o disco todo
	 */
	up->p_name[0]	= 's';		up->p_name[1] = 'd';
	up->p_name[2]	= letter;	up->p_name[3] = '\0';

	up->p_offset	= 0;
	up->p_size	= BL4SZ / BLSZ;	/* Provisório, para ler a "parttb" */

	up->p_dev	= MAKEDEV (major, minor);
	up->p_unit	= unit;
	up->p_target	= target;

   /***	up->p_type	= 0; ***/
   /***	up->p_flags	= 0; ***/

   /***	up->p_head	= 0; ***/
   /***	up->p_sect	= 0; ***/
   /***	up->p_cyl	= 0; ***/

	up++; minor++;

	/*
	 *	Entrada para uma partição
	 */
	up->p_name[0]	= 's';		up->p_name[1] = 'd';
	up->p_name[2]	= letter;	up->p_name[3] = part_letter;
	up->p_name[4]	= '\0';

	up->p_offset	= 0;		/* Ainda desconhecido */
	up->p_size	= BL4SZ / BLSZ;	/* Provisório, para ler a "parttb" */

	up->p_dev	= MAKEDEV (major, minor);
	up->p_unit	= unit;
	up->p_target	= target;

   /***	up->p_type	= 0; ***/
   /***	up->p_flags	= 0; ***/

   /***	up->p_head	= 0; ***/
   /***	up->p_sect	= 0; ***/
   /***	up->p_cyl	= 0; ***/

   /***	up->p_nopen	= 0; ***/
   /***	up->p_lock	= 0; ***/

	up++; up->p_name[0] = '\0'; next_disktb = up;

	/*
	 *	Retorna o índice da primeira entrada
	 */
	return (begin_up);

}	/* end disktb_attach_entries */

/*
 ****************************************************************
 *	Le as partições do volume e atualiza a "disktb"		*
 ****************************************************************
 */
int
disktb_open_entries (dev_t dev, const SCSI *sp)
{
	DISKTB		*up;
	BHEAD		*bp;
	const PARTTB	*reg_pp;
	HDINFO		hdinfo;
	int		reg_index;

	/*
	 *	Obtém a partição inicial do dispositivo
	 */
	if ((up = disktb_get_first_entry (dev)) == NODISKTB)
		return (-1);

	/*
	 *	Le o Bloco 0 do volume
	 */
	bp = bread (up->p_dev, 0, 0);

	if (bp->b_flags & B_ERROR)
		{ block_put (bp); return (-1); }

	if (*(ushort *)(bp->b_addr + MAGIC_OFF) != 0xAA55)
	{
		printf
		(	"%s: O bloco 0 NÃO contém a assinatura \"0x55AA\"\n",
			up->p_name
		);
	}

	/*
	 *	Tenta obter a geometria do disco
	 */
	geo_get_parttb_geo (up->p_name, bp->b_addr, &hdinfo, sp->scsi_disksz);

	/*
	 *	Processa (por enquanto) somente a primeira partição regular
	 */
	for
	(	reg_index = 0, reg_pp = (PARTTB *)(bp->b_addr + PARTB_OFF);
		/* abaixo */;
		reg_index++, reg_pp++
	)
	{
		if (reg_index >= NPART)
		{
			printf
			(	"%s: Volume NÃO tem partição regular\n",
				up->p_name
			);
			u.u_error = ENXIO; block_put (bp); return (-1);
		}

		if (reg_pp->pt_size == 0)
			continue;

		if (!IS_EXT (reg_pp->pt_type))
			break;

		printf
		(	"%s: Partição ESTENDIDA ignorada\n",
			up->p_name
		);
	}

	/*
	 *	Completa a entrada do disco inteiro
	 */
   /***	up->p_offset	= ...; ***/
	up->p_size	= sp->scsi_disksz;

   /***	up->p_type	= 0; ***/
   /***	up->p_flags	= 0; ***/
   /***	up->p_lock	= 0; ***/
	up->p_blshift	= sp->scsi_blshift;

   /***	up->p_dev	= ...; ***/
   /***	up->p_unit	= ...; ***/
   /***	up->p_target	= ...; ***/

	up->p_head	= hdinfo.h_head;
	up->p_sect	= hdinfo.h_sect;
	up->p_cyl	= hdinfo.h_cyl;
   /***	up->p_nopen	= 0; ***/

	/*
	 *	Prepara a entrada da partição regular
	 */
	up++;

	up->p_offset	= reg_pp->pt_offset;
	up->p_size	= reg_pp->pt_size;

	up->p_type	= reg_pp->pt_type;
	up->p_flags	= (reg_pp->pt_active == 0x80) ?  DISK_ACTIVE : 0;
   /***	up->p_lock	= 0; ***/
	up->p_blshift	= sp->scsi_blshift;

   /***	up->p_dev	= ...; ***/
   /***	up->p_unit	= ...; ***/
   /***	up->p_target	= ...; ***/

	up->p_head	= hdinfo.h_head;
	up->p_sect	= hdinfo.h_sect;
	up->p_cyl	= hdinfo.h_cyl;
   /***	up->p_nopen	= 0; ***/

	block_put (bp);
	return (0);

}	/* end disktb_open_entries */

/*
 ****************************************************************
 *	Lê as partições do volume e atualiza a "disktb"		*
 ****************************************************************
 */
DISKTB *
disktb_create_partitions (DISKTB *base_up)
{
	DISKTB		*up, *slot_up = NODISKTB;
	const DISKTB	*end_up;
	dev_t		dev;
	BHEAD		*bp;
	const PARTTB	*reg_pp, *log_pp;
	daddr_t		log_offset, ext_begin, ext_end;
	int		reg_index, log_index, minor_index = 1;
	int		slot_sz, dec;
	HDINFO		hdinfo;
	char		area[BLSZ];

	/*
	 *	A partição global do volume acabou de ser criada
	 *
	 *	("base_up" é idêntico a "next_disktb")
	 *
	 *	Lê o Bloco 0 do volume e extrai a tabela de partições
	 */
	up = base_up; dev = up->p_dev;

	bp = bread (dev, 0, 0);

	if (bp->b_flags & B_ERROR)
		{ block_put (bp); return (NODISKTB); }

	if (*(ushort *)(bp->b_addr + MAGIC_OFF) != 0xAA55)
		printf ("%s: O bloco 0 NÃO contém a assinatura \"0x55AA\"\n", up->p_name);

	memmove (area, bp->b_addr, BLSZ);

	block_put (bp); bp = NOBHEAD;

	/*
	 *	Tenta obter a geometria do volume
	 */
	geo_get_parttb_geo (up->p_name, area, &hdinfo, up->p_size);

	/*
	 *	Completa a entrada para o volume inteiro
	 */
   /***	up->p_name	= ...;	***/

   /***	up->p_offset	= 0;	***/
   /***	up->p_size	= ...;	***/

   /***	up->p_type	= 0;	***/
   /***	up->p_flags	= 0;	***/
   /***	up->p_lock	= 0;	***/
   /***	up->p_blshift	= ...;	***/

   /***	up->p_dev	= ...;	***/
   /***	up->p_unit	= ...;	***/
   /***	up->p_target	= ...;	***/

	up->p_head	= hdinfo.h_head;
	up->p_sect	= hdinfo.h_sect;
	up->p_cyl	= hdinfo.h_cyl;

   /***	up->p_nopen	= 0;	   ***/
   /***	up->p_sb	= NOSB;    ***/
   /***	up->p_inode	= NOINODE; ***/

	up++;

	/*
	 *	Processa todas as partições
	 */
	for (reg_pp = (PARTTB *)(area + PARTB_OFF), reg_index = 1; reg_index <= NPART; reg_index++, reg_pp++)
	{
		if (reg_pp->pt_size == 0)
			continue;

		/* Encontrou uma partição regular/estendida válida */

		if (up >= end_disktb - 1)
			{ printf ("%s: DISKTB cheia\n", base_up->p_name); break; }

		sprintf (up->p_name, "%s%d", base_up->p_name, reg_index);

		up->p_offset	= reg_pp->pt_offset;
		up->p_size	= reg_pp->pt_size;

		up->p_type	= reg_pp->pt_type;
		up->p_flags	= (reg_pp->pt_active == 0x80) ? DISK_ACTIVE : 0;
	   /***	up->p_lock	= 0;       ***/
		up->p_blshift	= base_up->p_blshift;

		up->p_dev	= dev + minor_index++;	/* incrementa o MINOR */
		up->p_unit	= base_up->p_unit;
		up->p_target	= base_up->p_target;

		up->p_head	= hdinfo.h_head;
		up->p_sect	= hdinfo.h_sect;
		up->p_cyl	= hdinfo.h_cyl;

	   /***	up->p_nopen	= 0;	   ***/
	   /***	up->p_sb	= NOSB;	   ***/
	   /***	up->p_inode	= NOINODE; ***/

		up++;

		if (!IS_EXT (reg_pp->pt_type))
			continue;

		/*
		 *	Temos uma partição estendida
		 */
		ext_begin  = reg_pp->pt_offset;
		ext_end    = reg_pp->pt_offset + reg_pp->pt_size;

		log_offset = ext_begin;

		/*
		 *	Percorre a cadeia de partições lógicas
		 */
		for (log_index = 0; /* abaixo */; log_index++)
		{
			if (log_offset < ext_begin || log_offset >= ext_end)
			{
				printf
				(	"%s: Deslocamento %d inválido na partição estendida %d\n",
					base_up->p_name, log_offset, reg_index
				);
				break;
			}

			if (bp != NOBHEAD)
				block_put (bp);

			bp = bread (dev, log_offset, 0);

			if (bp->b_flags & B_ERROR)
				{ block_put (bp); bp = NOBHEAD; break; }

			if (*(ushort *)(bp->b_addr + MAGIC_OFF) != 0xAA55)
			{
				printf
				(	"%s: O bloco %d da partição estendida %d "
					"NÃO contém a assinatura \"0x55AA\"\n",
					base_up->p_name, log_offset, reg_index
				);
			}

			log_pp = (PARTTB *)(bp->b_addr + PARTB_OFF);

			if (log_pp[0].pt_size == 0) /* Primeiro método de EOF */
				break;

			/* Encontrou uma partição lógica válida */

			if (up >= end_disktb - 1)
				{ printf ("%s: DISKTB cheia\n", base_up->p_name); goto out; }

			sprintf (up->p_name, "%s%d%c", base_up->p_name, reg_index, 'a' + log_index);

			up->p_offset	= log_offset + log_pp->pt_offset;
			up->p_size	= log_pp->pt_size;

			up->p_type	= log_pp->pt_type;
			up->p_flags	= (log_pp->pt_active == 0x80) ? DISK_ACTIVE : 0;
		   /***	up->p_lock	= 0;       ***/
			up->p_blshift	= base_up->p_blshift;

			up->p_dev	= dev + minor_index++;	/* incrementa o MINOR */
			up->p_unit	= base_up->p_unit;
			up->p_target	= base_up->p_target;

			up->p_head	= hdinfo.h_head;
			up->p_sect	= hdinfo.h_sect;
			up->p_cyl	= hdinfo.h_cyl;

		   /***	up->p_nopen	= 0;	   ***/
		   /***	up->p_sb	= NOSB;	   ***/
		   /***	up->p_inode	= NOINODE; ***/

			up++;

			/*
			 *	Procura a partição seguinte
			 */
			if (log_pp[1].pt_size == 0)	/* Segundo método de EOF */
				break;

			log_offset = ext_begin + log_pp[1].pt_offset;

		}	/* end for (cadeia de partições lógicas) */

	}	/* end for (partições regulares/estendeidas) */

    out:
	if (bp != NOBHEAD)
		block_put (bp);

	/*
	 *	Atualiza o final da "disktb"
	 */
	next_disktb = up; up->p_name[0] = '\0';

	/*
	 *	Neste ponto, "minor_index" contém o número de entradas total
	 *
	 *	Procura um grupo de "minor_index" entradas vagas contíguas
	 */
	for (slot_sz = 0, up = disktb; /* abaixo */; up++)
	{
		if (up >= base_up)
			return (base_up);

		if (up->p_name[0] != '*')
			{ slot_sz = 0; continue; }

		if (slot_sz++ == 0)
			slot_up = up;

		if (slot_sz >= minor_index)
			break;
	}

	/*
	 *	Encontrou um grupo adequado
	 */
	memmove (slot_up, base_up, minor_index * sizeof (DISKTB));

	dec = base_up - slot_up;

	for (up = slot_up, end_up = up + minor_index; up < end_up; up++)
		up->p_dev -= dec;	/* Supondo que o MINOR esteja na parte baixa */

	memclr (base_up, minor_index * sizeof (DISKTB));

	next_disktb = base_up;

	return (slot_up);

}	/* end disktb_create_partitions */

/*
 ****************************************************************
 *	Encerra o uso das entradas da tabela "disktb"		*
 ****************************************************************
 */
int
disktb_remove_partitions (DISKTB *base_up)
{
	DISKTB		*up;
	const DISKTB	*end_up;

	/*
	 *	Prólogo
	 */
	if (base_up == NODISKTB || base_up < disktb || base_up >= next_disktb)
		{ printf ("disktb_remove_partitions: Entrada %P inválida\n", base_up); return (-1); }

	/*
	 *	Não esquece de remover os blocos do CACHE
	 */
	block_free (base_up->p_dev, NOINO);

	/*
	 *	Procura o final do dispositivo
	 */
	for (end_up = base_up; memeq (end_up->p_name, base_up->p_name, 3); end_up++)
	{
		if (end_up->p_nopen > 0)
			{ printf ("disktb_remove_partitions: \"%s\" está aberto\n", end_up->p_name); return (-1); }

		if (end_up->p_sb != NOSB)
			{ printf ("disktb_remove_partitions: \"%s\" está montado\n", end_up->p_name); return (-1); }
	}

	/*
	 *	Remove as entradas selecionadas
	 */
	while (base_up[-1].p_name[0] == '*')
		base_up--;

	while (end_up[0].p_name[0] == '*')
		end_up++;

	memclr (base_up, (char *)end_up - (char *)base_up);

	if (end_up >= next_disktb)
		{ next_disktb = base_up; return (0); }

	for (up = base_up; up < end_up; up++)
		up->p_name[0] = '*';

	return (0);

}	/* end disktb_remove_partitions */

/*
 ****************************************************************
 *	Encerra o uso das entradas da tabela "disktb"		*
 ****************************************************************
 */
int
disktb_close_entry (dev_t dev)
{
	DISKTB		*up;
	int		i;
	BHEAD		*bp;

	/*
	 *	Obtém a partição inicial do dispositivo
	 */
	if ((up = disktb_get_first_entry (dev)) == NODISKTB)
		return (-1);

	/*
	 *	Força a releitura da tabela de partições
	 */
	bp = block_get (up->p_dev, 0, 0);

#ifdef	DEBUG
	printf ("%v: O bloco 0 == %d\n", up->p_dev, EVENTTEST (&bp->b_done));
#endif	DEBUG

	EVENTCLEAR (&bp->b_done); block_put (bp);

	/*
	 *	Encerra as entradas
	 */
	for (i = 0; i < 2; i++, up++)
	{
	   /***	up->p_name = ...; ***/

		up->p_offset = 0;
		up->p_size   = BL4SZ / BLSZ;	/* Provisório, para ler a "parttb" */

	   /***	up->p_dev    = ...; ***/
	   /***	up->p_unit   = ...; ***/
	   /***	up->p_target = ...; ***/

		up->p_type  = 0;
		up->p_flags = 0;

		up->p_head  = 0;
		up->p_sect  = 0;
		up->p_cyl   = 0;

		if (up->p_nopen != 0)
		{
			printf
			(	"%s: p_nopen residual: %d\n",
				up->p_name, up->p_nopen
			);
		}

		up->p_nopen  = 0;
		up->p_lock  = 0;
	}

	return (0);

}	/* end disktb_close_entry */

/*
 ****************************************************************
 *	Acha a primeira entrada do dispositivo na DISKTB	*
 ****************************************************************
 */
DISKTB *
disktb_get_first_entry (dev_t dev)
{
	DISKTB		*up, *dev_up;
	char		letter, first;

	if ((dev_up = up = disktb_get_entry (dev)) == NODISKTB)
		return (NODISKTB);

	if ((letter = up->p_name[2]) == '\0')
		goto bad;

	first = up->p_name[0];

	for (/* acima */; up >= disktb; up--)
	{
		if (up->p_name[0] != first || up->p_name[1] != 'd')
			break;

		if (up->p_name[2] != letter)
			break;

		if (up->p_name[3] == '\0')
			return (up);
	}

    bad:
	printf
	(	"%s: NÃO encontrei a primeira entrada do dispositivo\n",
		dev_up->p_name
	);

	u.u_error = ENXIO;
	return (NODISKTB);

}	/* end disktb_get_first_entry */

/*
 ****************************************************************
 *	Acha a entrada do dispositivo na DISKTB			*
 ****************************************************************
 */
DISKTB *
disktb_get_entry (dev_t dev)
{
	DISKTB		*up;

	up = &disktb[MINOR (dev)];

	if (up->p_dev == dev)
		return (up);

	printf ("%g: Inconsistência na DISKTB para o dispositivo %v\n", dev);

	u.u_error = ENXIO; return (NODISKTB);

}	/* end disktb_get_entry */

/*
 ****************************************************************
 *	Tenta obter a geometria a partir das partições		*
 ****************************************************************
 */
int
geo_get_parttb_geo (const char *nm, const char *block0, HDINFO *hp, daddr_t disksz)
{
	daddr_t		CYLSZ;

	/*
	 *	Tenta os dois métodos
	 */
	if
	(	geo_get_std_method (block0, hp) < 0  &&
		geo_get_heu_method (block0, hp) < 0
	)
	{
		printf
		(	"%s: NÃO consegui obter a geometria. "
			"Usando valores fictícios\n",
			nm
		);

		if (disksz > (1 * GBSZ / BLSZ))
		{
			hp->h_head = 255;	/* Esquema de 8 GB */
			hp->h_sect = 63;
		}
		else
		{
			hp->h_head = 64;	/* Esquema de 1 GB */
			hp->h_sect = 32;
		}
	}

	/*
	 *	Calcula o número de cilindros
	 */
	CYLSZ = hp->h_head * hp->h_sect;
	hp->h_cyl = (disksz + CYLSZ - 1) / CYLSZ;

	return (0);

}	/* end geo_get_parttb_geo */

/*
 ****************************************************************
 *	Método 1: 2 equações com duas incógnitas		*
 ****************************************************************
 */
int
geo_get_std_method (const char *block0, HDINFO *hp)
{
	int		index;
	const PARTTB	*pp;
	int		CYLSZ;

	/*
	 *	Calcula o tamanho do cilindro
	 */
	for
	(	index = 0, pp = (PARTTB *)(block0 + PARTB_OFF);
		/* abaixo */;
		index++, pp++
	)
	{
		if (index >= NPART)
			return (-1);

		if (pp->pt_size == 0)
			continue;

		if ((CYLSZ = geo_get_cyl (pp)) > 0)
			break;
	}

	/*
	 *	Calcula o tamanho do setor
	 */
	for
	(	index = 0, pp = (PARTTB *)(block0 + PARTB_OFF);
		/* abaixo */;
		index++, pp++
	)
	{
		if (index >= NPART)
			return (-1);

		if (pp->pt_size == 0)
			continue;

		hp->h_sect = geo_get_sec
		(
			CYLSZ,
			pp->pt_offset,
			&pp->pt_start
		);

		if (hp->h_sect > 0)
			break;

		hp->h_sect = geo_get_sec
		(
			CYLSZ,
			pp->pt_offset + pp->pt_size - 1,
			&pp->pt_end
		);

		if (hp->h_sect > 0)
			break;
	}

	/*
	 *	Calcula o número de cabeças
	 */
	hp->h_head = CYLSZ / hp->h_sect;

	if (CYLSZ % hp->h_sect)
		return (-1);

	/*
	 *	Agora confere em todas as partições
	 */
	return (geo_tst_geometry (block0, hp));

}	/* end geo_get_std_method */

/*
 ****************************************************************
 *	Calcula o tamanho do cilindro				*
 ****************************************************************
 */
int
geo_get_cyl (const PARTTB *pp)
{
	ulong		start_bno = pp->pt_offset;
	ulong		end_bno = pp->pt_offset + pp->pt_size - 1;
	ulong		big_b_h, small_b_h;
	ulong		big_h_c, small_h_c;
	ulong		above, below, CYLSZ;

	big_b_h = (start_bno - (pp->pt_start.sect & 0x3F) + 1) * pp->pt_end.head;
	small_b_h = (end_bno - (pp->pt_end.sect & 0x3F) + 1) * pp->pt_start.head;

	big_h_c = pp->pt_end.head * (pp->pt_start.cyl | ((pp->pt_start.sect & 0xC0) << 2));
	small_h_c = pp->pt_start.head * (pp->pt_end.cyl | ((pp->pt_end.sect & 0xC0) << 2));

	above = big_b_h - small_b_h;
	below = big_h_c - small_h_c;

	if ((int)above < 0)
		{ above = -above; below = -below; }

	if (above == 0 || below == 0)
		return (-1);

	CYLSZ = above / below;

	if (above % below)
		return (-1);

	return (CYLSZ);

}	/* end geo_get_cyl */

/*
 ****************************************************************
 *	Calcula o número de setores por trilha			*
 ****************************************************************
 */
int
geo_get_sec (int CYLSZ, ulong bno, const TRIO *tp)
{
	ulong		above, below;
	int		NSECT;

	if ((below = tp->head) == 0)
		return (-1);

	above =  (bno - (tp->sect & 0x3F) + 1);
	above -= CYLSZ * (tp->cyl | ((tp->sect & 0xC0) << 2));

	if (above == 0)
		return (-1);

	NSECT = above / below;

	if (above % below)
		return (-1);

	return (NSECT);

}	/* end geo_get_sec */

/*
 ****************************************************************
 *    Heurística supondo que a partição termine em um cilindro	*
 ****************************************************************
 */
int
geo_get_heu_method (const char *block0, HDINFO *gp)
{
	const PARTTB	*pp;
	int		index;

	/*
	 *	Primeiro procura uma partição talvez terminada em cilindro
	 */
	for
	(	index = 0, pp = (PARTTB *)(block0 + PARTB_OFF);
		/* abaixo */;
		index++, pp++
	)
	{
		if (index >= NPART)
			return (-1);

		if (pp->pt_size == 0)
			continue;

		gp->h_sect = (pp->pt_end.sect & 0x3F) - 1 + 1;
		gp->h_head =  pp->pt_end.head + 1;

		if (geo_tst_one_geometry (pp, gp) >= 0)
			break;
	}

	/*
	 *	Agora confere em todas as partições
	 */
	return (geo_tst_geometry (block0, gp));

}	/* end geo_get_heu_method */

/*
 ****************************************************************
 *	Confere tôdas as partições, dada uma geometria		*
 ****************************************************************
 */
int
geo_tst_geometry (const char *block0, const HDINFO *gp)
{
	const PARTTB	*pp;
	int		index;

	for
	(	index = 0, pp = (PARTTB *)(block0 + PARTB_OFF);
		index < NPART;
		index++, pp++
	)
	{
		if (pp->pt_size == 0)
			continue;

		if (geo_tst_one_geometry (pp, gp) < 0)
			return (-1);
	}

	return (0);

}	/* end geo_tst_geometry */

/*
 ****************************************************************
 *	Confere uma partição, dada uma geometria		*
 ****************************************************************
 */
int
geo_tst_one_geometry (const PARTTB *pp, const HDINFO *gp)
{
	TRIO		trio;

	if (pp->pt_size == 0)
		return (-1);

	if (geo_get_trio_blno (&trio, gp, pp->pt_offset) < 0)
		return (-1);

	if
	(	trio.head != pp->pt_start.head ||
		trio.sect != pp->pt_start.sect ||
		trio.cyl  != pp->pt_start.cyl
	)
	{
#ifdef	DEBUG
		printf
		(	"ERRO: %d (%d, %d, %d) :: (%d, %d, %d)\n",
			pp->pt_offset,
			trio.cyl | (trio.sect & 0xC0) << 2, 
			trio.head,
			trio.sect & 0x3F,
			pp->pt_start.cyl | ((pp->pt_start.sect & 0xC0) << 2),
			pp->pt_start.head,
			pp->pt_start.sect & 0x3F
		);
#endif	DEBUG
		return (-1);
	}

	if (geo_get_trio_blno (&trio, gp, pp->pt_offset + pp->pt_size - 1) < 0)
		return (-1);

	if
	(	trio.head != pp->pt_end.head ||
		trio.sect != pp->pt_end.sect ||
		trio.cyl  != pp->pt_end.cyl
	)
	{
#ifdef	DEBUG
		printf
		(	"ERRO: %d (%d, %d, %d) :: (%d, %d, %d)\n",
			pp->pt_offset + pp->pt_size - 1,
			trio.cyl | (trio.sect & 0xC0) << 2, 
			trio.head,
			trio.sect & 0x3F,
			pp->pt_end.cyl | ((pp->pt_end.sect & 0xC0) << 2),
			pp->pt_end.head,
			pp->pt_end.sect & 0x3F
		);
#endif	DEBUG
		return (-1);
	}

	return (0);

}	/* end geo_tst_one_geometry */

/*
 ****************************************************************
 *	Calcula o trio a partir do bloco linear			*
 ****************************************************************
 */
int
geo_get_trio_blno (TRIO *tp, const HDINFO *gp, daddr_t blkno)
{
	int	head, sect, cyl;

	if (gp->h_head <= 0 || gp->h_sect <= 0)
		return (-1);

	sect   = (blkno % gp->h_sect) + 1;
	blkno /= gp->h_sect;
	head   =  blkno % gp->h_head;
	blkno /= gp->h_head;
	cyl    = CYLMAX (blkno);

	tp->head = head;
	tp->sect = ((cyl & 0x0300) >> 2) | (sect & 0x3F);
	tp->cyl  = cyl;

	return (0);

}	/* end geo_get_trio_blno */

#if (0)	/*******************************************************/
long		geo_get_linear_blno (const TRIO *, const HDINFO *);
/*
 ****************************************************************
 *	Calcula o bloco linear a partir do trio			*
 ****************************************************************
 */
long
geo_get_linear_blno (const TRIO *tp, const HDINFO *hp)
{
	long		blkno;

	blkno  = hp->h_head * (tp->cyl | ((tp->sect & 0xC0) << 2));
	blkno += tp->head;
	blkno *= hp->h_sect;
	blkno += (tp->sect & 0x3F) - 1;

	return (blkno);

}	/* end geo_get_linear_blno */
#endif	/*******************************************************/
