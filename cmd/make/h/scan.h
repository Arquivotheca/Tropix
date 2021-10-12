/*
 ****************************************************************
 *								*
 *			scan.h					*
 *								*
 *	Definições utilizadas pelo Analisador Léxico.		*
 *								*
 *	Versão	3.0.0, de 29.03.96				*
 *								*
 *	Módulo: Make: Construtor de Programas			*
 *		Interpretador de Comandos			*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 								*
 ****************************************************************
 */

#ifdef	MAJOR
#undef	MAJOR
#endif	MAJOR

/*
 ****************************************************************
 *	Majors dos tokens 					*
 ****************************************************************
 */
typedef	enum
{
	NAME,		/* Nome de macro ou de arquivo */
	CL,		/* : */
	EQ,		/* = */
	BOC,		/* Seqüência <nl><ht> */
	EOL,		/* Final da linha: <nl> não seguido de <ht> */
	EOT		/* Final do texto: <nul> */

}	MAJOR;

/*
 ****************************************************************
 *	Estrutura preenchida pelo analisador léxico		*
 ****************************************************************
 */
struct	token
{
	MAJOR	t_major;	/* Major do Token */
	RANGE	t_range;	/* Localização do token no texto */
	SYM	*t_sym;		/* Ponteiro para a estrutura SYM */
};
