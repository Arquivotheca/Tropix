/*
 ****************************************************************
 *								*
 *			kaddr.c					*
 *								*
 *	Funções de endereços da INTERNET			*
 *								*
 *	Versão	3.0.0, de 27.05.93				*
 *		4.5.0, de 23.10.03				*
 *								*
 *	Funções:						*
 *		k_node_to_addr, k_domain_resolver,		*
 *		k_addr_to_node					*
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
#include "../h/scb.h"
#include "../h/sync.h"
#include "../h/region.h"

#include "../h/uerror.h"
#include "../h/signal.h"
#include "../h/uproc.h"
#include "../h/itnet.h"
#include "../h/xti.h"

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define	FAR_FUTURE	 0x7FFFFFFF	/* Tempo bem no futuro */
#define	WORST_PREFERENCE 0x7FFFFFFF	/* Pior Preferência */

#define d_transmissions	d_mail_list_ok	/* Tentativas */
#define DNS_RETRANS_TIME	3	/* Segundos */
#define DNS_RETRANS_MAX		5	/* Retransmissões */

entry EVENT	work_for_the_domain_resolver;

entry int	we_have_a_domain_server_daemon;

entry LOCK	dns_lock;

/*
 ******	Protótipos de funções ***********************************
 */
int		ask_name_resolver (DNS *, char *);
DNS		*get_nm_dns_entry (char *, char);
DNS		*get_addr_dns_entry (IPADDR);
void		rm_dns_ptr (void);

/*
 ****************************************************************
 *	Obtém o endereço de um NÓ a partir do seu nome		*
 ****************************************************************
 */
int
k_node_to_addr (void *usp)
{
	DNS		*dp;
	const ROUTE	*rp;
	char		*cp, *bp;
	struct	{ char id[NETNM_SZ]; IPADDR dst_addr; IPADDR my_addr; } s;

	/*
	 *	Obtém a estrutura com o endereço ou nome do nó
	 */
	if (unimove (&s, usp, sizeof (s), US) < 0)
		{ u.u_error = EFAULT; return (-1); }

	/*
	 *	Procura na tabela
	 */
	if   (s.id[0])				/* Foi dado o nome de um NÓ */
	{
		/*
		 *	Acrescenta o nome do "domain", se for o caso
		 */
		for (cp = s.id; *cp != '.'; cp++)
		{
			if (*cp == '\0')
			{
				*cp++ = '.';

				for (bp = domain_name; cp < &s.id[NETNM_SZ]; /* vazio */)
				{
					if ((*cp++ = *bp++) == '\0')
						break;
				}

				break;
			}
		}

		/*
		 *	Procura o nome na tabela
		 */
		SPINLOCK (&dns_lock);

		dp = get_nm_dns_entry (s.id, 'C');

		if (EVENTTEST (&dp->d_resolver_done) < 0 || dp->d_error_code != DNS_NO_ERROR)
		{
			if (!we_have_a_domain_server_daemon)
				{ SPINFREE (&dns_lock); u.u_error = TDNSSERVER; return (-1); }

			/* Envia o pedido */

		   /***	memmove (dp->d_host_nm, s.id, NETNM_SZ); ***/
		   /***	dp->d_host_addr = ...; ***/

		   /***	dp->d_entry_code = 'C'; ***/
		   	EVENTCLEAR (&dp->d_resolver_done);
		   	dp->d_error_code = DNS_NO_ERROR;
		   /***	dp->d_server_index = ...; ***/

		   /***	dp->d_expir_time = FAR_FUTURE; ***/
		   /***	dp->d_last_use_time = time; ***/

		   /***	dp->d_preference = ...; ***/
			dp->d_resolver_got_req = 0;
		   /***	dp->d_transmissions = ...; ***/

		   /***	dp->d_can_ptr = ...; ***/
		   /***	dp->d_mail_ptr = ...; ***/

			SPINFREE (&dns_lock);

			EVENTDONE (&work_for_the_domain_resolver);

			/* Espera a resposta */

			EVENTWAIT (&dp->d_resolver_done, PITNETIN);
		}
		else
		{
			SPINFREE (&dns_lock);
		}

		/*
		 *	Analisa a resposta
		 */
		switch (dp->d_error_code)
		{
		    case DNS_NO_ERROR:
			break;

		    case DNS_NAME_ERROR:
			{ u.u_error = TUNKNET; return (-1); }

		    case DNS_SERVER_ERROR:
		    default:
			{ u.u_error = TDNSSERVER; return (-1); }

		}	/* end switch */

		/* Verifica se é um "alias" do próprio computador */

		for (rp = scb.y_route; rp->r_dev_nm[0] != '\0'; rp++)
		{
			if (rp->r_my_addr == dp->d_host_addr)
				{ dp = scb.y_dns; break; }
		}

		s.dst_addr = dp->d_host_addr;
	}
	elif (s.dst_addr == 0)			/* O próprio computador */
	{
		dp = scb.y_dns; s.dst_addr = dp->d_host_addr;
	}
	else					/* Foi dado dd.dd.dd.dd */
	{
		for (dp = scb.y_dns; /* abaixo */; dp++)
		{
			if (dp >= scb.y_end_dns || dp->d_host_nm[0] == 0)
				{ dp = NODNS; break; }

			if (dp->d_host_addr == s.dst_addr)
				break;
		}

		/* Se não achou na tabela DNS ... */

		if (dp == NODNS) for (rp = scb.y_route; rp->r_dev_nm[0] != '\0'; rp++)
		{
			if (rp->r_my_addr == s.dst_addr)
				{ dp = scb.y_dns; s.dst_addr = dp->d_host_addr; break; }
		}
	}

	/*
	 *	Obtém o meu endereço associado
	 */
	if ((rp = get_route_entry (s.dst_addr)) == NOROUTE)
		{ u.u_error = TNOROUTE; return (-1); }

	s.my_addr = rp->r_my_addr;

	if (dp != NODNS)
		dp->d_last_use_time = time;

	/*
	 *	Copia a estrutura de volta para o usuário
	 */
	if (unimove (usp, &s, sizeof (s), SU) < 0)
		u.u_error = EFAULT;

	return (UNDEF);

}	/* end k_node_to_addr */

