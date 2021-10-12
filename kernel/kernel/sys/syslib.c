/*
 ****************************************************************
 *								*
 *			syslib.c				*
 *								*
 *	Chamadas ao sistema: "shlib"				*
 *								*
 *	Versão	3.2.3, de 29.11.99				*
 *		4.6.0, de 03.08.04				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <stdarg.h>

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/mmu.h"
#include "../h/stat.h"
#include "../h/map.h"
#include "../h/inode.h"
#include "../h/a.out.h"
#include "../h/shlib.h"
#include "../h/sysdirent.h"
#include "../h/signal.h"
#include "../h/uproc.h"
#include "../h/uerror.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e definições globais				*
 ****************************************************************
 */
entry SHLIB	shlib_tb[NSHLIB];	/* A Tabela */

entry int	shlib_col;		/* Colisões */

entry off_t	shlib_text_rel,		/* Para a relocação */
		shlib_data_rel,
		shlib_bss_rel;

/*
 ******	Máscara de bits para o endereço virtual *****************
 */
entry ulong	shlib_vaddr_mask;	/* Máscara de endereços virtuais (128 MB) */

#define	SHLIB_PG_MAX_SZ	(32 * KBSZ)	/* KB PG por bit */

#define	KBSZROUND(x)	(((unsigned)(x) + (KBSZ - 1)) & ~KBMASK)

/*
 ******	Protótipos de funções ***********************************
 */
void		shlib_resolve_dependencies (void);
int		shlib_add_symbols_to_hashtb (SHLIB *);
void		shlib_remove_symbols_from_hashtb (int index);
const SYM	**shlib_get_hash (const char *name, int nm_len);
int		shlib_load (INODE *);
int		shlib_unload (int);

int		shlib_relocate_symtb (SHLIB *);
void		shlib_relocate_lib_sections (SHLIB *shp, const HEADER *hp, IOREQ *);

void
shlib_relocate_section
(
	SHLIB		*shp,		/* Entrada da SHLIB */
	const char	*sec_nm,	/* Nome da seção ("TEXT/DATA") */
	void		*sec_image,	/* Endereço da seção */
	off_t		sec_sz,		/* Tamanho da seção a relocar */
	RELOC		*rel_image,	/* Endereço da reloação */
	off_t		rel_sz,		/* Tamanho da relocação dada */
	const SYM	* const vector[], /* Endereço do vetor da SYMTB */
	int		vector_elem	/* Número de elementos do vetor */
);

/*
 ****************************************************************
 *	Chamada ao sistema "shlib"				*
 ****************************************************************
 */
int
shlib (int op, ...)
{
	va_list		ap;
	const char	*nm;
	int		index;
	SHLIB		*ptr;
	INODE		*ip = NOINODE;
	IOREQ		io;
	DIRENT		de;

	/*
	 *	Analisa a operação
	 */
	va_start (ap, op);

	switch (op)
	{
	    case SHLIB_LOAD:
		if (superuser () < 0)
			return (-1);

		nm = va_arg (ap, const char *); va_end (ap);

		if ((ip = iname (nm, getuchar, ISEARCH, &io, &de)) == NOINODE)
			return (-1);

		return (shlib_load (ip));

	    case SHLIB_UNLOAD:
		if (superuser () < 0)
			return (-1);

		index = va_arg (ap, int); va_end (ap);

		return (shlib_unload (index));

	    case SHLIB_TABLE:
		ptr = va_arg (ap, SHLIB  *); va_end (ap);

		if (unimove (ptr, shlib_tb, sizeof (shlib_tb), SU) < 0)
			{ u.u_error = EFAULT; return (-1); }

		return (0);

	    default:
		u.u_error = EINVAL;
		return (-1);

	}	/* end switch */

}	/* end shlib */

/*
 ****************************************************************
 *   Carrega uma biblioteca compartilhada (interface interna)	*
 ****************************************************************
 */
int
shlib_kernel_load (const char *path)
{
	INODE		*ip;
	IOREQ		io;
	DIRENT		de;

	/*
	 *	Procura o arquivo
	 */
	if ((ip = iname (path, getschar, ISEARCH, &io, &de)) == NOINODE)
		return (-1);

	return (shlib_load (ip));

}	/* end shlib_kernel_load */

/*
 ****************************************************************
 *	Carrega uma biblioteca compartilhada			*
 ****************************************************************
 */
