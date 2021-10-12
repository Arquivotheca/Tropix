/*
 ****************************************************************
 *								*
 *			categ.h					*
 *								*
 *	Definições acerca das Categorias Léxicas		*
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

/*
 ****************************************************************
 *	Categorias dos caracteres				*
 ****************************************************************
 */
#define	C_INV	0	/* Caractere inválido */
#define	C_COM	1	/* Início de comentário: # */
#define	C_SEP	2	/* Separadores: <sp>, <ht>, <vt> e <ff> */
#define	C_LET	3	/* Letras e _ */
#define	C_DIG	4	/* Dígitos */
#define	C_VAL	5	/* Outros caractere válidos para nomes de arquivos */
#define	C_QUO	6	/* " e ' */
#define	C_MAC	7	/* $ */
#define	C_CL	8	/* : */
#define	C_EQ	9	/* = */
#define	C_BL	10	/* \ */
#define	C_SM	11	/* ; */
#define	C_EOL	12	/* Fim de linha: <nl> */
#define	C_EOT	13	/* Fim de texto: <nul> */

/*
 ****************************************************************
 *	Máscaras e macros para acesso à "chartb"		*
 ****************************************************************
 *
 *	A "chartb" possui uma entrada (do tipo unsigned char)
 *	para cada um dos 256 caracteres ISO existentes.
 *
 *	Esta entrada tem o seguinte formato:
 *
 *		 7 6 5 4 3 2 1 0
 *		+-+-+-+-+-+-+-+-+
 *		|M|C| | | categ |
 *		+-+-+-+-+-+-+-+-+
 *
 *	bite 7:		0 => não é metacaractere para a SHELL
 *			1 => é metacaractere para a SHELL
 *	bite 6:		0 => não pode ser usado em chamada de macro
 *			1 => pode ser usado em chamada de macro
 *	bites 5 e 4	não utilizados
 *	bites 3 - 0	categoria (ver #defines acima)
 *		
 */
#define	C_MASK	0x0F		/* Máscara para obter a categoria */
#define	C_META	0x80		/* É metacaractere para a SHELL */
#define	C_MCALL	0x40		/* É válido para chamada de macro */

#define	CATEG(t)	((t) & C_MASK)
#define	META(t)		((t) & C_META)
#define	MCALL(t)	((t) & C_MCALL)

/*
 ****************************************************************
 *	Tabela de categorias léxicas				*
 ****************************************************************
 */
extern	const uchar	chartb[];
