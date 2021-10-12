/*
 ****************************************************************
 *								*
 *			obj.h					*
 *								*
 *	Definições para o modo visual				*
 *								*
 *	Versão	2.0.0, de 28.07.86				*
 *		3.0.0, de 23.03.95				*
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

#define	H_OBJ		/* Para declarar as funções */

/*
 ******	Valores retornados por object () ************************
 */
#define		OBJ_ERROR	(-1)
#define		OBJ_NULL	0
#define		OBJ_FLINE	1
#define		OBJ_FCHAR	2
#define		OBJ_BLINE	3
#define		OBJ_BCHAR	4

#define	M4	0xF		/* Máscara para acesso as tabelas */

/*
 ******	Tabela de interpretação: classes léxicas ****************
 */
#define   L_NULL  	(0x0)
#define   L_BUFFER	(0x1)
#define   L_COUNT	(0x2)
#define   L_COMPLETE	(0x3)
#define   L_OPERATOR	(0x4)
#define   L_OBJECT	(0x5)
#define   L_SEARCH	(0x6)
#define   L_FIND  	(0x7)
#define   L_MARK  	(0x8)
#define   L_DEFMARK	(0x9)
#define   L_ESCAPE	(0xA)
#define   L_SYNONYM	(0xB)

/*
 ******	Macros para as classes léxicas: *************************
 */
#define   LN	L_NULL
#define   LB	L_BUFFER
#define   LD	L_COUNT
#define   LC	L_COMPLETE
#define   LP	L_OPERATOR
#define   LJ	L_OBJECT
#define   LS	L_SEARCH
#define   LF	L_FIND 
#define   LK	L_MARK
#define   LM	L_DEFMARK
#define   LE	L_ESCAPE
#define   LY	L_SYNONYM

#define   SCLASS(c)	(int_tab[(c)]  & M4)

/*
 ******	Tabela de interpretação: classes funcionais *************
 */
#define   F_NULL  	(0x0)
#define   F_WINDOW	(0x1)
#define   F_MOVE  	(0x2)
#define   F_MARK  	(0x3)
#define   F_YANK	(0x4)
#define   F_PUT		(0x5)
#define   F_INSERT	(0x6)
#define   F_DELETE	(0x7)
#define   F_CHANGE	(0x8)
#define   F_SHIFT	(0x9)
#define   F_JOIN	(0xA)
#define   F_REPLACE	(0xB)
#define   F_C		(0xC)
#define   F_D		(0xD)
#define   F_UNDO	(0xE)
#define   F_EXTRA  	(0xF)

/*
 ******	Macros para as classes funcionais ***********************
 */
#define   FN	(F_NULL		<< 4)
#define   FW	(F_WINDOW	<< 4)
#define   FM	(F_MOVE		<< 4)
#define   FK	(F_MARK		<< 4)
#define   FY	(F_YANK		<< 4)
#define   FP	(F_PUT		<< 4)
#define   FI	(F_INSERT	<< 4)
#define   FD	(F_DELETE	<< 4)
#define   FC	(F_CHANGE	<< 4)
#define   FS	(F_SHIFT	<< 4)
#define   FJ	(F_JOIN		<< 4)
#define   FR	(F_REPLACE	<< 4)
#define   FU	(F_UNDO		<< 4)
#define   FX	(F_EXTRA	<< 4)

#define   FCLASS(c)	((int_tab[(c)] >> 4)  & M4)

/*
 ******	Tabela dos objetos: "majors" ****************************
 */
#define   M_NULL	0x0
#define   M_FIND	0x1
#define   M_WORD	0x2
#define   M_INLINE	0x3
#define   M_COLUMN	0x4
#define   M_LINE	0x5
#define   M_SEARCH	0x6
#define   M_MARK	0x7
#define   M_PAGE	0x8
#define   M_BALANCE	0x9
#define   M_SENTENCE	0xA
#define   M_SECTION	0xB
#define   M_PARAGRAPH	0xC

/*
 ******	Macros para acesso aos "majors": ************************
 */
#define   MF	(M_FIND		<< 4)
#define   MW	(M_WORD		<< 4)
#define   ML	(M_INLINE	<< 4)
#define   MC	(M_COLUMN	<< 4)
#define   MH	(M_LINE		<< 4)
#define   MS	(M_SEARCH	<< 4)
#define   MK	(M_MARK		<< 4)
#define   MP	(M_PAGE		<< 4)
#define   MB	(M_BALANCE	<< 4)
#define   ME	(M_SENTENCE	<< 4)
#define   MO	(M_SECTION	<< 4)
#define   MG	(M_PARAGRAPH	<< 4)

#define   MAJOR(c)	((obj_tab[(c)] >> 4)& M4)

/*
 ******	Tabela dos objetos: "minors" para cada "major" **********
 */
/*
 *	M_FIND:		0 -> para frente inclusive
 *			1 -> para frente exclusive
 *			2 -> para tras inclusive
 *			3 -> para tras exclusive
 *			4 -> repete ultimo find
 *			5 -> reverte ultimo find
 *
 *	M_WORD:		0 -> w		3 -> W
 *			1 -> e		4 -> E
 *			2 -> b		5 -> B
 *
 *	M_INLINE:	0 -> primeiro caracter na linha corrente
 *			1 -> retrocede um caracter
 *			2 -> avanca um caracter
 *			3 -> ultimo caracter na linha corrente
 *			4 -> avanca tabulação
 *
 *	M_COLUMN:	0 -> retrocede linha na mesma coluna
 *			1 -> coluna mais proxima de contagem na linha corrente
 *			2 -> avanca linha na mesma coluna
 *
 *	M_LINE		0 -> avanca linha para primeiro não branco
 *			1 -> primeiro não branco na linha corrente
 *			2 -> retrocede linha para primeiro não branco
 *
 *	M_SEARCH:	0 -> busca para tras
 *			1 -> busca para frente
 *			2 -> repetição da ultima busca
 *			3 -> reversão da ultima busca
 *			4 -> final do match
 *
 *	M_MARK:		0 -> marca de um caracter
 *			1 -> marca de uma linha
 *
 *	M_PAGE:		0 -> primeira linha na pagina
 *			1 -> linha media na pagina
 *			2 -> ultima linha na pagina
 *
 *	M_SENTENCE:	0 -> avanca	1 -> retrocede
 *
 *	M_SECTION:	0 -> avanca	1 -> retrocede
 *
 *	M_PARAGRAPH:	0 -> avanca	1 -> retrocede
 */
/*
 *	Macros para acesso aos Minors
 */
#define   MINOR(c)	(obj_tab[(c)]  & M4)

/*
 ******	Estrutura objeto ****************************************
 */
typedef	struct
{
	short	o_obj;		/* Objeto */
	short	o_arg;		/* Argumento */

}	OBJ;

/*
 ******	Estrutura do comando visual *****************************
 */
typedef	struct
{
	int	v_count;
	char	v_fun;
	char	v_buf;
	char	v_com;
	char	v_arg;
	OBJ	v_obj;

}	VIS;

#define	NOVIS	(VIS *)0
