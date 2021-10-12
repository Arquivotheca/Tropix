/*
 ****************************************************************
 *								*
 *			<sys/common.h>				*
 *								*
 *	Parâmetros de geração e constantes universais		*
 *								*
 *	Versão	3.0.0, de 28.07.94				*
 *		4.8.0, de 05.10.05				*
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

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define	NULL	0
#define	NIL	(-1)		/* Ponteiro NULO alternativo */
#define	NOSTR	(char *)NULL
#define	NOSTRP	(char **)NULL	/* Pont. de pont. de Caracter Nulo */
#define	NOVOID	(void *)NULL
#define	NILVOID	(void *)NIL

#define	elif	else if
#define	EVER	;;

#define STR(x)	# x
#define XSTR(x)	STR (x)

/*
 ******	Constantes do Sistema ***********************************
 */
#define	MSG			/* Mensagens das Chaves */

#define NCPU	1		/* No. Maximo de CPU's ativas */

#define	IDSZ	14		/* No. de Caracteres por nome  */

#define	NUFILE	32		/* No. max. de arq. abertos por processo */

#define	CLBSZ	60		/* No. de caracteres por bloco da CLIST */
#define	CLMASK	63		/* Arred. da CLIST:  sizeof(int *)+CLBSZ-1 */

#define	NSYSCALL (64+32+32)	/* No. Maximo de System calls */
#define	UNDEF	0		/* Retorno de SYS CALL com possivel erro */

#define	SWAP_OFFSET 1		/* Deslocamento do SWAP (para evitar NIL) */

#define	MKVERSION(v,m)	(((v) << 8) | (m))	/* Versão/modificação */

/*
 ******	Prioridades: Maior é melhor *****************************
 */
#define	PSCHED	1000		/* Escalação e paginação */
#define	PINODE	 900		/* Tratamento de Inodes */
#define	PBLKIO	 800		/* E/S estruturada */

#define	PLIMIT	 750		/* limiar entre adia/não-adia sinais */

#define	PPIPE	 740		/* Tratamento de pipes */
#define	PTTYIN	 720		/* Entrada de terminal */
#define PTTYOUT	 710		/* Saida   de terminal */
#define	PWAIT	 700		/* Esperando o termino de um filho */
#define	PPAUSE	 600		/* Esperando o sinal para acordar */
#define	PUSER	 500		/* Base de calculo para pri. de usuario */

/*
 ******	Tipos fundamentais **************************************
 */
typedef	unsigned long	ulong;
typedef	unsigned short	ushort;
typedef	unsigned char 	uchar;
typedef	signed char 	schar;
typedef	unsigned int	bit;
typedef	unsigned int	uint;

/*
 ******	Tipos do sistema ****************************************
 */
typedef	long		daddr_t;	/* Enderecos de Blocos dos Discos */
typedef	int		pg_t;		/* Endereco de paginas de memoria ou disco */
typedef	ulong		ino_t;		/* Numeros de Inodes */
typedef	long		time_t;		/* Tempo em segundos desde 1.1.70 */
typedef	long		ctxt_t[6];	/* Contexto: registros r3-r5, fp, sp e pc */
typedef	long		jmp_t[6];	/* Desvio:   registros r3-r5, fp, sp e pc */
typedef	int		dev_t;		/* Identificação do Dispositivo */
typedef	ulong		off_t;		/* Offset de Arquivo em Disco */
typedef	long long	loff_t;		/* Offset de Arquivo em Disco */
typedef	long		sid_t[2];	/* Identificador curto */
typedef	long		id_t[4];	/* Identificador */
typedef	long		lid_t[8];	/* Identificador longo */
typedef	long		*idp_t;		/* Ponteiro pata um identificador */
typedef	ulong		mmu_t;		/* Entrada de tabelas da MMU */
typedef	ushort		OFLAG;		/* Modo de abertura de arquivos */
typedef	long		clusno_t;	/* Clusters usados na FAT e NTFS */

/*
 ******	Definição dos Tipos das Estruturas das Tabelas **********
 */