int
shlib_load (INODE *ip)
{
	int		index;
	int		text_data_symtb_size;
	pg_t		tvaddr, dvaddr;
	SHLIB		*shp;
	REGIONG		*rgp;
	pg_t		text_pg_size, data_bss_pg_size;
	pg_t		pg_size, inc;
	ulong		vaddr_mask = 0;
	IOREQ		io;
	REGIONL		rl;
	HEADER		h;

	/*
	 *	Analisa o acesso e o tipo do arquivo
	 */
	if (!S_ISREG (ip->i_mode))
		{ u.u_error = ENOTREG; goto bad; }

	/*
	 *	Le o Prólogo do Programa
	 */
   /***	io.io_fd	  = ...;	***/
   /***	io.io_fp	  = ...;	***/
	io.io_ip	  = ip;
   /***	io.io_dev	  = ...;	***/
	io.io_area	  = &h;
	io.io_count	  = sizeof (HEADER);
	io.io_offset_low  = 0;
	io.io_cpflag	  = SS;
   /***	io.io_rablock	  = ...;	***/

	(*ip->i_fsp->fs_read) (&io);

	if (u.u_error != NOERR)
		goto bad;

	if (io.io_count > 0)
		{ u.u_error = EIO; goto bad; }

	/*
	 *	Analisa o formato
	 */
	if (h.h_magic != SMAGIC || h.h_machine != 0x486)
		{ u.u_error = EBADFMT; goto bad; }

	/*
	 *	Verifica se é o formato NOVO
	 */
	if (h.h_tstart != 0)
		{ u.u_error = EBADFMT; goto bad; }

	/*
	 *	Calcula o tamanho necessário, em páginas
	 */
	text_pg_size	 = KBSZROUND (BYUPPG (h.h_tsize));
	data_bss_pg_size = KBSZROUND (BYUPPG (h.h_dsize + h.h_bsize));

	if ((pg_size = text_pg_size + data_bss_pg_size) > SHLIB_PG_MAX_SZ)
		{ u.u_error = ENOMEM; goto bad; }

	/*
	 *	Procura endereços virtuais livres
	 */
	for (inc = 0; inc < pg_size; inc += KBSZ)
		vaddr_mask = (vaddr_mask << 1) | 1;

	for (tvaddr = USER_SHLIB_PGA; /* abaixo */; tvaddr += KBSZ)
	{
		if (tvaddr + pg_size > USER_SHLIB_PGA + SHLIB_PG_MAX_SZ)
			{ u.u_error = ENOMEM; goto bad; }

		if ((shlib_vaddr_mask & vaddr_mask) == 0)
			break;

		vaddr_mask <<= 1;
	}

	/*
	 *	Procura uma entrada vaga na tabela
	 */
	for (index = 0, shp = shlib_tb; /* abaixo */; index++, shp++)
	{
		if (index >= NSHLIB)
			{ u.u_error = EAGAIN; goto bad; }

		if (shp->sh_region == NOREGIONG)
			break;

		if (streq (shp->sh_name, u.u_name))
			{ u.u_error = EBUSY; goto bad; }
	}

	/*
	 *	Prepara a entrada da tabela
	 */
	strcpy (shp->sh_name, u.u_name);
	shp->sh_time	 = h.h_time;

   /***	shp->sh_dep_mask = ...;	***/
   /***	shp->sh_hash_seq = ...;	***/

	shp->sh_tsize	= h.h_tsize;
	shp->sh_dsize	= h.h_dsize;
	shp->sh_bsize	= h.h_bsize;
	shp->sh_ssize	= h.h_ssize;

	shp->sh_tvaddr = tvaddr;
	shp->sh_dvaddr = dvaddr = tvaddr + text_pg_size;

	shp->sh_vaddr_mask = vaddr_mask;

   /***	shp->sh_region	= ...; /* abaixo ***/

	/*
	 *	Prepara uma região local (provisória)
	 */
	rl.rgl_type	= RG_TEXT;
	rl.rgl_prot	= URO;
	rl.rgl_vaddr	= tvaddr;
	rl.rgl_regiong	= NOREGIONG;

	/*
	 *	Aloca memória para a região
	 */
	text_data_symtb_size = h.h_tsize + h.h_dsize + h.h_ssize;

	if (regiongrow (&rl, 0, BYUPPG (text_data_symtb_size), RG_NOMMU) < 0)
		{ u.u_error = ENOMEM; goto clr; }

	shp->sh_region = rgp = rl.rgl_regiong;

   /***	rgp->rgg_size	 = ...; ***/
   /***	rgp->rgg_paddr	 = ...; ***/
   /***	rgp->rgg_pgtb	 = ...; ***/
   /***	rgp->rgg_type	 = ...; ***/
	rgp->rgg_flags	 = RGG_METX;
   	rgp->rgg_count   = 0;

	/*
	 *	Le o TEXT, DATA e SYMTB
	 */
   /***	io.io_fd	  = ...;	***/
   /***	io.io_fp	  = ...;	***/
   /***	io.io_ip	  = ip;		***/
   /***	io.io_dev	  = ...;	***/
	io.io_area	  = (void *)PGTOBY (rgp->rgg_paddr);
	io.io_count	  = text_data_symtb_size;
	io.io_offset_low  = sizeof (HEADER);
   /***	io.io_cpflag	  = SS;		***/
   /***	io.io_rablock	  = ...;	***/

	(*ip->i_fsp->fs_read) (&io);

	if (u.u_error != NOERR)
		goto release;

	if (io.io_count > 0)
		{ u.u_error = EIO; goto release; }

	/*
	 *	Prepara os endereços de relocação
	 */
	shlib_text_rel = PGTOBY (tvaddr);
	shlib_data_rel =
	shlib_bss_rel  = PGTOBY (dvaddr) - h.h_tsize;

	/*
	 *	Reloca a SYMTB
	 */
	if (shlib_relocate_symtb (shp) < 0)
		goto release;

	/*
	 *	Insere os símbolos na tabela HASH
	 */
	if (shlib_add_symbols_to_hashtb (shp) < 0)
		goto remove;

	/*
	 *	Reloca o TEXT e DATA da biblioteca compartilhada
	 */
	shlib_relocate_lib_sections (shp, &h, &io);

	if (u.u_error != NOERR)
		goto remove;

	/*
	 *	Completa as dependências entre as bibliotecas compartilhadas
	 */
	shlib_resolve_dependencies ();

	iput (ip);

	shlib_vaddr_mask |= vaddr_mask;

#undef	DEBUG
#ifdef	DEBUG
	printf
	(	"shlib (%s): tsize = %d, dsize = %d, bsize = %d, tvaddr = %P, tdaddr = %P\n",
		shp->sh_name, shp->sh_tsize, shp->sh_dsize, shp->sh_bsize,
		shp->sh_tvaddr, shp->sh_dvaddr
	);

	printf
	(	"shlib: size = %d, paddr = %P, pgtb = %P, ",
	   	rgp->rgg_size, rgp->rgg_paddr, rgp->rgg_pgtb
	);

	printf
	(	"type = %P, flags = %P, count = %d, byte0 = %X\n",
		rgp->rgg_type, rgp->rgg_flags, rgp->rgg_count, *(char *)(rgp->rgg_paddr * 4096)
	);
#endif	DEBUG

	return (0);

	/*
	 *	Em caso de erro, ...
	 */
    remove:
	shlib_remove_symbols_from_hashtb (index);
    release:
	regionrelease (&rl);
    clr:
	memclr (shp, sizeof (SHLIB));
    bad:
	iput (ip);

	return (-1);

}	/* end shlib_load */

