/*
 ****************************************************************
 *								*
 *			fsckt1.h				*
 *								*
 *	Definições universais					*
 *								*
 *	Versão	4.3.0, de 26.07.02				*
 *		4.3.0, de 16.08.02				*
 *								*
 *	Módulo: fsckt1						*
 *		Utilitários Especiais				*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ****************************************************************
 *	Definições Universais					*
 ****************************************************************
 */
#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

/*
 ******	Definições do sistema de arquivos ***********************
 */
#define	MAP_SZ(sz)	((((sz) >> 1) + BL4MASK) & ~BL4MASK)

/*
 ******	Definições relativas aos INODEs (16 estados) *************
 */
typedef enum
{
	NO_STATE,	/* INODE não alocado */
	CLR_STATE,	/* INODE deve ser removido */
	FILE_STATE,	/* INODE de um arquivo */
	DIR_STATE,	/* INODE de um diretorio */
	SEEN_DIR_STATE	/* INODE de um diretorio já caminhado */

}	INO_STATE;

typedef struct ino_hash	INO_HASH;

#define NO_INO_HASH	(INO_HASH *)0

struct ino_hash
{
	ino_t		i_ino;
	INO_STATE	i_state;
	int		i_mem_link;
	int		i_disk_link;
	INO_HASH	*i_next;
};

extern INO_HASH		**inode_hash_tb,  **link_place;

#define INO_HASH_SZ	(1 << 12)
#define INO_HASH_MASK	(INO_HASH_SZ - 1)

/*
 ******	Estrutura da caminhada por diretórios *******************
 */
typedef	struct dirinfo 	DIRINFO;

struct dirinfo
{
	INO_HASH *f_hash_ip;	/* Estrutura HASH dos INODEs */
	long	f_mode;		/* Modo do diretório */
	daddr_t	f_addr[T1_NADDR]; /* No. dos blocos */
	off_t	f_offset;	/* Deslocamento no diretório */
	ino_t	f_dir_ino;	/* No. do inode do diretório */
	ino_t	f_ent_ino;	/* No. do inode na entrada corrente */
	ino_t	f_data;		/* Informação herdada */
	int	f_path_len;	/* Comprimento do caminho completo */
	char	*f_path;	/* Caminho completo */
};

/*
 ******	Retorno das Rotinas *************************************
 */
typedef enum
{
	STOP	 = (1 << 0),	/* Pare o Processamento */
	SKIP	 = (1 << 1),	/* Ignore este passo */
	CONTINUE = (1 << 2),	/* Continue normalmente */
	ALTERED	 = (1 << 3)	/* Foi realizada a Alteração desejada */

}	SEQ;

/*
 ******	Definição de cache de UM elemento ***********************
 */
typedef struct
{
	daddr_t	b_bno;		/* Número do Bloco */
	int	b_dirty;	/* Buffer deve ser escrito */
	char	b_area[BL4SZ];	/* Conteúdo do Bloco */

}	CACHE;

#define	NOCACHE	(CACHE *)NULL

/*
 ******	Estrutura da tabela de blocos duplicados ****************
 */
typedef struct
{
	daddr_t	dup_bno;	/* No. do bloco duplicado */	
	int	dup_n;		/* No. de vezes que está duplicado */	

}	DUPBLK;

#define	NODUPBLK (DUPBLK *)NULL

/*
 ******	Variáveis globais ***************************************
 */
extern int		pflag;		/* Modo de Pânico */
extern int		fflag;		/* == 0: Pergunta; > 0: SIM; < 0: NÃO */ 
extern int		vflag;		/* Verbose */

extern const char	fstab_nm[];	/* O arquivo "fstab" */

extern jmp_buf		fs_env;		/* Para o caso de erro */

extern const char	*fs_nm;	/* Nome do FS sendo verificado */
extern dev_t		fs_dev;		/* O Dispositivo */

extern dev_t		root_dev;	/* Dispositivo Raiz */
extern int		dev_is_root;	/* O FS é a Raiz do Sistema */

extern int		read_only;	/* Não é permitido escrever no FS */

extern int		dev_fd;		/* Descritor para leitura escritas */

extern int		fs_modified;	/* FS foi modificado */

