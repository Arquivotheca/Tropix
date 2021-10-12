/*
 ****************************************************************
 *								*
 *			proto.h					*
 *								*
 *	Protótipos das funções					*
 *								*
 *	Versão	1.0.0, de 19.09.92				*
 *		4.5.0, de 04.12.03				*
 *								*
 *	Módulo: AS-80386/486/PENTIUM				*
 *		Tradutores					*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

extern int	align (int);
extern int	get_function_code (void);
extern int	getescape (char *, char **);
extern int	getline (void);
extern int	analyse_sdi (EXP *, int, int, int);
extern int	putB (int);
extern int	putdouble (double);
extern int	putextd (double);
extern int	putfloat (double);
extern int	putL (ulong);
extern int	putW (int);
extern int	putbigW (int);
extern long	getbitfield (void);
extern long	labelupdate (char, long);
extern long	writesymtb (void);
extern long	writelnotb (void);
extern long	writedbtb (void);
extern int	invert (int);
extern void	defspace (int);
extern void	explst1 (int);
extern void	explst2 (int);
extern void	SDIs_metabolism (void);
extern void	finishcode (void);
extern void	help (int);
extern void	ifpseudo (void);
extern void	initcode (void);
extern void	initlist (void);
extern void	listsym (void);
extern void	make_pre_define (const char *);
extern void	pass1 (void);
extern void	pass2 (void);
extern void	prefix_analysis (int, int);
extern void	printline (void);
extern void	prntcode (int, int);
extern void	prntbigcode (int, int);
extern void	prntln (void);
extern void	prntloc (long);
extern void	prntvalue (unsigned);
extern void	pseudo1 (ulong);
extern void	pseudo2 (ulong);
extern void	putout (const void *, int);
extern void	quit (int);
extern void	sdistat (void);
extern void	symnumber (void);
extern void	msg (int, int, char *, ...);
extern void	putmark (int, char *, char *);
extern int	reglist (EXP *);
extern int	reloc (EXP *, int);
extern int	reloc_rel (EXP *, int, int);
extern long	checkrexp (EXP *, int, int);
extern long	expression (EXP	*);
extern void	checkexp (EXP *, long, long);
extern void	nonabsexp (EXP *);
extern void	nonrealexp (EXP *);
extern SYMTB	*id_hash (const char *, int, SYMTB ***);
extern void	if_stack_check (void);
extern SYMTB	*putsym (const char *, int, int);
extern void	put_jmp_sdi (long, SYMTB *, int, int);
extern void	put_org_sdi (long);
extern void	put_align_sdi (long, int, int);
extern void	testphase (void);
extern OPCODE	*op_hash (const char *);
extern int	regular1 (OPCODE *);
extern int	regular2 (OPCODE *);
extern int	putopnd (OPND *, int);
extern int	get_opnd_sz (OPND *, int);
extern long	getoperand (OPND *);
extern ulong	get_base_index (OPND *);
extern int	sdicode (OPND *, ushort, ushort);
extern MAJOR	gettoken (void);
extern MAJOR	getstr (void);
extern MAJOR	getnum (void);
extern MAJOR	getreal (void);
extern MAJOR	getid (void);
extern MAJOR	getch (void);
extern int	get_code_sz (OPADDR *, OPND *, OPND *);
extern int	needed_immed_set (int);
extern int	needed_immed_tst (int);
extern int	needed_disp_set (int);
extern int	needed_disp_tst (int);
extern void	pseudo_if (int);
extern void	pseudo_ifdef (int);
extern void	pseudo_elif (int);
extern void	pseudo_eldef (int);
extern void	pseudo_else (void);
extern void	pseudo_endif (void);
extern int	putcode (OPADDR *, OPND *, OPND *);
extern LNOTB	*putlno (void);
extern DBTB	*putdbtb (void);
