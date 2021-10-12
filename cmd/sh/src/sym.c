/*
 ****************************************************************
 *								*
 *			sym.c					*
 *								*
 *	Manipulação da Tabela de Símbolos			*
 *								*
 *	Versão	3.2.2, de 27.11.99				*
 *		4.5.0, de 21.10.03				*
 *								*
 *	Módulo: SHELL						*
 *		Interpretador de Comandos			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../h/common.h"
#include "../h/sym.h"
#include "../h/scan.h"
#include "../h/command.h"

#include "../h/proto.h"
#include "../h/extern.h"

/*
 ****************************************************************
 *	Variáveis Globais					*
 ****************************************************************
 */
/*
 ******	Nó cabeça da lista circular de variáveis ****************
 */
entry	SYM	var_list =
{
	S_UNDEF,	/* s_type */
	{ 0 },		/* s_name */
	0,		/* s_union */
	NOSTR,		/* s_value */
	&var_list,	/* s_next */
	&var_list,	/* s_prev */
	NOSYMP		/* s_hnext */
};

/*
 ******	Ponteiros para diversas entradas importantes ***********
 */
entry	SYM	*home_sp,		/* variável HOME */
		*ps1_sp,		/* variável PS1 */
		*ps2_sp,		/* variável PS2 */
		*current_prompt,	/* prompt corrente */
		*hist_sp,		/* variável HIST */
		*path_sp;		/* Variávei PATH */

/*
 ****************************************************************
 *	Protótipos de Funções					*
 ****************************************************************
 */
int	add_to_environment (SYM *);

/*
 ****************************************************************
 *	Define uma variável, a partir de uma cadeia		*
 ****************************************************************
 */
SYM *
define_variable_from_string (char *str)
{
	char		*pt;
	SYM		*sp;

	if ((pt = strchr (str, '=')) == NOSTR)
		return (NOSYMP);

	*pt = NOCHR;

	sp = define_variable (str, pt + 1, -1 /* não altera flags */);

	*pt = '=';

	return (sp);

}	/* end define_variable_from_string */

/*
 ****************************************************************
 *	Define uma variável					*
 ****************************************************************
 */
SYM *
define_variable (const char *name, const char *value, int flags)
{
	SYM		*sp, *vsp;
	char		*str;

	/*
	 *	Insere o nome da variável e verifica se o tipo não é
	 *	S_FUNC ou S_PROG.
	 */
	if ((sp = put_sym (name)) == NOSYMP || sp->s_type > S_VAR)
		return (NOSYMP);

	/*
	 *	Verifica se o valor será modificado ou não.
	 */
	if (value == NOSTR)
		value = "";

	if ((str = strdup (value)) == NOSTR)
	{
		error (NOSTR);
		return (NOSYMP);
	}

	/*
	 *	Se a variável é nova, insere em ordem crescente
	 *	de nomes na lista de variáveis.
	 */
	if (sp->s_type == S_UNDEF)
	{
		strscpy (var_list.s_name, name, sizeof (var_list.s_name));	/* sentinela */

		for (vsp = var_list.s_next; strcmp (name, vsp->s_name) > 0; vsp = vsp->s_next)
			/* vazio */;

		vsp->s_prev->s_next = sp;
		sp->s_prev	    = vsp->s_prev;
		vsp->s_prev	    = sp;
		sp->s_next	    = vsp;

		sp->s_type = S_VAR;
	}

	/*
	 *	Modifica o valor e os indicadores, se necessário.
	 */
	if (flags >= 0)
		sp->s_flags = flags;

	if (sp->s_flags & V_READONLY)
	{
		error
		(	"O valor da variável \"%s\" não pode "
			"ser alterado",
			name
		);

		free (str);
		return (sp);
	}

	/*
	 *	Verifica se o novo valor é idêntico ao antigo.
	 */
	if (sp->s_value != NOSTR && streq (str, sp->s_value))
	{
		free (str);
		return (sp);
	}

	/*
	 *	Libera o valor anterior e atribui o novo.
	 */
	if (sp->s_value != NOSTR)
		free (sp->s_value);

	sp->s_value = str;

	/*
	 *	O valor foi alterado e a variável é
	 *	V_EXPORT: modifica também o ambiente.
	 */
	if (sp->s_flags & V_EXPORT)
		add_to_environment (sp);

	/*
	 *	Modificar certas variáveis causa efeitos colaterais.
	 */
	if   (sp == hist_sp)
		set_history_size (atol (str));
	elif (sp == path_sp)
		exec_hash_create (script == 0);

	return (sp);

}	/* end define_variable */

/*
 ****************************************************************
 *	Modifica apenas os indicadores de uma variável		*
 ****************************************************************
 */
int
set_variable_flag (const char *name, int flag)
{
	SYM		*sp;

	/*
	 *	Verifica se o identificador está definido e se é variável.
	 */
	if ((sp = search_sym (name)) == NOSYMP)
	{
		error ("\"%s\" não foi definido", name);
		return (-1);
	}

	if (sp->s_type != S_VAR)
	{
		error ("\"%s\" não é uma variável", name);
		return (-1);
	}

	/*
	 *	Altera os identificadorese, se for o caso, modifica o ambiente.
	 */
	if ((sp->s_flags & V_EXPORT) == 0 && (flag & V_EXPORT))
		add_to_environment (sp);

	sp->s_flags |= flag;

	return (0);

}	/* end set_variable_flag */

