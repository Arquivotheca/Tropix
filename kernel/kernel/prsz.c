#include "h/common.h"
#include "h/sync.h"
#include "h/syscall.h"

#include <stdio.h>
#include <stddef.h>

#include "h/mntent.h"
#include "h/sb.h"
#include "h/bcb.h"
#include "h/scb.h"
#include "h/bhead.h"
#include "h/region.h"
#include "h/inode.h"
#include "h/kfile.h"
#include "h/cdfs.h"

#undef	i_par_ino

#include "h/v7.h"
#include "h/t1.h"
#include "h/nt.h"
#include "h/ext2.h"
#include "h/fat.h"
#include "h/tss.h"
#include "h/seg.h"
#include "h/mon.h"
#include "h/utsname.h"
#include "h/disktb.h"
#include "h/sysdirent.h"
#include "h/a.out.h"
#include "h/tty.h"
#include "h/signal.h"
#include "h/itnet.h"
#include "h/nfs.h"
#include "h/uproc.h"
#include "h/ustat.h"
#include "h/devmajor.h"
#include "h/video.h"

#include "h/devhead.h"
#include "h/pci.h"
#include "h/scsi.h"
#include "h/ata.h"
#include "aic/aic7xxx.h"

int
main ()
{
	printf ("sizeof (FSTAB) = %d\n", sizeof (FSTAB));
	printf ("sizeof (SCSI) = %d\n", sizeof (SCSI));
	printf ("sizeof (ahc_softc) = %d\n", sizeof (struct ahc_softc));
	printf ("sizeof (ahc_scb) = %d", sizeof (struct ahc_scb));

	if (sizeof (struct ahc_scb) & 0x7)
		printf ("\t\t*** CUIDADO\n");
	else
		printf ("\n");

	printf ("offsetof (struct ahc_scb, sg_list) = %d", offsetof (struct ahc_scb, sg_list));

	if (offsetof (struct ahc_scb, sg_list) & 0x7)
		printf ("\t\t*** CUIDADO\n");
	else
		printf ("\n");

	printf ("sizeof (struct mntent) = %d\n", sizeof (struct mntent));
	printf ("sizeof (SB) = %d\n", sizeof (SB));
	printf ("sizeof (V7SB) = %d\n", sizeof (V7SB));
	printf ("sizeof (EXT2SB) = %d\n", sizeof (EXT2SB));
	printf ("offsetof (EXT2SB, s_volume_name) = %d\n", offsetof (EXT2SB, s_volume_name));
	printf ("sizeof (BHEAD) = %d\n", sizeof (BHEAD));
	printf ("sizeof (BCB) = %d\n", sizeof (BCB));
	printf ("sizeof (HDINFO) = %d\n", sizeof (HDINFO));
	printf ("sizeof (SCB) = %d\n", sizeof (SCB));
	printf ("sizeof (TSS) = %d\n", sizeof (struct tss));
	printf ("sizeof (EXCEP_DC) = %d\n", sizeof (EXCEP_DC));
	printf ("sizeof (UTSNAME) = %d\n", sizeof (UTSNAME));
	printf ("sizeof (INODE) = %d\n", sizeof (INODE));
	printf ("sizeof (INODE.c) = %d\n", sizeof (((INODE *)0)->c));
	printf ("sizeof (V7DINO) = %d\n", sizeof (V7DINO));
	printf ("sizeof (T1DINO) = %d\n", sizeof (T1DINO));
	printf ("sizeof (T1SB) = %d\n", sizeof (struct t1sb));
	printf ("sizeof (KFILE) = %d\n", sizeof (KFILE));
	printf ("sizeof (DIRENT) = %d\n", sizeof (DIRENT));
	printf ("sizeof (FATDIR) = %d\n", sizeof (FATDIR));
	printf ("sizeof (UPROC) = %d\n", sizeof (UPROC));
	printf ("sizeof (REGIONL) = %d\n", sizeof (REGIONL));
	printf ("sizeof (REGIONG) = %d\n", sizeof (REGIONG));
	printf ("sizeof (TTY) = %d\n", sizeof (TTY));
	printf ("sizeof (TERMIO) = %d\n", sizeof (TERMIO));
	printf ("sizeof (DISKTB) = %d\n", sizeof (DISKTB));
	printf ("sizeof (ITSCB) = %d\n", sizeof (ITSCB));
	printf ("sizeof (ITBLOCK) = %d\n", sizeof (ITBLOCK));
	printf ("sizeof (DNS) = %d\n", sizeof (DNS));
	printf ("sizeof (ETH_H) = %d\n", sizeof (ETH_H));
	printf ("sizeof (ROUTE) = %d\n", sizeof (ROUTE));
	printf ("sizeof (EXPORT) = %d\n", sizeof (EXPORT));
	printf ("sizeof (ETHER) = %d\n", sizeof (ETHER));
	printf ("sizeof (IP_H) = %d\n", sizeof (IP_H));
	printf ("sizeof (ICMP_H) = %d\n", sizeof (ICMP_H));
	printf ("sizeof (TCP_H) = %d\n", sizeof (TCP_H));
	printf ("sizeof (RAW_EP) = %d\n", sizeof (RAW_EP));
	printf ("sizeof (UDP_EP) = %d\n", sizeof (UDP_EP));
	printf ("sizeof (TCP_EP) = %d\n", sizeof (TCP_EP));
	printf ("sizeof (FHANDLE) = %d\n", sizeof (FHANDLE));
	printf ("sizeof (MON) = %d\n", sizeof (MON));
	printf ("sizeof (HEADER) = %d\n", sizeof (HEADER));
#if (0)	/*******************************************************/
	printf ("sizeof (ATA) = %d\n", sizeof (ATA));
#endif	/*******************************************************/
	printf ("sizeof (CDVOL) = %d\n", sizeof (CDVOL));
	printf ("offsetof (CDVOL, cd_root_dir)   = %d\n", offsetof (CDVOL, cd_root_dir));
	printf ("sizeof (USTAT) = %d\n", sizeof (USTAT));
	printf ("FRAMESZ = %d\n", FRAMESZ);
	printf ("DGSZ = %d\n", DGSZ);
	printf ("MIN_IP_H_SZ = %d\n", MIN_IP_H_SZ);
	printf ("MIN_TCP_H_SZ = %d\n", MIN_TCP_H_SZ);
	printf ("MAX_SGSZ = %d\n", MAX_SGSZ);
	printf ("ETHADDR = %d\n", sizeof (ETHADDR));
	printf ("NTBOOT = %d\n", sizeof (struct ntboot));
	printf ("offsetof (UPROC, u_ofile)   = %d\n", offsetof (UPROC, u_ofile));
	printf ("offsetof (UPROC, u_no_user_mmu)  = %d\n", offsetof (UPROC, u_no_user_mmu));
	printf ("offsetof (UPROC, u_ctxt_sw_type)  = %d\n", offsetof (UPROC, u_ctxt_sw_type));
	printf ("offsetof (UPROC, u_nfs)  = %d\n", offsetof (UPROC, u_nfs));
	printf ("offsetof (FATSB0, u.fat_32.f_signature) = %d\n", offsetof (FATSB0, u.fat_32.f_signature));
	printf ("NFSDATA = %d\n", sizeof (struct nfsdata));
	printf ("FHANDLE = %d\n", sizeof (FHANDLE));
	printf ("FATTR = %d\n", sizeof (FATTR));
	printf ("offsetof (NFSDATA, d_par_ino) = %d\n", offsetof (NFSDATA, d_par_ino));
	printf ("cada VIDEO = %d\n", sizeof (VIDEODISPLAY));
	printf ("VIDEOLINE = %d\n", sizeof (VIDEOLINE));

	return (0);
}
