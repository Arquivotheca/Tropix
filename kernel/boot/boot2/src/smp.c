/*
 ****************************************************************
 *								*
 *			smp.c					*
 *								*
 *	Suporte ao Multiprocessamento				*
 *								*
 *	Versão	3.0.0, de 05.07.94				*
 *		4.7.0, de 11.02.05				*
 *								*
 *	Módulo: Boot2						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2005 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <common.h>

#include "../h/smp.h"
#include "../h/proto.h"

MPF	*get_mpf_table (int, int);

/*
 ****************************************************************
 *	Verifica se é multiprocessado				*
 ****************************************************************
 */
void
smpattach (void)
{
	MPF		*mp;
	MPHEADER	*hp;
	int		i;
	char		*cp;
	static char	*type_nm[] = { "INT", "NMI", "SMI", "EXT" };

	/*
	 *	Examina as regiões prováveis onde a tabela MP Floating pode
	 *	situar-se
	 */
	if
	(	(mp = get_mpf_table (  0 * KBSZ,            1 * KBSZ)) == NOMPF &&
		(mp = get_mpf_table (639 * KBSZ,            1 * KBSZ)) == NOMPF &&
		(mp = get_mpf_table (   0xF0000,           64 * KBSZ)) == NOMPF &&
		(mp = get_mpf_table (*(ushort *)0x40E << 4, 4 * KBSZ)) == NOMPF
	)
	{
		printf ("Tabela MP AUSENTE\n");
		return;
	}

#if (0)	/*******************************************************/
	printf
	(	"Tabela Floating: addr = %P, versão = 1.%d, feature = %d\n",
		mp, mp->mpf_spec, mp->mpf_feature[0]
	);
#endif	/*******************************************************/

	if (mp->mpf_feature[0] != 0)
	{
		printf ("Tabela MP com formato \"default\" %d\n", mp->mpf_feature[0]);
		return;
	}

	hp = mp->mpf_addr;

	if (hp->mph_signature != MPH_IDENT)
	{
		printf ("A assinatura da tabela MP não confere: %X\n", hp->mph_signature);
		return;
	}

	printf
	(	"Tabela MP, versão = 1.%d, %d entradas, addr = %P\n",
		hp->mph_spec, hp->mph_count, hp
	);

#if (0)	/*******************************************************/
	printf ("Tamanho = %d\n", hp->mph_length);
	printf ("Versão = 1.%d\n", hp->mph_spec);
	printf ("Número de entradas = %d\n", hp->mph_count);
#endif	/*******************************************************/

	/*
	 *	Varre as entradas da tabela
	 */
	cp = (char *)hp + sizeof (MPHEADER);

	for (i = 0; i < hp->mph_count; i++)
	{
		printf ("%2d: ", i);

		switch (*cp)
		{
		    case MPE_PROC:
		    {
			MPPROC		*pp = (MPPROC *)cp;

			if (pp->pe_flags & 1)
			{
				printf
				(	"CPU, id = %d, sig = %08X, fflags = %08X%s\n",
					pp->pe_apicid, pp->pe_signature,
					pp->pe_fflags, pp->pe_flags & 2 ? ", boot" : ""
				);
			}

			cp += sizeof (MPPROC);
			break;
		    }

		    case MPE_BUS:
			printf ("BUS, ...\n");
			cp += 8;
			break;

		    case MPE_APIC:
		    {
			MPAPIC	*ap = (MPAPIC *)cp;

			printf
			(	"APIC, id = %d, flags = %02X, addr = %P\n",
				ap->ae_id, ap->ae_flags, ap->ae_addr
			);

			cp += sizeof (MPAPIC);
			break;
		    }

		    case MPE_INTR:
		    {
			MPINTR	*ip = (MPINTR *)cp;

			printf
			(	"INTR, type = %s, irq = %2d, destid = %3d, intin = %2d\n",
				type_nm[ip->ie_itype & 3], ip->ie_irq,
				ip->ie_destid, ip->ie_intin
			);

			cp += sizeof (MPINTR);
			break;
		    }

		    case MPE_LINTR:
		    {
			MPINTR	*ip = (MPINTR *)cp;

			printf
			(	"LINT, type = %s, irq = %2d, destid = %3d, intin = %2d\n",
				type_nm[ip->ie_itype & 3], ip->ie_irq,
				ip->ie_destid, ip->ie_intin
			);

			cp += sizeof (MPINTR);
			break;
		    }

		    default:
			printf ("Tipo inválido (%02X)\n", *cp);
			break;
		}
	}

	printf ("Tamanho: %d :: %d\n", hp->mph_length, cp - (char *)hp);

}	/* end smpattach */

/*
 ****************************************************************
 *	Tenta achar a tabela SMP em uma região específica	*
 ****************************************************************
 */
MPF *
get_mpf_table (int addr, int len)
{
	MPF		*mp;
	ulong		*smp, *endp;
	uchar		*cp, sum;
	int		i;

	endp = (ulong *)(addr + len);

	for (smp = (ulong *)addr; smp < endp; smp++)
	{
		if (*smp != SMP_IDENT)
			continue;

		mp = (MPF *)smp;

		if (mp->mpf_length != 1)
			continue;

		/* Confere o checksum */

		for (cp = (uchar *)mp, sum = 0, i = 0; i < 16; i++)
			sum += *cp++;

		if (sum == 0)
			return (mp);
	}

	return (NOMPF);

}	/* end get_mpf_table */