/*
 ****************************************************************
 *	Reloca os valores dos símbolos da SYMTB			*
 ****************************************************************
 */
int
shlib_relocate_symtb (SHLIB *shp)
{
	SYM		*symtb, *sp;
	const SYM	*end_symtb;

	/*
	 *	Obtém o endereço da SYMTB
	 */
	symtb = (SYM *)(PGTOBY (shp->sh_region->rgg_paddr) + shp->sh_tsize + shp->sh_dsize);
	end_symtb = (SYM *)((int)symtb + shp->sh_ssize);

	/*
	 *	Atribui valores definitivos aos símbolos
	 */
	for (sp = symtb; sp < end_symtb; sp = SYM_NEXT_PTR (sp))
	{
		switch (sp->s_type)
		{
		    case EXTERN|UNDEF:
			if (sp->s_value == 0)		/* UNDEF: Será detectado na relocação */
				break;

			/* COMMON */

			printf ("%g: COMMON \"%s\" na biblioteca \"%s\"\n", sp->s_name, shp->sh_name);

			u.u_error = EBADFMT; return (-1);

		    case EXTERN|TEXT:
			sp->s_value += shlib_text_rel;
			break;

		    case EXTERN|DATA:
			sp->s_value += shlib_data_rel;
			break;

		    case EXTERN|BSS:
			sp->s_value += shlib_bss_rel;
			break;

		    default:				/* Por exemplo, ABS */
			break;

		}	/* end switch */

	}	/* end for */

	if (sp != end_symtb)
		printf ("%g: Defasamento na SYMTB da biblioteca \"%s\"\n", shp->sh_name);

	return (0);

}	/* end shlib_relocate_symtb */