typedef	struct uproc	UPROC;		/*  u_  */
typedef	struct uprocv	UPROCV;		/*  u_  */
typedef	struct phys	PHYS;		/*  ph_ */
typedef	struct phashtb	PHASHTB;	/*  h_  */
typedef	struct ulimit	ULIMIT;		/*  l_  */
typedef	struct pty	PTY;		/*  p_  */
typedef	struct tty	TTY;		/*  t_  */
typedef	struct chead	CHEAD;		/*  c_  */
typedef	struct cblk	CBLK;		/*  c_  */
typedef	struct termio	TERMIO;		/*  c_  */
typedef	struct bhead	BHEAD;		/*  b_  */
typedef	struct bhashtb	BHASHTB;	/*  h_  */
typedef	struct devhead	DEVHEAD;	/*  v_  */
typedef	struct disktb	DISKTB;		/*  p_  */
typedef	struct biotab	BIOTAB;		/*  w_  */
typedef	struct ciotab	CIOTAB;		/*  w_  */
typedef	struct fstab	FSTAB;		/*  fs_ */
typedef	struct v7dir	V7DIR;		/*  d_  */
typedef	struct t1dir	T1DIR;		/*  d_  */
typedef	struct ext2dir	EXT2DIR;	/*  d_  */
typedef	struct fatdir	FATDIR;		/*  d_  */
typedef	struct v7freeblk V7FREEBLK;	/*  r_  */
typedef	struct kfile	KFILE;		/*  f_  */
typedef	struct mntent	MNTENT;		/*  me_  */
typedef	struct sb	SB;		/*  sb_  */
typedef	struct v7sb	V7SB;		/*  s_  */
typedef	struct t1sb	T1SB;		/*  s_  */
typedef	struct ext2sb	EXT2SB;		/*  s_  */
typedef	struct fatsb	FATSB;		/*  s_  */
typedef	struct ntsb	NTSB;		/*  s_  */
typedef	struct v7dino	V7DINO;		/*  n_  */
typedef	struct t1dino	T1DINO;		/*  n_  */
typedef	struct ext2dino	EXT2DINO;	/*  n_  */
typedef	struct inode	INODE;		/*  i_  */
typedef	struct lockf	LOCKF;		/*  k_  */
typedef	struct ihashtb	IHASHTB;	/*  h_  */
typedef	struct map	MAP;		/*  a_  */
typedef	struct timeout	TIMEOUT;	/*  o_  */
typedef	struct synch	SYNCH;		/*  e_  */
typedef	struct bcb	BCB;		/*  y_  */
typedef	struct scb	SCB;		/*  y_  */
typedef	struct tss	TSS;		/*  tss_  */
typedef	struct mon	MON;		/*  y_  */
typedef	struct ueventl	UEVENTL;	/*  uel_  */
typedef	struct ueventg	UEVENTG;	/*  ueg_  */
typedef	struct usemal	USEMAL;		/*  usl_  */
typedef	struct usemag	USEMAG;		/*  usg_  */
typedef	struct regionl	REGIONL;	/*  rgl_  */
typedef	struct regiong	REGIONG;	/*  rgg_  */
typedef	struct dns 	DNS;		/*  d_ */
typedef	struct route 	ROUTE;		/*  r_ */
typedef	struct ether 	ETHER;		/*  e_ */
typedef	struct export 	EXPORT;		/*  e_ */
typedef struct itblock	ITBLOCK;	/*  it_ */
typedef	struct raw_ep 	RAW_EP;		/*  rp_ */
typedef	struct udp_ep 	UDP_EP;		/*  up_ */
typedef	struct tcp_ep 	TCP_EP;		/*  tp_ */
typedef	struct excep_dc	EXCEP_DC;	/*  ed_ */
typedef	struct ioreq	IOREQ;		/*  io_ */
typedef	union frame	FRAME;		/*  [eis]f_ */
typedef	struct	ustat	USTAT;		/*  f_  */

/*
 ******	Os Diversos Nulos das Estruturas ************************
 */
