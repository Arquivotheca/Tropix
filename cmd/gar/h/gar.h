/*
 ****************************************************************
 *								*
 *			gar.h					*
 *								*
 *	Header do Formato GAR					*
 *								*
 *	Versão	1.1.0, de 11.01.88				*
 *		4.4.0, de 19.03.03				*
 *								*
 *	Módulo: GAR						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */


/*
 ****************************************************************
 *	Definições Diversas					*
 ****************************************************************
 */
#define	PATHSZ	512	/* Tamanho Maximo de um Nome */

#define	ROUND(i,j)	((((i) + (j) - 1) / (j)) * (j))
#define	BLROUND(i)	((i + BLSZ-1) & ~(BLSZ-1))
#define	WDROUND(i)	((i + 1) & ~1)

/*
 ****************************************************************
 *	Estrutura do Header do Volume				*
 ****************************************************************
 */
typedef	struct
{
	char	g_magic[12];	/* Cadeia magica */
	char	g_date[12];	/* Data de criação do arquivo */
	char	g_vol[6];	/* No. do Volume */
	char	g_version[9];	/* Versão do GAR */
	char	g_vres[13];	/* Reservado */
	char	g_vcrc[8];	/* CRC */

}	GVOL;

#define	GVOLCRCSZ	(sizeof (GVOL) - 8)

#define	GARMSZ	12

extern const char	garvolume[];
extern const char	garheader[];
extern const char	garlink[];
extern const char	gartrailer[];

/*
 ****************************************************************
 *	Estrutura do Cabeçalho de Arquivo			*
 ****************************************************************
 */
typedef	struct		/* Antigo (até 2002) */
{
	char	g_magic[12];	/* Cadeia magica */
	char	g_dev[6];	/* Dispositivo */
	char	g_ino[6];	/* No. do Inode */
	char	g_mode[12];	/* Modo do arquivo */
	char	g_nlink[6];	/* No. de Links */
	char	g_uid[6];	/* Dono */
	char	g_gid[6];	/* Grupo */
	char	g_rdev[6];	/* Dispositivo Especial */
	char	g_size[12];	/* Tamanho */
	char	g_atime[12];	/* tempo de acesso */
	char	g_mtime[12];	/* tempo de modificação */
	char	g_ctime[12];	/* tempo de criação */
	char	g_namesz[6];	/* Tamanho do nome */
	char	g_linkflag[2];	/* Este nome é um Link */
	char	g_origsz[12];	/* Tamanho original, caso comprimido */
	char	g_bits[3];	/* No. de bits (LZW), nível (ZIP) */
	char	g_compcrc[2];	/* Compressão com CRC */
	char	g_method[3];	/* Método de compressão "C" (LZW), "D" (ZIP) */
	char	g_res[16];	/* Reservado para uso futuro */
	char	g_crc[8];	/* CRC */

}	OGHEAD;

#define	OGHEADCRCSZ	(sizeof (OGHEAD) - 8)

typedef	struct		/* Novo (a partir de 2003) */
{
	char	g_magic[12];	/* Cadeia magica */
	char	g_dev[12];	/* Dispositivo */
	char	g_ino[12];	/* No. do Inode */
	char	g_mode[12];	/* Modo do arquivo */
	char	g_nlink[6];	/* No. de Links */
	char	g_uid[12];	/* Dono */
	char	g_gid[12];	/* Grupo */
	char	g_rdev[12];	/* Dispositivo Especial */
	char	g_size[12];	/* Tamanho */
	char	g_atime[12];	/* tempo de acesso */
	char	g_mtime[12];	/* tempo de modificação */
	char	g_ctime[12];	/* tempo de criação */
	char	g_namesz[6];	/* Tamanho do nome */
	char	g_linkflag[2];	/* Este nome é um Link */
	char	g_origsz[12];	/* Tamanho original, caso comprimido */
	char	g_bits[3];	/* No. de bits (LZW), nível (ZIP) */
	char	g_compcrc[2];	/* Compressão com CRC */
	char	g_method[3];	/* Método de compressão "C" (LZW), "D" (ZIP) */
	char	g_res[16];	/* Reservado para uso futuro */
	char	g_crc[8];	/* CRC */

}	GHEAD;

#define	GHEADCRCSZ	(sizeof (GHEAD) - 8)

/*
 ****************************************************************
 *	Formato Interno (Igual para GAR, TAR, CPIO,.....)	*
 ****************************************************************
 */
