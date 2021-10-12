/*
 ****************************************************************
 *								*
 *			id_insert.h				*
 *								*
 *	Lista de identificadores (registros) iniciais da symtb 	*
 *								*
 *	Versão	3.0.0, de 21.02.94				*
 *		3.0.0, de 30.03.94				*
 *								*
 *	Módulo: AS-80386/486/PENTIUM				*
 *		Tradutores					*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 *	Argumentos da macro DEF:
 *
 *		DEF (índice do registrador, código interno, nome)
 */


/*
 ******	O acumulador ("eax)" ************************************
 */
DEF (0,    0,	"r0")
DEF (0,	   0,	"ra")
DEF (0,	   0,	"eax")
DEF (0,	   0,	"ax")
DEF (0,	   0,	"al")

DEF (8,	   4,	"h0")
DEF (8,	   4,	"ha")
DEF (8,	   4,	"ah")

/*
 ******	Registro "ebx" ******************************************
 */
DEF (1,	   3,	"r3")
DEF (1,	   3,	"rb")
DEF (1,	   3,	"ebx")
DEF (1,	   3,	"bx")
DEF (1,	   3,	"bl")

DEF (9,    7,	"h3")
DEF (9,    7,	"hb")
DEF (9,    7,	"bh")

/*
 ******	Registro "ecx" ******************************************
 */
DEF (2,	   1,	"r1")
DEF (2,	   1,	"rc")
DEF (2,	   1,	"ecx")
DEF (2,	   1,	"cx")
DEF (2,	   1,	"cl")

DEF (10,   5,	"h1")
DEF (10,   5,	"hc")
DEF (10,   5,	"ch")

/*
 ******	Registro "edx" ******************************************
 */
DEF (3,	   2,	"r2")
DEF (3,	   2,	"rd")
DEF (3,	   2,	"edx")
DEF (3,	   2,	"dx")
DEF (3,	   2,	"dl")

DEF (11,   6,	"h2")
DEF (11,   6,	"hd")
DEF (11,   6,	"dh")

/*
 ******	Registro "sp" *******************************************
 */
DEF (4,	   4,	"sp")
DEF (4,	   4,	"esp")

/*
 ******	Registro "fp" *******************************************
 */
DEF (5,    5,	"fp")
DEF (5,    5,	"bp")
DEF (5,    5,	"ebp")

/*
 ******	Registro "esi" ******************************************
 */
DEF (6,    6,	"r4")
DEF (6,    6,	"si")
DEF (6,    6,	"esi")

/*
 ******	Registro "edi" ******************************************
 */
DEF (7,    7,	"r5")
DEF (7,    7,	"di")
DEF (7,    7,	"edi")

/*
 ******	Registros Flutuantes ***********************************
 */
DEF (12,   0,	"f0")
DEF (13,   1,	"f1")
DEF (14,   2,	"f2")
DEF (15,   3,	"f3")
DEF (16,   4,	"f4")
DEF (17,   5,	"f5")
DEF (18,   6,	"f6")
DEF (19,   7,	"f7")

/*
 ******	Registros de contrôle ***********************************
 */
DEF (20,   0,	"cr0")
DEF (21,   2,	"cr2")
DEF (22,   3,	"cr3")

/*
 ******	Registros de depuração **********************************
 */
DEF (23,   0,	"dr0")
DEF (24,   1,	"dr1")
DEF (25,   2,	"dr2")
DEF (26,   3,	"dr3")
DEF (27,   6,	"dr6")
DEF (28,   7,	"dr7")

/*
 ******	Registro de teste ***************************************
 */
DEF (29,   6,	"tr6")
DEF (30,   7,	"tr7")

/*
 ******	Registros de segmentação ********************************
 */
DEF (31,   0,	"es")
DEF (32,   1,	"cs")
DEF (33,   2,	"ss")
DEF (34,   3,	"ds")
DEF (35,   4,	"fs")
DEF (36,   5,	"gs")
