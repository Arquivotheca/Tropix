/*
 ****************************************************************
 *								*
 *			<sys/t1.h>				*
 *								*
 *	Estruturas do sistema de arquivos T1			*
 *								*
 *	Versão	4.3.0, de 01.07.02				*
 *		4.3.0, de 26.07.02				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *		/usr/include/sys				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#define	T1_H			/* Para definir os protótipos */

/*
 ******	Definições globais **************************************
 */
#define	T1_BOOTNO	((daddr_t)0)		/* No. do bloco do BOOT */
#define	T1_SBNO		((daddr_t)1)		/* No. do bloco do SUPERBLOCO */
#define	T1_BMNO		((daddr_t)2)		/* No. do bloco do 1. mapa de bits */
#define	T1_ROOTNO	((daddr_t)3)		/* No. do bloco do INODE da RAIZ */

#define	T1_BL4toBL(x)	((x) << (BL4SHIFT - BLSHIFT))	/* Para converter os no. de blocos */

/*
 ******	A estrutura do SUPER BLOCO ******************************
 */
struct	t1sb
{
	long	s_magic;	/* No. mágico para validação de SB */

	char	s_fname[16];	/* Nome do Sistema de Arquivos */
	char	s_fpack[16];	/* Nome do Disco */

	int	s_bl_sz;	/* Tamanho do bloco (em bytes) */

	ulong	s_fs_sz;	/* No. de blocos (acima) do sistema de arquivos */
	ulong	s_busy_bl;	/* No. de blocos alocados */
	ulong	s_busy_ino;	/* No. de INODEs alocados */

	long	s_reser[1011];	/* Reserva para 4 KB */
};

#define	T1_SBMAGIC	0xC91FDAB2	/* No. de Controle para a montagem */

/*
 ******	Diretório do T1FS ***************************************
 */
#define	MAXNAMLEN	255	/* Tamanho máximo do nome */

struct	t1dir
{
	ino_t	d_ino;			/* No. do Inode */
	short	d_ent_sz;		/* Tamanho da entrada */
	short	d_nm_len;		/* Comprimento do nome */
	char	d_name[4];		/* Nome da entrada */
};

#define	T1DIR_SIZEOF(len)	(sizeof (T1DIR) + ((len) & ~3))
#define	T1DIR_SZ_PTR(dp)	(T1DIR *)((int)(dp) + (dp)->d_ent_sz)
#define	T1DIR_LEN_PTR(dp)	(T1DIR *)((int)(dp) + T1DIR_SIZEOF ((dp)->d_nm_len))
#define	T1DIR_NM_SZ(len)	(((len) + 4) & ~3)

/*
 ******	Estrutura do INODE **************************************
 */
#define	T1_NADDR	(16 + 3)	 /* No. de endereços */

typedef	struct t1dino	T1DINO;

struct t1dino
{
	ulong	n_magic;		/* Número Mágico */
	ulong	n_mode;			/* Modo e tipo do arquivo */
	ushort	n_uid;      		/* Dono  do Arquivo */
	ushort	n_gid;      		/* Grupo do Arquivo */
	int	n_nlink;    		/* No. de Ponteiros para este arquivo */
	off_t	n_size;     		/* Tamanho do arquivo em bytes */
	off_t	n_high_size;   		/* Reservado para a parte alta do tamanho */
	time_t	n_atime;   		/* Tempo do ultimo acesso */
	time_t	n_mtime;   		/* Tempo da ultima modificação */
	time_t	n_ctime;   		/* Tempo da criação */
	daddr_t	n_addr[T1_NADDR];	/* Endereços dos blocos do arquivo */
	int	n_reser1[4];
};

/*
 ******	Definições relativas a INODEs **************************
 */
#define	T1_INDIR_SHIFT		(BL4SHIFT - 2)
#define	T1_INDIR_SZ		(1 << T1_INDIR_SHIFT)
#define	T1_INDIR_MASK		(T1_INDIR_SZ - 1)

#define	T1_INOtoADDR(x)		(daddr_t)((unsigned)(x) >> 5)
#define	T1_INOtoINDEX(x)	(int)((x) & 31)

#define	T1_MAKE_INO(bno,index)	(((bno) << 5) | (index))

#define	T1_ROOT_INO		T1_MAKE_INO (T1_ROOTNO, 0)

#define	T1_INOinBL4		(BL4SZ / sizeof (T1DINO))

/*
 ******	Definições do Mapa de Bits ******************************
 */
enum
{
	T1_MAP_FREE		= 0,	/* Bloco Livre */
	T1_MAP_MAP		= 2,	/* Bloco do próprio Mapa */
	T1_MAP_DATA		= 4,	/* Bloco de Dados */
	T1_MAP_INDIR		= 6,	/* Bloco de Endereços Indiretos */
	T1_MAP_INODE_EMPTY	= 8,	/* Bloco de INODEs parcialmente cheio */
	T1_MAP_INODE_FULL	= 9,	/* Bloco de INODEs totalmente cheio */
	T1_MAP_BAD		= 14,	/* Bloco Defeituoso */
	T1_MAP_OUT		= 15	/* Bloco NÃO-existente */
};

#define	T1_ZONE_SHIFT	(BL4SHIFT + 1)		/* Cada byte do Bitmap define 2 blocos */
#define	T1_ZONE_SZ	(1 << T1_ZONE_SHIFT)
#define	T1_ZONE_MASK	(T1_ZONE_SZ - 1)

#define	T1_ASSIGN_MAP_PARAM(bno, map_bno, map_index)	\
							\
	map_bno	  = (bno & ~T1_ZONE_MASK) + T1_BMNO,	\
	map_index = (bno &  T1_ZONE_MASK)

#define	T1_GET_MAP(bp,i)			\
						\
	(((i) & 1) ?				\
	((bp)[(i) >> 1] & 0x0F) :		\
	((bp)[(i) >> 1] >> 4))

#define	T1_PUT_MAP(bp,i,val)			\
{						\
	char *_cp = &((bp)[(i) >> 1]);		\
						\
	*_cp = ((i) & 1) ?			\
		((*_cp & 0xF0) | (val)) :	\
		((*_cp & 0x0F) | (val) << 4);	\
}