#define	NODISKTB	(DISKTB *)NULL
#define	NOPTY		(PTY *)NULL
#define	NOTTY		(TTY *)NULL
#define	NOCBLK		(CBLK *)NULL
#define	NOBHEAD		(BHEAD *)NULL
#define	NOKFILE		(KFILE *)NULL
#define	NOMNTENT	(MNTENT *)NULL
#define	NOSB		(SB *)NULL
#define	NOV7SB		(V7SB *)NULL
#define	NOT1SB		(T1SB *)NULL
#define	NOEXT2SB	(EXT2SB *)NULL
#define	NOFATSB		(FATSB *)NULL
#define	NONTSB		(NTSB *)NULL
#define	NOV7DINO	(V7DINO *)NULL
#define	NOT1DINO	(T1DINO *)NULL
#define	NOEXT2DINO	(EXT2DINO *)NULL
#define	NOINODE		(INODE *)NULL
#define	NOIOREQ		(IOREQ *)NULL
#define	NOLOCKF		(LOCKF *)NULL
#define	NOUEVENTL	(UEVENTL *)NULL
#define	NOUEVENTG	(UEVENTG *)NULL
#define	NOUSEMAL	(USEMAL *)NULL
#define	NOUSEMAG	(USEMAG *)NULL
#define	NOREGIONL	(REGIONL *)NULL
#define	NOREGIONG	(REGIONG *)NULL
#define	NOUPROC		(UPROC *)NULL
#define	NOUPROCV	(UPROCV *)NULL
#define	NOSYM		(SYM *)NULL
#define	NO_UDP_EP	(UDP_EP *)NULL
#define	NO_TCP_EP	(TCP_EP *)NULL
#define	NOV7DIR		(V7DIR *)NULL
#define	NOT1DIR		(T1DIR *)NULL
#define	NOEXT2DIR	(EXT2DIR *)NULL
#define	NOFATDIR	(FATDIR *)NULL

#define	NODADDR		(daddr_t)NULL	/* Endereco de Disco inexistente */
#define	NOPG		(pg_t)NULL	/* Endereco de Pag. inexistente 1 */
#define	NILPG		(pg_t)(-1)	/* Endereco de Pag. inexistente 2 */
#define	NOINO		(ino_t)NULL	/* I-number inexistente */
#define	NODEV		(dev_t)NIL	/* Dispositivo de E/S inexistente */
#define	NOOFF		(off_t)NULL	/* Offset Nulo */
#define	NOFUNC		(int (*) ())NULL  /* Função Inexistente */
#define	NOVOIDFUNC	(void (*) ())NULL /* Função Inexistente */
#define	NOMMU		(mmu_t *)0	/* Ponteiro NULO de MMU */

/*
 ****************************************************************
 *	Códigos das cópias para "u_cpflag" e "unimove"		*
 ****************************************************************
 */
typedef	enum
{
	SS,		/* Supervisor => Supervisor */
	SU,		/* Supervisor => Usuario */
	US		/* Usuario    => Supervisor */

}	CPFLAG;

/*
 ******	Código do tipo de Sistema de Arquivos *******************
 */
enum { FS_NONE, FS_CHR, FS_BLK, FS_PIPE, FS_V7, FS_T1, FS_FAT, FS_CD, FS_EXT2, FS_NT, FS_NFS2, FS_LAST};

#define	FS_NAMES {"NONE", "CHR", "BLK", "PIPE", "V7", "T1", "FAT", "CDROM", "EXT2", "NTFS", "NFS2", NOSTR}

/*
 ******	Definições relativas à ordem de bytes da CPU ************
 */
#ifdef	LITTLE_ENDIAN

#define	ENDIAN_LONG(x)	long_endian_cv (x)
#define	ENDIAN_SHORT(x)	short_endian_cv (x)

#define ENDIAN_IMMED(x)	(((x) << 24)		  |	\
			(((x) & 0x0000FF00) << 8) |	\
			(((x) & 0x00FF0000) >> 8) |	\
			(((unsigned)(x) >> 24)))

#else

#define	ENDIAN_LONG(x)	(x)
#define	ENDIAN_SHORT(x)	(x)
#define ENDIAN_IMMED(x)	(x)

#endif	LITTLE_ENDIAN

/*
 ******	Tamanhos ************************************************
 */
#define	KBSHIFT		10		/* No. de bytes de um Kilobyte */
#define	KBSZ		(1 << KBSHIFT)
#define KBMASK		(KBSZ - 1)

#define	MBSHIFT		20		/* No. de bytes de um Megabyte */
#define	MBSZ		(1 << MBSHIFT)
#define MBMASK		(MBSZ - 1)

#define	GBSHIFT		30		/* No. de bytes de um Gigabyte */
#define	GBSZ		(1 << GBSHIFT)

#define	BLSHIFT		9		/* No. de bytes de um bloco do disco */
#define	BLSZ		(1 << BLSHIFT)
#define	BLMASK		(BLSZ - 1)

#define	BL4SHIFT	12		/* Blocos do cache */
#define	BL4SZ		(1 << BL4SHIFT)
#define	BL4MASK		(BL4SZ - 1)

#define	BL4BASEBLK(blk)	((blk) & ~((BL4SZ / BLSZ) - 1))
#define BL4BASEOFF(blk)	(((blk) & ((BL4SZ / BLSZ) - 1)) << BLSHIFT)

