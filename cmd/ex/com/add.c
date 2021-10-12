/*
 ****************************************************************
 *								*
 *			add.c					*
 *								*
 *	Reconhecimento e avaliação de endereços			*
 *								*
 *	Versão	2.0.0, de 17.08.88				*
 *		3.0.0, de 23.03.95				*
 *								*
 *	Módulo: EX/VI						*
 *		Editores de texto				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdlib.h>
#include <ctype.h>

#include "../h/global.h"
#include "../h/text.h"
#include "../h/commands.h"
#include "../h/scan.h"
#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Condições no reconhecimento e avaliação de endereços	*
 ****************************************************************
 */
#define  S_INIT		000	/* Estado Inicial */
#define  S_BASE		001	/* Endereço base ja' fornecido */
#define  S_PLUS		002	/* Existe um '+' pendente */
#define  S_MINUS	004	/* Existe um '-' pendente */

/*
 ****************************************************************
 *	Reconhecimento e avaliação de endereços			*
 ****************************************************************
 */
BASE *
get_address (BASE *current)
{
	int		state;	/* Estado na Leitura e Calculo de Endereços */
	BASE		*add = NOBASE;	/* Linha endereçada */ /* CUIDADO */
	char		*bp;	/* Início do Endereço em Line */

	char		mark;
	int		count;
	bool		advanced;
	char		*string;
	bool		found = false; /* CUIDADO */

	error_msg = NOSTR;

	state = S_INIT;
	advanced = false;

	while (isspace (*nextp))
		nextp++;

	bp = nextp;

	while (*nextp) switch (*nextp++)
	{
	    case '.':
		if (state & S_BASE)
			goto s_error;

		add = current;
		state |= S_BASE;
		continue;

	    case '$':
		if (state & S_BASE)
			goto s_error;

		add = LAST ();
		state |= S_BASE;
		continue;

	    case '\'':
		if (state & S_BASE)
			goto s_error;

		if ((mark = scn_lower ()) == '\0')
			return (NOBASE);

		state |= S_BASE;

		if ((add = mrk_base (mark)) == NOBASE)
			goto m_error;

		continue;

	    case '1':
	    case '2':
	    case '3':
	    case '4':
	    case '5':
	    case '6':
	    case '7':
	    case '8':
	    case '9':
		count = *--nextp - '0';

		while (isdigit (*++nextp))
		{
			count = count * 10 + *nextp - '0';

			if (count < 0)
				goto c_error;
		}

		advanced = true;

		if ((state & S_BASE) == 0)
		{
			if ((add = txt_base (count)) == NOBASE)
				goto o_error;

			state |= S_BASE;
		}
		elif (state & S_MINUS)
		{
			if ((add = txt_advance (add, -count)) == NOBASE)
				goto o_error;

			state &= ~S_MINUS;
		}
		else
		{
			if ((add = txt_advance (add, count)) == NOBASE)
				goto o_error;

			state &= ~S_PLUS;
		}

		advanced = false;
		continue;

	    case '/':
		if ((state & S_BASE) == 0)
		{
			add = current;
			state |= S_BASE;
		}
		elif (state & S_MINUS)
		{
			if (ISFIRST (add))
				goto o_error;

			add = add->ant;
			state &= ~S_MINUS;
		}
		elif (state & S_PLUS)
		{
			if (ISLAST (add))
				goto o_error;

			add = add->pos;
			state &= ~S_PLUS;
		}

		if ((string = scn_string ('/')) == NOSTR)
			return (NOBASE);

		if (*string == '\0')
		{
			if (Search == NOSTR)
				goto e_error;
		}
		elif (! re_compile (string, &Search))
		{
			return (NOBASE);
		}

		current = add;

		for (add = current->pos ; add != current ; add = add->pos)
		{
			if (found = re_execute (Search, add->ch, add->ch) >= 0)
				break;
		}

		if (! found)
			goto n_error;

		continue;

	    case '?':
		if ((state & S_BASE) == 0)
		{
			add = current;
			state |= S_BASE;
		}
		elif (state & S_MINUS)
		{
			if (ISFIRST (add))
				goto o_error;

			add = add->ant;
			state &= ~S_MINUS;
		}
		elif (state & S_PLUS)
		{
			if (ISLAST (add))
				goto o_error;

			add = add->pos;
			state &= ~S_PLUS;
		}

		if ((string = scn_string ('?')) == NOSTR)
			return (NOBASE);

		if (*string == '\0')
		{
			if (Search == NOSTR)
				goto e_error;
		}
		elif (! re_compile (string, &Search))
		{
			return (NOBASE);
		}

		current = add;

		for (add = current->ant ; add != current ; add = add->ant)
		{
			if (found = re_execute (Search, add->ch, add->ch) >= 0)
				break;
		}

		if (! found)
			goto n_error;

		continue;

	    case '-':
		if ((state & S_BASE) == 0)
		{
			add = current;
			state |= S_BASE | S_MINUS;
		}
		elif (state & S_PLUS)
		{
			add = add->pos;
			state &= ~S_PLUS;
			state |= S_MINUS;
		}
		elif (state & S_MINUS)
		{
			if (ISFIRST (add))
				goto o_error;

			add = add->ant;
		}
		else
		{
			state |= S_MINUS;
		}

		continue;

	    case '+':
		if ((state & S_BASE) == 0)
		{
			add = current;
			state |= S_BASE | S_PLUS;
		}
		elif (state & S_MINUS)
		{
			add = add->ant;
			state &= ~S_MINUS;
			state |= S_PLUS;
		}
		elif (state & S_PLUS)
		{
			if (ISLAST (add))
				goto o_error;

			add = add->pos;
		}
		else
		{
			state |= S_PLUS;
		}

		continue;

	    case ' ':
	    case '\t':
		continue;

	    default:
		nextp--;
		advanced = true;
		goto end;

	}	/* end while (*nextp) switch (*nextp++) */

    end:
	if ((state & S_BASE) == 0)
		return (NOBASE);

	/*
	 *	Processamento de '+' ou '-' pendentes
	 */
	if (state & S_MINUS)
	{
		if (ISFIRST (add))
			goto o_error;

		add = add->ant;
		state &= ~S_MINUS;
	}
	elif (state & S_PLUS)
	{
		if (ISLAST (add))
			goto o_error;

		add = add->pos;
		state &= ~S_PLUS;
	}

	token.t_begin = bp;
	token.t_end = (advanced) ? nextp - 1 : nextp;

	return (add);

	/*
	 *	Tratamento de erros
	 */
    c_error:
	token.t_begin = bp;
	token.t_end = nextp;
	error_msg = "Número grande demais";
	return (NOBASE);

    e_error:
	token.t_begin = nextp - 2;
	token.t_end = nextp - 1;
	error_msg = "Expressão regular para busca indefinida";
	return (NOBASE);

    m_error:
	error_msg = "Marca não definida";
	return (NOBASE);

    n_error:
	error_msg = "Expressão regular não encontrada";
	return (NOBASE);

    o_error:
	token.t_begin = bp;
	token.t_end = (advanced) ? nextp - 1 : nextp;
	error_msg = "Endereço inexistente";
	return (NOBASE);

    s_error:
	token.t_begin = nextp;
	token.t_end = nextp;
	error_msg = "Erro de sintaxe";
	return (NOBASE);

}	/* end get_addresses */
