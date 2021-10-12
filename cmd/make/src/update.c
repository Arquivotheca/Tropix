/*
 ****************************************************************
 *								*
 *			update.c				*
 *								*
 *	Atualização de Módulos					*
 *								*
 *	Versão	3.0.0, de 10.06.96				*
 *		3.2.3, de 14.12.99				*
 *								*
 *	Módulo: Make: Construtor de Programas			*
 *		Interpretador de Comandos			*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "../h/common.h"
#include "../h/error.h"
#include "../h/sym.h"
#include "../h/scan.h"
#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Definições locais de macros				*
 ****************************************************************
 */
#define	MAXARGS	256	/* Número máximo de argumentos para um comando */

/*
 ****************************************************************
 *	Protótipos de funções locais				*
 ****************************************************************
 */
int	update (SYM *, SYM *);
void	update_dynamic_macros (SYM *);
void	reset_dynamic_macros (void);
int	execute_command_list (DEPEN *);
int	exec (char *, int, int);
int	check_and_update_time (SYM *);
int	update_time (SYM *, int);
void	avoid_cycles (SYM *);
void	update_flags (SYM *, int);

/*
 ****************************************************************
 *	Atualiza um módulo e analisa o retorno			*
 ****************************************************************
 */
entry int
make (SYM *sp)
{
	int	not_virtual;

	not_virtual = (sp->s_node.d_flags & D_VIRTUAL) == 0;

	switch (update (sp, NOSYM))
	{
	    case NIL:
		if (not_virtual)
		{
			msg
			(	"***** \"%s\" não foi atualizado devido a erros",
				sp->s_name
			);
		}

		return (1);

	    case 0:
		if (not_virtual)
		{
			msg
			(	"\"%s\" já está atualizado",
				sp->s_name
			);
		}

		return (0);

	    default:
		return (0);

	}	/* end switch (update (sp)) */

}	/* end make */

/*
 ****************************************************************
 *	Atualiza um módulo					*
 ****************************************************************
 *
 *	Retorna:
 *	  NIL	se houve erro na atualização
 *	  0	se não foi preciso atualizar
 *	  > 0	se houve atualização
 */
int
update (SYM *sp, SYM *psp)
{
	SYM		*dsp;
	DEPEN		*dp, *ddp;
	NLIST		*np;
	time_t		target_time;
	int		update_error, must_update;

	dp = &sp->s_node;

	if (dp->d_flags & D_READY)
		return (dp->d_flags & D_ERROR ? NIL : 0);

#ifdef	DEBUG
	fprintf (stderr, "Atualizando %s\n", sp->s_name);
	fflush (stderr);
#endif	DEBUG

	target_time = dp->d_time;

	if (dp->d_flags & D_EXIST && target_time != 0)
	{	/*
		 *	O módulo já existe e sua lista de dependências
		 *	contém apenas ".exist". Nada é feito.
		 */
		dp->d_flags |= D_READY;
		return (0);
	}

	/*
	 *	Marca o alvo e seus irmãos, a fim de detectar ciclos
	 *	no decorrer da busca em profundidade.
	 */
	avoid_cycles (sp);

	/*
	 *	Analisa a lista de pre-requisitos.
	 */
	np = dp->d_depen;

	update_error =	0;
	must_update  =	target_time == 0 && psp != NOSYM ||
		      	np == NONLIST && dp->d_command != NOCLIST;

	for (/* np inicializado acima */; np != NONLIST; np = np->n_next)
	{
		dsp = np->n_sym;
		ddp = &dsp->s_node;

		if (ddp->d_flags & D_CYCLE)
		{
			msg
			(	"O grafo de dependências é cíclico: "
				"\"%s\" => \"%s\" => \"%s\"",
				psp->s_name, sp->s_name, dsp->s_name
			);
			exit (1);
		}

		/*
		 *	Chama recursivamente a função "update".
		 *	Se não houver erros, compara os tempos.
		 */
		if (update (dsp, sp) < 0)
		{
			update_error++;
			must_update = 0;
		}
		elif (update_error == 0 && (build || ddp->d_time > target_time))
		{
			must_update++;
		}

	}	/* end for (...; np != NONLIST; np = np->n_next) */

	/*
	 *	Verifica se é preciso atualizar.
	 */
	if (must_update)
	{
		if (why)
		{	/*
			 *	Explica a atualização a ser feita.
			 */
			fprintf (stderr, "\n%s:", sp->s_name);

			for (np = dp->d_depen; np != NONLIST; np = np->n_next)
			{
				dsp = np->n_sym;
				ddp = &dsp->s_node;

				if (build || ddp->d_time > target_time)
					fprintf (stderr, " %s", dsp->s_name);
			}

			putc ('\n', stderr);
		}

		if (touch)
		{	/*
			 *	Foi dada a opção "-t". Dá um "touch" no
			 *	módulo em vez de executar os comandos.
			 */
			if ((dp->d_flags & D_VIRTUAL) == 0)
			{
				fprintf (stderr, "=> touch %s\n", sp->s_name);

				if (execcmd)
				{
					if
					(	utime
						(	sp->s_name,
							(time_t *)NULL
						) < 0
					)
					{
						msg
						(	"Não consegui dar "
							"\"touch\" em"
							" \"%s\"",
							sp->s_name
						);
						update_error++;
					}
				}
			}
		}
		elif (dp->d_command == NOCLIST)
		{
			if (!build && dp->d_depen == NONLIST)
			{
				msg
				(	"Não foi descrito como %s \"%s\"",
					target_time == 0 ? "criar" : "atualizar",
					sp->s_name
				);

				update_error++;
			}
		}
		else
		{	/*
			 *	Executa a lista de comandos.
			 */
			update_dynamic_macros (sp);

			if (execute_command_list (dp) != 0)
				update_error++;

			reset_dynamic_macros ();
		}

		/*
		 *	Atualiza o tempo do alvo e seus irmãos.
		 */
		must_update = execcmd && !update_error
				? check_and_update_time (sp)
				: update_time (sp, update_error);
	}
	else
	{	/*
		 *	Desliga "D_CYCLE" no alvo e seus irmãos.
		 */
		update_flags (sp, update_error);

	}	/* end if (must_update) */

	return (update_error ? NIL : must_update);

}	/* end update */

