/*
 ****************************************************************
 *								*
 *			<sys/stat.h>				*
 *								*
 *	Estrutura retornada pela chamada ao sistema "stat"	*
 *								*
 *	Versão	3.0.0, de 01.09.94				*
 *		4.8.0, de 26.08.05				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *		/usr/include/sys				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2005 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#define	STAT_H			/* Para definir os protótipos */

/*
 ******	A estrutura *********************************************
 */
typedef	struct	stat	STAT;

struct	stat
{
	dev_t	st_dev;		/* Dispositivo de residencia */
	ino_t	st_ino;		/* No. do Inode */
	long	st_mode;	/* Modo e Tipo do Arquivo */
	int	st_nlink;	/* No. de Ponteiros para este Arquivo */
	ushort 	st_uid;		/* Dono  do Arquivo */
	ushort 	st_gid;		/* Grupo do Arquivo */
	dev_t	st_rdev;	/* Dispositivo (BLK e CHR) */
	off_t	st_size;	/* Tamanho do Arquivo */
	time_t	st_atime;	/* Tempo de Acesso */
	time_t	st_mtime;	/* Tempo de Modificação */
	time_t	st_ctime;	/* Tempo de Criação */
};

/*
 ******	Modos e tipos *******************************************
 */
#define	S_7	 0x800000
#define	S_6	 0x400000
#define	S_5	 0x200000
#define	S_4	 0x100000
#define	S_3	 0x080000
#define	S_2	 0x040000
#define	S_1	 0x020000
#define	S_IMETX	 0x010000	/* Salva a imagem do texto na memória */

#define	S_IFMT	 0x00F000	/* Tipo do Arquivo */
#define     S_IFLNK   0x00A000	/* "Link" simbólico */
#define     S_IFREG   0x008000	/* Regular */
#define	    S_IFDIR   0x004000	/* Diretorio */
#define     S_IFIFO   0x001000	/* FIFO */
#define     S_IFBLK   0x006000	/* Bloco */
#define	    S_IFCHR   0x002000	/* Caracter */
#define	S_ISUID	 0x000800	/* "Set user  id" na execução */
#define	S_ISGID	 0x000400	/* "Set group id" na execução */
#define S_ISVTX	 0x000200	/* Salva a imagem do texto no SWAP */
#define	S_IREAD	 0x000100	/* Permissão de leitura do dono */
#define	S_IWRITE 0x000080	/* Permissão de escrita do dono */
#define	S_IEXEC	 0x000040	/* Permissão de execução do dono */

/*
 ******	Macros do POSIX *****************************************
 */
#define	S_ISLNK(m)	(((m) & S_IFMT) == S_IFLNK)
#define	S_ISREG(m)	(((m) & S_IFMT) == S_IFREG)
#define	S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)
#define	S_ISFIFO(m)	(((m) & S_IFMT) == S_IFIFO)
#define	S_ISCHR(m)	(((m) & S_IFMT) == S_IFCHR)
#define	S_ISBLK(m)	(((m) & S_IFMT) == S_IFBLK)

#define	S_IRWXU		0x0001C0	/* Permissão de RWX do dono */
#define	S_IRUSR		0x000100	/* Permissão de leitura do dono */
#define	S_IWUSR		0x000080	/* Permissão de escrita do dono */
#define	S_IXUSR		0x000040	/* Permissão de execução do dono */

#define	S_IRWXG		0x000038	/* Permissão de RWX do grupo */
#define	S_IRGRP		0x000020	/* Permissão de leitura do grupo */
#define	S_IWGRP		0x000010	/* Permissão de escrita do grupo */
#define	S_IXGRP		0x000008	/* Permissão de execução do grupo */

#define	S_IRWXO		0x000007	/* Permissão de RWX dos outros */
#define	S_IROTH		0x000004	/* Permissão de leitura dos outros */
#define	S_IWOTH		0x000002	/* Permissão de escrita dos outros */
#define	S_IXOTH		0x000001	/* Permissão de execução dos outros */

#define	S_IRWXA		(S_IRWXU|S_IRWXG|S_IRWXO) /* Permissão RWX de todos */

/*
 ******	Protótipos de funções ***********************************
 */
extern int	stat (const char *, STAT *); 
extern int	lstat (const char *, STAT *); 
extern int	fstat (int, STAT *); 
extern int	instat (int, int, STAT *); 
extern int	upstat (int, STAT *); 
