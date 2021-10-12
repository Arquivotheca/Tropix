#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stat.h>
#include <ustat.h>
#include <errno.h>

main (int argc, register const char *argv[])
{
	STAT	statb;
	USTAT	buf;

	if (stat (argv[1], &statb) < 0)
		error ("$*Erro no stat");

	if (ustat (statb.st_rdev, &buf) < 0)
		error ("$*Erro no ustat, rdev = (%d, %d)", MAJOR (statb.st_rdev), MINOR (statb.st_rdev));

	printf ("f_fsize = %d\n", buf.f_fsize);
	printf ("f_tfree = %d\n", buf.f_tfree);
	printf ("f_isize = %d\n", buf.f_isize);
	printf ("f_tinode = %d\n", buf.f_tinode);
	printf ("f_fname = %s\n", buf.f_fname);
	printf ("f_fpack = %s\n", buf.f_fpack);
	printf ("f_m = %d\n", buf.f_m);	
	printf ("f_n = %d\n", buf.f_n);
	printf ("f_symlink_sz = %d\n", buf.f_symlink_sz);

}