/*
 ****************************************************************
 *	Acrescenta uma variável ao ambiente			*
 ****************************************************************
 */
int
add_to_environment (SYM *sp)
{
	int		name_len, value_len;
	char		*env;

	/*
	 *	Prepara a cadeia "var=valor".
	 */
	name_len  = strlen ((const char *)sp->s_name);
	value_len = strlen (sp->s_value);

	if ((env = malloc (name_len + value_len + 2)) == NOSTR)
		return (-1);

	memmove (env, (const char *)sp->s_name, name_len);
	env[name_len] = '=';
	memmove (env + name_len + 1, sp->s_value, value_len + 1);

	/*
	 *	Acrescenta ao ambiente.
	 */
	if (putenv (env) < 0)
	{
		error ("*Erro em putenv (\"%s\")", env);
		free (env);
		return (-1);
	}

	return (0);

}	/* end add_to_environment */

/*
 ****************************************************************
 *	Remove uma variável					*
 ****************************************************************
 */
int
delete_variable (const char *name)
{
	SYM		**hp, *sp, *bsp;

	/*
	 *	Localiza o identificador, usando a tabela "hash".
	 */
	hp = &hashtb[strhash (name, strlen (name), hashsz)];

	for (bsp = NOSYMP, sp = *hp; sp != NOSYMP; bsp = sp, sp = sp->s_hnext)
	{
		if (streq (sp->s_name, name))
			goto found;
	}

	error ("Variável \"%s\" não definida", name);

	return (-1);

	/*
	 *	Achou: verifica se é uma variável e se pode ser removida.
	 */
    found:
	if (sp->s_type != S_VAR)
	{
		error ("\"%s\" não é uma variável", name);
		return (-1);
	}

	if (sp->s_flags & V_PERMANENT)
	{
		error ("A variável \"%s\" não pode ser removida", name);
		return (-1);
	}

	/*
	 *	Retira da cadeia hash.
	 */
	if (bsp == NOSYMP)
		*hp          = sp->s_hnext;
	else
		bsp->s_hnext = sp->s_hnext;

	/*
	 *	Retira também da lista de variáveis.
	 */
	sp->s_prev->s_next = sp->s_next;
	sp->s_next->s_prev = sp->s_prev;

	/*
	 *	Se for "export", tira também do ambiente.
	 */
	if (sp->s_flags & V_EXPORT)
	{
		if (delenv (name) < 0)
			error ("Variável \"%s\" não retirada do ambiente", name);
	}

	/*
	 *	Libera o espaço.
	 */
	free (sp->s_value);
	free (sp);

	return (0);

}	/* end delete_variable */

/*
 ****************************************************************
 *	Apenas Busca o Nome do Símbolo				*
 ****************************************************************
 */
SYM *
search_sym (const char *sym)
{
	SYM		*sp;

	for (sp = hashtb[strhash (sym, strlen (sym), hashsz)]; sp != NOSYMP; sp = sp->s_hnext)
	{
		if (streq (sp->s_name, sym))
			return (sp);
	}

	return (NOSYMP);

}	/* end search_sym */

/*
 ****************************************************************
 *	Busca/Insere um símbolo					*
 ****************************************************************
 */
SYM *
put_sym (const char *id)
{
	SYM		**hp;
	SYM		*sp, *bsp;

	/*
	 *	Calcula a posição na tabela hash.
	 */
	hp = &hashtb[strhash (id, strlen (id), hashsz)];

	/*
	 *	Busca na cadeia de colisões.
	 */
	for (bsp = NOSYMP, sp = *hp; sp != NOSYMP; bsp = sp, sp = sp->s_hnext)
	{
		if (streq (sp->s_name, id))
			return (sp);		/* achou */
	}

	/*
	 *	Cria um novo símbolo, ao final da cadeia de colisões.
	 */
	if ((sp = calloc (1, sizeof (SYM))) == NOSYMP)
		return (NOSYMP);

	strscpy (sp->s_name, id, sizeof (sp->s_name));

	if (bsp == NOSYMP)
		*hp          = sp;
	else
		bsp->s_hnext = sp;

/***	sp->s_value = S_UNDEF;	***/
/***	sp->s_type  = S_UNDEF;	***/
/***	sp->s_hnext = NOSYMP;	***/

	return (sp);

}	/* end put_sym */

/*
 ****************************************************************
 *	Mostra as variáveis					*
 ****************************************************************
 */
void
show_variables (FILE *fp, int mask)
{
	SYM		*sp;
	int		c;

	for (sp = var_list.s_next; sp != &var_list; sp = sp->s_next)
	{
		if (mask > 0 && (sp->s_flags & mask) == 0)
			continue;

		c = (sp->s_flags & V_EXPORT) ? 'e' : '-';
		putc (c, fp);

		c = (sp->s_flags & V_READONLY) ? 'r' : '-';
		putc (c, fp);

		c = (sp->s_flags & V_PERMANENT) ? 'p' : '-';
		putc (c, fp);

		fprintf (fp, "\t%-14.14s\t%s\n", sp->s_name, sp->s_value);
	}

}	/* end show_variables */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Mostra o ambiente da SHELL				*
 ****************************************************************
 */
int
show_environment (FILE *fp)
{
	const char	**cp;

	for (cp = environ; *cp != NOSTR; cp++)
		fprintf (fp, "%s\n", *cp);

	return (0);

}	/* end show_environment */
#endif	/*******************************************************/
