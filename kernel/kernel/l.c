#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct sb
{
	ulong		s_inodes_count;
	ulong		s_blocks_count;
	ulong		s_r_blocks_count;
	ulong		s_free_blocks_count;
	ulong		s_free_inodes_count;
	ulong		s_first_data_block;
	ulong		s_log_block_size;
	ulong		s_log_frag_size;
	ulong		s_blocks_per_group;
	ulong		s_frags_per_group;
	ulong		s_inodes_per_group;
	time_t		s_mtime;
	time_t		s_wtime;
	ushort		s_mnt_count;
	ushort		s_max_mnt_count;
	ushort		s_magic;
	ushort		s_state;
	ushort		s_errors;
	ushort		s_minor_rev_level;
	time_t		s_last_check;
	int		s_check_interval;
	ulong		s_creator_os;
	ulong		s_rev_level;
	ushort		s_def_resuid;
	ushort		s_def_resgid;

}	SB;

typedef struct bg
{
	ulong		bg_block_bitmap;
	ulong		bg_inode_bitmap;
	ulong		bg_inode_table;
	ushort		bg_free_blocks_count;
	ushort		bg_free_inodes_count;
	ushort		bg_used_dirs_count;
	ushort		bg_pad;
	ulong		bg_reserved[3];

}	BG;

#define	EXT2_NADDR	(12 + 3)

typedef struct inode
{
	ushort		n_mode;
	ushort		n_uid;
	off_t		n_size;
	time_t		n_atime;
	time_t		n_ctime;
	time_t		n_mtime;
	time_t		n_dtime;
	ushort		n_gid;
	ushort		n_links_count;
	ulong		n_blocks;
	ulong		n_flags;
	ulong		n_osd1[1];
	daddr_t		n_addr[EXT2_NADDR];
	ulong		n_version;
	ulong		n_file_acl;
	ulong		n_dir_acl;
	ulong		n_faddr;
	ulong		n_osd2[3];

}	INODE;

/*
 ******	Diretório do T1FS ***************************************
 */
#define	MAXNAMLEN	255	/* Tamanho máximo do nome */

typedef struct	t1dir
{
	ino_t	d_ino;			/* No. do Inode */
	short	d_ent_sz;		/* Tamanho da entrada */
	char	d_nm_len;		/* Comprimento do nome */
	char	d_type;			/* Tipo do arquivo */
	char	d_name[4];		/* Nome da entrada */

}	T1DIR;

#define	T1DIR_SIZEOF(len)	(sizeof (T1DIR) + ((len) & ~3))
#define	T1DIR_SZ_PTR(dp)	(T1DIR *)((int)(dp) + (dp)->d_ent_sz)
#define	T1DIR_LEN_PTR(dp)	(T1DIR *)((int)(dp) + T1DIR_SIZEOF ((dp)->d_nm_len))
#define	T1DIR_NM_SZ(len)	(((len) + 4) & ~3)