/*
 ****************************************************************
 *	Obtém o nome de um servidor de correio			*
 ****************************************************************
 */
int
k_mail_to_node (DNS *udp)
{
	DNS		*dp;
	DNS		dns;
	int		i, best_preference = WORST_PREFERENCE;
	DNS		*best_dp = NODNS;

	/*
	 *	Obtém a estrutura com o nome do domínio e preferência
	 */
	if (unimove (&dns, udp, sizeof (DNS), US) < 0)
		{ u.u_error = EFAULT; return (-1); }

	/*
	 *	Procura o domínio na tabela
	 */
	SPINLOCK (&dns_lock);

	dp = get_nm_dns_entry (dns.d_host_nm, 'M');

	if (EVENTTEST (&dp->d_resolver_done) < 0 || dp->d_error_code != DNS_NO_ERROR)
	{
		if (!we_have_a_domain_server_daemon)
			{ SPINFREE (&dns_lock); u.u_error = TDNSSERVER; return (-1); }

		/* Envia o pedido */

	   /***	memmove (dp->d_host_nm, dns.d_host_nm, NETNM_SZ); ***/
	   /***	dp->d_host_addr = ...; ***/

	   /***	dp->d_entry_code = 'M'; ***/
	   	EVENTCLEAR (&dp->d_resolver_done);
	   	dp->d_error_code = DNS_NO_ERROR;
	   /***	dp->d_server_index = ...; ***/

	   /***	dp->d_expir_time = FAR_FUTURE; ***/
	   /***	dp->d_last_use_time = time; ***/

	   /***	dp->d_preference = ...; ***/
		dp->d_resolver_got_req = 0;
	   /***	dp->d_transmissions = ...; ***/

	   /***	dp->d_can_ptr = ...; ***/
	   /***	dp->d_mail_ptr = ...; ***/

		SPINFREE (&dns_lock);

		EVENTDONE (&work_for_the_domain_resolver);

		/* Espera a resposta */

		EVENTWAIT (&dp->d_resolver_done, PITNETIN);
	}
	else
	{
		SPINFREE (&dns_lock);
	}

	/*
	 *	Analisa a resposta
	 */
	switch (dp->d_error_code)
	{
	    case DNS_NO_ERROR:
		break;

	    case DNS_NAME_ERROR:
		{ u.u_error = TUNKNET; return (-1); }

	    case DNS_SERVER_ERROR:
	    default:
		{ u.u_error = TDNSSERVER; return (-1); }

	}	/* end switch */

	/*
	 *	Percorre a lista de servidores de correio
	 */
	SPINLOCK (&dns_lock);

	dp->d_last_use_time = time;

	for (i = scb.y_n_dns; /* abaixo */; i--)
	{
		if (i <= 0)
			{ printf ("%g: Ponteiros em malha\n"); break; }

		if ((dp = dp->d_mail_ptr) == NODNS)
			break;

		if (dp->d_preference >= dns.d_preference)
		{
			if (dp->d_preference < best_preference)
			{
				best_dp = dp;
				best_preference = dp->d_preference;
			}
		}
	}

	SPINFREE (&dns_lock);

	/*
	 *	Copia a estrutura de volta para o usuário
	 */
	if (best_dp == NODNS)
		{ u.u_error = ENOENT; return (-1); }

	best_dp->d_last_use_time = time;

	memmove (dns.d_host_nm, best_dp->d_host_nm, NETNM_SZ);
	dns.d_preference = best_dp->d_preference;

	if (unimove (udp, &dns, sizeof (DNS), SU) < 0)
		u.u_error = EFAULT;

	return (UNDEF);

}	/* end k_mail_to_node */