#define PGSHIFT		12		/* No. de bytes de uma pagina */
#define	PGSZ		(1 << PGSHIFT)
#define	PGMASK		(PGSZ - 1)

#define	USIZE		2		/* Tamanho (em pgs) da UPROC */
#define	UPROCSZ		(USIZE * PGSZ)	/* Tamanho (em bytes) da UPROC */

/*
 ******	Conversão de unidades ***********************************
 */

/*
 *	Páginas para Bytes
 */
#define	PGTOBY(x)	((unsigned)(x) << PGSHIFT)

/*
 *	Bytes para Páginas
 */
#define	BYTOPG(x)	(pg_t)((unsigned)(x) >> PGSHIFT)

/*
 *	Bytes para Páginas (Necessarias para conte-los)
 */
#define	BYUPPG(x)	(pg_t)(((unsigned)(x) + (PGSZ-1)) >> PGSHIFT)

/*
 *	Arredondamento para a Página imediatamente superior.
 */
#define	PGROUND(x)	(((unsigned)(x) + (PGSZ-1)) & ~PGMASK)

/*
 *	Páginas para Kb (1024 bytes)
 */
#define	PGTOKB(x)	((unsigned)(x) << (PGSHIFT-KBSHIFT))

/*
 *	Kb para Páginas (necessárias para conte-los)
 */
#define	KBTOPG(x)	(((unsigned)(x) + PGSZ/KBSZ-1) >> (PGSHIFT-KBSHIFT))

/*
 *	Páginas para UPROC
 */
#define	PGTOUPROC(x)	((UPROC *)((unsigned)(x) << PGSHIFT))

/*
 *	Bytes para Kb (necessárias para conte-los)
 */
#define	BYTOKB(x) 	(((unsigned)(x) + KBSZ-1) >> KBSHIFT)

/*
 *	Kb para Bytes
 */
#define	KBTOBY(x)	((unsigned)(x) << KBSHIFT)

/*
 *	Blocos (de 512 bytes) para Bytes
 */
#define	BLTOBY(x)	((unsigned)(x) << BLSHIFT)  

/*
 *	Bytes para Blocos (512 bytes) (necessários para conte-los)
 */
#define	BYTOBL(x)	((((unsigned)(x)) + BLSZ-1) >> BLSHIFT)  

/*
 *	Blocos (de 4 KB) para Bytes
 */
#define	BL4TOBY(x)	((unsigned)(x) << BL4SHIFT)  

/*
 *	Bytes para Blocos (de 4 KB) (necessários para conte-los)
 */
#define	BYTOBL4(x)	((((unsigned)(x)) + BL4SZ-1) >> BL4SHIFT)  

/*
 *	Páginas para Blocos 
 */
#define	PGTOBL(x)	(((unsigned)(x)) << (PGSHIFT-BLSHIFT))

/*
 *	Blocos para Kb (necessários para conte-los)
 */
#define	BLTOKB(x)	(((unsigned)(x) + KBSZ/BLSZ-1) >> (KBSHIFT-BLSHIFT))

/*
 *	MAJOR de um Dispositivo
 */
#define	MAJOR(x)	(int)((unsigned)(x) >> 8)

/*
 *	MINOR de um Dispositivo
 */
#define	MINOR(x)	(int)((x) & 0xFF)

/*
 *	Composição de um "dev_t"
 */
#define	MAKEDEV(x,y)	(dev_t)((x) << 8 | (y))

/*
 *	Maximo e Minimo.
 */
#define	MAX(a,b)	((a) > (b) ? (a) : (b))

#define	MIN(a,b)	((a) < (b) ? (a) : (b))

/*
 ****** Endereços VIRTUAIS **************************************
 */
#define	SYS_ADDR  ((unsigned)(2 * GBSZ))	/* do início do núcleo */

#define BASE_END  (SYS_ADDR +  640 * KBSZ)	/* Final da área BASE */
#define EXT_BEGIN (SYS_ADDR + 1024 * KBSZ)	/* Começo da área EXT */

#define	VIRT_TO_PHYS_ADDR(n)	((unsigned)(n) - (unsigned)SYS_ADDR)
#define	PHYS_TO_VIRT_ADDR(n)	((unsigned)(n) + (unsigned)SYS_ADDR)

/*
 ******	Macro para DEBUG ****************************************
 */
#define	CSWT(i)	(scb.y_CSW == (i))