/*
 ****************************************************************
 *	Adiciona os símbolos na tabela HASH			*
 ****************************************************************
 */
int
shlib_add_symbols_to_hashtb (SHLIB *shp)
{
	SYM		*symtb,	*sp;
	const SYM	*end_symtb;
	const SYM	**hp;
	int		index, sym_count;
	static int	next_hash_seq;

	/*
	 *	Prólogo
	 */
	symtb = (SYM *)(PGTOBY (shp->sh_region->rgg_paddr) + shp->sh_tsize + shp->sh_dsize);
	end_symtb = (SYM *)((int)symtb + shp->sh_ssize);

	index = shp - shlib_tb; sym_count = shlib_col = 0;

	/*
	 *	Insere os símbolos
	 */
	for (sp = symtb; sp < end_symtb; sp = SYM_NEXT_PTR (sp))
	{
		if (sp->s_type == EXTERN|UNDEF)
			continue;

		sym_count++;	/* NÃO leva em conta os UNDEFs */

		if ((hp = shlib_get_hash (sp->s_name, sp->s_nm_len)) == NULL)
		{
			printf ("%g: Tabela HASH de símbolos cheia\n", sp->s_name);
			u.u_error = ENOMEM; return (-1);
		}

		if (*hp != NOSYM)
		{
			printf ("%g: Símbolo \"%s\" duplicado\n", sp->s_name);
			u.u_error = EINVAL; return (-1);
		}

		*hp = sp; sp->s_flags = index;	/* Guarda o número da biblioteca */
	}

	/*
	 *	Agora, atualiza o índice de inserção HASH e dados estatísticos
	 */
	shp->sh_hash_seq  = next_hash_seq++;
	shp->sh_sym_count = sym_count;
	shp->sh_hash_col  = shlib_col;

	return (0);

}	/* end shlib_add_symbols_to_hashtb */

/*
 ****************************************************************
 *	Remove os símbolos da tabela HASH			*
 ****************************************************************
 */
void
shlib_remove_symbols_from_hashtb (int index)
{
	const SYM	**hp, *sp;

	/*
	 *	Remove os símbolos
	 */
	for (hp = (const SYM **)scb.y_shlib_hash; hp < (const SYM **)scb.y_end_shlib_hash; hp++)
	{
		if ((sp = *hp) == NOSYM)
			continue;

		if (sp->s_flags == index)
			*hp = NOSYM;
	}

}	/* end shlib_remove_symbols_from_hashtb */

/*
 ****************************************************************
 *	Reloca uma biblioteca compartilhada			*
 ****************************************************************
 */
