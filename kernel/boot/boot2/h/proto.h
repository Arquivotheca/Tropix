/*
 ****************************************************************
 *								*
 *			proto.h					*
 *								*
 *	Coleção dos protótipos das funções			*
 *								*
 *	Versão	3.0.0, de 04.07.94				*
 *		4.6.0, de 21.10.04				*
 *								*
 *	Módulo: Boot2						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

/*
 ******	Protótipos de funções ***********************************
 */
extern long	atol (const char *);
extern int	askyesno (int);
extern long	long_endian_cv (long);
extern int	short_endian_cv (int);
extern void	DELAY (int);
extern void	dmaexternal (int);
extern int	inflate (void);
extern void	fdattach (void);
extern int	fd_open (int);
extern void	fd_close (int);
extern void	fd_close_all (void);
extern void	fdisk (void);
extern void	fdisk_print_MB (daddr_t, int);
extern void	init_console (void);
extern long	little_to_big_long (long);
extern short	little_to_big_short (short);
extern int	get_all_parttb (void);
extern int	getchar (void);
extern char	*gets (char *);
extern int	haschar (void);
extern int	ideq (const char *, const char *);
extern int	memclr (void *, int);
extern int	memeq (const void *, const void *, int);
extern void	memmove (void *, const void *, int);
extern void	memset (void *, int, int);
extern void	memsetw (void *, int, int);
extern void	memsetl (void *, long, int);
extern void	pci_init (void);
extern void	printf (const char *, ...);
extern void	snprintf (char *, int, const char *, ...);
extern void	print_part_type_nm (int);
extern ulong	mul_div_64 (ulong, ulong, ulong);
extern void	prtable (void);
extern void	putchar (int);
extern long	read_TSC (void);
extern int	read_cmos (int);
extern int	read_port (int);
extern short	read_port_short (int);
extern long	read_port_long (int);
extern int	read_port_slowly (int);
extern void	read_port_string_long (int, void *, int);
extern void	read_port_string_short (int, void *, int);
extern int	read_port_very_slowly (int);
extern void	reset_cpu (void);
extern void	scsi_sort (void);
extern char	*scsi_str_conv (char *, const char *, int);
extern int	sdattach (void);
extern int	sdopen (int);
extern int	streq (const char *, const char *);
extern void	strcpy (char *, const char *);
extern char	*strend (const char *);
extern void	video_move_32 (unsigned short *, unsigned short *, int);
extern void	video_set_32  (unsigned short *, int, int);
extern void	ataattach (void);
extern void	*malloc_byte (int);
extern void	free_byte (void *);
extern void	sd_print_param (void);
extern long	xtol (const char *);
extern void	wait_for_ctl_Q (void);
extern void	write_port (int, int);
extern void	write_port_long (long, int);
extern void	write_port_short (short , int);
extern void	write_port_slowly (int, int);
extern void 	write_port_string_byte (int, const void *, int);
extern void 	write_port_string_short (int, const void *, int);
extern void 	write_port_string_long (int, const void *, int);
extern void	write_port_very_slowly (int, int);

#ifdef	ATA_H
extern int	ata_command (ATA_DEVICE *, int, daddr_t, int, int, int);
extern int	ata_wait (ATA_DEVICE *, int);
extern void	ata_bswap (char *, int);
extern void	ata_btrim (char *, int);
extern void	ata_bpack (char *, char *, int);
#endif	ATA_H

#ifdef	DISKTB_H
extern int	ataopen (const DISKTB *);
extern int	ahc_open (const DISKTB *);
extern int	bopen (const DISKTB *);
extern int	bread (const DISKTB *, daddr_t blkno, void *area, int count);
extern int	bctl (const DISKTB *up, int cmd, int arg);
extern int	bwrite (const DISKTB *, daddr_t blkno, void *area, int count);
extern int	getdisktb (DISKTB *, const char *);
extern int	atastrategy (BHEAD *);
extern int	fd_strategy (BHEAD *);
extern int	sdstrategy (BHEAD *);
extern int	scstrategy (BHEAD *);
extern int	ahc_strategy (BHEAD *);
extern int	ahc_ctl (const DISKTB *up, int cmd, int arg);
extern int	ata_ctl (const DISKTB *up, int cmd, int arg);
extern int	sd_ctl (const DISKTB *up, int cmd, int arg);
#endif	DISKTB_H

#if	defined (DISKTB_H) && defined (T1_H)
extern T1DINO	*t1_iget (const DISKTB *, ino_t inode);
extern T1DINO	*t1_iname (const DISKTB *, const char *);
#endif	defined (DISKTB_H)

#if	defined (DISKTB_H) && defined (CDFS_H)
extern CDDIR	*cd_iname (const DISKTB *, const char *);
#endif	defined (DISKTB_H)

#if	defined (DISKTB_H) && defined (V7_H)
extern V7DINO	*v7_iget (const DISKTB *, ino_t inode);
extern V7DINO	*v7_iname (const DISKTB *, const char *);
#endif	defined (DISKTB_H)

#if	defined (BCB_H) && defined (ATA_H)
extern int	geo_get_parttb_geo (const PARTTB *parttb, HDINFO *, daddr_t, const char *);
extern int	geo_tst_geometry (const PARTTB *parttb, const HDINFO *);
#endif	defined (BCB_H) && defined (ATA_H)

#ifdef	SCSI_H
extern int	scsi_attach (SCSI *);
extern int	scsi_inquiry (SCSI *);
extern int	scsi_open (SCSI *);
extern int	scsi_sense (SCSI *, int);
extern char	*scsi_cmd_name (int);
extern int	scsi_ctl (SCSI *sp, const DISKTB *up, int cmd, int arg);
#endif	SCSI_H
