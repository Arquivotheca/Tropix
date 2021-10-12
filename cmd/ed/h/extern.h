/*
 ****************************************************************
 *								*
 *			h/extern.h				*
 *								*
 *	Declaração de variáveis globais				*
 *								*
 *	Versão	1.0.0, de 13.11.86				*
 *		3.0.0, de 26.04.97				*
 *								*
 *	Modulo: ED						*
 *		Editores de Texto				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ******	Variáveis globais de "main.c" ***************************
 */
extern const char pgversion[];	/* Versão do programa */

extern int	vflag;		/* Verbose */

extern TEXT	Text;		/* Informações sobre o Texto em edição */
extern BASE	*Root;		/* Início da Lista com o Texto em edição */
extern BASE	*Current;	/* Posição Corrente */
extern AUTO	Search;		/* Expressão Regular p/ Busca */

extern bool	Interactive;	/* Edição no terminal */
extern bool	Interruption;	/* Houve uma interrupção */
extern char	temp_buffer[LSIZE]; /* Buffer temporario para uso geral */

/*
 ******	Variáveis globais de "msg" ******************************
 */
extern const char	M_ok[];
extern const char	M_nspace[];
extern const char	M_wrerror[];
extern const char	M_rderror[];
extern const char	M_permission[];
extern const char	M_nfound[];
extern const char	M_directory[];
extern const char	M_iregular[];
extern const char	M_object[];
extern const char	M_empty[];
extern const char	M_rdonly[];
extern const char	M_exists[];
extern const char	M_saved[];
extern const char	M_newfile[];
extern const char	M_size[];

/*
 ******	Variáveis globais de "io" *******************************
 */
extern const char	*M_txtio;	/* Mensagem correspondente a TXTIO */

/*
 ******	Variáveis globais de "re" *******************************
 */
extern char	*_end_match;	/* Primeiro caracter na linha após a expressão */

/*
 ******	Variáveis globais de "scan" *****************************
 */
extern char		Line[LSIZE];
extern char		*nextp;
extern const char	*error_msg;	/* Mensagem de Erro */

#ifdef	SCAN_H
extern TOKEN		token;
#endif	SCAN_H