/*
 ****************************************************************
 *	Obtém o nome de um NÓ a partir do seu endereço		*
 ****************************************************************
 */
int
k_addr_to_node (IPADDR host_addr, char *uidp, int do_reverse)
{
	DNS		*dp;
	const ROUTE	*rp;

	/*
	 *	O endereço 0.0.0.0 refere-se ao próprio computador
	 */
	if (host_addr == 0)
		{ dp = &scb.y_dns[0]; goto found; }

	/*
	 *	Procura o endereço na tabela ROUTE
	 */
	for (rp = scb.y_route; /* abaixo */; rp++)
	{
		if (rp->r_dev_nm[0] == '\0')
			break;

		if (rp->r_my_addr == host_addr)
			{ dp = &scb.y_dns[0]; goto found; }
	}

	/*
	 *	Procura o endereço na tabela DNS
	 */
	SPINLOCK (&dns_lock);

	dp = get_addr_dns_entry (host_addr);

	if (EVENTTEST (&dp->d_resolver_done) < 0 || dp->d_error_code != DNS_NO_ERROR)
	{
		if (!do_reverse)
			{ memclr (dp, sizeof (DNS)); SPINFREE (&dns_lock); u.u_error = TUNKNET; return (-1); }

		if (!we_have_a_domain_server_daemon)
			{ SPINFREE (&dns_lock); u.u_error = TDNSSERVER; return (-1); }

		/* Envia o pedido */

	   /***	memmove (dp->d_host_nm, ..., NETNM_SZ); ***/
	   /***	dp->d_host_addr = ...; ***/

	   /***	dp->d_entry_code = 'R'; ***/
	   	EVENTCLEAR (&dp->d_resolver_done);
	   	dp->d_error_code = DNS_NO_ERROR;
	   /***	dp->d_server_index = ...; ***/

	   /***	dp->d_expir_time = FAR_FUTURE; ***/
	   /***	dp->d_last_use_time = time; ***/

	   /***	dp->d_preference = ...; ***/
		dp->d_resolver_got_req = 0;
	   /***	dp->d_transmissions = ...; ***/

	   /***	dp->d_can_ptr = ...; ***/
	   /***	dp->d_mail_ptr = ...; ***/

		SPINFREE (&dns_lock);

		EVENTDONE (&work_for_the_domain_resolver);

		/* Espera a resposta */

		EVENTWAIT (&dp->d_resolver_done, PITNETIN);
	}
	else
	{
		SPINFREE (&dns_lock);
	}

	/*
	 *	Analisa a resposta
	 */
	switch (dp->d_error_code)
	{
	    case DNS_NO_ERROR:
		break;

	    case DNS_NAME_ERROR:
		{ u.u_error = TUNKNET; return (-1); }

	    case DNS_SERVER_ERROR:
	    default:
		{ u.u_error = TDNSSERVER; return (-1); }

	}	/* end switch */

	/*
	 *	Encontrou
	 */
    found:
	dp->d_last_use_time = time;

	if (unimove (uidp, dp->d_host_nm, NETNM_SZ, SU) < 0)
		u.u_error = EFAULT;

	return (UNDEF);

}	/* end k_addr_to_node */

