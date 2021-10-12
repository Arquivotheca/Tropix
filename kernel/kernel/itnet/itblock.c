/*
 ****************************************************************
 *								*
 *			itblock.c				*
 *								*
 *	Funções auxiliares do INTERNET				*
 *								*
 *	Versão	3.0.0, de 26.07.91				*
 *		4.0.0, de 16.08.00				*
 *								*
 *	Funções:						*
 *		get_it_block, put_it_block, compute_checksum	*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/scb.h"
#include "../h/sync.h"

#include "../h/itnet.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****** Estrutura de contrôle geral da INTERNET *****************
 */
entry ITSCB	itscb;			/* O bloco de contrôle */

/*
 ****** Lista de debug dos blocos fornecidos ********************
 */
#undef	ITNET_DEBUG
#ifdef	ITNET_DEBUG

entry LOCK	it_block_busy_list_lock;
entry ITBLOCK	*it_block_busy_list;

#endif	ITNET_DEBUG

/*
 ******	Protótipos de funções ***********************************
 */
char		*edit_itblenum (ITBLENUM);

/*
 ****************************************************************
 *	Inicializa os blocos de entrada/saída da INTERNET	*
 ****************************************************************
 */
void
init_it_block (void)
{
	ITSCB		*ip = &itscb;
	ITBLOCK		*bp;
	int		frac;

	/*
	 *	Percorre todos os blocos da lista
	 */
   /***	SPINLOCK (&ip->it_lock_free); ***/

	for (bp = scb.y_end_itblock - 1; bp >= scb.y_itblock; bp--)
	{
		bp->it_in_free_list = 1;

		bp->it_forw = ip->it_block_free;
		ip->it_block_free = bp;
	}

   	ip->it_bl_count = 0;

	frac = scb.y_n_itblock / 5;		/* 20 % */

	ip->it_bl_other     = 0;
	ip->it_bl_max_other = scb.y_n_itblock - frac;
	SEMAINIT (&ip->it_block_sema, frac, 0 /* sem histórico */);

   /***	SPINFREE (&ip->it_lock_free); ***/

#undef	GET_DEBUG
#ifdef	GET_DEBUG
	printf
	(	"get_it_block_max_other = %d, frac = %d, total = %d\n",
		get_it_block_max_other, frac, get_it_block_max_other + frac
	);
#endif	GET_DEBUG

}	/* end init_it_block */

/*
 ****************************************************************
 *	Aloca um bloco de entrada/saída da INTERNET		*
 ****************************************************************
 */
ITBLOCK *
get_it_block (ITBLENUM mode)
{
	ITSCB		*ip = &itscb;
	ITBLOCK		*bp;

	/*
	 *	Verifica o tipo do pedido
	 */
	switch (mode)
	{
	    case IT_IN:
	    case IT_OUT_CTL:
		SPINLOCK (&ip->it_lock_free);

		if (ip->it_bl_other >= ip->it_bl_max_other)
			{ SPINFREE (&ip->it_lock_free); return (NOITBLOCK); }
		break;

	    case IT_OUT_DATA:
		SEMALOCK (&ip->it_block_sema, PBLKIO);
		SPINLOCK (&ip->it_lock_free);
		break;

	}	/* end switch */

	/*
	 *	Obtém um bloco livre na lista
	 */
   /***	SPINLOCK (&ip->it_lock_free); ***/

	if ((bp = ip->it_block_free) == NOITBLOCK)
	{
		SPINFREE (&ip->it_lock_free);
#ifdef	MSG
		printf ("%g: Surpreendentemente NÃO achei bloco (%s)\n",  edit_itblenum (mode));
#endif	MSG
		return (NOITBLOCK);
	}

	ip->it_block_free = bp->it_forw;

	/*
	 *	Achou
	 */
	bp->u1.it_flags1 = 0;
	bp->u2.it_flags2 = 0;
	bp->u3.it_flags3 = 0;
   /***	bp->it_in_free_list = 0; ***/
	bp->it_get_enum = mode;

	bp->it_src_addr = 0;
	bp->it_dst_addr = 0;

   	ip->it_bl_count++;

	if (mode == IT_IN || mode == IT_OUT_CTL)
		ip->it_bl_other++;

	SPINFREE (&ip->it_lock_free);

#ifdef	GET_DEBUG
	printf ("%g: GET (%s, %d)\n", edit_itblenum (mode), bp - scb.y_itblock);
#endif	GET_DEBUG

	return (bp);

}	/* end get_it_block */

/*
 ****************************************************************
 *	Desaloca um bloco de entrada/saída da INTERNET		*
 ****************************************************************
 */
