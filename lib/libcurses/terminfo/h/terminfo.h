/*
 ****************************************************************
 *								*
 *			terminfo.h				*
 *								*
 *	Definições sobre as descrições de terminais		*
 *								*
 *	Versão	2.0.0, de 21.04.87				*
 *		3.0.0, de 21.03.95				*
 *								*
 *	Módulo: terminfo.h					*
 *		libcurses/terminfo				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 1995 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 *	Se introduzir/remover alguma capacidade, alterar também em 
 *
 *		"terminfo/capnames/capnames.c"
 *
 */

#define	H_TERMINFO	/* Para declarar as funções */

/*
 ******	Estrutura INFO ******************************************
 */
typedef	struct
{
	char	**i_strings;
	short	*i_numbers;
	char	*i_booleans;

	short	i_boolno;
	short	i_numno;
	short	i_strno;

	short	i_tabsz;
	char	*i_table;

	char	*i_names;

}	INFO;

/*
 ******	Capacidades booleanas ***********************************
 */
typedef	enum
{
	b_bw,
	b_am,
	b_xsb,
	b_xhp,
	b_xenl,
	b_eo,
	b_gn,
	b_hc,
	b_chts,
	b_km,
	b_hs,
	b_in,
	b_da,
	b_db,
	b_mir,
	b_msgr,
	b_nxon,
	b_nrrmc,
	b_npc,
	b_os,
	b_mc5i,
	b_eslok,
	b_xt,
	b_hz,
	b_ul,
	b_xon

}	BOOLEANS;

/*
 ******	Capacidades numéricas ***********************************
 */
typedef	enum
{
	n_cols,
	n_it,
	n_lh,
	n_lw,
	n_lines,
	n_lm,
	n_xmc,
	n_nlab,
	n_pb,
	n_vt,
	n_wsl

}	NUMBERS;

/*
 ******	Capacidades de cadeias de caracteres ********************
 */
