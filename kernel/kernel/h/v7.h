/*
 ****************************************************************
 *								*
 *			<sys/v7.h>				*
 *								*
 *	Estruturas do sistema de arquivos V7			*
 *								*
 *	Versão	4.2.0, de 28.01.02				*
 *		4.2.0, de 28.01.02				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *		/usr/include/sys				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#define	V7_H			/* Para definir os protótipos */

/*
 ******	Definições globais **************************************
 */
#define	V7_ROOT_INO	((ino_t)2)	/* I-number da RAIZ */
#define	V7_SBNO		((daddr_t)1)	/* No. do bloco do SUPERBLOCO */

#define	V7_SBFDINO	100		/* No. de INODEs livres no superbloco */
#define	V7_SBFBLK	50		/* No. de Blocos livres no superbloco */

/*
 ******	Definições relativas a INODEs **************************
 */
#define	V7_INOSZ	64		/* Tamanho do INODE no disco */
#define	V7_IMASK	(V7_INOSZ-1)
#define	V7_ISHIFT	6
#define	V7_INOPBL	(BLSZ/V7_INOSZ)
#define V7_IOFFSET	15		/* Inodes perdidos no inicio do disco - 1 */

#define	V7_NDAINBL	(BLSZ/sizeof (daddr_t)) /* No. de End.'s em um bloco */
#define	V7_NSHIFT	7

/*
 ******	Conversões relativas a INODEs ***************************
 *
 */
#define	V7_INTODA(x)	(daddr_t)(((unsigned)(x) + V7_IOFFSET) >> (BLSHIFT-V7_ISHIFT))
#define	V7_INTODO(x)	(int)(((x) + V7_IOFFSET) & (BLSZ/V7_INOSZ-1))

/*
 ******	A estrutura do SUPER BLOCO da Versão 7 ******************
 */
struct	v7sb
{
	ushort	s_isize;	/* No. de Blocos da I-list */
	ushort	s_fsize[2];   	/* (== daddr_t) No. de Blocos do Sis. de Arq. */

	short  	s_nfblk;   	/* No. de Enderecos em s_fblk */
	daddr_t	s_fblk[V7_SBFBLK];	/* Lista de Blocos livres */

	short  	s_nfdino;  	/* No. de Inodes em s_fdino */
	ushort 	s_fdino[V7_SBFDINO]; /* Lista de Inode livres */

	LOCK   	s_flock;   	/* Lock para manipular a s_fblk */
	LOCK   	s_ilock;   	/* Lock para manipular a s_fdino */

	char	s_sbmod;	/* O SB foi modificado */
	char	s_ronly;	/* O FS é READONLY */

	ushort 	s_time[2];    	/* (== time_t) Ultima atualização do SB */
	ushort	s_tfblk[2];   	/* (== daddr_t) No. total de blocos livres */
	ushort 	s_tfdino;  	/* No. total de inodes livres */

	short  	s_m;       	/* interleave factor */
	short  	s_n;       	/* " " */

	char   	s_fname[16];	/* Nome do Sistema de Arquivos */
	char   	s_fpack[16];	/* Nome do Disco */

	long	s_magic;	/* No. magico para validação de SB */

	char	s_passwd[16];	/* Senha para montagem */

	LOCK	s_mlock;	/* Lock para montagem e contagem */
	char	s_mbusy;	/* Operação de Mount/Umount em Andamento */
	ushort	s_mcount;	/* No. Referencias a Arq. neste Dev */

	INODE	*s_inode;	/* Local de Montagem deste FS */
	V7SB	*s_next;	/* Lista de SB's */
	BHEAD	*s_bhead;	/* Bhead do buffer que contem o SB */

	dev_t	s_dev;		/* Dispositivo referente a este SB */

	short	s_res[6];	/* Para completar 512 bytes */
};

#define	V7_SBMAGIC	0xC90FDAA2	/* No. de Controle para a montagem */

/*
 ******	Diretório do V7FS ***************************************
 */
struct	v7dir
{
	ushort	d_ino;			/* No. do Inode */
	char	d_name[IDSZ];		/* Nome da entrada */
};

/*
 ******	Estrutura da entrada ************************************
 */
struct v7dino
{
	ushort	n_mode;		/* Modo e tipo do arquivo */
	ushort	n_nlink;    	/* No. de Ponteiros para este arquivo */
	ushort	n_uid;      	/* Dono  do Arquivo */
	ushort	n_gid;      	/* Grupo do Arquivo */
	off_t	n_size;     	/* Tamanho do arquivo em bytes */
	char  	n_addr[39];	/* Enderecos dos blocos do arquivo (3 bytes) */
	char	n_emode;	/* Extensão do Modo do arquivo */
	time_t	n_atime;   	/* Tempo do ultimo acesso */
	time_t	n_mtime;   	/* Tempo da ultima modificação */
	time_t	n_ctime;   	/* Tempo da criação */
};

#define	V7_NADDR	13	/* No. de endereços */

/*
 ******	A estrutura da lista livre ******************************
 */
struct	v7freeblk
{
	short	r_nfblk;		/* No. de blocos livres */
	short	r_fblk[2 * V7_SBFBLK];	/* (== daddr_t) End. dos bl. livres */
};
