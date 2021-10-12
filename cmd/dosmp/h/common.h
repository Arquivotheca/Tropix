/*
 ****************************************************************
 *								*
 *			common.h				*
 *								*
 *	Definições utilizadas em todos os módulos		*
 *								*
 *	Versão	3.0.0, de 28.10.93				*
 *		3.0.7, de 04.08.97				*
 *								*
 *	Módulo: DOSMP						*
 *		Utilitários Sofisticados			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 *		Inspirado no PLDOS de Norival Ribeiro Figueira	*
 *								*
 ****************************************************************
 */


/*
 ******	Definições úteis ****************************************
 */
#define	NULL	0
#define	NOSTR	(char *)NULL
#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

#define	MIN(a,b) ((a) < (b) ? (a) : (b))

/*
 ******	Tipos úteis *********************************************
 */
typedef unsigned int	bit;

/*
 ******	Definições das macros ***********************************
 */
#ifdef	LITTLE_ENDIAN

#define	GET_SHORT(cp)		(*(unsigned short *)(cp))
#define	GET_LONG(cp)		(*(unsigned long *)(cp))
#define	PUT_SHORT(n, cp) 	(*(unsigned short *)(cp) = n)
#define	PUT_LONG(n, cp) 	(*(unsigned long *)(cp) = n)

#else

#define	GET_SHORT(cp)		get_bad_short (cp)
#define	GET_LONG(cp)		get_bad_long (cp)
#define	PUT_SHORT(n, cp) 	put_bad_short (n, cp)
#define	PUT_LONG(n, cp) 	put_bad_long (n, cp)

extern int		get_bad_short (const char *);
extern long		get_bad_long (const char *);
extern void		put_bad_short (int, char *);
extern void		put_bad_long (long, char *);

#endif	LITTLE_ENDIAN

/*
 ******	Parâmetros do programa **********************************
 */
extern const char	pgversion[];	/* Versão */
extern int		exit_code;	/* Código de retorno */
extern const char	*sys_nm;	/* Nome do sistema operacional */
extern const char	**color_vector;	/* Para o vetor de cores */

/*
 ******	Indicadores do programa *********************************
 */
extern int		cflag;		/* Cria uma sistema de arquivos */
extern int		rflag;		/* Read only */
extern int		vflag;		/* Verbose */

/*
 ******	Textos comuns *******************************************
 */
extern const char	options_ifv[];	/* Textos comum em "cmd.c" */
extern const char	somewhere_expand[];
extern const char	num_dev_list[];
extern const char	num_dev_option[];

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
 ****** Variáveis dos padrões ***********************************
 */
#define	NPAT	20

extern const char	*inc_pat[NPAT+1];	/* Os Padrões de inclusão */
extern int		inc_pati;

extern const char	*exc_pat[NPAT+1];	/* Os Padrões de exclusão */
extern int		exc_pati;

/*
 ****** Variáveis de entrada/saída ******************************
 */
extern char		dev_nm[64];	/* Nome do dispositivo */
extern long		dev_mode;	/* Modo do dispositivo */
extern int		dev_fd;		/* Descritor do dispositivo */
extern int		dev_dev;	/* Major + minor do dispositivo */
extern int		dev_type;	/* Tipo da partição */
extern int		dev_ro;		/* Detetada proteção de escrita */

extern char		vol_nm[];	/* Nome do volume */
extern char		stdin_is_a_tty;	/* Estamos lendo do terminal */

/*
 ******	Estado do sistema de arquivos DOS ***********************
 */
typedef enum
{
	FS_CLOSED = 0,		/* Fechado */
	FS_OPEN,		/* Aberto o dispositivo, mas sem DOS válido */
	FS_MOUNT_RO,		/* DOS OK, pronto para leituras  */
	FS_MOUNT_RW		/* DOS OK, pronto para leituras e escritas */

}	FS_STATUS;

extern FS_STATUS	fs_status;	/* Estado atual */

/*
 ******	Quasi-constantes do tempo *******************************
 */
extern time_t		HOJE;		/* Início do dia de hoje (GMT) */
extern time_t		AMANHA;		/* Início do dia de amanhã (GMT) */
extern time_t		SEMANA;		/* Início da semana (GMT) */

/*
 ******	Dados da raiz *******************************************
 */
extern int		root_dev;	/* Dispositivo da raiz */
extern int		root_ino;	/* No. do inode da raiz */

/*
 ****** Variáveis de "cmd_getopt" *******************************
 */
extern const char	*cmd_nm;	/* Nome do comando em execução */
extern const char	*cmd_optarg; 	/* Argumento de uma opção */
extern int		cmd_optind;	/* Indice do argumento apos as opções */
extern int		cmd_optinit;	/* Na próxima chamada, inicializa */

/*
 ****** Variáveis de "signal" ***********************************
 */
extern int		received_intr;	/* Veio um <^C> */
extern int		ignore_intr;	/* Diferente de zero, ignora <^C> */

/*
 ******	Outras variáveis ****************************************
 */
extern int		cwd_cluster;	/* Diretório corrente */
extern int		bin_mode;	/* Modo binário ao invés ISO */
#define iso_mode (!bin_mode)		/* Modo ISO */
extern int		lfn_mode;	/* == 0 => 8.3 * != 0 => 255 (W 95) */