/*
 ****************************************************************
 *	Espera até chegar um pedido de consulta			*
 ****************************************************************
 */
int
k_dns_wait_req (void *udp)
{
	DNS		*dp;

	/*
	 *	Verifica se é superusuário
	 */
	if (!superuser () < 0)
		return (-1);

	/*
	 *	O server_daemon está ativo
	 */
	we_have_a_domain_server_daemon = 1;

	/*
	 *	Procura mais serviço
	 */
	for (EVER)
	{
		EVENTWAIT (&work_for_the_domain_resolver, PITNETIN);

		SPINLOCK (&dns_lock);

		for (dp = scb.y_dns; dp < scb.y_end_dns; dp++)
		{
			if (dp->d_host_nm[0] == '\0' && dp->d_entry_code != 'R')
				continue;

			if (EVENTTEST (&dp->d_resolver_done) >= 0)
				continue;

		   	if (dp->d_resolver_got_req)
				continue;

		   	dp->d_resolver_got_req = 1;
		   	dp->d_transmissions++;

			if (unimove (udp, dp, sizeof (DNS), SU) < 0)
				u.u_error = EFAULT;

			SPINFREE (&dns_lock); return (0);
		}

		EVENTCLEAR (&work_for_the_domain_resolver);

		SPINFREE (&dns_lock);

	}	/* end for (EVER) */

}	/* end k_dns_wait_req */

/*
 ****************************************************************
 *	Retransmite depois de alguns segundos			*
 ****************************************************************
 */
void
dns_resolver_every_second_work (void)
{
	DNS		*dp;
	char		found = 0;

	SPINLOCK (&dns_lock);

	for (dp = scb.y_dns; dp < scb.y_end_dns; dp++)
	{
		if (dp->d_host_nm[0] == '\0' && dp->d_entry_code != 'R')
			continue;

		if (EVENTTEST (&dp->d_resolver_done) >= 0)
			continue;

		if (time - dp->d_last_use_time < DNS_RETRANS_TIME)
			continue;

	   	if (dp->d_transmissions > DNS_RETRANS_MAX)
		{
			dp->d_error_code |= DNS_SERVER_ERROR;
			EVENTDONE (&dp->d_resolver_done);
			continue;
		}

		dp->d_last_use_time = time;
	   	dp->d_resolver_got_req = 0;
	   /***	EVENTCLEAR (&dp->d_resolver_done); ***/
		found = 1;
#ifdef	MSG
		if (CSWT (33))
			printf ("%g: Retransmitindo \"%s\" (%d)\n", dp->d_host_nm, dp->d_transmissions);
#endif	MSG

	}	/* end for (tabela DNS) */

	SPINFREE (&dns_lock);

	if (found)
		EVENTDONE (&work_for_the_domain_resolver);

}	/* end dns_resolver_every_second_work */

/*
 ****************************************************************
 *	Recebe a resposta do servidor de nomes			*
 ****************************************************************
 */
