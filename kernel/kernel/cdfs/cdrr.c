/*
 ****************************************************************
 *								*
 *			cdmount.c				*
 *								*
 *	Montagem do Sistema de Arquivos ISO9660			*
 *								*
 *	Versão	4.2.0, de 31.01.02				*
 *		4.2.0, de 16.04.02				*
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
#include "../h/region.h"

#include "../h/mntent.h"
#include "../h/sb.h"
#include "../h/cdfs.h"
#include "../h/inode.h"
#include "../h/bhead.h"
#include "../h/signal.h"
#include "../h/uproc.h"
#include "../h/uerror.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Examina e interpreta uma lista de campos RR		*
 ****************************************************************
 */
int
cd_rr_fields_analysis (const CDDIR *dp, const SB *unisp, int mission, void *ptr)
{
	const void	*vp;
	const RR	*rp, *end_rp;
	BHEAD		*bp = NOBHEAD;
	CDSB		*sp = unisp->sb_ptr;
	int		field_len, nm_len, result = 0;
	daddr_t		con_block = 0;
	off_t		con_offset = 0;
	int		con_len = 0;

	/*
	 *	Retorno das missões:
	 *
	 *	    START:	Se for: liga "sp->s_rock_ridge"
	 *	    NAME:	Se achou: "nm_len", se não achou: "0", dir. relocado: "-1"
	 *	    STAT:	Normalmente: "0", Diretório filho: "No. do bloco"
	 *	    PARENT:	Se encontrou o Diretório pai: "No. do bloco", se não achou: "0"
	 *	    SYMLINK:	Se armazenou o "link" symbólico: "0", se não achou: "-1"
	 */

	/*
	 *	Acha o começo dos campos
	 */
	vp = dp->d_name + dp->d_name_len;

	if ((dp->d_name_len & 1) == 0)		/* O nome começa ímpar */
		vp++;

	/*
	 *	Leva em conta o deslocamento
	 */
	if
	(	dp->d_name_len == 1 && dp->d_name[0] == '\0' &&
		ISO_GET_LONG (dp->d_first_block) == sp->s_root_first_block
	)
		vp += sp->s_rock_ridge_skip0;
	else
		vp += sp->s_rock_ridge_skip;

	/*
	 *	Percorre a lista de campos
	 */
	for (rp = end_rp = NORR; /* abaixo */; rp = (RR *)((int)rp + field_len))
	{
		if (mission == RR_NO_MISSION)	/* Se a missão já acabou, ... */
			break;

		if (rp >= end_rp)
		{
			if   (rp == NORR)
			{
				rp = vp; end_rp = (RR*)((int)dp + dp->d_len - RR_HEADER_SZ + 1);
			}
			elif (con_block != 0)
			{
				if (bp != NOBHEAD)
					block_put (bp);

				bp = bread (unisp->sb_dev, ISOBL_TO_BL (con_block), 0);

				if (u.u_error)
					break;

				rp = bp->b_addr + con_offset;

				end_rp = (RR*)((int)rp + con_len - RR_HEADER_SZ + 1);

				con_block = 0;
			}
			else
			{
				break;
			}
		}

		if ((field_len = rp->rr_len) < RR_HEADER_SZ)
			{ printf ("cd_rr_fields_analysis: len %d < 4\n", field_len); break; }

		if (rp->rr_version != 1)
		{
			printf
			(	"cd_rr_fields_analysis: "
				"version %d != 1 (\"%c%c\"), \"%s\"\n",
				rp->rr_version, rp->rr_type[0], rp->rr_type[1], dp->d_name
			);
			break;
		}

		/* Examina os campos desta lista */

#undef	RR_DEBUG
#ifdef	RR_DEBUG
		printf ("cd_rr_fields_analysis: Examinando \"%c%c\"\n", rp->rr_type[0], rp->rr_type[1]);
#endif	RR_DEBUG

		/*
		 *	Examina o nome do campo
		 */
		switch (*(ushort *)rp->rr_type)
		{
		    case ('C' | ('E' << 8)):	/* "CE": Continuation Area */
		    {
			con_block  = ISO_GET_LONG (rp->rr_con_block);
			con_offset = ISO_GET_LONG (rp->rr_con_offset);
			con_len    = ISO_GET_LONG (rp->rr_con_len);

			break;
		    }

		    case ('S' | ('P' << 8)):	/* "SP": Protocol Indicator */
		    {
			if (mission == RR_START_MISSION)
				sp->s_rock_ridge_skip = rp->rr_skip;

			break;
		    }

		    case ('S' | ('T' << 8)):	/* "ST": Protocol Terminator */
		    {
			rp = (RR *)end_rp;
			break;
		    }

		    case ('E' | ('R' << 8)):	/* "ER": Extensions Reference */
		    {
			if (mission != RR_START_MISSION)
				break;

			if (rp->rr_er_len_id != 10 || rp->rr_er_version != 1)
				break;

			if (memeq (rp->rr_er_id, "RRIP_1991A", 10))
				sp->s_rock_ridge = 1;

			break;
		    }

		    case ('P' | ('X' << 8)):	/* "PX": POSIX File Attributes */
		    {
			INODE		*ip = ptr;

			if (mission != RR_STAT_MISSION)
				break;

			ip->i_mode  = ISO_GET_LONG (rp->rr_mode);
			ip->i_nlink = ISO_GET_LONG (rp->rr_nlink);
			ip->i_uid   = ISO_GET_LONG (rp->rr_uid);
			ip->i_gid   = ISO_GET_LONG (rp->rr_gid);

			break;
		    }

		    case ('P' | ('N' << 8)):	/* "PN": POSIX Device Modes */
		    {
			INODE		*ip = ptr;
			unsigned	high, low;

			if (mission != RR_STAT_MISSION)
				break;

			high = ISO_GET_LONG (rp->rr_high);
			low  = ISO_GET_LONG (rp->rr_low);

			if (high == 0)
				ip->i_rdev = low;
			else
				ip->i_rdev = MAKEDEV (high, low);

			break;
		    }

		    case ('S' | ('L' << 8)):	/* "SL": Symbolic Link */
		    {
			INODE		*ip;
			const IOREQ	*iop;
			char		*area;
			const char	*cp, *end_cp;
			int		len, cpflag, ret;
			char		first_time = 1;

			if   (mission == RR_STAT_MISSION)
			{
				ip = ptr; iop = NOIOREQ; area = NOSTR; cpflag = 0;

				ip->i_size = 0;
			}
			elif (mission == RR_SYMLINK_MISSION)
			{
				ip = NOINODE; iop = ptr; area = iop->io_area; cpflag = iop->io_cpflag;

				if (iop->io_count < iop->io_ip->i_size)
					{ u.u_error = ENOMEM; break; }
			}
			else
			{
				break;
			}

			if (rp->rr_sl_flags)
				printf ("cd_rr_fields_analysis: Componente com continuação\n");

			cp = rp->rr_component; end_cp = cp + field_len - (RR_HEADER_SZ + 1 + 2);

			while (cp <= end_cp)
			{
				/* Coloca (ou não) a barra inicial */

				if   (first_time)
					first_time = 0;
				elif (area == NOSTR)
					ip->i_size += 1;
				elif (unimove (area++, "/", 1, cpflag) < 0)
					{ u.u_error = EFAULT; break; }

				len = cp[1]; ret = 0;

				switch (cp[0] & 0xFE)
				{
				    case 0x00:	/* Nome simples */
					if (area == NOSTR)
						ip->i_size += len;
					else
						{ ret = unimove (area, &cp[2], len, cpflag); area += len; }
					break;

				    case 0x02:	/* "." */
					if (area == NOSTR)
						ip->i_size += 1;
					else
						ret = unimove (area++, ".", 1, cpflag);
					break;

				    case 0x04:	/* ".." */
					if (area == NOSTR)
						ip->i_size += 2;
					else
						{ ret = unimove (area, "..", 2, cpflag); area += 2; }
					break;

				    case 0x08:	/* "/" */
					if (area == NOSTR)
						ip->i_size += 1;
					else
						ret = unimove (area++, "/", 1, cpflag);
					break;

				    default:
					printf ("cd_rr_fields_analysis: Máscara esdrúxula: 0x%02X\n", cp[0]);
					break;

				}	/* end switch */

				if (ret < 0)
					{ u.u_error = EFAULT; break; }

				cp += len + 2;

			}	/* end for (EVER) */

			if (area != NOSTR)
			{
				if (iop->io_count > iop->io_ip->i_size)
				{
					if (unimove (area++, "\0", 1, cpflag) < 0)
						{ u.u_error = EFAULT; break; }
				}

				result = area - (char *)iop->io_area;
			}

			break;
		    }

		    case ('N' | ('M' << 8)):	/* "NM": Alternate Name */
		    {
			char		*dst = ptr;

			if (mission != RR_NAME_MISSION)
				break;

			if (rp->rr_nm_flags != 0)
			{
				printf
				(	"cd_rr_fields_analysis: "
					"Campo NM com indicador 0x%02X\n",
					rp->rr_nm_flags
				);
			}

			nm_len = rp->rr_len - (RR_HEADER_SZ + 1);

			memmove (dst, rp->rr_name, nm_len); dst[nm_len] = '\0';

			result = nm_len;

			break;
		    }

		    case ('C' | ('L' << 8)):	/* "CL": Child Link */
		    {
			if (mission != RR_STAT_MISSION)
				break;

			result = ISO_GET_LONG (rp->rr_child_dir); mission = RR_NO_MISSION;

			break;
		    }

		    case ('P' | ('L' << 8)):	/* "PL": Parent Link */
		    {
			if (mission != RR_PARENT_MISSION)
				break;

			result = ISO_GET_LONG (rp->rr_parent_dir); mission = RR_NO_MISSION;

			break;
		    }

		    case ('R' | ('E' << 8)):	/* "RE": Relocated Directory */
		    {
			if (mission != RR_NAME_MISSION)
				break;

			result = -1; mission = RR_NO_MISSION;

			break;
		    }

		    case ('T' | ('F' << 8)):	/* "TF": Time Stamps */
		    {
			INODE		*ip = ptr;
			const char	*cp = rp->rr_time;

			if (mission != RR_STAT_MISSION)
				break;

			if (rp->rr_time_flag & 0x80)
			{
				printf ("cd_rr_fields_analysis: Tempo longos\n");
				break;
			}

			if (rp->rr_time_flag & 0x01)
				{ ip->i_ctime = cd_get_time (cp); cp += sizeof (rp->rr_time); }

			if (rp->rr_time_flag & 0x02)
				{ ip->i_mtime = cd_get_time (cp); cp += sizeof (rp->rr_time); }

			if (rp->rr_time_flag & 0x04)
				{ ip->i_atime = cd_get_time (cp); }

			break;
		    }

		}	/* end switch */

	}	/* end for (lista de campos) */

	/*
	 *	Epílogo
	 */
	if (bp != NOBHEAD)
		block_put (bp);

	return (result);

}	/* end cd_rr_fields_analysis */

/*
 ****************************************************************
 *	Extrai o tempo						*
 ****************************************************************
 */
time_t
cd_get_time (const char *cd_time)
{
	time_t		tropix_time;
	int		tz, days;

	days	= cd_time[0] + 1900 + (cd_time[1] + 9) / 12;

	days	= 367 * (cd_time[0] - 60) - 7 * days / 4 -
			3 * ((days - 1) / 100 + 1) / 4 +
			275 * cd_time[1] / 9 + cd_time[2] - 239;

	tropix_time = ((((days * 24) + cd_time[3]) * 60 + cd_time[4]) * 60) + cd_time[5];

	tz = cd_time[6];

	if (-48 <= tz && tz <= 52)
		tropix_time -= tz * 15 * 60;

	return (tropix_time);

}	/* end cd_get_time */
