/*
 ****************************************************************
 *								*
 *			<pwd.h>					*
 *								*
 *	Formato do arquivo de senhas				*
 *								*
 *	Versão	1.0.0, de 28.10.86				*
 *		3.0.0, de 07.07.95				*
 *								*
 *	Módulo: pwd.h						*
 *		/usr/include					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 1995 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#define	PWD_H

/*
 ******	Definições universais ***********************************
 */
#define	NULL	 0
#define	NOPASSWD (PASSWD *)NULL

/*
 ******	Estrutura "PASSWD" **************************************
 */
typedef struct	passwd	PASSWD;

struct	passwd
{
	char	*pw_name;	/* Nome do Usuario */
	char	*pw_passwd;	/* Senha Encriptada */
	char	*pw_comment;	/* Comentario */
	int	pw_uid;		/* No. da Conta */
	int	pw_gid;		/* No. do Grupo */
	long	pw_quota;	/* Limite de Blocos em Disco */
	time_t	pw_expir;	/* Data de Expiracao */
	char	*pw_lock;	/* Tranca */
	char	*pw_dir;	/* Diretorio Inicial */
	char	*pw_shell;	/* Programa para usar como "sh" */
};

/*
 ******	Protótipos de funções ***********************************
 */
extern PASSWD	*getpwent (void);
extern void	setpwent (void);
extern void	endpwent (void);
extern char	*getlogin (void);
extern char	*getpass (const char *);
extern PASSWD	*getpwuid (int);
extern PASSWD	*getpwnam (const char *);
extern char	*pwcache (int);
extern off_t	getpwpos (void);
extern int	getpwslot (void);