int
k_dns_put_info (void *udp)
{
	DNS		*dp;
	DNS		*can_dp, *tst_dp;
	DNS		dns[2];
	int		i;

	/*
	 *	Verifica se é superusuário
	 */
	if (!superuser () < 0)
		return (-1);

	/*
	 *	Obtém a estrutura DNS de resposta
	 */
	if (unimove (dns, udp, sizeof (dns), US) < 0)
		{ u.u_error = EFAULT; return (-1); }

#ifdef	MSG
	if (CSWT (33))
	{
		printf
		(	"%g: => \"%s\" (%c, %s, error = %d)",
			dns[0].d_host_nm, dns[0].d_entry_code,
			edit_ipaddr (dns[0].d_host_addr), dns[0].d_error_code
		);

		if (dns[0].d_entry_code == 'A' || dns[0].d_entry_code == 'M') printf
		(
			" ...\n\t[\"%s\" (%c, %s, pref = %d)]",
			dns[1].d_host_nm, dns[1].d_entry_code,
			edit_ipaddr (dns[1].d_host_addr), dns[1].d_preference
		);

		printf ("\n");
	}
#endif	MSG

	/*
	 *	Pesquisa a tabela
	 */
	SPINLOCK (&dns_lock);

	if (dns[0].d_entry_code == 'R')
		dp = get_addr_dns_entry (dns[0].d_host_addr);
	else
		dp = get_nm_dns_entry (dns[0].d_host_nm, dns[0].d_entry_code);

	if
	(	EVENTTEST (&dp->d_resolver_done) >= 0 &&
		dp->d_error_code == DNS_NO_ERROR
	)
	{
		if (dns[0].d_error_code != DNS_NO_ERROR)
			{ SPINFREE (&dns_lock); return (0); }

		if (dns[0].d_entry_code != 'M')
			{ SPINFREE (&dns_lock); return (0); }
	}

	/*
	 *	Verifica se é o final da lista de servidores de correio
	 */
	if (dns[0].d_entry_code == 'M' && dns[1].d_entry_code == 'E')
		{ EVENTDONE (&dp->d_resolver_done); SPINFREE (&dns_lock); return (0); }

	/*
	 *	Processa detalhes básicos
	 */
	if (dns[0].d_entry_code == 'R')
	{
		char		c, *cp;

	   	strcpy (dp->d_host_nm, dns[0].d_host_nm);
	   /***	dp->d_host_addr = ...; */
	   	dp->d_entry_code = 'C';

		for (cp = dp->d_host_nm; (c = *cp) != '\0'; cp++)
		{
			if (c >= 'A' && c <= 'Z')
				*cp += ('a' - 'A');
		}
	}
	else
	{
	   /***	dp->d_host_nm = ...; ***/
		dp->d_host_addr = dns[0].d_host_addr;
	   	dp->d_entry_code = dns[0].d_entry_code;
	}

   /***	dp->d_resolver_done = (abaixo); ***/
   	dp->d_error_code = dns[0].d_error_code;

	if (dp->d_server_index == 0)
		dp->d_server_index = dns[0].d_server_index;

	dp->d_expir_time = dns[0].d_expir_time + time;
	dp->d_last_use_time = time;

   /***	dp->d_preference = ...; ***/

   /***	dp->d_can_ptr = ...; ***/
   /***	dp->d_mail_ptr = ...; ***/

	SPINFREE (&dns_lock);

	if (dp->d_error_code != DNS_NO_ERROR)
		{ EVENTDONE (&dp->d_resolver_done); return (0); }

	/*
	 *	Processa detalhes adicionais
	 */
	if   (dp->d_entry_code == 'A')
	{
		SPINLOCK (&dns_lock);

		can_dp = get_nm_dns_entry (dns[1].d_host_nm, 'C');

	   /***	can_dp->d_host_nm = ...; ***/
		can_dp->d_host_addr = dns[1].d_host_addr;

	   	can_dp->d_entry_code = dns[1].d_entry_code;
	   /***	can_dp->d_resolver_done = (abaixo); ***/
	   	can_dp->d_error_code = dns[1].d_error_code;

		if (can_dp->d_server_index == 0)
			can_dp->d_server_index = dns[1].d_server_index;

		can_dp->d_expir_time = dns[1].d_expir_time + time;
		can_dp->d_last_use_time = time;

		dp->d_can_ptr = can_dp;

		SPINFREE (&dns_lock);

		EVENTDONE (&can_dp->d_resolver_done);
	}
	elif (dp->d_entry_code == 'M')
	{
		SPINLOCK (&dns_lock);

		can_dp = get_nm_dns_entry (dns[1].d_host_nm, 'C');

	   /***	can_dp->d_host_nm = ...; ***/

		if (dns[1].d_host_addr != 0)
			can_dp->d_host_addr = dns[1].d_host_addr;

	   /***	can_dp->d_entry_code = 'C'; ***/
	   /***	can_dp->d_resolver_done = ...; ***/
	   	can_dp->d_error_code = dns[1].d_error_code;

		if (can_dp->d_server_index == 0)
			can_dp->d_server_index = dns[1].d_server_index;

		can_dp->d_expir_time = dns[1].d_expir_time + time;
		can_dp->d_last_use_time = time;

		can_dp->d_preference = dns[1].d_preference;

		/* Processa a cadeia de servidores de mail */

		tst_dp = dp->d_mail_ptr;

		for (i = scb.y_n_dns; /* abaixo */; i--)
		{
			if (tst_dp == NODNS)
			{
				can_dp->d_mail_ptr = dp->d_mail_ptr;
				dp->d_mail_ptr = can_dp;
				break;
			}

			if (tst_dp == can_dp)
				break;

			if (i <= 0)
				{ printf ("%g: Ponteiros MAIL em malha\n"); break; }

			tst_dp = tst_dp->d_mail_ptr;
		}

		SPINFREE (&dns_lock);

		if (can_dp->d_host_addr != 0)
			EVENTDONE (&can_dp->d_resolver_done);
	}

	if (dns[0].d_entry_code != 'M')
		EVENTDONE (&dp->d_resolver_done);

	return (0);

}	/* end k_dns_put_info */