void
shlib_relocate_lib_sections (SHLIB *shp, const HEADER *hp, IOREQ *iop)
{
	pg_t		vec_rel_pg_addr;
	const SYM	**vec_addr, **vp;
	const SYM	*hsp;
	int		vec_size, rel_size, sym_count = 0;
	SYM		*symtb, *sp;
	const SYM	*end_symtb;
	const SYM	**hash_p;
	RELOC		*rel_addr;
	void		*paddr;

	/*
	 *	Obtém o endereço da SYMTB
	 */
	paddr = (void *)PGTOBY (shp->sh_region->rgg_paddr);

	symtb = paddr + shp->sh_tsize + shp->sh_dsize;
	end_symtb = (SYM *)((int)symtb + shp->sh_ssize);

	/*
	 *	Obtém o número TOTAL de símbolos
	 */
	for (sp = symtb; sp < end_symtb; sp = SYM_NEXT_PTR (sp), vp++)
		sym_count++;

	/*
	 *	Aloca memória para o vetor da SYMTB e relocações RT & RD
	 */
	vec_size = sym_count * sizeof (SYM *);

	rel_size = hp->h_rtsize + hp->h_rdsize;

	if ((vec_rel_pg_addr = malloc (M_CORE, BYUPPG (vec_size + rel_size))) == NOPG)
		{ u.u_error = ENOMEM; return; }

	vec_addr = vp = (const SYM **)PGTOBY (vec_rel_pg_addr);

	memclr (vec_addr, vec_size);

	rel_addr = (RELOC *)((void *)vec_addr + vec_size);

	/*
	 *	Le as relocações
	 */
   /***	iop->io_fd	  = ...;	***/
   /***	iop->io_fp	  = ...;	***/
   /***	iop->io_ip	  = ip;		***/
   /***	iop->io_dev	  = ...;	***/
	iop->io_area	  = rel_addr;
	iop->io_count	  = rel_size;
	iop->io_offset_low = sizeof (HEADER) + hp->h_tsize + hp->h_dsize + hp->h_ssize;
   /***	iop->io_cpflag	  = SS;		***/
   /***	iop->io_rablock	  = ...;	***/

	(*iop->io_ip->i_fsp->fs_read) (iop);

	if (u.u_error != NOERR)
		goto bad;

	if (iop->io_count > 0)
		{ u.u_error = EIO; goto bad; }

	/*
	 *	Constrói o vetor de ponteiros para a SYMTB
	 */
	for (sp = symtb; sp < end_symtb; sp = SYM_NEXT_PTR (sp), vp++)
	{
		/*
		 *	Escolhe os símbolos significativos
		 */
		switch (sp->s_type)
		{
		    case EXTERN|TEXT:
		    case EXTERN|DATA:
		    case EXTERN|BSS:
		    case EXTERN|ABS:
		    case EXTERN|UNDEF:
			break;

		    default:		/* Ignora os outros símbolos */
			continue;

		}	/* end switch */

		/*
		 *	Obtém a entrada da SYMTB global
		 */
		hash_p = shlib_get_hash (sp->s_name, sp->s_nm_len);

		if ((hsp = *hash_p) != NOSYM)
			{ *vp = hsp; continue; }

		printf ("%g: Símbolo não encontrado na tabela HASH: \"%s\"\n", sp->s_name);

		u.u_error = EBADFMT; goto bad;

	}	/* end for */

	/*
	 *	Se for o caso, reloca o TEXT
	 */
	if (hp->h_tsize != 0 && hp->h_rtsize)
	{
		shlib_relocate_section
		(
			shp,
			"TEXT",
			paddr,
			shp->sh_tsize,
			rel_addr,
			hp->h_rtsize,
			vec_addr,
			sym_count
		);
	}

	/*
	 *	Se for o caso, reloca o DATA
	 */
	if (u.u_error == NOERR && hp->h_dsize != 0 && hp->h_rdsize)
	{
		shlib_relocate_section
		(
			shp,
			"DATA",
			paddr + hp->h_tsize,
			hp->h_dsize,
			(RELOC *)((int)rel_addr + hp->h_rtsize),
			hp->h_rdsize,
			vec_addr,
			sym_count
		);
	}

	/*
	 *	Em caso de erro, ...
	 */
    bad:
	mrelease (M_CORE, BYUPPG (vec_size + rel_size), vec_rel_pg_addr);

}	/* end shlib_relocate_lib_sections */

/*
 ****************************************************************
 *	Reloca uma Seção de uma biblioteca compartilhada	*
 ****************************************************************
 */
