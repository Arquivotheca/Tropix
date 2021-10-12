/*
 ****************************************************************
 *								*
 *			mklib.h					*
 *								*
 *	Definições universais					*
 *								*
 *	Versão	1.0.0, de 06.10.86				*
 *		4.0.0, de 20.08.01				*
 *								*
 *	Módulo: MKLIB						*
 *		Utilitários Especiais				*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ******	Definições universais * *********************************
 */
#define	LIBMAGIC	0x11DE784A	/* No. mágico (velocidade da luz) */

#define	elif		else if
#define	EVER		;;

/*
 ****** Nulos das diversas estruturas ***************************
 */
#define	NOFUNC		((int (*) ()) NULL)

/*
 ******	Estrutura da lista de módulas ***************************
 */
typedef struct symtb	SYMTB;

typedef struct mod	MOD;

struct mod
{
	const char	*m_mod_nm;
	int		m_ino;
	SYMTB		*m_sym;
	int		m_n_sym;
	MOD		*m_next;
};

#define	NOMOD	(MOD *)NULL

extern MOD	*mod_first, *mod_last;

/*
 ******	Estrutura da tabela de símbolos *************************
 */
struct symtb
{
	const char	*z_sym_nm;
	int		z_sym_len;
	const MOD	*z_mod;
	SYMTB		*z_sym_next;
	SYMTB		*z_link;
};

#define	NOSYMTB	(SYMTB *)NULL

/*
 ******	Tamanho da tabela HASH **********************************
 */
#define	HASHSZ	1009		/* Tamanho da Tabela Hash */

/*
 ******	Variáveis globais ***************************************
 */
extern int		exit_code;	/* Código de retorno */

extern int		vflag;		/* Verbose */
extern int		fflag;		/* Atualiza, sem consultar */

extern int		(*command) (const char *[]); /* Função a executar */

/*
 ****** Nomes das bibliotecas ***********************************
 */
extern const char	*lib_dir_nm;	/* Nome da biblioteca */

extern int		lib_dir_nm_len; /* Comprimento do nome da biblioteca */

extern const char	lib_sym_nm[];	/* Normalmente == ".LIBSYM" */

extern char		*lib_sinop_nm;	/* Nome do arquivo de sinopse */

/*
 ****** Dados das bibliotecas ***********************************
 */
extern int		lib_dev;	/* Dispositivo da biblioteca */
extern char		lib_dev_nm[16];	/* Nome do dispositivo da biblioteca */

/*
 ****** Arquivo de sinopse **************************************
 */
extern int		n_lib_sym;	/* No. de entradas LIBSYM */
extern int		n_lib_mod;	/* No. de entradas LIBMOD */

/*
 ******	Tabela HASH de símbolos *********************************
 */
extern SYMTB		**hash_tb;	/* Endereço da tabela HASH */

/*
 ******	Protótipos de funções ***********************************
 */
extern int		do_consistency (const char *[]);
extern int		do_create (const char *[]);
extern int		do_delete (const char *[]);
extern int		do_replace (const char *[]);
extern int		do_table (const char *[]);
extern int		do_update (const char *[]);

extern void		hash_all_sym (void);
extern void		help (void);
extern const char	*last_nm (const char *);
extern MOD		*last_nm_mod_search (const char *);
extern void		list_mod (const MOD *);
extern int		mk_mod_sym (const char *, MOD *, SYM *, SYM *, int);
extern MOD		*mod_search (const char *);
extern int		compar_mod (const void *ve, const void *vd);
extern int		compar_sym (const void *ve, const void *vd);
extern SYMTB		*hash (const char *, int, SYMTB ***);
extern const char	*path_can (const char *);
extern void		read_sinop_file (int);
extern MOD		*sort_mod_list (MOD *first);
extern void		sort_mod_sym (MOD *mp);
extern void		sort_all_sym_lists (void);
extern void		write_sinop_file (void);

#ifdef	STAT_H
extern int		consist_ftw_function (const char *, STAT *, int);
extern int		ftw_create_function (const char *, STAT *, int);
extern int		mod_copy (const char *mod_nm, const char *mod_path, STAT *sp, HEADER *hp);
extern int		mod_insert (const char *, MOD *, const STAT *, HEADER *);
extern int		mod_replace (MOD *mp, STAT *sp, HEADER *hp, int cmd_nm);
#endif	STAT_H
