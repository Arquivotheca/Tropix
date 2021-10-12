/*
 ****************************************************************
 *								*
 *			tcmpto_c.h				*
 *								*
 *	Comparação remota de árvores (cliente) 			*
 *								*
 *	Versão	2.3.0, de 28.10.91				*
 *		4.2.0, de 02.05.02				*
 *								*
 *	Módulo: TCMPTO_C/TNET					*
 *		Utilitários de comunicações			*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define	DEBUG			/* Gera ou não o código de depuração */

#define	NPAT	20		/* No. maximo de Padrões */

/*
 ****** Macros úteis ********************************************
 */
#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

#define MIN(a,b) (a < b ? a : b)

/*
 ****** Os diversos (e muitos) indicadores **********************
 */
extern int		fromflag;	/* Caminhada na árvore REMOTA */
extern int		cpflag;		/* Copie diretamente (sem comparação) */
extern int		pipeflag;	/* Pipe */

extern int		tflag,		/* copy flags */
			fflag,
			dflag;
	
extern int		Cflag;		/* Use compressão */
extern int		gflag;		/* Gere o "garfield" */
extern int		Lflag;		/* local = Não entre em diretórios */
extern int		iflag;		/* Iterativo */
extern int		vflag;		/* verbose flag */
extern int		qflag;		/* Comparação rápida de objetos */
extern int		mflag;		/* Copia também o estado do arquivo */
extern int		dotflag;	/* Não ignorar ".old" */
extern int		Nflag;		/* Nomes do "stdin" */

extern int		nflag;		/* Foi dado o nome do nó */
extern int		uflag;		/* Foi dado o nome do usuário remoto */
extern int		Dflag;		/* Foi dado o diretório remoto */

extern int		Zflag;		/* Do not execute Commands */
extern int		Bflag;		/* Debug */

extern const char	**color_vector; /* Para as cores dos modos */

/*
 ****** Estrutura de LINK ***************************************
 */
typedef struct litem	LITEM;

struct litem
{
	char		*l_name;	/* Nome do Link */
	dev_t		l_dev;		/* Dispositivo */
	ino_t		l_ino;		/* No. do Inode */
	LITEM		*l_next;	/* o Proximo */
};

extern LITEM		*local_linklist_first,	/* Começo da Lista */
			*local_linklist_last;	/* Final da Lista */

/*
 ****** Estrutura das operações *********************************
 */
typedef struct op_bl	OP_BL;

struct op_bl
{
	OP_NM		o_op;		/* Operação a realizar */
	char		*o_file_nm;	/* Nome do arquivo */
	char		*o_link_nm;	/* Nome do LINK */
	OP_BL		*o_next;	/* Ponteiro para o próximo */

};

/*
 ****** Variáveis globais ***************************************
 */
extern const char	*pgname;		/* Nome do programa */
extern const char	pgversion[];

extern int		tcp_fd;			/* Descritor do TCP */

extern int		snd_dis;		/* Disconecta em caso de erro (se != 0) */

extern char		local_node_nm[];	/* Nome do nó local */

extern const char	*remote_node_nm; 	/* Nome do nó remoto */

extern int		sigint_received;	/* Veio o sinal SIGINT */

extern const char	garfield[];		/* Nome do arquivo "garfield" */

extern FILE		*gar_fp;		/* Arquivo "garfield" */

extern FILE		*tty_fp;		/* Arquivo para "askyesno" */

extern int		todiff, fromdiff;	/* Nr. de Arquivos diferentes */

extern int		diff_files_cnt;		/* Contador de nr. de Arq. diferentes */

extern int		crc_diff;		/* Contador de CRCs diferentes */

extern int		ftw_file_cnt;		/* Contador de arquivos dados por "ftw" */

/*
 ******	Os padrões **********************************************
 */
extern const char	*inc_pat[NPAT+1];	/* Os Padrões de inclusão */
extern int		inc_pati;

extern const char	*exc_pat[NPAT+1];	/* Os Padrões de exclusão */
extern int		exc_pati;

/*
 ****** Protótipos de funções ***********************************
 */
extern void		help (void);

extern void		execute_op_list (void);
extern void		execute_cmp_list (void);
extern void		execute_pipe (const char **);

extern void		walk (const char *[]);
extern void		remote_walk (const char *);
extern int		name_analysis (const char *, STAT *, int);

extern int		file_cmp (const char *, STAT *);
extern void		crc_cmp (ushort[], ushort[], int, int);

extern int		save_op_info (const char *, const STAT *, const STAT *, OP_NM, ...);
extern void		print_interval (time_t);

extern void		time_cmp (time_t, time_t);

extern char		*local_link_proc (const char *, STAT *);
extern void		delete_local_link_list (void);

extern void		copy_to (const char *);
extern void		copy_from (const char *);
extern void		delete_remote_file (const char *);
extern void		delete_local_file (const char *);
extern void		link_remote_file (const char *, const char *);
extern void		link_local_file (const char *, const char *);

extern void		info (void);

extern void		t_snd_msg (int, const char *);
extern void		t_snd_msg_no_push (int, const char *);
extern int		t_rcv_msg (char **);
extern void		error (const char *, ...);
extern void		msg (const char *, ...);
extern char		*plural (int);
extern void		on_sigint (int, ...);
extern void		on_signal (int, ...);
extern void		tcp_abort (int);
extern void		quit (int);

extern void		print_file_nm (const char *, const STAT *, const char *, char *);
extern void		put_s_option (void);
extern int		put_p_option (const char *, const char *[], int *);
extern int		pattern_accept (const char *);

extern void		stat_endian_cv (STAT *, const STAT *);

#ifdef	A_OUT_H
extern void		header_endian_cv (HEADER *hp);
#endif	A_OUT_H