typedef	struct
{
	STAT	k_stat;		/* O estado */
#if (0)	/*******************************************************/
	dev_t	k_dev;		/* Dispositivo */
	ino_t	k_ino;		/* No. do Inode */
	long	k_mode;		/* Tipo do arquivo */
	int	k_nlink;	/* No. de Links */
	ushort	k_uid;		/* Dono */
	ushort	k_gid;		/* Grupo */
	dev_t	k_rdev;		/* Dispositivo Especial */
	off_t	k_size;		/* Tamanho */
	time_t	k_atime;	/* tempo de acesso */
	time_t	k_mtime;	/* tempo de modificação */
	time_t	k_ctime;	/* tempo de criação */
#endif	/*******************************************************/
	char	k_linkflag;	/* Este nome é um Link */
	off_t	k_origsz;	/* Tamanho original, caso comprimido */
	char	k_bits;		/* No. de bits usados na compressão */
	char	k_compcrc;	/* Compressão com CRC */
	char	k_method;	/* Método de compressão ('C' ou 'D') */
	char	k_variant;	/* Variante do método de compressão */
	char	k_name[PATHSZ]; /* Nome */
	char	k_linkname[PATHSZ]; /* Nome do link */

}	KHEAD;

extern KHEAD	khead;		/* Header interno */

/*
 ******	Abreviações úteis ***************************************
 */
#define	k_dev	k_stat.st_dev
#define	k_ino	k_stat.st_ino
#define	k_mode	k_stat.st_mode
#define	k_nlink	k_stat.st_nlink
#define	k_uid	k_stat.st_uid
#define	k_gid	k_stat.st_gid
#define	k_rdev	k_stat.st_rdev
#define	k_size	k_stat.st_size
#define	k_atime	k_stat.st_atime
#define	k_mtime	k_stat.st_mtime
#define	k_ctime	k_stat.st_ctime

/*
 ****************************************************************
 *	Buffer de entrada/saída					*
 ****************************************************************
 */
typedef struct
{
	GVOL	convol;			/* Continuação do volume */
	char	buf[1];			/* Leitura/escrita da coleção */
#if (0)	/*******************************************************/
	char	buf[NBMAX*BLSZ];	/* Leitura/escrita da coleção */
#endif	/*******************************************************/

}	BUF;

extern BUF	*b;		/* Continuação + leitura/escrita da coleção */

/*
 ****************************************************************
 *	Funções	e Enums						*
 ****************************************************************
 */
typedef	enum
{
	NOFTN,
	IN,		/* Leitura da Fita */
	OUT		/* Escrita na Fita */

}	FUNC;

typedef	enum
{
	NOMODIF,	
	CREATE,		/* Cria os Arquivos */
	TAB,		/* Tabela do Conteudo */
	COMP,		/* Comparação */
	PIPE,		/* Pipe */
	APPEND		/* Acrecente no final */

}	MODIF;

extern MODIF	modif;	/* Modificador */

typedef	enum
{
	GAR,		/* Fita GAR */
	CPIO,		/* Fita CPIO */
	CPIOR,		/* Fita CPIO com bytes invertidos */
	TAR		/* Fita TAR */

}	TYPE;

extern TYPE	type;		/* Tipo da Coleção */

/*
 ****************************************************************
 *	Protótipos						*
 ****************************************************************
 */
extern void		add_file_mode (const char *);
extern void		append (const char **);
extern int		gar_askyesno (int);
extern int		bback (int, int);
extern void		bflush (void);
extern int		cpiotok (KHEAD *);
extern int		file_rew (void);
extern void		garheader_search (void);
extern void		tarheader_search (void);
extern int		gartok (KHEAD *);
extern int		getheader (void);
extern char		*get_tmp_nm (int);
extern void		help (void);
extern void		in (void);
extern int		inblocks (int);
extern void		inskip (int);
extern char		*link_proc (const char *, STAT *);
extern int		mkfile (const char *);
extern int		newdev (FUNC, int);
extern int		otoi (const char *);
extern void		out (const char *[]);
extern void		out_tar (const char *, STAT *, const char *);
extern void		outblocks (const char *, STAT *);
extern int		outfile (const char *, STAT *, int);
extern int		pattern_accept (const char *file_nm);
extern void		prein (void);
extern char		*print_interval (time_t);
extern void		procfile (void);
extern void		proc_mode_list (void);
extern void		putgfile (void);
extern void		putnl (void);
extern void		put_tabs (const char *, FILE *);
extern void		quit (int);
extern void		red_block_cmp (char *, int);
extern void		red_block_write (char *, int);
extern int		red_obj_cmp (char *, char *);
extern void		set_file_mode (const char *, const KHEAD *);
extern int		tartok (KHEAD *);
extern void		tbline (void);
extern void		time_cmp (time_t, time_t);
extern TYPE		typeid (void);
extern int		verdir (char *);
extern void		volhwrite (void);
extern void		volin (void);
extern void		volout (void);