int
main (int argc, const char *argv[])
{
	int		fd;
	SB		sb;
	BG		bg[BL4SZ / sizeof (BG)];
	INODE		inode[BL4SZ / sizeof (INODE)];
	char		root_dir[BL4SZ];
	const T1DIR	*dp, *end_dp;
	const INODE	*np;
	const BG	*bgp;
	int		index;
	int		blshift;

	printf ("sizeof (INODE) = %d\n", sizeof (INODE));

	if (argv[1] == NOSTR)
		return (0);

	if ((fd = open (argv[1], 0)) < 0)
		error ("$*Não consegui abrir o SB");

	lseek (fd, 1024, 0);

	if (read (fd, &sb, sizeof (SB)) < 0)
		error ("$*Não consegui ler o SB");

	printf ("s_inodes_count		= %d\n", sb.s_inodes_count);
	printf ("s_blocks_count		= %d\n", sb.s_blocks_count);
	printf ("s_r_blocks_count	= %d\n", sb.s_r_blocks_count);
	printf ("s_free_blocks_count	= %d\n", sb.s_free_blocks_count);
	printf ("s_free_inodes_count	= %d\n", sb.s_free_inodes_count);
	printf ("s_first_data_block	= %d\n", sb.s_first_data_block);
	printf ("s_log_block_size	= %d\n", sb.s_log_block_size);
	printf ("s_log_frag_size		= %d\n", sb.s_log_frag_size);
	printf ("s_blocks_per_group	= %d\n", sb.s_blocks_per_group);
	printf ("s_frags_per_group	= %d\n", sb.s_frags_per_group);
	printf ("s_inodes_per_group	= %d\n", sb.s_inodes_per_group);
	printf ("s_mtime			= %s", btime (&sb.s_mtime));
	printf ("s_wtime			= %s", btime (&sb.s_wtime));
	printf ("s_mnt_count		= %d\n", sb.s_mnt_count);
	printf ("s_max_mnt_count		= %d\n", sb.s_max_mnt_count);
	printf ("s_magic			= %04X\n", sb.s_magic);
	printf ("s_state			= %d\n", sb.s_state);
	printf ("s_errors		= %d\n", sb.s_errors);
	printf ("s_minor_rev_level	= %d\n", sb.s_minor_rev_level);
	printf ("s_last_check		= %s", btime (&sb.s_last_check));
	printf ("s_check_interval	= %d dias\n", sb.s_check_interval / (60 * 60 * 24));
	printf ("s_creator_os		= %d\n", sb.s_creator_os);
	printf ("s_rev_level		= %d\n", sb.s_rev_level);
	printf ("s_def_resuid		= %d\n", sb.s_def_resuid);
	printf ("s_def_resgid		= %d\n\n", sb.s_def_resgid);

	blshift = 10 + sb.s_log_block_size;

#if (0)	/*******************************************************/
	lseek (fd, 1024 << sb.s_log_block_size, 0);
#endif	/*******************************************************/
	lseek (fd, 1 << blshift, 0);

	if (read (fd, bg, sizeof (bg)) < 0)
		error ("$*Não consegui ler o SB");

#if (0)	/*******************************************************/
	index = 0, bgp = bg;
	for (index = 0, bgp = bg; index < 5; bgp++, index++)
#endif	/*******************************************************/
	for (index = 0, bgp = bg; bgp->bg_block_bitmap != 0; bgp++, index++)
	{
		printf ("************* GRUPO %d ************\n\n", index);

		printf ("bg_block_bitmap		= %d\n", bgp->bg_block_bitmap);
		printf ("bg_inode_bitmap		= %d\n", bgp->bg_inode_bitmap);
		printf ("bg_inode_table		= %d\n", bgp->bg_inode_table);
		printf ("bg_free_blocks_count	= %d\n", bgp->bg_free_blocks_count);
		printf ("bg_free_inodes_count	= %d\n", bgp->bg_free_inodes_count);
		printf ("bg_used_dirs_count	= %d\n\n", bgp->bg_used_dirs_count);
	}

	/*
	 *	Tenta ler o INODE da raiz
	 */
#if (0)	/*******************************************************/
	lseek (fd, 1024 << bg[0].bg_inode_table, 0);
#endif	/*******************************************************/
	lseek (fd, bg[0].bg_inode_table << blshift, 0);

	if (read (fd, inode, sizeof (inode)) < 0)
		error ("$*Não consegui ler o inode");

	np = &inode[1];

	printf ("n_mode			= %04X (%s)\n", np->n_mode, modetostr (np->n_mode));
	printf ("n_uid			= %d\n", np->n_uid);
	printf ("n_size			= %d\n", np->n_size);
	printf ("n_atime			= %s", btime (&np->n_atime));
	printf ("n_ctime			= %s", btime (&np->n_ctime));
	printf ("n_mtime			= %s", btime (&np->n_mtime));
	printf ("n_dtime			= %s", btime (&np->n_dtime));
	printf ("n_gid			= %d\n", np->n_gid);
	printf ("n_links_count		= %d\n", np->n_links_count);
	printf ("n_blocks		= %d\n", np->n_blocks);
	printf ("n_flags			= %d\n", np->n_flags);
	printf ("n_addr[0]		= %d\n", np->n_addr[0]);
	printf ("n_addr[1]		= %d\n", np->n_addr[1]);
	printf ("n_addr[2]		= %d\n", np->n_addr[2]);
	printf ("n_version		= %d\n", np->n_version);
	printf ("n_file_acl		= %d\n", np->n_file_acl);
	printf ("n_dir_acl		= %d\n", np->n_dir_acl);
	printf ("n_faddr			= %d\n\n", np->n_faddr);

	/*
	 *	Tenta ler o diretório RAIZ
	 */
	lseek (fd, np->n_addr[0] << blshift, 0);

	if (read (fd, root_dir, sizeof (root_dir)) < 0)
		error ("$*Não consegui ler a RAIZ");

	for (dp = (T1DIR *)root_dir, end_dp = (T1DIR *)(root_dir + BL4SZ); dp < end_dp; dp = T1DIR_SZ_PTR (dp))
	{
		printf ("%d\t", dp->d_ino);
		printf ("%d\t", dp->d_ent_sz);
		printf ("%d\t", dp->d_nm_len);
		printf ("%*.*s\n", dp->d_nm_len, dp->d_nm_len, dp->d_name);
	}





	return (0);
}
