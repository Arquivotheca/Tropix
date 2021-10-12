/*
 ****************************************************************
 *								*
 *			<sys/nfs.h>				*
 *								*
 *	Formato do INODE na Memória				*
 *								*
 *	Versão	4.8.0, de 09.08.05				*
 *		4.8.0, de 03.01.06				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *		/usr/include/sys				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2006 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#define	NFS_H			/* Para incluir os protótipos */

/*
 ******	Definições de Portas UDP ********************************
 */
enum {	PMAP_PORT = 111, RPC_PORT = 2049, NFS_CLIENT_PORT = 700 };

/*
 ******	Definições do XDR ***************************************
 */
#define ROUND_XDR(x)    ((x + 3) & ~3)

/*
 ******	Definições do RPC ***************************************
 */
enum {	RPC_VERS = 2 };

enum {	RPC_CALL, RPC_REPLY };

enum {	AUTH_NULL, AUTH_UNIX, AUTH_SHORT, AUTH_DES };

enum {	RPC_MSG_ACCEPTED, RPC_MSG_DENIED };

enum {	SUCCESS, PROG_UNAVAIL, PROG_MISMATCH, PROC_UNAVAIL, GARBAGE_ARGS };

enum {	RPC_MISMATCH, AUTH_ERROR };

enum {	SQUASH_ID = -2 };

/*
 ******	Definições do PMAP **************************************
 */
enum {	PMAP_PROG = 100000, PMAP_VERS = 2 };

enum {	PMAPPROC_NULL, PMAPPROC_SET, PMAPPROC_UNSET, PMAPPROC_GETPORT, PMAPPROC_DUMP };

/*
 ******	Definições do MNTPROG ***********************************
 */
enum {	MOUNTPROG = 100005, MOUNTVERS = 1 };

enum {	MOUNTPROG_NULL, MOUNTPROG_MNT, MOUNTPROG_DUMP, MOUNTPROG_UMNT, MOUNTPROG_UMNTALL };

enum {	MAXPATHLEN = 1024 };

/*
 ******	Definições do NFS ***************************************
 */
enum {	NFS_PROG = 100003, NFS_VERS = 2 };

enum
{
	NFSPROC_NULL,		NFSPROC_GETATTR,	NFSPROC_SETATTR,
	NFSPROC_ROOT,		NFSPROC_LOOKUP,		NFSPROC_READLINK,
	NFSPROC_READ,		NFSPROC_WRITECACHE,	NFSPROC_WRITE,
	NFSPROC_CREATE,		NFSPROC_REMOVE,		NFSPROC_RENAME,
	NFSPROC_LINK,		NFSPROC_SYMLINK,	NFSPROC_MKDIR,
	NFSPROC_RMDIR,		NFSPROC_READDIR,	NFSPROC_STATFS
};

enum {	MAX_NFS2_DATA = KBSZ };

/*
 ******	Estrutura de chamado de RPC *****************************
 */
typedef struct
{
	int		f_xid;		 /* Identificação */
	int		f_vers;		 /* Versão */
	int		f_proc;		 /* No. da função a executar */
	char		f_client_nm[16]; /* Nome do cliente */
	IPADDR		f_peer_addr;	 /* O endereço IP */
	int		f_peer_port;	 /* A porta IP */
	int		f_export;	 /* Índice na tabela EXPORT */
	int		f_index;	 /* Índice na tabela NFS_MOUNT */
	int		f_ronly;	 /* Somente para leitura */
	int		f_uid;		 /* Dono do pedido */
	int		f_gid;		 /* Grupo do pedido */
	const void	*f_area;	 /* Área do argumentos */
	const void	*f_end_area;	 /* O final da área */

}	RPCINFO;

/*
 ******	Prólogo de uma resposta RPC *****************************
 */
typedef struct
{
	int		p_xid;		/* No. do pedido */
	int		p_mtype;	/* Pedido/resposta */
	int		p_rstat;	/* A Mensagem foi aceita? */
	int		p_verf[2];	/* A autenticação */
	int		p_rpcstat;	/* Código de retorno RPC */
	int		p_nfsstat;	/* Código de retorno NFS */

}	RPCPLG;

/*
 ******	Estrutura de um HANDLE **********************************
 */
enum {	HANDLE_MAGIC = 0x11DE784A };

typedef struct
{
	int		h_magic;	/* Número mágico para autenticação */
	int		h_export;	/* Índice na tabela EXPORT */
	int		h_index;	/* Índice na tabela NFS_MOUNT */
	void		*h_inode;	/* Endereço do INODE */
	dev_t		h_dev;		/* Dispositivo */
	ino_t		h_ino;		/* No. do INODE */
	int		h_reser[2];	/* Para completar 32 bytes */

}	FHANDLE;

#define	NOFHANDLE	(FHANDLE *)NULL

#define	i_handle	i_addr		/* Superpõe o "handle" com os 8 primeiros endereços */
#define	i_forw		i_addr[8]	/* Ponteiro da fila de atualização */