/*
 ****************************************************************
 *	Atualiza os valores das macros $@, $*, $< e $?		*
 ****************************************************************
 *
 *	Macro $@ =>	nome do alvo a ser construído
 *	Macro $* =>	$@ sem a extensão
 *	Macro $< =>	lista de todos os pre-requisitos
 *	Macro $? =>	lista dos pre-requisitos mais recentes
 *			do que o alvo a ser construído
 */
void
update_dynamic_macros (SYM *sp)
{
	DEPEN	*dp, *ddp;
	SYM	*dsp;
	NLIST	*np;
	time_t	target_time;
	int	len, ask_size, minor_size;
	char	*minor_text, *cmp;
	char	*ask_text,  *cap;
	char	*name, *cp;

	/*
	 *	Trata as macros $@ e $*.
	 */
	at_macro->m_begin = sp->s_name;

	if ((name = strdup (sp->s_name)) == NOSTR)
	{
		msg ("Memória insuficiente");
		exit (1);
	}

	if ((cp = strrchr (name, '.')) != NOSTR)
		*cp = NOCHR;

	star_macro->m_begin  = name;

	dp = &sp->s_node;
	if ((np = dp->d_depen) == NONLIST)
		return;

	target_time = dp->d_time;

	/*
	 *	Calcula os tamanhos dos textos das macros $< e $?.
	 */
	ask_size = minor_size = 0;
	for (/* np inicializado acima */; np != NONLIST; np = np->n_next)
	{
		dsp = np->n_sym;

		len = dsp->s_namelen + 1;
		minor_size += len;

		if (build || dsp->s_node.d_time > target_time)
			ask_size += len;
	}

	cmp = minor_text = emalloc (minor_size);

	if (ask_size == 0)
		cap = (ask_text = nullstr) + 1;
	else
		cap = ask_text = emalloc (ask_size);

	/*
	 *	Gera os textos das macros $< e $?.
	 */
	for (np = dp->d_depen; np != NONLIST; np = np->n_next)
	{
		dsp = np->n_sym;
		ddp = &dsp->s_node;
		len = dsp->s_namelen;

		(void)strcpy (cmp, dsp->s_name);
		cmp += len;
		*cmp++ = ' ';

		if (build || ddp->d_time > target_time)
		{
			(void)strcpy (cap, dsp->s_name);
			cap += len;
			*cap++ = ' ';
		}
	}

	*--cmp = NOCHR;
	minor_macro->m_begin = minor_text;

	*--cap = NOCHR;
	ask_macro->m_begin   = ask_text;

}	/* end update_dynamic_macros */

