/*
 ****************************************************************
 *								*
 *			extern.h				*
 *								*
 *	Declaração de variáveis					*
 *								*
 *	Versão	3.0.0, de 29.04.97				*
 *		3.0.1, de 30.04.97				*
 *								*
 *	Módulo: EX/VI						*
 *		Editores de texto				*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ******	Variáveis globais ***************************************
 */
extern const char	pgversion[];

extern char		Line[];
extern char		dos_mode;	/* Modo DOS (<cr><nl>) */
extern char		*nextp;
extern char		*Search;	/* Autômato para Busca */
extern char		*Esearch;	/* Expressão regular para Busca */
extern bool		Interruption;	/* Houve uma interrupção */
extern char		*System;	/* Último comando para o Shell */
extern bool		Verbose;
extern bool		Mdebug;
extern bool		Debug;
extern const char	*error_msg;	/* Mensagens de Erro */
extern char		temp_buffer[];	/* Buffer temporário para uso geral */
extern char		Message[80];	/* Mensagem pendente */

extern char		*_end_match;	/* Primeiro caracter na linha após a expressão */

extern char		*M_txtio;	/* Mensagem correspondente a TXTIO */
extern const char	int_tab[];
extern const char	obj_tab[];
extern bool		Redo;		/* Repetindo a Última Edição */
extern int		LINES, COLS;	/* Número de linhas/colunas */

#ifdef	H_SCAN
extern TOKEN		token;
#endif	H_SCAN

#ifdef	H_CURSES
extern WINDOW		*Win;		/* Janela onde ocorre a edição */
#endif	H_CURSES

#ifdef	H_TEXT
extern TEXT		*Text;		/* Texto em Edição */
#endif	H_TEXT

#ifdef	H_COMMANDS
extern COMM		*Comm;		/* Informações sobre o Texto em edição */
#endif	H_COMMANDS

#ifdef	H_VISUAL
extern VIEW		*View;		/* Estado da Edição Visual */
#endif	H_VISUAL