void
shlib_relocate_section
(
	SHLIB		*shp,		/* Entrada da SHLIB */
	const char	*sec_nm,	/* Nome da seção ("TEXT/DATA") */
	void		*sec_image,	/* Endereço da seção */
	off_t		sec_sz,		/* Tamanho da seção a relocar */
	RELOC		*rel_image,	/* Endereço da reloação */
	off_t		rel_sz,		/* Tamanho da relocação dada */
	const SYM	* const vector[], /* Endereço do vetor da SYMTB */
	int		vector_elem	/* Número de elementos do vetor */
)
{
	const RELOC	*rp;
	const RELOC	*end_rel_image = (void *)rel_image + rel_sz;
	off_t		offset;
	void		*bp;
	unsigned int	index;
	const SYM	*sp;
	long		r = 0;

	/*
	 *	Realiza a Relocação
	 */
	for (rp = rel_image; rp < end_rel_image; rp++)
	{
		int		rel_len = rp->r_flags & RSZMASK;
		int		rel_seg = rp->r_flags & RSEGMASK;

		if ((offset = rp->r_pos) >= sec_sz)
		{
			printf
			(	"%g: Posição %d de relocação inválido na seção %s de \"%s\"",
				offset, sec_nm, shp->sh_name
			);

			u.u_error = EBADFMT; return;
		}

		bp = sec_image + offset;

		/*
		 ****** Obtem o valor do elemento a ser relocado ****************
		 */
		switch (rel_len)
		{
		    case RWORD:
			r = *((short *)bp);
			break;

		    case RLONG:
			r = *((long *)bp);
			break;

		    default:
			printf
			(	"%g: Tamanho %04X de relocação inválido na seção %s de \"%s\"",
				rel_len, sec_nm, shp->sh_name
			);

			u.u_error = EBADFMT; return;

		}	/* end switch (rel_len) */

		/*
		 ****** Obtem o novo valor do elemento **************************
		 */
		switch (rel_seg)
		{
		    case RTEXT:
			r += shlib_text_rel;
			break;

		    case RDATA:
			r += shlib_data_rel;
			break;

		    case RBSS:
			r += shlib_bss_rel;
			break;

		    case REXTREL:	/* "call" relativo ==> Muito sutil */
			r -= shlib_text_rel;
		    case REXT:
			if
			(	(index = rp->r_symbol) >= vector_elem ||
				(sp = vector[index]) == NOSYM
			)
			{
				printf
				(	"%g: Índice %d de relocação inválido na seção %s de \"%s\"",
					index, sec_nm, shp->sh_name
				);

				u.u_error = EBADFMT; return;
			}

			if (sp->s_type != (EXTERN|UNDEF))
			{
				r += sp->s_value;

				shp->sh_dep_mask |= (1 << sp->s_flags);

				break;
			}

			printf
			(	"%g: Símbolo indefinido \"%s\" na relocação da seção %s de \"%s\"",
				sp->s_name, sec_nm, shp->sh_name
			);

			u.u_error = EBADFMT; return;

		    default:
			printf
			(	"%g: Seção %04X inválida na relocação da seção %s de \"%s\"",
				rel_seg, sec_nm, shp->sh_name
			);

			u.u_error = EBADFMT; return;

		}	/* end switch (rel_seg) */

		/*
		 ****** Armazena o novo valor do elemento ***********************
		 */
		switch (rel_len)
		{
		    case RWORD:
			*((short *)bp) = r;
			break;

		    case RLONG:
			*((long *)bp) = r;
			break;
		}

	}	/* end for */

	return;

}	/* end shlib_relocate_section */

/*
 ****************************************************************
 * Completa as dependências entre as bibliotecas compartilhadas	*
 ****************************************************************
 */
void
shlib_resolve_dependencies (void)
{
	unsigned	mask;
	char		modified;
	SHLIB		*tst_shp;
	const SHLIB	*shp, *end_shlib_tb = shlib_tb + NSHLIB;

	/*
	 *	Executa até que não seja achada nenhuma nova dependência
	 */
	do
	{
		modified = 0;

		for (tst_shp = shlib_tb; tst_shp < end_shlib_tb; tst_shp++)
		{
			if (tst_shp->sh_region == NOREGIONG)
				continue;

			if (tst_shp->sh_dep_mask == 0)
				continue;

			for (mask = 1, shp = shlib_tb; shp < end_shlib_tb; mask <<= 1, shp++)
			{
				if ((tst_shp->sh_dep_mask & mask) == 0)
					continue;

				if ((shp->sh_dep_mask & ~tst_shp->sh_dep_mask) == 0)
					continue;

				tst_shp->sh_dep_mask |= shp->sh_dep_mask; modified = 1;
			}
		}
	}
	while (modified);

}	/* end shlib_resolve_dependencies */

/*
 ****************************************************************
 *	Resolve as referências externas de um módulo		*
 ****************************************************************
 */