/*
 ******	Estrutura de um ATRIBUTO de arquivo *********************
 */
enum { NFNON, NFREG, NFDIR, NFBLK, NFCHR, NFLNK };

typedef	struct { time_t sec, usec; } TIMEVAL;

typedef struct
{
	int		fa_type;	/* Tipo */
	int		fa_mode;	/* Modo */
	int		fa_nlink;	/* No. de elos */
	int		fa_uid;		/* Dono */
	int		fa_gid;		/* Grupo */
	off_t		fa_size;	/* Tamanho do arquivo */
	int		fa_blksz;	/* Tamanho do bloco */
	int		fa_rdev;	/* Para arquivos especiais */
	int		fa_blocks;	/* No. de blocos no disco */
	int		fa_dev;		/* Id. do sistema de arquivos */
	int		fa_ino;		/* Id. dentro do arq. no sis. de arquivos */
	TIMEVAL		fa_atime;	/* Último acesso */
	TIMEVAL		fa_mtime;	/* Última modificação do conteúdo */
	TIMEVAL		fa_ctime;	/* Última modificação do estado */

}	FATTR;

#define NOFATTR	(FATTR *)NULL

typedef struct
{
	int		sa_mode;	/* Modo */
	int		sa_uid;		/* Dono */
	int		sa_gid;		/* Grupo */
	off_t		sa_size;	/* Tamanho do arquivo */
	TIMEVAL		sa_atime;	/* Último acesso */
	TIMEVAL		sa_mtime;	/* Última modificação do conteúdo */

}	SATTR;

/*
 ******	Estrutura de um Estado de um Sistema de Arquivos ********
 */
typedef struct
{
	uint		fs_tsize;	/* Tamanho ótimo de transferência */
	int		fs_bsize;	/* Tamanho do bloco */
	uint		fs_blocks;	/* No. total de blocos */
	uint		fs_bfree;	/* No. de blocos livres */
	uint		fs_bavail;	/* No. de blocos livres para usuários regulares */

}	STATFS;

/*
 ******	Códigos de erros ****************************************
 */
enum {	NFS_ERRNO = -1 };		/* Para a função "nfs_send_status_datagram ()" */

enum
{	NFS_OK		= 0,	NFSERR_PERM	= 1,	NFSERR_NOENT	= 2,
	NFSERR_IO	= 5,	NFSERR_NXIO	= 6,	NFSERR_ACCES	= 13,
	NFSERR_EXIST	= 17,	NFSERR_NODEV	= 19,	NFSERR_NOTDIR	= 20,
	NFSERR_ISDIR	= 21,	NFSERR_FBIG	= 27,	NFSERR_NOSPC	= 28,
	NFSERR_ROFS	= 30,	NFSERR_NM2LONG	= 63,	NFSERR_NOTEMPTY	= 66,
	NFSERR_DQUOT	= 69,	NFSERR_STALE	= 70,	NFSERR_WFLUSH	= 99
};

/*
 ******	A estrutura do SUPER BLOCO NFS **************************
 */
enum {	XID_PROC_SHIFT = 12, XID_PROC_MASK = (1 << XID_PROC_SHIFT) - 1 }; /* Até 4096 processos */

enum {	NFS_HASH_SHIFT = 6, NFS_HASH_SZ = (1 << NFS_HASH_SHIFT), NFS_HASH_MASK = NFS_HASH_SZ - 1 };

/* A entrada do CACHE */

typedef struct nfsdata	NFSDATA;

struct	nfsdata
{
	ino_t		d_ino;		/* No. do Inode */

	NFSDATA		*d_next;	/* Ponteiro para a próxima */

	char		d_sz;		/* Tamanho da entrada no momento */
	char		d_handle_pres;	/* O Handle está presente */
	char		d_nm_len;	/* Comprimento do nome */

	FHANDLE		d_handle;	/* O HANDLE */

	off_t		d_size;		/* O tamanho do arquivo */
	time_t		d_mtime;	/* Tempo de modificação */
	ino_t		d_par_ino;	/* No. do Inode do pai */

	/* Parte nem sempre presente */

	char		d_name[4];	/* Nome da entrada */
};

#define	NONFSDATA (NFSDATA *)NULL

/* O Super bloco */

typedef struct nfssb	NFSSB;

struct nfssb
{
	SB		*sb_sp;			/* O Super bloco genérico */

	long		sb_server_addr;		/* Endereço IP do servidor */
	char		sb_server_nm[64];	/* Nome do Servidor */

	int		sb_mount_port;		/* Portas de acesso ao servidor */
	int		sb_nfs_port;

	char		sb_fname[16];		/* Nome do Sistema de Arquivos */
	char		sb_fpack[16];		/* Nome do Disco */

	int		sb_root_ino;		/* No. do INODE da raiz */

	long		sb_update_pid;		/* PID do processo de atualização */

	BHEAD		*sb_update_bhead_queue;	/* Fila de atualização de BHEAD */
	BHEAD		*sb_update_bhead_last;

	INODE		*sb_update_inode_queue;	/* Fila de atualização de INODE */
	INODE		*sb_update_inode_last;