/*
 ****************************************************************
 *	Procura a entrada DNS de nome dado			*
 ****************************************************************
 */
DNS *
get_nm_dns_entry (char *nm, char entry_code)
{
	DNS		*dp, *nm_dp = NODNS;
	DNS		*old_dp = NODNS, *free_dp = NODNS, *expir_dp = NODNS;
	time_t		old_time = FAR_FUTURE;
	char		*cp, c;

	/*
	 *	Canoniza o nome; Supõe que tem
	 *	espaço para NETNM_SZ caracteres
	 */
	nm[NETNM_SZ-1] = '\0';

	for (cp = nm; (c = *cp) != '\0'; cp++)
	{
		if (c >= 'A' && c <= 'Z')
			*cp += ('a' - 'A');
	}

	/*
	 *	Supõe que a tabela DNS já esteja com SPINLOCK
	 */
	for (dp = scb.y_dns; /* abaixo */; dp++)
	{
		if (dp >= scb.y_end_dns)
			{ dp = NODNS; break; }

		if (dp->d_host_nm[0] == 0 && dp->d_entry_code != 'R')
		{
			if (free_dp == NODNS)
				free_dp = dp;

			continue;
		}

		if (streq (dp->d_host_nm, nm))
		{
			/* Encontrou o nome */

			nm_dp = dp;

			if
			(	dp->d_entry_code == entry_code ||
				dp->d_entry_code == 'A' && entry_code == 'C' ||
				dp->d_entry_code == 'C' && entry_code == 'A'
			)
			{

				if (dp->d_expir_time <= time)
					break;

				return (dp);
			}
		}

		if (dp->d_server_index != 0)	/* Não permanente */
		{
			if (dp->d_expir_time <= time && expir_dp == NODNS)
				expir_dp = dp;

			if (dp->d_last_use_time < old_time)
				{ old_dp = dp; old_time = dp->d_last_use_time; }
		}
	}

	/*
	 *	Não encontrou o nome, mas uma entrada vazia (assim esperamos)
	 */
	if (dp == NODNS)
		dp = free_dp;

	if (dp == NODNS)
		dp = expir_dp;

	if (dp == NODNS)
		dp = old_dp;

	if (dp == NODNS)
		{ printf ("%g: Não obtive entrada DNS\n"); dp = scb.y_end_dns - 1; }

	/*
	 *	Remove todas as entradas que apontam para entradas vazias
	 */
	if (dp != nm_dp)
		{ dp->d_host_nm[0] = '\0'; rm_dns_ptr (); }

	/*
	 *	Preenche parcialmente a entrada
	 */
   	memclr (dp, sizeof (DNS));

	strcpy (dp->d_host_nm, nm);
   /***	dp->d_host_addr = ...; ***/

	dp->d_entry_code = entry_code;
   /***	EVENTCLEAR (&dp->d_resolver_done); ***/
   /***	dp->d_resolver_got_req = 0; ***/
   /***	dp->d_error_code = ...; ***/
   /***	dp->d_server_index = ...; ***/

	dp->d_expir_time = FAR_FUTURE;
	dp->d_last_use_time = time;

   /***	dp->d_preference = ...; ***/
   	dp->d_transmissions = 0;

   /***	dp->d_can_ptr = ...; ***/
   /***	dp->d_mail_ptr = ...; ***/

	return (dp);

}	/* end get_nm_dns_entry */