int
shlib_extref_resolve (void *text_data, int text_data_size, const void *reftb, int refsize)
{
	const EXTREF	*erp, *end_ref_image;
	const off_t	*op;
	const void	*end_text_data = text_data + text_data_size;
	off_t		value, *lp;
	const SYM	**hp, *sp;
	const SHLIB	*shp;
	int		i, index, mask = 0;
	int		sym_count = 0;

	/*
	 *	Prólogo
	 */
	shlib_col = 0;

	/*
	 *	Resolve cada símbolo
	 */
	for (erp = reftb, end_ref_image = reftb + refsize; erp < end_ref_image; /* abaixo */)
	{
		hp = shlib_get_hash (erp->r_name, erp->r_nm_len); sym_count++;

		if ((sp = *hp) == NOSYM)
			{ printf ("%g: Símbolo indefinido: \"%s\"\n", erp->r_name); return (-1); }

		op = EXTREF_REFPTR (erp);

		if ((EXTREF *)(op + erp->r_ref_len) > end_ref_image)
		{
		    bad:
			printf ("%g: Inconsistência na tabela de referências do módulo executável");
			return (-1);
		}

		value = sp->s_value;

		mask |= (1 << sp->s_flags);	/* Acumula as bibliotecas referenciadas */

		/* Resolve as referências externas do TEXT e DATA para êste símbolo */

		for (i = erp->r_ref_len; i > 0; i--, op++)
		{
			if ((lp = text_data + *op) >= (off_t *)end_text_data)
				goto bad;

			*lp += value;
		}

		erp = (EXTREF *)op;
	}

	/*
	 *	Adiciona as interdependências
	 */
	for (index = 1, shp = shlib_tb; index < (1 << NSHLIB); index <<= 1, shp++)
	{
		if (mask & index)
			mask |= shp->sh_dep_mask;
	}

#ifdef	MSG
	if (CSWT (28))
	{
		int	col = shlib_col;

		printf
		(	"%g: sym_count = %d, col = %d, No. de tentativas = %d.%02d, mask = %02X\n",
			sym_count, col,
			(sym_count + col) / sym_count,
			(((sym_count + col) * 100) / sym_count) % 100, mask
		);
	}
#endif	MSG

	return (mask);

}	/* end shlib_extref_resolve */

/*
 ****************************************************************
 *	Determina a posição de um determinado símbolo		*
 ****************************************************************
 */
const SYM **
shlib_get_hash (const char *name, int nm_len)
{
	int		incr;
	const SYM	**hp, *sp;

	/*
	 *	Em geral a primeira tentativa é a correta
	 */
	hp = (const SYM **)scb.y_shlib_hash + strhash (name, nm_len, scb.y_n_shlib_hash);

	if ((sp = *hp) == NOSYM || streq (name, sp->s_name))
		return (hp);

	/*
	 *	Processa as colisões
	 */
	for (incr = scb.y_n_shlib_hash >> 1; /* abaixo */; incr--)
	{
		if (incr <= 0)
			{ hp = (const SYM **)NULL; break; }

		if ((hp += incr) >= (const SYM **)scb.y_end_shlib_hash)
			hp -= scb.y_n_shlib_hash;

		shlib_col++;

		if ((sp = *hp) == NOSYM || streq (name, sp->s_name))
			break;
	}

	return (hp);

}	/* end shlib_get_hash */

/*
 ****************************************************************
 *	Descarrega uma biblioteca compartilhada			*
 ****************************************************************
 */
