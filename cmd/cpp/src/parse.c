/*
 ****************************************************************
 *								*
 *			parse.c					*
 *								*
 *	Analisa o arquivo fonte					*
 *								*
 *	Versão	1.0.0, de 23.08.88				*
 *		4.1.0, de 23.08.01				*
 *								*
 *	Módulo: CPP						*
 *		Tradutores					*
 *		Categoria D					*	
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <stdio.h>
#include <setjmp.h>

#include "../h/scan.h"
#include "../h/lex.h"
#include "../h/error.h"
#include "../h/symtb.h"
#include "../h/cpp.h"

/*
 ****************************************************************
 *	Variáveis Utilizadas					*
 ****************************************************************
 */
entry jmp_buf	eof_env;	/* Ponto de EOF */

entry jmp_buf	fatal_env;	/* Ponto de erro fatal */

extern int	falselevel;	/* No. de Condicionais FALSE aninhadas */

extern const char categ[];	/* Tabela de categorias */

/*
 ****************************************************************
 *	Rotina Principal					*
 ****************************************************************
 */
void
parsefile (void)
{
	const char	*lp;
	char		c;
	char		area[6*MACSZ];

	putlineno (1);

	falselevel = 0;

	/*
	 *	Prepara o desvio de EOF
	 */
	if (setjmp (eof_env))
	{
		err_msg (ERRO, NOMARK, "Fim inesperado do arquivo");
		goto eof;
	}

	/*
	 *	Prepara o desvio de Erro irrecuperável
	 */
	if (setjmp (fatal_env))
		goto eof;

	/*
	 *	Malha pelos diversos arquivos
	 */
	do
	{
		while ((c = *(lp = nextp)) != '\0')
		{
			if (c == '#')
			{
				nextp = lp + 1;
				directive ();
			}
			elif (falselevel != 0)
			{
				skipline ();
			}
			else
			{
				if (lineout != lineno)
					putlineno (lineno);

				putlineout (expand (nextp, area));

				lineno++;
			}

		}	/* end while c != '\0'  */

	     eof:
		/* vazio */;

	}	while (pop () >= 0);

	stackcheck ();

}	/* end parsefile */

/*
 ****************************************************************
 *	Copia para o Arquivo de Saída				*
 ****************************************************************
 */
void
putlineout (const char *begin)
{
	const char	*cp;
	int		c;

	for (cp = begin; (c = *cp++) != '\0'; /* vazio */)
	{
		putc (c, fpout);

		if (c == '\n')
			lineout++;
	}

}	/* end putlineout */