/*
 ****************************************************************
 *	Procura a entrada DNS de endereço dado			*
 ****************************************************************
 */
DNS *
get_addr_dns_entry (IPADDR host_addr)
{
	DNS		*dp;
	DNS		*old_dp = NODNS, *free_dp = NODNS, *expir_dp = NODNS;
	time_t		old_time = FAR_FUTURE;

	/*
	 *	Supõe que a tabela DNS já esteja com SPINLOCK
	 */
	for (dp = scb.y_dns; /* abaixo */; dp++)
	{
		if (dp >= scb.y_end_dns)
			{ dp = NODNS; break; }

		if (dp->d_host_nm[0] == 0 && dp->d_entry_code != 'R')
		{
			if (free_dp == NODNS)
				free_dp = dp;

			continue;
		}

		if (dp->d_entry_code != 'C' && dp->d_entry_code != 'R')
			continue;

		if (dp->d_host_addr == host_addr)
		{
			/* Encontrou o endereço */

			if (dp->d_expir_time <= time)
				break;

			return (dp);
		}

		if (dp->d_server_index != 0)	/* Não permanente */
		{
			if (dp->d_expir_time <= time && expir_dp == NODNS)
				expir_dp = dp;

			if (dp->d_last_use_time < old_time)
				{ old_dp = dp; old_time = dp->d_last_use_time; }
		}
	}

	/*
	 *	Não encontrou o endereço, mas uma entrada vazia (assim esperamos)
	 */
	if (dp == NODNS)
		dp = free_dp;

	if (dp == NODNS)
		dp = expir_dp;

	if (dp == NODNS)
		dp = old_dp;

	if (dp == NODNS)
		{ printf ("%g: Não obtive entrada DNS\n"); dp = scb.y_end_dns - 1; }

	/*
	 *	Remove todas as entradas que apontam para entradas vazias
	 */
   	memclr (dp, sizeof (DNS));

	rm_dns_ptr ();

	/*
	 *	Preenche parcialmente a entrada
	 */
   /***	strcpy (dp->d_host_nm, ...); ***/
   	dp->d_host_addr = host_addr;

	dp->d_entry_code = 'R';
   /***	EVENTCLEAR (&dp->d_resolver_done); ***/
   /***	dp->d_resolver_got_req = 0; ***/
   /***	dp->d_error_code = ...; ***/
   /***	dp->d_server_index = ...; ***/

	dp->d_expir_time = FAR_FUTURE;
	dp->d_last_use_time = time;

   /***	dp->d_preference = ...; ***/
   	dp->d_transmissions = 0;

   /***	dp->d_can_ptr = ...; ***/
   /***	dp->d_mail_ptr = ...; ***/

	return (dp);

}	/* end get_addr_dns_entry */

/*
 ****************************************************************
 *	Remove as entradas referenciadoras			*
 ****************************************************************
 */
void
rm_dns_ptr (void)
{
	const DNS	*ptr_dp;
	DNS		*rm_dp;

	for (EVER) for (rm_dp = scb.y_dns; /* abaixo */; rm_dp++)
	{
		if (rm_dp >= scb.y_end_dns)
			return;

		if (rm_dp->d_host_nm[0] == 0)
			continue;

		if
		(	(ptr_dp = rm_dp->d_can_ptr) == NODNS &&
			(ptr_dp = rm_dp->d_mail_ptr) == NODNS
		)
			continue;

		if (ptr_dp->d_host_nm[0] != '\0')
			continue;
#ifdef	MSG
		if (CSWT (33))
			printf ("%g: Removendo \"%s\"\n", rm_dp->d_host_nm);
#endif	MSG
		memclr (rm_dp, sizeof (DNS));
		break;

	}	/* end for (EVER) */

}	/* end rm_dns_ptr */
