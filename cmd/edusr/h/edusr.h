/*
 ****************************************************************
 *								*
 *			edusr.h					*
 *								*
 *	Gerenciador de contas					*
 *								*
 *	Versão	1.5.0, de 04.06.84				*
 *		3.0.0, de 30.07.97				*
 *								*
 *	Módulo: edusr						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

/*
 ******	Tamanhos ************************************************
 */
#define	NIG	20		/* UID e GID mínimos >= NIG */
#define	NSPD	86400		/* Número de segundos em um dia */
#define	PWSZ	13		/* No. de caracteres da senha */

/*
 ******	Variáveis globais ***************************************
 */
extern const char	pgversion[];	/* Versão do programa */

extern int		exit_code;	/* Código de retorno */

extern int		vflag;		/* Verbose */

extern int		stdin_is_a_tty;	/* Não zero se interativo */

extern const char	no_pwd[];	/* Senha nula */

extern char		root_pwd[];	/* Para a senha do "root" */

extern int		hoje;		/* A data de hoje (decimal) */

extern const char	std_shell[];	/* SHELL padrão */

extern const char	*pwd_file_nm;	/* Nome do arquivo pwd */

extern const char	*group_file_nm;	/* Nome do arquivo group */

extern const char	lock_file_nm[];	/* Tranca do arquivo de senhas */

extern char		mbox_nm[];	/* Caixa postal */
extern char		*mbox_nm_ptr;

/*
 ******	Valores correntes****************************************
 */
extern int		current_group_id;	/* GID */
extern char		current_group_nm[];	/* Nome do Grupo */
extern int		current_quota;		/* Cota */
extern int		current_expir_date;	/* Data de expiração */
extern char		current_home[];		/* Diretório HOME */
extern char		current_shell[];	/* Interpretador */

/*
 ****** Indicadores do comandos	*********************************
 */
extern int		cmd_iflag;	/* Interativo */
extern int		cmd_vflag;	/* Verboso */
extern int		cmd_fflag;	/* Força */
extern int		cmd_lflag;	/* Longo */
extern int		cmd_dflag;	/* Debug */
extern int		cmd_dotflag;	/* Não ignora ".old" */
extern int		cmd_cflag;	/* Comparação */
extern int		cmd_uflag;	/* Atualização */
extern int		cmd_aflag;	/* Todas as entradas */
extern int		cmd_sflag;	/* Somente os totais */

/*
 ****** Variáveis de "cmd_getopt" *******************************
 */
extern const char	*cmd_nm;	/* Nome do comando em execução */
extern const char	*cmd_optarg; 	/* Argumento de uma opção */
extern int		cmd_optind;	/* Indice do argumento apos as opções */
extern int		cmd_optinit;	/* Na próxima chamada, inicializa */

/*
 ****** Protótipos de funções ***********************************
 */
extern int		cmd_getopt (const char *[], const char *);
extern void		copyfile (FILE *, const char *);
extern char		*env_var_scan (char *, char *);
extern void		error (const char *, ...);
extern void		exec_cmd (const char *);
extern void		exec_local_cmd (char *);
extern void		help (void);
extern int		reset (void);
extern void		print_hist (void);
extern const char	*plural (int);
extern int		exec_hist (const char *, int, char *);
extern void		edit_hist (void);
extern void		on_intr (int, ...);
extern void		do_help (int, const char *[]);
extern void		quit (void);

extern void		do_mkgrp_current (void);
extern void		do_expir_current (void);
extern void		do_cota_current (void);
extern void		do_shell_current (void);
extern void		do_home_current (void);

extern void		do_cota (int, const char *[]);
extern void		do_expir (int, const char *[]);
extern void		do_home (int, const char *[]);
extern void		do_mkgrp (int, const char *[]);
extern void		do_mkusr (int, const char *[]);
extern void		do_prgrp (int, const char *[]);
extern void		do_prusr (int, const char *[]);
extern void		do_quit (int, const char *[]);
extern void		do_rmgrp (int, const char *[]);
extern void		do_rmusr (int, const char *[]);
extern void		do_set (int, const char *[]);
extern void		do_shell (int, const char *[]);

#ifdef	TIME_H
extern const char	*time_to_external_cv (time_t);
extern time_t		time_to_internal_cv (const char *);
#endif	TIME_H

#ifdef	PWD_H
extern void		putpwent (const PASSWD *, FILE *);
#endif	PWD_H

#ifdef	GRP_H
extern void		putgrent (const GROUP *, FILE *);
#endif	GRP_H
