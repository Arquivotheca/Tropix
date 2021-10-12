#include <sys/types.h>

#include <stddef.h>

#include "h/common.h"
#include "h/dos.h"

main ()
{
	printf ("sizeof (SB) = %d\n", sizeof (SB));
	printf ("offset (s_fat_msg_16) = 0x%X\n", offsetof (SB, u.fat_16.s_fat_msg));
	printf ("offset (s_fat_msg_32) = 0x%X\n", offsetof (SB, u.fat_32.s_fat_msg));
#if (0)	/*******************************************************/
	printf ("offset (bytset) = %d\n", offsetof (SB, s_byte_sec));
	printf ("offset (desfat) = %d\n", offsetof (SB, s_fat_off));
	printf ("offset (n_ent) = %d\n", offsetof (SB, s_n_root_dir));
	printf ("offset (n_set) = %d\n", offsetof (SB, s_n_sec));
	printf ("offset (setfat) = %d\n", offsetof (SB, s_sec_fat));
	printf ("offset (settrk) = %d\n", offsetof (SB, s_sec_trk));
	printf ("offset (nlados) = %d\n", offsetof (SB, s_n_heads));
	printf ("offset (nhidden) = %d\n", offsetof (SB, s_n_hidden));
	printf ("offset (s_size) = %d\n", offsetof (SB, s_size));

	printf ("offset (s_phys_drive) = %d\n", offsetof (SB, s_phys_drive));
	printf ("offset (s_signature) = %d\n", offsetof (SB, s_signature));
	printf ("offset (s_vol_ser_no) = %d\n", offsetof (SB, s_vol_ser_no));
	printf ("offset (s_label) = %d\n", offsetof (SB, s_label));
#endif	/*******************************************************/
	printf ("\n");

	printf ("sizeof (UNI) = %d\n", sizeof (UNI));
	printf ("\n");

	printf ("sizeof (DOSDIR) = %d\n", sizeof (DOSDIR));
	printf ("offset (d_mtime) = %d\n", offsetof (DOSDIR, d_mtime));
	printf ("offset (d_mdate) = %d\n", offsetof (DOSDIR, d_mdate));
	printf ("offset (d_low_cluster) = %d\n", offsetof (DOSDIR, d_low_cluster));
	printf ("offset (d_size) = %d\n", offsetof (DOSDIR, d_size));
	printf ("\n");

	exit (0);
}