int
shlib_unload (int index)
{
	SHLIB		*shp;
	REGIONG		*rgp;
	int		i, hash_seq;
	int		mask = (1 << index);

	/*
	 *	O sincronismo não está muito convincente!!
	 */
	if ((unsigned)index >= NSHLIB)
		{ u.u_error = EINVAL; return (-1); }

	shp = &shlib_tb[index];

	if ((rgp = shp->sh_region) == NOREGIONG)
		{ u.u_error = EINVAL; return (-1); }

   /***	if (rgp->rgg_count > 0)		(abaixo)	***/
   /***		{ u.u_error = EBUSY; return (-1); }	***/

	hash_seq = shp->sh_hash_seq;

	/*
	 *	Verifica se as bibliotecas que foram inseridas posteriormente estão livres
	 */
	for (i = 0, shp = shlib_tb; i < NSHLIB; i++, shp++)
	{
		if ((rgp = shp->sh_region) == NOREGIONG)
			continue;

		if (shp->sh_dep_mask & mask)
			{ u.u_error = EBUSY; return (-1); }

		if (shp->sh_hash_seq < hash_seq)
			continue;

		if (rgp->rgg_count > 0)
			{ u.u_error = EBUSY; return (-1); }
	}

	/*
	 *	Retira os símbolos da tabela HASH
	 */
	for (i = 0, shp = shlib_tb; i < NSHLIB; i++, shp++)
	{
		if ((rgp = shp->sh_region) == NOREGIONG)
			continue;

		if (shp->sh_hash_seq < hash_seq)
			continue;
#define	DEBUG
#ifdef	DEBUG
		printf ("%g: Removendo os símbolos da SHLIB \"%s\"\n", shp->sh_name);
#endif	DEBUG
		shlib_remove_symbols_from_hashtb (i);
	}

	/*
	 *	Desaloca a Biblioteca pedida
	 */
	shp = &shlib_tb[index]; rgp = shp->sh_region;

	SPINLOCK (&rgp->rgg_regionlock);

	if (rgp->rgg_count > 0)
		{ SPINFREE (&rgp->rgg_regionlock); u.u_error = EBUSY; return (-1); }

	rgp->rgg_flags &= ~RGG_METX;

	SPINFREE (&rgp->rgg_regionlock);

	regiongrelease (rgp, 1);

	shlib_vaddr_mask &= ~shp->sh_vaddr_mask;

	memclr (shp, sizeof (SHLIB));

	/*
	 *	Reinsere os símbolos da tabela HASH
	 */
	for (i = 0, shp = shlib_tb; i < NSHLIB; i++, shp++)
	{
		if ((rgp = shp->sh_region) == NOREGIONG)
			continue;

		if (shp->sh_hash_seq <= hash_seq)
			continue;
#ifdef	DEBUG
		printf ("%g: Reintroduzindo os símbolos da SHLIB \"%s\"\n", shp->sh_name);
#endif	DEBUG
		shlib_add_symbols_to_hashtb (shp);
	}

	return (0);

}	/* end shlib_unload */

/*
 ****************************************************************
 *	Acopla a biblioteca compartilhada			*
 ****************************************************************
 */
int
shlib_attach (int index)
{
	SHLIB		*shp;
	REGIONL		*rlpd, *rlpt;
	REGIONG		*rgpt;

	/*
	 *	Aloca a região DATA para a bilioteca compartilhada
	 */
	shp = &shlib_tb[index];

	if (CSWT (25))
		printf ("%g: Acoplando região DATA da bibloteca \"%s\"\n", shp->sh_name);

	/*
	 *	Verifica se a biblioteca está carregada
	 */
	if (shp->sh_region == NOREGIONG)
		{ u.u_error = ENOEXEC; return (-1); }

	/*
	 *	Se ainda não tinha DATA alocado ...
	 */
	rlpd = &u.u_shlib_data[index];

	if (rlpd->rgl_regiong != NOREGIONG && rlpd->rgl_regiong->rgg_count > 1)
		regionrelease (rlpd);

	rlpd->rgl_type	  = RG_DATA;
	rlpd->rgl_prot	  = URW;
	rlpd->rgl_vaddr	  = shp->sh_dvaddr;
   /***	rlpd->rgl_regiong = ...;	/* abaixo ***/

	if (regiongrow (rlpd, 0, BYUPPG (shp->sh_dsize + shp->sh_bsize), 0) < 0)
	{
		printf ("%g: Não consegui memória para região DATA\n");
		u.u_error = ENOMEM; return (-1);
	}

	/*
	 *	Se ainda não tinha TEXT alocado ...
	 */
	rlpt = &u.u_shlib_text[index];
	rgpt = shp->sh_region;

	if (rlpt->rgl_regiong == NOREGIONG)
	{
		rlpt->rgl_type	  = RG_TEXT;
		rlpt->rgl_prot	  = URO;
		rlpt->rgl_vaddr	  = shp->sh_tvaddr;
	   	rlpt->rgl_regiong = rgpt;

		SPINLOCK (&rgpt->rgg_regionlock);
		rgpt->rgg_count++;
		SPINFREE (&rgpt->rgg_regionlock);
	}
	else
	{
		/* Nada precisa fazer */
	}

	mmu_dir_load (rlpt);

	/*
	 *	Copia o conteúdo do DATA
	 */
	memmove
	(	(void *)PGTOBY (rlpd->rgl_regiong->rgg_paddr),
		(void *)PGTOBY (rgpt->rgg_paddr) + shp->sh_tsize,
		shp->sh_dsize
	);

	memclr
	(	(void *)PGTOBY (rlpd->rgl_regiong->rgg_paddr) + shp->sh_dsize,
		shp->sh_bsize
	);

	return (0);

}	/* end shlib_attach */