void
put_it_block (ITBLOCK *bp)
{
	ITSCB		*ip = &itscb;

	/*
	 *	Esta função é chamada também dentro de interrupções;
	 *	Não esquecer de desligar "bp->it_in_driver_queue"
	 *
	 *	Pequena consistência
	 */
	if (bp == NOITBLOCK)
	{
#ifdef	MSG
		printf ("%g: Devolvendo bloco NULO\n");
#endif	MSG
		return;
	}

	if (bp < scb.y_itblock || bp >= scb.y_end_itblock)
	{
#ifdef	MSG
		printf ("%g: Devolvendo bloco com endereço inválido: %P\n", bp);
#endif	MSG
		return;
	}

	if (bp->it_in_free_list)
	{
#ifdef	MSG
		printf ("%g: Devolvendo bloco livre\n");
		print_calls ();
#endif	MSG
		return;
	}

	if (bp->it_in_driver_queue)
	{
#ifdef	MSG
		if (ip->it_report_error)
			printf ("%g: Devolvendo bloco em lista de driver\n");
#endif	MSG
		for (EVER)
		{
			if (bp->it_in_driver_queue)
				EVENTWAIT (&every_second_event, PITNETOUT);
			else
				break;
		}
	}

	/*
	 *	Devolve o bloco
	 */
	SPINLOCK (&ip->it_lock_free);

   	ip->it_bl_count--;
	bp->it_in_free_list = 1;

	bp->it_forw = ip->it_block_free;
	ip->it_block_free = bp;

	switch (bp->it_get_enum)
	{
	    case IT_IN:
	    case IT_OUT_CTL:
		ip->it_bl_other--;
		break;

	    case IT_OUT_DATA:
		SEMAFREE (&ip->it_block_sema);
		break;

	}	/* end switch */

#ifdef	GET_DEBUG
	printf ("%g: PUT (%s, %d)\n", edit_itblenum (bp->it_get_enum), bp - scb.y_itblock);
#endif	GET_DEBUG

	SPINFREE (&ip->it_lock_free);

}	/* end put_it_block */

/*
 ****************************************************************
 *	Libera todos os	blocos restantes			*
 ****************************************************************
 */
void
put_all_it_block (void)
{
#ifdef	ITNET_DEBUG

	int		i;
	ITBLOCK		*bp;
	ITBLOCK		*it_next;

	SPINLOCK (&it_block_busy_list_lock);

	bp = it_block_busy_list;

	it_block_busy_list = NOITBLOCK;

	SPINFREE (&it_block_busy_list_lock);

	for (i = 0; bp != NOITBLOCK; bp = it_next)
		{ it_next = bp->it_busy_next; put_it_block (bp); i++; }

	if (i > 0 && itscb.it_report_error)
		printf ("%g: %d blocos residuais na fila BUSY\n", i);

#endif	ITNET_DEBUG

}	/* end put_all_it_block */

#define	CHECKSUM_IN_ASM
#ifndef	CHECKSUM_IN_ASM
/*
 ****************************************************************
 *	Realiza o cálculo do "checksum"				*
 ****************************************************************
 */
int
compute_checksum (void *area, int count)
{
	int		result = 0;
	ushort		*sp;

	/*
	 *	Verifica se foi dado um número ímpar de bytes
	 */
	if (count & 1)
		{ ((char *)area)[count] = '\0'; count++; }

	/*
	 *	Faz o cálculo
	 */
	for (sp = (ushort *)area; count > 0; count -= 2)
		result += *sp++;

	result = (result & 0xFFFF) + (result >> 16);
	result += (result >> 16);

	return (ENDIAN_SHORT (~result & 0xFFFF));

}	/* end compute_checksum */
#endif	CHECKSUM_IN_ASM

/*
 ****************************************************************
 *	Formata um endereço INTERNET				*
 ****************************************************************
 */
const char *
edit_ipaddr (IPADDR addr)
{
	char		*cp = (char *)&addr;
	static char	buf[16];

#ifdef	LITTLE_ENDIAN
	snprintf (buf, sizeof (buf), "%u.%u.%u.%u", cp[3], cp[2], cp[1], cp[0]);
#else
	snprintf (buf, sizeof (buf), "%u.%u.%u.%u", cp[0], cp[1], cp[2], cp[3]);
#endif	LITTLE_ENDIAN

	return (buf);

}	/* end t_addr_to_str */

/*
 ****************************************************************
 *	Imprime um endereço INTERNET				*
 ****************************************************************
 */
void
pr_itn_addr (IPADDR addr)
{
	char		*cp = (char *)&addr;

#ifdef	LITTLE_ENDIAN
	printf ("%u.%u.%u.%u", cp[3], cp[2], cp[1], cp[0]);
#else
	printf ("%u.%u.%u.%u", cp[0], cp[1], cp[2], cp[3]);
#endif	LITTLE_ENDIAN

}	/* end pr_itn_addr */

/*
 ****************************************************************
 *	Edita o nome do modo					*
 ****************************************************************
 */
char *
edit_itblenum (ITBLENUM mode)
{
	switch (mode)
	{
	    case IT_IN:
		    return ("IN");

	    case IT_OUT_DATA:
		    return ("OUT_DATA");

	    case IT_OUT_CTL:
		    return ("OUT_CTL");

	    default:
		    return ("???");

	}	/* end switch */

}	/* end edit_itblenum */
