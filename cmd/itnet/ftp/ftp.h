/*
 ****************************************************************
 *								*
 *			ftp.h					*
 *								*
 *	File transfer protocol 					*
 *								*
 *	Versão	3.0.0, de 12.10.92				*
 *		3.0.4, de 23.08.97				*
 *								*
 *	Módulo: FTP						*
 *		Utilitários de comunicações			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */


/*
 ******	Códigos de retorno **************************************
 */
typedef enum
{
	PRELIM = 1,	/* Positivo preliminar */
	COMPLETE,	/* Positivo completo */
	CONTINUE,	/* Positivo intermediário */
	TRANSIENT,	/* Negativo transiente */
	ERROR		/* Negativo completo */

}	REPLY_ENUM;

/*
 ******	Códigos dos tipos ***************************************
 */
typedef enum
{
	TYPE_A = 1,	/* ASCII */
	TYPE_E,		/* EBCDIC */
	TYPE_I,		/* IMAGEM */
	TYPE_L		/* LOCAL */

}	TYPE_ENUM;

/*
 ******	Códigos das formas **************************************
 */
typedef enum
{
	FORM_N = 1,	/* Não impressão */
	FORM_T,		/* TELNET */
	FORM_C		/* Controle da impressora (ASA) */

}	FORM_ENUM;

/*
 ******	Códigos das estruturas **********************************
 */
typedef enum
{
	STRU_F = 1,	/* Sem estrutura */
	STRU_R,		/* Estrutura de registro */
	STRU_P		/* Estrutura de página */

}	STRU_ENUM;

/*
 ******	Códigos dos modos ***************************************
 */
typedef enum
{
	MODE_S = 1,	/* Fluxo */
	MODE_B,		/* Bloco */
	MODE_C		/* Comprimido */

}	MODE_ENUM;
