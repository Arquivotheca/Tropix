/*
 ****************************************************************
 *								*
 *			info.h					*
 *								*
 *	Definições do formato do sistema de arquivos DOS	*
 *								*
 *	Versão	3.0.0, de 15.06.95				*
 *		3.0.0, de 15.06.95				*
 *								*
 *	Módulo: DOSMP						*
 *		Utilitários Sofisticados			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 		Obtido do "HELPPC"				*
 * 								*
 ****************************************************************
 */


/*
 ******	Boot sector: boot record ********************************
 *
 *
 *     Offset  Size		Description
 *
 *	00   3bytes	jump to executable code
 *	03   8bytes	OEM name and version
 *	0B   word	bytes per sector
 *	0D   byte	sectors per cluster (allocation unit size)
 *	0E   word	number of reserved sectors (starting at 0)
 *	10   byte	number of FAT's on disk
 *	11   word	number of root directory entries (directory size)
 *	13   word	number of total sectors (0 if partition > 32Mb)
 *	15   byte	media descriptor byte  (see "MEDIA DESCRIPTOR")
 *	16   word	sectors per FAT
 *	18   word	(DOS 3.0+) sectors per track
 *	1A   word	(DOS 3.0+) number of heads
 *	1C   word	(DOS 3.0+) number of hidden sectors
 *	20   dword	(DOS 4.0+) number of sectors if offset 13 was 0
 *	24   byte	(DOS 4.0+) physical drive number
 *	25   byte	(DOS 4.0+) reserved
 *	26   byte	(DOS 4.0+) signature byte (29h)
 *	27   dword	(DOS 4.0+) volume serial number
 *	2B  11bytes	(DOS 4.0+) volume label
 *	36   8bytes	(DOS 4.0+) reserved
 *	3E   ....	....
 *
 *
 *	- implementation format not guaranteed in all OEM DOS releases
 *	- BIOS expects a boot sector of 512 bytes
 *	- DOS 3.2 began reading BIOS Parameter Block ("BPB") information from
 *	  the boot sector, previous versions used only the media byte in FAT
 *	- DOS 4.x added offsets 20-3Dh and offset 20h determines the number
 *	  of sectors if offset 13h is zero
 *	- hard disks have a master boot record and partition boot records;
 *	  the master boot record and "Disk Partition Table" (DPT) share the
 *	  same sector
 */

/*
 ******	Media Descriptor Byte ***********************************
 *
 *	|7|6|5|4|3|2|1|0| IBM Media Descriptor Byte
 *	 | | | | | | | |
 *	 | | | | | | | +-- 1 = 2 sided, 0 = not 2 sided
 *	 | | | | | | +---- 1 = 8 sector, 0 = not 8 sector
 *	 | | | | | +------ 1 = removable, 0 = not removable
 *	 | | | | |
 *	 +-+-+-+-+------- must be set to 1
 *
 *	F8	Hard disk
 *		Double sided  18 sector diskette  PS/2 1.44 Mb. DSQD
 *
 *	F9	5"  Double sided, High Density diskette (15 sector)
 *		3"  Double Sided, Double High Density diskette (9 sector)
 *
 *	FA	Ram disk (not all Ramdisks use this)
 *
 *	FC	5" Single Sided, Double Density diskette (9 sector)
 *		8" inch Double Sided, Single Density diskette
 *
 *	FD	5" Double Sided, Double Density (9 sector)
 *		8" inch Double Sided Single Density (26 sector) (IBM 3740
 *		   format) 128 bytes per sector, soft sector, 4 sectors
 *		   cluster, 4 reserved sectors, 2 FATs, 68 directory entries,
 *		   77*26*2 sectors.
 *
 *	FE	5" Single Sided, Double Density diskette (8 sector)
 *		8" inch Single Sided, Single Density diskette (26 sector)
 *		   (IBM 3740 format) 128 bytes per sector, soft sector,
 *		   4 sectors per cluster, 1 reserved sector, 2 FATs. 68
 *		   directory entries, 77*26 sectors.
 *		8" inch Double Sided, Double Density (8 sector), 1024 bytes
 *		   per sector, soft sector, 1 sector per cluster, 1 reserved
 *		   sector, 2 FATs, 192 directory entries, 77*8*2 sectors. To
 *		   distinguish from 8 inch SS/SD attempt read of side 2.
 *
 *	FF	5" Double Sided, Double Density diskette (8 sector)
 */

/*
 ******	FAT: File Allocation Table ******************************
 *
 *
 * 	12 Bit			  Meaning		    16 Bit
 *
 *	 000			 free space		     0000
 *	 FF1-FF7	      bad track marking		     FFF1-FFF7
 *	 FF8-FFE   may be used to mark end of a file chain   FFF8-FFFE
 *	 FFF	   standard marker for end of a file chain   FFFF
 *
 *
 *	- the FAT is implemented as an array containing a linked list
 *	  for each file;  the files directory entry has a pointer to the
 *	  first cluster which contains the cluster number of the next
 *	  cluster in the chain until the pointer contained is FFFh
 *	  (12 bit FAT) and FFFFh (16 bit FAT) marking end of file
 *	- DOS maintains two copies of the FAT, but does not use the
 *	  second copy for anything other than a mirror image of the
 *	  first;  CHKDSK doesn't even read the second FAT
 *	- disks with FF1h clusters and above use 16 bit FAT tables, disk
 *	  with less use 12 bit FAT tables
 *	- DOS 4.x did not change the size of the cluster number as some
 *	  suggest, but instead increased the size of the sector number
 *	- bytes 0 of the FAT contains the Media Descriptor Byte
 *
 *
 * Calculating 12 bit FAT Entries
 *
 *	1. Get starting cluster from directory entry.
 *
 *	2. Multiply the cluster number just used by 1.5
 *
 *	3. The whole part of the product is the offset into the FAT,
 *	   of the entry that maps to the cluster in the directory.
 *	   This entry contains the number of the next cluster.
 *
 *	4. Move the word at the calculated FAT into a register.
 *
 *	5. If the last cluster used was an even number, keep the low order
 *	   12 bits of the register, otherwise, keep the high order 12 bits.
 *
 *	6. If the resultant 12 bits are (0FF8h-0FFFh) no more clusters
 *	   are in the file.  Otherwise, the next 12 bits contain the
 *	   cluster number of the next cluster in the file.
 *
 *
 * Calculating 16 Bit FAT Entries
 *
 *	1. Get the starting cluster of the file from the directory.
 *
 *	2. Multiply the cluster number found by 2.
 *
 *	3. Load the word at the calculated FAT offset into a register.
 *
 *	4. If the 16 bits are (0FFF8h-0FFFFh) no more clusters are in
 *	   the file. Otherwise, the 16 bits contain the cluster number
 *	   of the next cluster in the file.
 *
 *
 *	To convert the cluster to a logical sector number (relative
 *	sector, similar to that used by DEBUG, int 25h and 26h):
 *
 *	1. Subtract 2 from the cluster number
 *	2. Multiply the result by the number of sectors per cluster.
 *	3. Add the logical sector number of the beginning of the data area.
 *
 */