/*
 ****************************************************************
 *	Libera as áreas das macros dinâmicas			*
 ****************************************************************
 */
void
reset_dynamic_macros (void)
{
	/*
	 *	Obs: "m_begin" é inicializado com "nullstr" em tbl/mksym.c
	 */
	if (minor_macro->m_begin != nullstr)
	{
		free (minor_macro->m_begin);
		minor_macro->m_begin = nullstr;
	}

	if (ask_macro->m_begin != nullstr)
	{
		free (ask_macro->m_begin);
		ask_macro->m_begin = nullstr;
	}

	at_macro->m_begin = nullstr;

	free (star_macro->m_begin);
	star_macro->m_begin = nullstr;

}	/* end reset_dynamic_macros */

/*
 ****************************************************************
 *	Executa a lista de comandos para atualizar um módulo	*
 ****************************************************************
 */
int
execute_command_list (DEPEN *dp)
{
	CLIST	*cp;
	char	*cmdp;
	int	metachar, ret, ignore;

	for
	(	ret = 0, cp = dp->d_command;
		ret == 0 && cp != NOCLIST;
		cp = cp->c_next
	)
	{
		/*
		 *	"nostop", ligado na opção "-k", age como se
		 *	todos os comandos estivessem precedidos de "-".
		 */
		ignore = nostop;

		cmdp = expand (cp->c_command, &metachar);

		if (*cmdp == '@')
		{
			while (*++cmdp == ' ')
				/* vazio */;
		}
		else
		{
			if (*cmdp == '-')
			{
				ignore++;
				while (*++cmdp == ' ')
					/* vazio */;
			}

			fprintf (stderr, "=> %s\n", cmdp);
		}

		if (execcmd && cmdp[0] != NOCHR)
			ret = exec (cmdp, metachar, ignore);
	}

	return (ret);

}	/* end execute_command_list */

/*
 ****************************************************************
 *	Executa um comando					*
 ****************************************************************
 */
int
exec (char *cmd, int metachar, int ignore)
{
	int		pid, status, code;
	char		**ap, *name, c;
	static char	*argv[MAXARGS];

	if (metachar)
	{	/*
		 *	Há metacaracteres: chama a shell.
		 */
		name    = shell;
		argv[0] = "sh";
		argv[1] = "-c";
		argv[2] = cmd;
		argv[3] = NOSTR;
	}
	else
	{	/*
		 *	Executa diretamente o comando.
		 */
		ap = argv;

		do
		{
			if (ap >= &argv[MAXARGS - 1])
			{
				msg
				(	"Número excessivo de argumentos. "
					"Aumente MAXARGS (> %d)",
					MAXARGS
				);
				exit (-1);
			}

			*ap++ = cmd;

			while ((c = *cmd) != ' ' && c != NOCHR)
				cmd++;

			if (c == ' ')
				*cmd++ = NOCHR;
		}
		while (c != NOCHR);

		*ap  = NOSTR;
		name = argv[0];
	}

	if ((pid = fork ()) < 0)
	{
		msg ("Não consegui criar um processo para \"%s\"", name);
		return (1);
	}

	if (pid == 0)
	{	/*
		 *	O Processo filho executa o comando.
		 */
		execvp ((const char *)name, (const char **)argv);
		msg ("Não consegui executar \"%s\"", name);
		exit (-1);
	}

	/*
	 *	Espera o processo filho terminar a execução.
	 *	Analisa o valor retornado.
	 */
	while (pid != wait (&status))
		/* vazio */;

	if (status)
	{
		if ((code = status & 0x7F) != 0)
		{	/*
			 *	O filho terminou em virtude de um sinal.
			 *	Emite um monte de mensagens inúteis.
			 */
			fprintf
			(	stderr,
				"%s: %s (\"%s\")",
				pgname,
				sigtostr (code),
				name
			);

			fprintf (stderr, "   PC: %P", _fpc);

			if (code == SIGBUS || code == SIGADR || code == SIGSEGV)
			{
				fprintf (stderr, "   Addr: %P", _faddr);
			}
			elif (code == SIGILL)
			{
				fprintf
				(	stderr,
					"   INST: %04X",
					(int)_faddr & 0xFFFF
				);
			}

			putc ('\n', stderr);

			if (status & 0x80)
			{
				fprintf
				(	stderr,
					"%s: \"%s\": core dumped",
					pgname,
					name
				);
			}

			/*
			 *	Trata os sinais de terminação explícita,
			 *	se não estiverem programados.
			 */
			if
			(	code == SIGINT  && sigint  == SIG_DFL ||
			    	code == SIGQUIT && sigquit == SIG_DFL ||
			    	code == SIGTERM && sigterm == SIG_DFL
			)
				    exit (1);
		}
		else
		{	/*
			 *	O filho deu "exit" com valor não-nulo.
			 */
			msg
			(	"***** (\"%s\"): Código de retorno = %d",
				name, status >> 8
			);
		}
	}

	return (ignore ? 0 : status);

}	/* end exec */

