/*
 ****************************************************************
 *								*
 *			$GEN.gen/config.h			*
 *								*
 *  Configuration for GNU C-compiler for Motorola 68000 family	*
 *								*
 *	Versão	3.0.0, de 09.01.94				*
 *		3.0.0, de 09.01.94				*
 *								*
 *	Módulo: GCC						*
 *		Compilador "C"					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 1993 UNITEC Informática Ltda	*
 * 		Baseado em software homônimo do GNU		*
 *								*
 ****************************************************************
 */


/*
 *	#defines that need visibility everywhere
 */
#define FALSE 0
#define TRUE 1

/*
 *	This describes the machine the compiler is hosted on
 */
#define HOST_BITS_PER_CHAR 8
#define HOST_BITS_PER_SHORT 16
#define HOST_BITS_PER_INT 32
#define HOST_BITS_PER_LONG 32
#define HOST_WORDS_BIG_ENDIAN

/*
 *	target machine dependencies. tm.h is a symbolic link
 *	to the actual target specific file
 */
#include "tm.h"

/*
 *	Arguments to use with `exit'
 */
#define SUCCESS_EXIT_CODE 0
#define FATAL_EXIT_CODE 33

/*
 *	If compiled with GNU C, use the built-in alloca
 */
#ifdef __GNUC__
#define alloca __builtin_alloca
#endif

/*
 *	Não gera nenhuma identificação
 */
#define	ASM_IDENTIFY_GCC