	LOCK		sb_update_lock;		/* A tranca das filas acima */
	EVENT		sb_update_nempty;	/* Evento de espera do processo */
	LOCK		sb_data_lock;		/* A tranca das filas abaixo */

	NFSDATA		*sb_nfsdata[NFS_HASH_SZ]; /* O vetor de ponteiros para o CACHE */
};

#define	NONFSSB		(NFSSB *)NULL

/*
 ******	Variáveis da fila de pedidos para RPC/NFS ***************
 */
typedef struct
{
	char		nfs_index;		/* NÚmero do "daemon" */
	LOCK		nfs_inq_lock;		/* Semáforo da fila */
	EVENT		nfs_inq_nempty;		/* Evento de fila NÃO vazia */

	ITBLOCK		*nfs_inq_first;		/* Ponteiros para o início e final da fila */
	ITBLOCK		*nfs_inq_last;

}	NFSINQ;

#define NONFSINQ        (NFSINQ *)NULL

/*
 ******	Protótipos de funções ***********************************
 */
#ifdef	ITNET_H
extern void		nfs_send_var_rpc_datagram (const RPCINFO *info, int count, ...);
extern void		nfs_receive_pmap_datagram (const RPCINFO *);
extern void		nfs2_receive_mntpgm_datagram (const RPCINFO *);
extern void		nfs2_receive_nfs_datagram (RPCINFO *);
extern void		nfs2_server_readdir (const RPCINFO *info);
extern void		nfs2_server_lookup (const RPCINFO *info);
extern void		nfs2_server_read (const RPCINFO *info);
extern void		nfs2_server_readlink (const RPCINFO *info);
extern void		nfs2_server_symlink (const RPCINFO *info);
extern void		nfs2_server_mkdir (const RPCINFO *info);
extern void		nfs2_server_rmdir (const RPCINFO *info);
extern void		nfs2_server_link (const RPCINFO *info);
extern void		nfs2_server_rename (const RPCINFO *info);
extern void		nfs2_server_getattr (const RPCINFO *info);
extern void		nfs2_server_setattr (const RPCINFO *info);
extern void		nfs2_server_write (const RPCINFO *info);
extern void		nfs2_server_create (const RPCINFO *info);
extern void		nfs2_server_remove (const RPCINFO *info);
extern void		nfs2_server_mkdir (const RPCINFO *info);
extern void		nfs2_server_statfs (const RPCINFO *info);
extern void		nfs_send_status_datagram (const RPCINFO *info, int status);
extern ITBLOCK		*nfs_setup_server_datagram_prolog (const RPCINFO *info, UDP_EP *uep);
extern void		nfs_receive_reply_datagram (const RPCINFO *info, ITBLOCK *bp);
extern void		*nfs_setup_client_datagram_prolog (const NFSSB *, UDP_EP *, int, ITBLOCK **);
#endif	ITNET_H

#ifdef	DIRENT_H
extern const void	*nfs_get_xdr_name (const int *ptr, DIRENT *dep);
extern NFSDATA		*nfs2_put_nfsdata (NFSSB *nfssp, ino_t ino, const FHANDLE *hp, const DIRENT *dep);
#endif	DIRENT_H

extern int		nfs_receive_rpc_datagram (ITBLOCK *bp);
extern INODE		*nfs2_get_handle_inode (const FHANDLE *hp);
extern void		nfs2_get_fattr_attribute (FATTR *ap, const INODE *ip);
extern void		nfs2_put_sattr_attribute (const SATTR *ap, INODE *ip);
extern const void	*nfs2_put_fattr_attribute (const FATTR *ap, INODE *ip);
extern void		nfs2_put_time_attribute (const FATTR *ap, const NFSDATA *dp, INODE *ip);
extern void		*nfs_put_xdr_name (int *ptr, const char *nm, int nm_len);
extern const char	*nfs_can_xdr_dirpath (const int *ptr);
extern int		nfs_access (const INODE *ip, long mode, const RPCINFO *info);
extern int		nfs_owner (const INODE *ip, const RPCINFO *info);
extern int		*nfs_put_auth_unix (int *ptr, int uid, int gid);
extern NFSSB		*nfs2c_send_mntpgm_datagram (const char *, const struct mntent *, SB *);
extern void		nfs2c_send_umntpgm_datagram (const NFSSB *nfssp, const char *);
extern NFSDATA		*nfs2_get_nfsdata (NFSSB *sp, ino_t ino);
extern void		nfs2_strip_nfsdata (NFSDATA *dp);
extern void		nfs2_free_all_nfsdata (NFSSB *nfssp);
extern void		nfs2_remove_nfsdata (NFSSB *nfssp, ino_t ino);
extern void		nfs_server_is_rofs (SB *sp);
extern int		*nfs_store_rpc_values (int *ptr, int count, ...); 
extern void		nfs2_update_daemon (NFSSB *nfssp);
extern void		nfs2_do_setattr (INODE *ip, off_t size);

extern int		nfs_req_source;