/*
 ****************************************************************
 *	Atualiza o tempo de uma lista de alvos irmãos		*
 ****************************************************************
 */
int
check_and_update_time (SYM *sp)
{
	DEPEN		*dp;
	NLIST		*np;
	SYM		*nsp, *dsp;
	time_t		target_time, now;
	int		changed, flags, time_error;
	STAT		st;

	now = time ((time_t *)NULL);
	changed = time_error = 0;

	nsp = sp;
	do
	{
		dp = &nsp->s_node;

		flags  = dp->d_flags;
		flags &= ~D_CYCLE;
		flags |= D_READY;

		if (stat (nsp->s_name, &st) < 0)
		{
			if ((flags & D_VIRTUAL) == 0)
			{	/*
				 *	O arquivo não existe e o alvo
				 *	não é virtual.
				 */
				msg
				(	"O alvo \"%s\" não foi criado",
					nsp->s_name
				);

				target_time = 0;
				time_error++;
				flags |= D_ERROR;
			}
			else
			{	/*
				 *	O alvo é virtual.
				 */
				target_time = now;
				changed++;
			}
		}
		else
		{
			if (flags & D_VIRTUAL)
			{
				msg
				(	"O alvo virtual \"%s\" corresponde "
					"a um arquivo!",
					nsp->s_name
				);
				continue;
			}

			/*
			 *	Verifica se o alvo ficou mais recente
			 *	que os pre-requisitos.
			 */
			target_time = st.st_mtime;

			for (np = dp->d_depen; np != NONLIST; np = np->n_next)
			{
				dsp = np->n_sym;

				if (target_time < dsp->s_node.d_time)	
				{
					msg
					(	"O alvo \"%s\" continua "
						"mais antigo que o "
						"pre-requisito \"%s\"",
						nsp->s_name,
						dsp->s_name
					);

					time_error++;
					flags |= D_ERROR;
					break;
				}
			}

			changed++;
		}

		dp->d_time = target_time;
		dp->d_flags = flags;

	}	while ((nsp = dp->d_sibling) != sp);

	return (time_error ? NIL : changed);

}	/* end check_and_update_time */

/*
 ****************************************************************
 *	Atualiza o tempo de uma lista de alvos irmãos		*
 ****************************************************************
 */
int
update_time (SYM *sp, int update_error)
{
	DEPEN		*dp;
	SYM		*nsp;
	time_t		now;
	int		mask;

	now = time ((time_t *)NULL);

	mask = D_READY;
	if (update_error)
		mask |= D_ERROR;

	nsp = sp;
	do
	{
		dp = &nsp->s_node;

		dp->d_flags &= ~D_CYCLE;
		dp->d_flags |= mask;
		dp->d_time = now;

	}	while ((nsp = dp->d_sibling) != sp);

	return (1);

}	/* end update_time */

/*
 ****************************************************************
 *	Liga o bite "D_CYCLE" nas flags de alvos irmãos		*
 ****************************************************************
 */
void
avoid_cycles (SYM *sp)
{
	SYM	*nsp;
	DEPEN	*dp;

	nsp = sp;
	do
	{
		dp = &nsp->s_node;
		dp->d_flags |= D_CYCLE;

	}	while ((nsp = dp->d_sibling) != sp);

}	/* end avoid_cycles */

/*
 ****************************************************************
 *	Atualiza "d_flags" para uma lista de alvos irmãos	*
 ****************************************************************
 */
void
update_flags (SYM *sp, int update_error)
{
	SYM	*nsp;
	DEPEN	*dp;
	int	mask;

	mask = D_READY;
	if (update_error)
		mask |= D_ERROR;

	sp->s_node.d_flags |= mask;

	nsp = sp;
	do
	{
		dp = &nsp->s_node;
		dp->d_flags &= ~D_CYCLE;

	}	while ((nsp = dp->d_sibling) != sp);

}	/* end update_flags */