typedef	enum
{
	s_ac,
	s_acsc,
	s_cbt,
	s_bel,
	s_cr,
	s_rmp,
	s_tbc,
	s_mgc,
	s_clear,
	s_el1,
	s_el,
	s_ed,
	s_cmdch,
	s_cud1,
	s_home,
	s_civis,
	s_cub1,
	s_cnorm,
	s_cuf1,
	s_ll,
	s_cuu1,
	s_cvvis,
	s_dch1,
	s_dl1,
	s_dsl,
	s_hd,
	s_enacs,
	s_smacs,
	s_smam,
	s_blink,
	s_bold,
	s_smcup,
	s_smdc,
	s_dim,
	s_smir,
	s_prot,
	s_rev,
	s_invis,
	s_smso,
	s_smul,
	s_smxon,
	s_rmacs,
	s_rmam,
	s_sgr0,
	s_rmcup,
	s_rmdc,
	s_rmir,
	s_rmso,
	s_rmul,
	s_rmxon,
	s_flash,
	s_ff,
	s_fsl,
	s_is1,
	s_is2,
	s_is3,
	s_if,
	s_iprog,
	s_ich1,
	s_il1,
	s_ip,
	s_ka1,
	s_ka3,
	s_kb2,
	s_kbs,
	s_kbeg,
	s_kcbt,
	s_kc1,
	s_kc3,
	s_kcan,
	s_ktbc,
	s_kclr,
	s_kclo,
	s_kcmd,
	s_kcpy,
	s_kcrt,
	s_kctab,
	s_kdch1,
	s_kdl1,
	s_kcud1,
	s_krmir,
	s_kend,
	s_kent,
	s_kel,
	s_ked,
	s_kext,
	s_kf0,
	s_kf1,
	s_kf2,
	s_kf3,
	s_kf4,
	s_kf5,
	s_kf6,
	s_kf7,
	s_kf8,
	s_kf9,
/**************************************/
	s_kf10,
	s_kf11,
	s_kf12,
	s_kf13,
	s_kf14,
	s_kf15,
	s_kf16,
	s_kf17,
	s_kf18,
	s_kf19,
	s_kf20,
	s_kf21,
	s_kf22,
	s_kf23,
	s_kf24,
	s_kf25,
	s_kf26,
	s_kf27,
	s_kf28,
	s_kf29,
	s_kf30,
	s_kf31,
	s_kf32,
	s_kf33,
	s_kf34,
	s_kf35,
	s_kf36,
	s_kf37,
	s_kf38,
	s_kf39,
	s_kf40,
	s_kf41,
	s_kf42,
	s_kf43,
	s_kf44,
	s_kf45,
	s_kf46,
	s_kf47,
	s_kf48,
	s_kf49,
	s_kf50,
	s_kf51,
	s_kf52,
	s_kf53,
	s_kf54,
	s_kf55,
	s_kf56,
	s_kf57,
	s_kf58,
	s_kf59,
	s_kf60,
	s_kf61,
	s_kf62,
	s_kf63,
/**************************************/
	s_kfnd,
	s_khlp,
	s_khome,
	s_kich1,
	s_kil1,
	s_kcub1,
	s_kll,
	s_kmrk,
	s_kmsg,
	s_kmov,
	s_knxt,
	s_knp,
	s_kopn,
	s_kopt,
	s_kpp,
	s_kprv,
	s_kprt,
	s_krdo,
	s_kref,
	s_krfr,
	s_krpl,
	s_krst,
	s_kres,
	s_kcuf1,
	s_ksav,
/**************************************/
	s_kBEG,
	s_kCAN,
	s_kCMD,
	s_kCPY,
	s_kCRT,
	s_kDC,
	s_kDL,
	s_kslt,
	s_kEND,
	s_kEOL,
	s_kEXT,
	s_kind,
	s_kFND,
	s_kHLP,
	s_kHOM,
	s_kIC,
	s_kLFT,
	s_kMSG,
	s_kMOV,
	s_kNXT,
	s_kOPT,
	s_kPRV,
	s_kPRT,
	s_kri,
	s_kRDO,
	s_kRPL,
	s_kRIT,
	s_kRES,
	s_kSAV,
	s_kSPD,
	s_khts,
	s_kUND,
	s_kspd,
	s_kund,
/**************************************/
	s_kcuu1,
	s_rmkx,
	s_smkx,
	s_lf0,
	s_lf1,
	s_lf3,
	s_lf4,
	s_lf5,
	s_lf6,
	s_lf7,
	s_lf8,
	s_lf9,
	s_lf10,
	s_rmln,
	s_smln,
	s_rmm,
	s_smm,
	s_nel,
	s_pad,
	s_mc0,
	s_mc5p,
	s_mc4,
	s_mc5,
	s_rfi,
	s_rs1,
	s_rs2,
	s_rs3,
	s_rf,
	s_rc,
	s_sc,
	s_ind,
	s_ri,
	s_smgl,
	s_smgr,
	s_hts,
	s_ht,
	s_uc,
	s_hu,
	s_xoffc,
	s_xonc,
	s_null,

	s_csr,
	s_hpa,
	s_cup,
	s_mrcup,
	s_ech,
	s_dch,
	s_dl,
	s_cud,
	s_ich,
	s_indn,
	s_il,
	s_cub,
	s_cuf,
	s_rin,
	s_cuu,
	s_pfkey,
	s_pfloc,
	s_pfx,
	s_pln,
	s_rep,
	s_vpa,
	s_sgr,
	s_wind,
	s_tsl

}	STRINGS;

/*
 ******	Classes das capacidades *********************************
 */
typedef	enum
{
	C_UNDEF,
	C_BOOLEAN,	/* Capacidade Booleana */
	C_NUMBER,	/* Capacidade Numérica */
	C_STRING,	/* Capacidade String */
	C_PSTRING

}	CAPCLASS;

/*
 ******	Entrada da tabela de capacidades ************************
 */
typedef	struct
{
	char	 h_id[16];	/* Nome da Capacidade */
	CAPCLASS h_class;	/* Classe da Capacidade */
	int	 h_order;	/* Ordem da Capacidade  em sua Classe */

}	CAPHASH;

#define	NOCAPHASH (CAPHASH *)0

/*
 ******	Variáveis externas **************************************
 */
extern	int	BOOLNO;		/* No. de Capacidades Booleanas */
extern	int	NUMNO;		/* No. de Capacidades Numericas */
extern	int	STRNO;		/* No. de Capacidades String */

extern	char	*capbnames[];
extern	char	*capnnames[];
extern	char	*capsnames[];

/*
 ******	Protótipos de funções ***********************************
 */
extern int	getinfo (const char *, INFO *);
extern char 	*parmexec (const char *, ...);
extern int	putinfo (INFO *, char *);
extern CAPHASH	*caphash (const char *);
extern char	*parmcomp (char *);
extern char	*parmuncomp (char *);
