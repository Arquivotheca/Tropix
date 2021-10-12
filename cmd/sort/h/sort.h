/*
 ****************************************************************
 *								*
 *			sort.h					*
 *								*
 *	Ordena e/ou intercala arquivos				*
 *								*
 *	Versão	1.0.0, de 03.10.86				*
 *		3.0.0, de 05.07.97				*
 *								*
 *	Módulo: sort						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ****** Definições universais ***********************************
 */
#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

/*
 ****** Estrutura da chave **************************************
 */
typedef	struct
{
	int	k_index;	/* No. do campo */
	int	k_begin;	/* Início do campo */
	int	k_size;		/* Tamanho do campo */
	char	k_up_down;	/* Ascendente/descendente */
	char	k_code;		/* Código de caracteres */

}	KEY;

extern KEY	key_vec[];	/* Vetor contendo as chaves dadas */

/*
 ****** Indicadores *********************************************
 */
extern int	vflag;		/* Verbose */
extern int	mflag;		/* Intercalação */
extern int	cflag;		/* Verifica se já está ordenado */
extern int	tflag;		/* Código de caracteres */
extern int	rflag;		/* Ordenação reversa */

/*
 ****** Variáveis globais/externas ******************************
 */
extern char	delim;		/* Delimitador entre as chaves */
extern int	numkeys;	/* No. de chaves dadas */
extern FILE	*out_fp;	/* Arquivo de saída */

extern const char *pgname;

/*
 ****** Protótipos de funções ***********************************
 */
extern void	help (void);
extern void	sort (const char *);
extern void	get_keys (const char *);
extern void	set_cmp_function (void);
extern void	merge (const char *[]);

extern int	cmp_fold_up (const char *, const char *);
extern int	cmp_fold_down (const char *, const char *);
extern int	cmp_dic_up (const char *, const char *);
extern int	cmp_dic_down (const char *, const char *);
extern int	cmp_key (const char *, const char *);

extern int	(*compar) (const void *, const void *);