extern CACHE		sb;		/* Superbloco */
extern CACHE		inoblk;		/* Bloco com INODEs */
extern CACHE		fileblk;	/* Bloco de Dados de um Arquivo */

extern char 		*disk_block_map; /* Mapa de blocos alocados no disco */
extern char 		*mem_block_map; /* Mapa de blocos alocados na memória */

extern int 		fs_n_dup_blk;	/* Numero de blocos duplicados (por INODE) */

extern long		fs_file_block_sz; /* Tamanho do Arquivo Sendo Analisado */

extern DUPBLK		*dup_block_tb;	/* Endereço da tabela */
extern int		dup_block_elem;	/* No. de entradas totais no momento */
extern int		dup_block_sz;	/* No. de entradas ocupadas no momento */

extern INO_HASH		*lost_found_ip;	/* INODE do Diretório "lost+found" */

extern long		fs_busy_ino;	/* Numero Total de INODEs ocupados no FS */
extern long		fs_busy_bl;	/* Numero Total de blocos ocupados no FS */

/*
 ****** Protótipos de funções ***********************************
 */
extern void		help (void);
extern void		next_dev (int);
extern void		check (const char *);
extern int		get_dev (const char *);
extern T1DINO		*inode_get (ino_t);
extern void		*bget (CACHE *, daddr_t);
extern void		bflush (CACHE *);
extern char		*cv_blocks_to_KB_MB_or_GB (unsigned long);
extern void		bread (void *, daddr_t, int);
extern void		bwrite (const void *, daddr_t, int);
extern int		phase1 (void);
extern int		phase2 (void);
extern int		phase3 (void);
extern int		phase4 (void);
extern int		phase5 (void);
extern int		phase6 (void);
extern int		valid_file_type (int);
extern void		print_inode (ino_t, const T1DINO *);
extern int		faskyesno (const char *);
extern INO_HASH		*ino_hash (ino_t ino);
extern int		search_bad_or_dup_blocks (T1DINO *, INO_HASH *, daddr_t, int type);
extern int		search_dup_blocks (T1DINO *, INO_HASH *, daddr_t, int type);
extern int		file_block_scan (T1DINO *, INO_HASH *, int (*) (T1DINO *, INO_HASH *, daddr_t, int));

extern int		file_ind_block_scan
(	
	T1DINO *,
	INO_HASH *,
	daddr_t,
	int,
	int (*) (T1DINO *, INO_HASH *, daddr_t, int)
);

extern void		put_in_dup_block_tb (daddr_t);
extern int		dir_walk (DIRINFO *, int (*) (DIRINFO *, DIRINFO *, T1DIR *, T1DIR *));
extern int		dir_check_entry (DIRINFO *, DIRINFO *, T1DIR *, T1DIR *);
extern int		dir_block_walk (DIRINFO *, daddr_t, int (*) (DIRINFO *, DIRINFO *, T1DIR *, T1DIR *));
extern int		dir_ind_walk (DIRINFO *, daddr_t, int, int (*) ());
extern int		make_dot (DIRINFO *, DIRINFO *, T1DIR *, T1DIR *);
extern int		make_dot_dot (DIRINFO *, DIRINFO *, T1DIR *, T1DIR *);
extern void		dir_remove_entry (T1DIR *adp, T1DIR *dp);
extern int		find_lost_found (DIRINFO *, DIRINFO *, T1DIR *, T1DIR *);
extern int		find_parent (DIRINFO *, DIRINFO *, T1DIR *, T1DIR *);
extern int		link_up (INO_HASH *, INO_HASH *);
extern int		make_numerical_entry (DIRINFO *, DIRINFO *, T1DIR *, T1DIR *);
extern int		dir_change_parent (DIRINFO *, DIRINFO *, T1DIR *, T1DIR *);
extern void		adjust_dev_link (INO_HASH *);
extern void		inode_clr (INO_HASH *, const char *);
extern int		dereference_file_block (T1DINO *np, INO_HASH *ip, daddr_t bno, int type);
extern int		getpercent (long, long);
extern char		*editpercent (int);
extern void		free_all (void);
extern const char	*edit_block_type (int type);
extern void		inode_block_scan (daddr_t bno);
