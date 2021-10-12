#*
#****************************************************************
#*	Makefile para o programa "tropix"			*
#****************************************************************
#*
DIR	= boot emer kernel 
PGM	= tropix
PROT	= u+x
DEV	= d5a
DEF	= -D PC -D LITTLE_ENDIAN
CC	= cc
AS	= as
LD	= ld
CHECKSUM = checksum.486
ADDR	= 0x80000000

.virtual	c modif lint sz cp cmp cmpq shift pento penfrom pensave

OBJ =	 	obj/ahc_pci.o			\
		obj/aic7xxx.o			\
						\
		obj/ata.o			\
#		obj/ataahci.o			\
		obj/ataall.o			\
		obj/ataatapi.o			\
		obj/atadma.o			\
		obj/ataintel.o			\
		obj/atajmicron.o		\
		obj/atapci.o			\
		obj/atasis.o			\
		obj/atavia.o			\
						\
#		obj/bget.o			\
		obj/bread.o			\
						\
		obj/dcache.o			\
						\
		obj/cddir.o			\
		obj/cdinode.o			\
		obj/cdmount.o			\
		obj/cdread.o			\
		obj/cdrr.o			\
						\
		obj/c.o				\
						\
		obj/arit.o			\
		obj/boot.o			\
		obj/ctxt.o			\
		obj/excep.s.o			\
		obj/ffbs.o			\
		obj/fpu.o			\
#		obj/id.o			\
		obj/idle.o			\
		obj/intr.o			\
		obj/mem.o			\
		obj/pg.o			\
		obj/port.o			\
		obj/start.o			\
		obj/str.o			\
		obj/umove.o			\
		obj/vector.o			\
		obj/video_iso_tb.o		\
						\
		obj/ncon.o			\
		obj/disktb.o			\
		obj/dma.o			\
		obj/dsort.o			\
		obj/ed.o			\
		obj/fd.o			\
		obj/key.o			\
		obj/live.o			\
		obj/lp.o			\
		obj/memdev.o			\
		obj/meta_dos.o			\
		obj/pci.o			\
		obj/physio.o			\
		obj/pnp.o			\
		obj/ppp.vj.o			\
		obj/ppp.sio.o			\
		obj/ps2m.o			\
		obj/pseudo_dev.o		\
		obj/pty.o			\
		obj/ramd.o			\
		obj/rtl.o			\
		obj/sb.o			\
		obj/screen_saver.o		\
		obj/scsi.o			\
		obj/sd.1542.o			\
		obj/sio.o			\
		obj/slip.sio.o			\
		obj/tty.o			\
		obj/video.o			\
#		obj/vt100.o			\
		obj/xcon.o			\
		obj/zip.o			\
						\
		obj/ext2blk.o			\
		obj/ext2dir.o			\
		obj/ext2inode.o			\
		obj/ext2mount.o			\
		obj/ext2read.o			\
						\
		obj/fatdir.o			\
		obj/fatfat.o			\
		obj/fatinode.o			\
		obj/fatlfn.o			\
		obj/fatmount.o			\
		obj/fatread.o			\
						\
		obj/etc.o			\
		obj/cmos.clock.o		\
		obj/dump.o			\
		obj/gets.o			\
		obj/kinit.o			\
		obj/main.o			\
		obj/printf.o			\
		obj/scb.o			\
		obj/spy.o			\
		obj/stdio.o			\
		obj/lowertoupper.o		\
		obj/uppertolower.o		\
						\
		obj/file.o			\
		obj/inode.o			\
		obj/iname.o			\
		obj/lockf.o			\
#		obj/text.o			\
						\
		obj/nfs2_aux.o			\
		obj/nfs2c_dir.o			\
		obj/nfs2c_inode.o		\
		obj/nfs2c_mnt.o			\
		obj/nfs2c_read.o		\
		obj/nfs2s_dir.o			\
		obj/nfs2s_mnt.o			\
		obj/nfs2s_read.o		\
		obj/nfs2s_stat.o		\
		obj/nfs2s_tree.o		\
		obj/rpc.o			\
						\
		obj/ntattr.o			\
		obj/ntdir.o			\
		obj/ntinode.o			\
		obj/ntmount.o			\
		obj/ntread.o			\
						\
		obj/iblk.o			\
		obj/ichr.o			\
		obj/inone.o			\
						\
		obj/$CHECKSUM.o			\
		obj/daemon.o			\
		obj/dhcp.o			\
		obj/ep.o			\
		obj/ether.o			\
		obj/frame.o			\
		obj/icmp.o			\
		obj/ip.o			\
		obj/itblock.o			\
		obj/itn.o			\
		obj/kaddr.o			\
		obj/kbind.o			\
		obj/kinfo.o			\
		obj/ksnd.o			\
		obj/psnd.o			\
		obj/raw.o			\
		obj/rcv.o			\
		obj/rnd.o			\
		obj/snd.o			\
		obj/udp.o			\
						\
		obj/malloc.o			\
		obj/malloc_byte.o		\
		obj/mmu.o			\
		obj/region.o			\
		obj/text.o			\
						\
		obj/pipeinode.o			\
		obj/piperead.o			\
						\
		obj/clock.o			\
		obj/dispat.o			\
		obj/excep.o			\
		obj/kexit.o			\
		obj/nproc.o			\
		obj/signal.o			\
		obj/swap.o			\
		obj/syscall.o			\
						\
		obj/event.o			\
		obj/ipc.o			\
		obj/sema.o			\
		obj/sleep.o			\
		obj/spinlock.o			\
		obj/sync.o			\
						\
		obj/sysctl.o			\
		obj/sysdir.o			\
		obj/sysent.o			\
		obj/sysetc.o			\
		obj/sysexec.o			\
		obj/sysfio.o			\
		obj/sysfork.o			\
		obj/sysipc.o			\
		obj/syslib.o			\
		obj/sysproc.o			\
		obj/sysstat.o			\
		obj/systime.o			\
		obj/systree.o			\
		obj/sysuser.o			\
						\
		obj/t1blk.o			\
		obj/t1dir.o			\
		obj/t1inode.o			\
		obj/t1mount.o			\
		obj/t1read.o			\
						\
		obj/chartb.o			\
		obj/clist.o			\
		obj/isotoatb.o			\
		obj/isotoisotb.o		\
		obj/isotou1tb.o			\
		obj/isotou2tb.o			\
		obj/ttyctl.o			\
		obj/ttyin.o			\
		obj/ttyread.o			\
						\
		obj/bus.o			\
#		obj/kobj.o			\
		obj/ehci.o			\
		obj/ohci.o			\
		obj/ud.o			\
		obj/uhci.o			\
		obj/uhub.o			\
		obj/ulp.o			\
		obj/usb.o			\
		obj/usb_subr.o			\
		obj/usbdi.o			\
		obj/usbdi_util.o		\
		obj/quirks.o			\
		obj/ums.o			\
		obj/hid.o			\
						\
		obj/v7blk.o			\
		obj/v7dir.o			\
		obj/v7inode.o			\
		obj/v7mount.o			\
		obj/v7read.o


SRC = 		aic/ahc_pci.c			\
		aic/aic7xxx.c			\
		aic/aicasm.c			\
		aic/aicasm.lex.c		\
		aic/aicasm.tab.c		\
		aic/aicasm_sym.c		\
		ata/ata.c			\
		ata/ataall.c			\
		ata/ataatapi.c			\
		ata/atadma.c			\
		ata/ataintel.c			\
		ata/atajmicron.c		\
		ata/atapci.c			\
		ata/atasis.c			\
		ata/atavia.c			\
		bio/bread.c			\
		bio/dcache.c			\
		cdfs/cddir.c			\
		cdfs/cdinode.c			\
		cdfs/cdmount.c			\
		cdfs/cdread.c			\
		cdfs/cdrr.c			\
		conf/c.c			\
		dev/disktb.c			\
		dev/dma.c			\
		dev/dsort.c			\
		dev/ed.c			\
		dev/fd.c			\
		dev/key.c			\
		dev/live.c			\
		dev/lp.c			\
		dev/memdev.c			\
		dev/meta_dos.c			\
		dev/ncon.c			\
		dev/pci.c			\
		dev/physio.c			\
		dev/pnp.c			\
		dev/ppp.sio.c			\
		dev/ppp.vj.c			\
		dev/ps2m.c			\
		dev/pseudo_dev.c		\
		dev/pty.c			\
		dev/ramd.c			\
		dev/rtl.c			\
		dev/sb.c			\
		dev/screen_saver.c		\
		dev/scsi.c			\
		dev/sd.1542.c			\
		dev/sio.c			\
		dev/slip.sio.c			\
		dev/tty.c			\
		dev/video.c			\
#		dev/vt100.c			\
		dev/xcon.c			\
		dev/zip.c			\
		etc/cmos.clock.c		\
		etc/dump.c			\
		etc/etc.c			\
		etc/gets.c			\
		etc/kinit.c			\
		etc/lowertoupper.c		\
		etc/main.c			\
		etc/printf.c			\
		etc/scb.c			\
		etc/spy.c			\
		etc/stdio.c			\
		etc/uppertolower.c		\
		ext2fs/ext2blk.c		\
		ext2fs/ext2dir.c		\
		ext2fs/ext2inode.c		\
		ext2fs/ext2mount.c		\
		ext2fs/ext2read.c		\
		fatfs/fatdir.c			\
		fatfs/fatfat.c			\
		fatfs/fatinode.c		\
		fatfs/fatlfn.c			\
		fatfs/fatmount.c		\
		fatfs/fatread.c			\
		fio/file.c			\
		fio/iname.c			\
		fio/inode.c			\
		fio/lockf.c			\
		invalfs/iblk.c			\
		invalfs/ichr.c			\
		invalfs/inone.c			\
		itnet/daemon.c			\
		itnet/dhcp.c			\
		itnet/ep.c			\
		itnet/ether.c			\
		itnet/frame.c			\
		itnet/icmp.c			\
		itnet/ip.c			\
		itnet/itblock.c			\
		itnet/itn.c			\
		itnet/kaddr.c			\
		itnet/kbind.c			\
		itnet/kinfo.c			\
		itnet/ksnd.c			\
		itnet/psnd.c			\
		itnet/raw.c			\
		itnet/rcv.c			\
		itnet/rnd.c			\
		itnet/snd.c			\
		itnet/udp.c			\
		mem/malloc.c			\
		mem/malloc_byte.c		\
		mem/mmu.c			\
		mem/region.c			\
		mem/text.c			\
						\
		nfs/nfs2_aux.c			\
		nfs/nfs2c_dir.c			\
		nfs/nfs2c_inode.c		\
		nfs/nfs2c_mnt.c			\
		nfs/nfs2c_read.c		\
		nfs/nfs2s_dir.c			\
		nfs/nfs2s_mnt.c			\
		nfs/nfs2s_read.c		\
		nfs/nfs2s_stat.c		\
		nfs/nfs2s_tree.c		\
		nfs/rpc.c			\
						\
		ntfs/ntattr.c			\
		ntfs/ntdir.c			\
		ntfs/ntinode.c			\
		ntfs/ntmount.c			\
		ntfs/ntread.c			\
		proc/clock.c			\
		proc/dispat.c			\
		proc/excep.c			\
		proc/kexit.c			\
		proc/nproc.c			\
		proc/signal.c			\
		proc/swap.c			\
		proc/syscall.c			\
		sync/event.c			\
		sync/ipc.c			\
		sync/sema.c			\
		sync/sleep.c			\
		sync/sync.c			\
		sys/sysctl.c			\
		sys/sysdir.c			\
		sys/sysent.c			\
		sys/sysetc.c			\
		sys/sysexec.c			\
		sys/sysfio.c			\
		sys/sysfork.c			\
		sys/sysipc.c			\
		sys/syslib.c			\
		sys/sysproc.c			\
		sys/sysstat.c			\
		sys/systime.c			\
		sys/systree.c			\
		sys/sysuser.c			\
		t1fs/t1blk.c			\
		t1fs/t1inode.c			\
		t1fs/t1mount.c			\
		t1fs/t1read.c			\
		tty/chartb.c			\
		tty/clist.c			\
		tty/isotoatb.c			\
		tty/isotoisotb.c		\
		tty/isotou1tb.c			\
		tty/isotou2tb.c			\
		tty/ttyctl.c			\
		tty/ttyin.c			\
		tty/ttyread.c			\
		usb/ehci.c			\
		usb/ohci.c			\
		usb/uhci.c			\
		usb/uhub.c			\
		usb/bus.c			\
		v7fs/v7blk.c			\
		v7fs/v7dir.c			\
		v7fs/v7inode.c			\
		v7fs/v7mount.c			\
		v7fs/v7read.c

#*
#******	Link-edição *********************************************
#*
$PGM:		obj l $OBJ
		$LD -u -T $ADDR -o $PGM $OBJ
		size $PGM
		ls -l $PGM

obj:		.exist
		mkdir obj

l:		.exist
		mkdir l

c:		obj $OBJ

#*
#****** Montagens ***********************************************
#*
obj/arit.o:	ctl/arit.s
		$AS -l l/arit.l -o obj/arit.o ctl/arit.s

obj/start.o:	ctl/start.s ctl/common.s
		$AS -l l/start.l -o obj/start.o ctl/common.s ctl/start.s

obj/excep.s.o:	ctl/excep.s ctl/common.s
		$AS -l l/excep.l -o obj/excep.s.o ctl/common.s ctl/excep.s

obj/ffbs.o:	ctl/ffbs.s
		$AS -l l/ffbs.l -o obj/ffbs.o ctl/ffbs.s

obj/fpu.o:	ctl/fpu.s ctl/common.s
		$AS -l l/fpu.l -o obj/fpu.o ctl/common.s ctl/fpu.s

obj/vector.o:	ctl/vector.s ctl/pat.s ctl/common.s
		$AS -l l/vector.l -o obj/vector.o ctl/common.s ctl/pat.s ctl/vector.s

obj/video_iso_tb.o:	ctl/video_iso_tb.s
		$AS -l l/video_iso_tb.l -o obj/video_iso_tb.o ctl/common.s ctl/pat.s ctl/video_iso_tb.s

obj/intr.o:	ctl/intr.s ctl/pat.s ctl/common.s
		$AS -l l/intr.l -o obj/intr.o ctl/common.s ctl/pat.s ctl/intr.s

obj/boot.o:	ctl/boot.s ctl/common.s
		$AS -l l/boot.l -o obj/boot.o ctl/common.s ctl/boot.s

obj/ctxt.o:	ctl/ctxt.s ctl/common.s
		$AS -l l/ctxt.l -o obj/ctxt.o ctl/common.s ctl/ctxt.s

obj/mem.o:	ctl/mem.s
		$AS -l l/mem.l -o obj/mem.o ctl/mem.s

obj/pg.o:	ctl/pg.s ctl/common.s
		$AS -l l/pg.l -o obj/pg.o ctl/common.s ctl/pg.s

obj/str.o:	ctl/str.s
		$AS -l l/str.l -o obj/str.o ctl/str.s

obj/umove.o:	ctl/umove.s ctl/common.s
		$AS -l l/umove.l -o obj/umove.o ctl/common.s ctl/umove.s

#obj/id.o:	ctl/id.s
#		$AS -l l/id.l -o obj/id.o ctl/id.s

obj/idle.o:	ctl/idle.s ctl/common.s
		$AS -l l/idle.l -o obj/idle.o ctl/common.s ctl/idle.s

obj/port.o:	ctl/port.s ctl/common.s
		$AS -l l/port.l -o obj/port.o ctl/common.s ctl/port.s

#*
#****** Diretório "aic" *****************************************
#*
obj/ahc_pci.o:	aic/ahc_pci.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/intr.h		\
		h/pci.h			\
		h/bhead.h		\
		h/scsi.h		\
		h/devhead.h		\
		h/uerror.h		\
		aic/aic7xxx_seq.h	\
		aic/aic7xxx.h		\
		aic/asm_format.h	\
		aic/seq.out	
		$CC -c -O $DEF -o obj aic/ahc_pci.c

obj/aic7xxx.o:	aic/aic7xxx.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/pci.h			\
		h/frame.h		\
		h/disktb.h		\
		h/scsi.h		\
		h/kfile.h		\
		h/inode.h		\
		h/devhead.h		\
		h/bhead.h		\
		h/cdio.h		\
		h/signal.h		\
		h/uproc.h		\
		h/uerror.h		\
		aic/aic7xxx_seq.h	\
		aic/aic7xxx.h	
		$CC -c -O $DEF -o obj aic/aic7xxx.c

#*
#****** Montador do diretório "aic" *****************************
#*
aic/aic7xxx_seq.h 	\
aic/seq.out:	aic/aic7xxx.seq.v aic/aic7xxx.reg.h aic/aicasm
		aic/aicasm -o aic/seq.out -r aic/aic7xxx_seq.h aic/aic7xxx.seq.v

aic/aicasm:	aic/aicasm.o aic/aicasm.tab.o aic/aicasm.lex.o aic/aicasm_sym.o
		$CC $DEF -u -s -o aic/aicasm aic/aicasm.o aic/aicasm.tab.o aic/aicasm.lex.o aic/aicasm_sym.o

aic/aicasm.o:	aic/aicasm.c		\
		h/types.h		\
		h/syscall.h		\
		h/queue.h		\
		aic/aicasm.h		\
		aic/asm_symbol.h	\
		aic/asm_format.h	\
		aic/sysexits.h	
		$CC -c -O -o aic aic/aicasm.c

aic/aicasm.tab.o: aic/aicasm.tab.c	\
		h/types.h		\
		h/queue.h		\
		aic/sysexits.h		\
		aic/aicasm.h		\
		aic/asm_symbol.h	\
		aic/asm_format.h	
		$CC -c -O -o aic aic/aicasm.tab.c

aic/aicasm.tab.c: aic/aicasm.y
		bison -d aic/aicasm.y

aic/aicasm.lex.o: aic/aicasm.lex.c	\
		h/types.h		\
		h/queue.h		\
		aic/sysexits.h		\
		aic/aicasm.h		\
		aic/asm_symbol.h	\
		aic/aicasm.tab.h	
		$CC -c -O -o aic aic/aicasm.lex.c

aic/aicasm.lex.c: aic/aicasm.l.v
		lex -oaic/aicasm.lex.c aic/aicasm.l.v

aic/aicasm_sym.o: aic/aicasm_sym.c	\
		h/types.h		\
		h/queue.h		\
		aic/sysexits.h		\
		aic/asm_symbol.h	\
		aic/aicasm.h	
		$CC -c -O -o aic aic/aicasm_sym.c

#*
#****** Diretório "ata" *****************************************
#*
obj/ata.o:	ata/ata.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/map.h			\
		h/pci.h			\
		h/frame.h		\
		h/intr.h		\
		h/disktb.h		\
		h/kfile.h		\
		h/inode.h		\
		h/devhead.h		\
		h/bhead.h		\
		h/ioctl.h		\
		h/signal.h		\
		h/uproc.h		\
		h/uerror.h		\
		h/scsi.h		\
		h/ata.h			\
		h/cdio.h
		$CC -c -O $DEF -o obj ata/ata.c

obj/ataahci.o:	ata/ataahci.c		\
		h/common.h		\
		h/sync.h		\
		h/pci.h			\
		h/scsi.h		\
		h/devhead.h		\
		h/ata.h
		$CC -c -O $DEF -o obj ata/ataahci.c

obj/ataall.o:	ata/ataall.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/devhead.h		\
		h/bhead.h		\
		h/disktb.h		\
		h/devmajor.h		\
		h/pci.h			\
		h/scsi.h		\
		h/ata.h
		$CC -c -O $DEF -o obj ata/ataall.c

obj/ataatapi.o:	ata/ataatapi.c		\
		h/common.h		\
		h/sync.h		\
		h/pci.h			\
		h/scsi.h		\
		h/devhead.h		\
		h/bhead.h		\
		h/ata.h
		$CC -c -O $DEF -o obj ata/ataatapi.c

obj/atadma.o:	ata/atadma.c		\
		h/common.h		\
		h/sync.h		\
		h/map.h			\
		h/pci.h			\
		h/scsi.h		\
		h/devhead.h		\
		h/bhead.h		\
		h/ata.h
		$CC -c -O $DEF -o obj ata/atadma.c

obj/ataintel.o:	ata/ataintel.c		\
		h/common.h		\
		h/sync.h		\
		h/pci.h			\
		h/scsi.h		\
		h/devhead.h		\
		h/ata.h
		$CC -c -O $DEF -o obj ata/ataintel.c

obj/atajmicron.o: ata/atajmicron.c	\
		h/common.h		\
		h/sync.h		\
		h/pci.h			\
		h/scsi.h		\
		h/devhead.h		\
		h/ata.h
		$CC -c -O $DEF -o obj ata/atajmicron.c

obj/atapci.o:	ata/atapci.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/devhead.h		\
		h/pci.h			\
		h/scsi.h		\
		h/ata.h
		$CC -c -O $DEF -o obj ata/atapci.c

obj/atasis.o:	ata/atasis.c		\
		h/common.h		\
		h/sync.h		\
		h/pci.h			\
		h/scsi.h		\
		h/devhead.h		\
		h/ata.h
		$CC -c -O $DEF -o obj ata/atasis.c

obj/atavia.o:	ata/atavia.c		\
		h/common.h		\
		h/sync.h		\
		h/devhead.h		\
		h/bhead.h		\
		h/scsi.h		\
		h/ata.h
		$CC -c -O $DEF -o obj ata/atavia.c

#*
#****** Diretório "bio" *****************************************
#*
obj/bread.o:	bio/bread.c		\
		h/common.h		\
		h/scb.h			\
		h/sync.h		\
		h/region.h		\
		h/devhead.h		\
		h/bhead.h		\
		h/iotab.h		\
		h/signal.h		\
		h/uproc.h		\
		h/uerror.h		\
		h/mon.h
		$CC -c -O $DEF -o obj bio/bread.c

obj/dcache.o:	bio/dcache.c		\
		h/common.h		\
		h/scb.h			\
		h/sync.h		\
		h/bhead.h		\
		h/devmajor.h		\
		h/mon.h			\
		h/region.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj bio/dcache.c

#*
#****** Diretório "cdfs" *****************************************
#*
obj/cddir.o:	cdfs/cddir.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/mntent.h		\
		h/sb.h			\
		h/kfile.h		\
		h/cdfs.h		\
		h/sysdirent.h		\
		h/inode.h		\
		h/stat.h		\
		h/bhead.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj cdfs/cddir.c

obj/cdinode.o:	cdfs/cdinode.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/mntent.h		\
		h/sb.h			\
		h/inode.h		\
		h/bhead.h		\
		h/sysdirent.h		\
		h/cdfs.h		\
		h/stat.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj cdfs/cdinode.c

obj/cdmount.o:	cdfs/cdmount.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/map.h			\
		h/mntent.h		\
		h/sb.h			\
		h/cdfs.h		\
		h/cdio.h		\
		h/ustat.h		\
		h/iotab.h		\
		h/inode.h		\
		h/bhead.h		\
		h/kfile.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj cdfs/cdmount.c

obj/cdread.o:	cdfs/cdread.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/iotab.h		\
		h/kfile.h		\
		h/cdfs.h		\
		h/inode.h		\
		h/bhead.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj cdfs/cdread.c

obj/cdrr.o:	cdfs/cdrr.c		\
		h/common.h		\
		h/sync.h		\
		h/region.h		\
		h/mntent.h		\
		h/sb.h			\
		h/cdfs.h		\
		h/inode.h		\
		h/bhead.h		\
		h/signal.h		\
		h/uproc.h		\
		h/uerror.h
		$CC -c -O $DEF -o obj cdfs/cdrr.c

#*
#****** Diretório "conf" *****************************************
#*
obj/c.o:	conf/c.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/inode.h		\
		h/sysdirent.h		\
		h/iotab.h
		$CC -c -O $DEF -o obj conf/c.c

#*
#****** Diretório "dev" *****************************************
#*
obj/disktb.o:	dev/disktb.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/bcb.h			\
		h/region.h		\
		h/bhead.h		\
		h/disktb.h		\
		h/scsi.h		\
		h/signal.h		\
		h/uproc.h		\
		h/uerror.h
		$CC -c -O $DEF -o obj dev/disktb.c

obj/dma.o:	dev/dma.c		\
		h/common.h		\
		h/scb.h			\
		h/sync.h		\
		h/bhead.h		\
		h/region.h		\
		h/signal.h		\
		h/uerror.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj dev/dma.c

obj/dsort.o:	dev/dsort.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/bhead.h		\
		h/devhead.h
		$CC -c -O $DEF -o obj dev/dsort.c

obj/ed.o:	dev/ed.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/itnet.h		\
		h/ed.h			\
		h/tty.h			\
		h/pci.h			\
		h/frame.h		\
		h/intr.h		\
		h/kfile.h		\
		h/inode.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj dev/ed.c

obj/fd.o:	dev/fd.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/cpu.h			\
		h/frame.h		\
		h/disktb.h		\
		h/bhead.h		\
		h/devhead.h		\
		h/intr.h		\
		h/kfile.h		\
		h/inode.h		\
		h/ioctl.h		\
		h/timeout.h		\
		h/signal.h		\
		h/uproc.h		\
		h/uerror.h
		$CC -c -O $DEF -o obj dev/fd.c

obj/key.o:	dev/key.c		\
		h/common.h		\
		h/scb.h			\
		h/kbd.h
		$CC -c -O $DEF -o obj dev/key.c

obj/live.o:	dev/live.c		\
		h/common.h		\
		h/sync.h		\
		h/region.h		\
		h/pci.h			\
		h/map.h			\
		h/live.h		\
		h/frame.h		\
		h/intr.h		\
		h/scb.h			\
		h/map.h			\
		h/itnet.h		\
		h/ed.h			\
		h/tty.h			\
		h/kfile.h		\
		h/timeout.h		\
		h/inode.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj dev/live.c

obj/lp.o:	dev/lp.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/frame.h		\
		h/cpu.h			\
		h/intr.h		\
		h/tty.h			\
		h/kfile.h		\
		h/inode.h		\
		h/timeout.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj dev/lp.c

obj/memdev.o:	dev/memdev.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/inode.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h		\
		h/ioctl.h
		$CC -c -O $DEF -o obj dev/memdev.c

obj/meta_dos.o:	dev/meta_dos.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/disktb.h		\
		h/bhead.h		\
		h/iotab.h		\
		h/map.h			\
		h/devhead.h		\
		h/devmajor.h		\
		h/kfile.h		\
		h/inode.h		\
		h/ioctl.h		\
		h/signal.h		\
		h/uproc.h		\
		h/uerror.h
		$CC -c -O $DEF -o obj dev/meta_dos.c

obj/ncon.o:	dev/ncon.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/frame.h		\
		h/kfile.h		\
		h/inode.h		\
		h/tty.h			\
		h/kbd.h			\
		h/video.h		\
		h/ioctl.h		\
		h/intr.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj dev/ncon.c

obj/pci.o:	dev/pci.c		\
		h/common.h		\
		h/pci.h
		$CC -c -O $DEF -o obj dev/pci.c

obj/physio.o:	dev/physio.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/inode.h		\
		h/bhead.h		\
		h/map.h			\
		h/signal.h		\
		h/uproc.h		\
		h/uerror.h
		$CC -c -O $DEF -o obj dev/physio.c

obj/pnp.o:	dev/pnp.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h
		$CC -c -O $DEF -o obj dev/pnp.c

obj/ppp.sio.o:	dev/ppp.sio.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/frame.h		\
		h/kfile.h		\
		h/inode.h		\
		h/sio.h			\
		h/tty.h			\
		h/ioctl.h		\
		h/intr.h		\
		h/uerror.h		\
		h/signal.h		\
		h/devmajor.h		\
		h/uproc.h		\
		h/itnet.h		\
		h/ppp.h
		$CC -c -O $DEF -o obj dev/ppp.sio.c

obj/ppp.vj.o:	dev/ppp.vj.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/itnet.h		\
		h/ppp.h
		$CC -c -O $DEF -o obj dev/ppp.vj.c

obj/ps2m.o:	dev/ps2m.c		\
		h/common.h		\
		h/sync.h		\
		h/region.h		\
		h/kfile.h		\
		h/tty.h			\
		h/kbd.h			\
		h/frame.h		\
		h/intr.h		\
		h/ioctl.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj dev/ps2m.c

obj/pseudo_dev.o: dev/pseudo_dev.c	\
		h/common.h		\
		h/sync.h		\
		h/region.h		\
		h/mntent.h		\
		h/sb.h			\
		h/disktb.h		\
		h/bhead.h		\
		h/inode.h		\
		h/iotab.h		\
		h/devmajor.h		\
		h/devhead.h		\
		h/t1.h			\
		h/fat.h			\
		h/cdfs.h		\
		h/signal.h		\
		h/uproc.h		\
		h/uerror.h
		$CC -c -O $DEF -o obj dev/pseudo_dev.c

obj/pty.o:	dev/pty.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/devmajor.h		\
		h/tty.h			\
		h/ioctl.h		\
		h/kfile.h		\
		h/inode.h		\
		h/sysdirent.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj dev/pty.c

obj/ramd.o:	dev/ramd.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/disktb.h		\
		h/devhead.h		\
		h/devmajor.h		\
		h/bhead.h		\
		h/inode.h		\
		h/ioctl.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj dev/ramd.c

obj/rtl.o:	dev/rtl.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/map.h			\
		h/itnet.h		\
		h/ed.h			\
		h/tty.h			\
		h/pci.h			\
		h/frame.h		\
		h/intr.h		\
		h/kfile.h		\
		h/inode.h		\
		h/timeout.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj dev/rtl.c

obj/sb.o:	dev/sb.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/pci.h			\
		h/inode.h		\
		h/frame.h		\
		h/mmu.h			\
		h/intr.h		\
		h/ioctl.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj dev/sb.c

obj/screen_saver.o:	dev/screen_saver.c	\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/tty.h			\
		h/video.h
		$CC -c -O $DEF -o obj dev/screen_saver.c

obj/scsi.o:	dev/scsi.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/map.h			\
		h/disktb.h		\
		h/kfile.h		\
		h/bhead.h		\
		h/scsi.h		\
		h/ioctl.h		\
		h/signal.h		\
		h/uproc.h		\
		h/uerror.h		\
		h/cdio.h
		$CC -c -O $DEF -D VERBOSE -o obj dev/scsi.c

obj/sd.1542.o:	dev/sd.1542.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/map.h			\
		h/frame.h		\
		h/intr.h		\
		h/disktb.h		\
		h/scsi.h		\
		h/kfile.h		\
		h/inode.h		\
		h/devhead.h		\
		h/bhead.h		\
		h/ioctl.h		\
		h/signal.h		\
		h/uproc.h		\
		h/uerror.h		\
		h/cdio.h
		$CC -c -O $DEF -o obj dev/sd.1542.c

obj/sio.o:	dev/sio.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/pci.h			\
		h/frame.h		\
		h/kfile.h		\
		h/inode.h		\
		h/sio.h			\
		h/tty.h			\
		h/ioctl.h		\
		h/intr.h		\
		h/devmajor.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj dev/sio.c

obj/slip.sio.o:	dev/slip.sio.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/frame.h		\
		h/kfile.h		\
		h/inode.h		\
		h/sio.h			\
		h/tty.h			\
		h/ioctl.h		\
		h/intr.h		\
		h/devmajor.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h		\
		h/itnet.h
		$CC -c -O $DEF -o obj dev/slip.sio.c

obj/tty.o:	dev/tty.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/kfile.h		\
		h/inode.h		\
		h/tty.h			\
		h/iotab.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj dev/tty.c

obj/video.o:	dev/video.c		\
		h/common.h		\
		h/scb.h			\
		h/sync.h		\
		h/cpu.h			\
		h/video.h		\
		h/timeout.h
		$CC -c -O $DEF -o obj dev/video.c

#obj/vt100.o:	dev/vt100.c		\
#		h/common.h		\
#		h/scb.h			\
#		h/video.h
#		$CC -c -O $DEF -o obj dev/vt100.c

obj/xcon.o:	dev/xcon.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/frame.h		\
		h/kfile.h		\
		h/inode.h		\
		h/tty.h			\
		h/video.h		\
		h/ioctl.h		\
		h/intr.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj dev/xcon.c

obj/zip.o:	dev/zip.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/cpu.h			\
		h/frame.h		\
		h/intr.h		\
		h/disktb.h		\
		h/kfile.h		\
		h/inode.h		\
		h/devhead.h		\
		h/bhead.h		\
		h/scsi.h		\
		h/ioctl.h		\
		h/timeout.h		\
		h/signal.h		\
		h/uproc.h		\
		h/uerror.h
		$CC -c -O $DEF -o obj dev/zip.c

#*
#****** Diretório "etc" *****************************************
#*
obj/cmos.clock.o: etc/cmos.clock.c	\
		h/common.h		\
		h/sync.h		\
		h/scb.h
		$CC -c -O $DEF -o obj etc/cmos.clock.c

obj/dump.o:	etc/dump.c		\
		h/common.h
		$CC -c -O $DEF -o obj etc/dump.c

obj/etc.o:	etc/etc.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/bcb.h			\
		h/region.h		\
		h/mmu.h			\
		h/a.out.h		\
		h/inode.h		\
		h/sysdirent.h		\
		h/signal.h		\
		h/uproc.h		\
		h/uerror.h
		$CC -c -O $DEF -o obj etc/etc.c

obj/gets.o:	etc/gets.c		\
		h/common.h
		$CC -c -O $DEF -o obj etc/gets.c

obj/kinit.o:	etc/kinit.c		\
		h/common.h		\
		h/bcb.h			\
		h/scb.h			\
		h/sync.h		\
		h/region.h		\
		h/a.out.h		\
		h/seg.h			\
		h/tss.h			\
		h/intr.h		\
		h/devmajor.h		\
		h/disktb.h		\
		h/video.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj etc/kinit.c

obj/main.o:	etc/main.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/mmu.h			\
		h/ipc.h			\
		h/disktb.h		\
		h/kfile.h		\
		h/inode.h		\
		h/bhead.h		\
		h/mntent.h		\
		h/sb.h			\
		h/v7.h			\
		h/devmajor.h		\
		h/iotab.h		\
		h/cpu.h			\
		h/signal.h		\
		h/sysdirent.h		\
		h/uproc.h		\
		h/uerror.h
		$CC -c -O $DEF -o obj etc/main.c

obj/printf.o:	etc/printf.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/a.out.h		\
		h/stat.h		\
		h/disktb.h		\
		h/tty.h			\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj etc/printf.c

obj/scb.o:	etc/scb.c		\
		h/common.h		\
		h/bcb.h			\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/default.h		\
		h/inode.h		\
		h/bhead.h		\
		h/kfile.h		\
		h/tty.h			\
		h/map.h			\
		h/mmu.h			\
		h/a.out.h		\
		h/timeout.h		\
		h/lockf.h		\
		h/ipc.h			\
		h/disktb.h		\
		h/devmajor.h		\
		h/kcntl.h		\
		h/itnet.h		\
		h/video.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj etc/scb.c

obj/spy.o:	etc/spy.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/map.h			\
		h/mmu.h			\
		h/kfile.h		\
		h/lockf.h		\
		h/inode.h		\
		h/mntent.h		\
		h/sb.h			\
		h/v7.h			\
		h/bhead.h		\
		h/tty.h			\
		h/timeout.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj etc/spy.c

obj/stdio.o:	etc/stdio.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/disktb.h		\
		h/devmajor.h		\
		h/tty.h			\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj etc/stdio.c

obj/lowertoupper.o: etc/lowertoupper.c
		$CC -c -O $DEF -o obj etc/lowertoupper.c

obj/uppertolower.o: etc/uppertolower.c
		$CC -c -O $DEF -o obj etc/uppertolower.c

#*
#****** Diretório "ext2fs" **************************************
#*
obj/ext2blk.o:	ext2fs/ext2blk.c	\
		h/common.h		\
		h/scb.h			\
		h/sync.h		\
		h/region.h		\
		h/mntent.h		\
		h/sb.h			\
		h/ext2.h		\
		h/bhead.h		\
		h/inode.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj ext2fs/ext2blk.c

obj/ext2dir.o:	ext2fs/ext2dir.c	\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/kfile.h		\
		h/mntent.h		\
		h/sb.h			\
		h/ext2.h		\
		h/sysdirent.h		\
		h/inode.h		\
		h/stat.h		\
		h/bhead.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj ext2fs/ext2dir.c

obj/ext2inode.o: ext2fs/ext2inode.c	\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/inode.h		\
		h/bhead.h		\
		h/mntent.h		\
		h/sb.h			\
		h/ext2.h		\
		h/sysdirent.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj ext2fs/ext2inode.c

obj/ext2mount.o: ext2fs/ext2mount.c	\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/disktb.h		\
		h/mntent.h		\
		h/sb.h			\
		h/ext2.h		\
		h/ustat.h		\
		h/iotab.h		\
		h/inode.h		\
		h/bhead.h		\
		h/kfile.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj ext2fs/ext2mount.c

obj/ext2read.o:	ext2fs/ext2read.c	\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/iotab.h		\
		h/kfile.h		\
		h/inode.h		\
		h/bhead.h		\
		h/mntent.h		\
		h/sb.h			\
		h/ext2.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj ext2fs/ext2read.c

#*
#****** Diretório "fatfs" ***************************************
#*
obj/fatdir.o:	fatfs/fatdir.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/mntent.h		\
		h/sb.h			\
		h/kfile.h		\
		h/fat.h			\
		h/sysdirent.h		\
		h/inode.h		\
		h/stat.h		\
		h/bhead.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj fatfs/fatdir.c

obj/fatfat.o:	fatfs/fatfat.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/mntent.h		\
		h/sb.h			\
		h/inode.h		\
		h/bhead.h		\
		h/fat.h			\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj fatfs/fatfat.c

obj/fatinode.o:	fatfs/fatinode.c 	\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/mntent.h		\
		h/sb.h			\
		h/inode.h		\
		h/bhead.h		\
		h/sysdirent.h		\
		h/fat.h			\
		h/stat.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj fatfs/fatinode.c

obj/fatlfn.o:	fatfs/fatlfn.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/mntent.h		\
		h/sb.h			\
		h/kfile.h		\
		h/fat.h			\
		h/sysdirent.h		\
		h/inode.h		\
		h/stat.h		\
		h/bhead.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj fatfs/fatlfn.c

obj/fatmount.o:	fatfs/fatmount.c 	\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/disktb.h		\
		h/mntent.h		\
		h/sb.h			\
		h/ustat.h		\
		h/fat.h			\
		h/bhead.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj fatfs/fatmount.c

obj/fatread.o:	fatfs/fatread.c 	\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/mntent.h		\
		h/sb.h			\
		h/fat.h			\
		h/inode.h		\
		h/stat.h		\
		h/bhead.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj fatfs/fatread.c

#*
#****** Diretório "fio" *****************************************
#*
obj/file.o:	fio/file.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/mntent.h		\
		h/sb.h			\
		h/mon.h			\
		h/inode.h		\
		h/stat.h		\
		h/kfile.h		\
		h/iotab.h		\
		h/sysdirent.h		\
		h/ipc.h			\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj fio/file.c

obj/iname.o:	fio/iname.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/inode.h		\
		h/bhead.h		\
		h/mntent.h		\
		h/sb.h			\
		h/sysdirent.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj fio/iname.c

obj/inode.o:	fio/inode.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/mon.h			\
		h/disktb.h		\
		h/bhead.h		\
		h/inode.h		\
		h/mntent.h		\
		h/sb.h			\
		h/ipc.h			\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj fio/inode.c

obj/lockf.o:	fio/lockf.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/inode.h		\
		h/lockf.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj fio/lockf.c

#*
#****** Diretório "invalfs" *************************************
#*
obj/iblk.o:	invalfs/iblk.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/mntent.h		\
		h/sb.h			\
		h/iotab.h		\
		h/kfile.h		\
		h/inode.h		\
		h/bhead.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj invalfs/iblk.c

obj/ichr.o:	invalfs/ichr.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/mntent.h		\
		h/sb.h			\
		h/iotab.h		\
		h/kfile.h		\
		h/inode.h		\
		h/bhead.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj invalfs/ichr.c

obj/inone.o:	invalfs/inone.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/iotab.h		\
		h/kfile.h		\
		h/inode.h		\
		h/bhead.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj invalfs/inone.c

#*
#****** Diretório "itnet" ***************************************
#*
obj/$CHECKSUM.o: itnet/$CHECKSUM.s
		$AS -l l/$CHECKSUM.l -o obj/$CHECKSUM.o itnet/$CHECKSUM.s

obj/daemon.o:	itnet/daemon.c		\
		h/common.h		\
		h/scb.h			\
		h/sync.h		\
		h/itnet.h
		$CC -c -O $DEF -o obj itnet/daemon.c

obj/dhcp.o:	itnet/dhcp.c		\
		h/common.h		\
		h/scb.h			\
		h/sync.h		\
		h/timeout.h		\
		h/itnet.h
		$CC -c -O $DEF -o obj itnet/dhcp.c

obj/ep.o:	itnet/ep.c		\
		h/common.h		\
		h/scb.h			\
		h/sync.h		\
		h/itnet.h
		$CC -c -O $DEF -o obj itnet/ep.c

obj/ether.o:	itnet/ether.c		\
		h/common.h		\
		h/scb.h			\
		h/sync.h		\
		h/itnet.h
		$CC -c -O $DEF -o obj itnet/ether.c

obj/frame.o:	itnet/frame.c		\
		h/common.h		\
		h/scb.h			\
		h/sync.h		\
		h/region.h		\
		h/cpu.h			\
		h/inode.h		\
		h/sysdirent.h		\
		h/timeout.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h		\
		h/itnet.h
		$CC -c -O $DEF -o obj itnet/frame.c

obj/icmp.o:	itnet/icmp.c		\
		h/common.h		\
		h/scb.h			\
		h/sync.h		\
		h/region.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h		\
		h/itnet.h
		$CC -c -O $DEF -o obj itnet/icmp.c

obj/ip.o:	itnet/ip.c		\
		h/common.h		\
		h/scb.h			\
		h/sync.h		\
		h/region.h		\
		h/inode.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h		\
		h/itnet.h
		$CC -c -O $DEF -o obj itnet/ip.c

obj/itblock.o:	itnet/itblock.c		\
		h/common.h		\
		h/scb.h			\
		h/sync.h		\
		h/itnet.h
		$CC -c -O $DEF -o obj itnet/itblock.c

obj/itn.o:	itnet/itn.c		\
		h/common.h		\
		h/scb.h			\
		h/sync.h		\
		h/region.h		\
		h/kfile.h		\
		h/inode.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h		\
		h/ioctl.h		\
		h/xti.h			\
		h/itnet.h
		$CC -c -O $DEF -o obj itnet/itn.c

obj/kaddr.o:	itnet/kaddr.c		\
		h/common.h		\
		h/scb.h			\
		h/sync.h		\
		h/region.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h		\
		h/itnet.h		\
		h/xti.h
		$CC -c -O $DEF -o obj itnet/kaddr.c

obj/kbind.o:	itnet/kbind.c		\
		h/common.h		\
		h/scb.h			\
		h/sync.h		\
		h/region.h		\
		h/kfile.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h		\
		h/cpu.h			\
		h/xti.h			\
		h/itnet.h		\
		h/nfs.h
		$CC -c -O $DEF -o obj itnet/kbind.c

obj/kinfo.o:	itnet/kinfo.c		\
		h/common.h		\
		h/scb.h			\
		h/sync.h		\
		h/region.h		\
		h/kfile.h		\
		h/inode.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h		\
		h/xti.h			\
		h/itnet.h
		$CC -c -O $DEF -o obj itnet/kinfo.c

obj/ksnd.o:	itnet/ksnd.c		\
		h/common.h		\
		h/scb.h			\
		h/sync.h		\
		h/region.h		\
		h/kfile.h		\
		h/inode.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h		\
		h/cpu.h			\
		h/xti.h			\
		h/itnet.h
		$CC -c -O $DEF -o obj itnet/ksnd.c

obj/psnd.o:	itnet/psnd.c		\
		h/common.h		\
		h/scb.h			\
		h/sync.h		\
		h/region.h		\
		h/kfile.h		\
		h/inode.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h		\
		h/itnet.h
		$CC -c -O $DEF -o obj itnet/psnd.c

obj/raw.o:	itnet/raw.c		\
		h/common.h		\
		h/scb.h			\
		h/sync.h		\
		h/region.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h		\
		h/itnet.h
		$CC -c -O $DEF -o obj itnet/raw.c

obj/rcv.o:	itnet/rcv.c		\
		h/common.h		\
		h/scb.h			\
		h/sync.h		\
		h/region.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h		\
		h/itnet.h
		$CC -c -O $DEF -o obj itnet/rcv.c

obj/rnd.o:	itnet/rnd.c		\
		h/common.h		\
		h/scb.h			\
		h/sync.h		\
		h/region.h		\
		h/map.h			\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h		\
		h/itnet.h
		$CC -c -O $DEF -o obj itnet/rnd.c

obj/snd.o:	itnet/snd.c		\
		h/common.h		\
		h/scb.h			\
		h/sync.h		\
		h/region.h		\
		h/inode.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h		\
		h/itnet.h
		$CC -c -O $DEF -o obj itnet/snd.c

obj/udp.o:	itnet/udp.c		\
		h/common.h		\
		h/scb.h			\
		h/sync.h		\
		h/region.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h		\
		h/itnet.h		\
		h/nfs.h
		$CC -c -O $DEF -o obj itnet/udp.c

#*
#****** Diretório "mem" *****************************************
#*
obj/malloc.o:	mem/malloc.c		\
		h/common.h		\
		h/scb.h			\
		h/sync.h		\
		h/map.h
		$CC -c -O $DEF -o obj mem/malloc.c

obj/malloc_byte.o: mem/malloc_byte.c	\
		h/common.h		\
		h/sync.h		\
		h/map.h
		$CC -c -O $DEF -o obj mem/malloc_byte.c

obj/mmu.o:	mem/mmu.c		\
		h/common.h		\
		h/scb.h			\
		h/sync.h		\
		h/region.h		\
		h/map.h			\
		h/cpu.h			\
		h/mmu.h			\
		h/kfile.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj mem/mmu.c

obj/region.o:	mem/region.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/cpu.h			\
		h/map.h			\
		h/inode.h		\
		h/bhead.h		\
		h/signal.h		\
		h/uproc.h		\
		h/uerror.h
		$CC -c -O $DEF -o obj mem/region.c

obj/text.o:	mem/text.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/map.h			\
		h/inode.h		\
		h/bhead.h		\
		h/a.out.h		\
		h/mmu.h			\
		h/signal.h		\
		h/uproc.h		\
		h/uerror.h
		$CC -c -O $DEF -o obj mem/text.c

#*
#****** Diretório "nfs" *****************************************
#*
obj/nfs2_aux.o:		nfs/nfs2_aux.c		\
			h/common.h		\
			h/scb.h			\
			h/sync.h		\
			h/region.h		\
			h/mntent.h		\
			h/sb.h			\
			h/inode.h		\
			h/stat.h		\
			h/sysdirent.h		\
			h/itnet.h		\
			h/nfs.h			\
			h/signal.h		\
			h/uproc.h		\
			h/uerror.h
			$CC -c -O $DEF -o obj nfs/nfs2_aux.c

obj/nfs2c_dir.o:	nfs/nfs2c_dir.c		\
			h/common.h		\
			h/scb.h			\
			h/sync.h		\
			h/region.h		\
			h/mntent.h		\
			h/sb.h			\
			h/itnet.h		\
			h/inode.h		\
			h/sysdirent.h		\
			h/nfs.h			\
			h/signal.h		\
			h/uproc.h		\
			h/uerror.h
			$CC -c -O $DEF -o obj nfs/nfs2c_dir.c

obj/nfs2c_inode.o:	nfs/nfs2c_inode.c	\
			h/common.h		\
			h/scb.h			\
			h/sync.h		\
			h/region.h		\
			h/mntent.h		\
			h/sb.h			\
			h/bhead.h		\
			h/itnet.h		\
			h/nfs.h			\
			h/inode.h		\
			h/sysdirent.h		\
			h/signal.h		\
			h/uproc.h		\
			h/uerror.h
			$CC -c -O $DEF -o obj nfs/nfs2c_inode.c

obj/nfs2c_mnt.o:	nfs/nfs2c_mnt.c		\
			h/common.h		\
			h/scb.h			\
			h/sync.h		\
			h/region.h		\
			h/itnet.h		\
			h/sysdirent.h		\
			h/nfs.h			\
			h/mntent.h		\
			h/sb.h			\
			h/ustat.h		\
			h/signal.h		\
			h/uproc.h		\
			h/uerror.h
			$CC -c -O $DEF -o obj nfs/nfs2c_mnt.c

obj/nfs2c_read.o:	nfs/nfs2c_read.c	\
			h/common.h		\
			h/scb.h			\
			h/sync.h		\
			h/region.h		\
			h/mntent.h		\
			h/sb.h			\
			h/itnet.h		\
			h/nfs.h			\
			h/kfile.h		\
			h/inode.h		\
			h/sysdirent.h		\
			h/devhead.h		\
			h/bhead.h		\
			h/signal.h		\
			h/uproc.h		\
			h/uerror.h
			$CC -c -O $DEF -o obj nfs/nfs2c_read.c

obj/nfs2s_dir.o:	nfs/nfs2s_dir.c		\
			h/common.h		\
			h/sync.h		\
			h/region.h		\
			h/mntent.h		\
			h/sb.h			\
			h/inode.h		\
			h/stat.h		\
			h/sysdirent.h		\
			h/itnet.h		\
			h/nfs.h			\
			h/signal.h		\
			h/uerror.h		\
			h/uproc.h
			$CC -c -O $DEF -o obj nfs/nfs2s_dir.c

obj/nfs2s_mnt.o:	nfs/nfs2s_mnt.c		\
			h/common.h		\
			h/scb.h			\
			h/sync.h		\
			h/region.h		\
			h/mntent.h		\
			h/inode.h		\
			h/sysdirent.h		\
			h/itnet.h		\
			h/nfs.h			\
			h/signal.h		\
			h/uproc.h		\
			h/uerror.h
			$CC -c -O $DEF -o obj nfs/nfs2s_mnt.c

obj/nfs2s_read.o:	nfs/nfs2s_read.c	\
			h/common.h		\
			h/sync.h		\
			h/region.h		\
			h/inode.h		\
			h/stat.h		\
			h/sysdirent.h		\
			h/itnet.h		\
			h/nfs.h			\
			h/signal.h		\
			h/uerror.h		\
			h/uproc.h
			$CC -c -O $DEF -o obj nfs/nfs2s_read.c

obj/nfs2s_stat.o:	nfs/nfs2s_stat.c	\
			h/common.h		\
			h/sync.h		\
			h/region.h		\
			h/mntent.h		\
			h/sb.h			\
			h/inode.h		\
			h/stat.h		\
			h/ustat.h		\
			h/itnet.h		\
			h/nfs.h			\
			h/uerror.h
			$CC -c -O $DEF -o obj nfs/nfs2s_stat.c

obj/nfs2s_tree.o:	nfs/nfs2s_tree.c	\
			h/common.h		\
			h/sync.h		\
			h/region.h		\
			h/mntent.h		\
			h/sb.h			\
			h/inode.h		\
			h/stat.h		\
			h/sysdirent.h		\
			h/itnet.h		\
			h/nfs.h			\
			h/signal.h		\
			h/uerror.h		\
			h/uproc.h
			$CC -c -O $DEF -o obj nfs/nfs2s_tree.c

obj/rpc.o:		nfs/rpc.c		\
			h/common.h		\
			h/scb.h			\
			h/sync.h		\
			h/region.h		\
			h/itnet.h		\
			h/mntent.h		\
			h/sb.h			\
			h/nfs.h			\
			h/uerror.h		\
			h/signal.h		\
			h/uproc.h
			$CC -c -O $DEF -o obj nfs/rpc.c

#*
#****** Diretório "ntfs" ****************************************
#*
obj/ntattr.o:	ntfs/ntattr.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/stat.h		\
		h/region.h		\
		h/inode.h		\
		h/bhead.h		\
		h/mntent.h		\
		h/sb.h			\
		h/nt.h			\
		h/sysdirent.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj ntfs/ntattr.c

obj/ntdir.o:	ntfs/ntdir.c 		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/mntent.h		\
		h/sb.h			\
		h/kfile.h		\
		h/nt.h			\
		h/sysdirent.h		\
		h/inode.h		\
		h/stat.h		\
		h/bhead.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj ntfs/ntdir.c

obj/ntinode.o:	ntfs/ntinode.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/stat.h		\
		h/region.h		\
		h/inode.h		\
		h/bhead.h		\
		h/mntent.h		\
		h/sb.h			\
		h/nt.h			\
		h/sysdirent.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj ntfs/ntinode.c

obj/ntmount.o:	ntfs/ntmount.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/disktb.h		\
		h/mntent.h		\
		h/sb.h			\
		h/nt.h			\
		h/ustat.h		\
		h/iotab.h		\
		h/inode.h		\
		h/bhead.h		\
		h/kfile.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj ntfs/ntmount.c

obj/ntread.o:	ntfs/ntread.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/stat.h		\
		h/iotab.h		\
		h/kfile.h		\
		h/inode.h		\
		h/bhead.h		\
		h/mntent.h		\
		h/sb.h			\
		h/nt.h			\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj ntfs/ntread.c

#*
#****** Diretório "proc" ****************************************
#*
obj/pipeinode.o: pipefs/pipeinode.c	\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/inode.h		\
		h/mntent.h		\
		h/sb.h			\
		h/signal.h		\
		h/ustat.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj pipefs/pipeinode.c

obj/piperead.o: pipefs/piperead.c	\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/map.h			\
		h/frame.h		\
		h/kfile.h		\
		h/inode.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj pipefs/piperead.c

#*
#****** Diretório "proc" ****************************************
#*
obj/clock.o:	proc/clock.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/seg.h			\
		h/frame.h		\
		h/cpu.h			\
		h/timeout.h		\
		h/mon.h			\
		h/a.out.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj proc/clock.c

obj/dispat.o:	proc/dispat.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/cpu.h			\
		h/signal.h		\
		h/uproc.h		\
		h/uerror.h
		$CC -c -O $DEF -o obj proc/dispat.c

obj/excep.o:	proc/excep.c		\
		h/common.h		\
		h/scb.h			\
		h/sync.h		\
		h/region.h		\
		h/frame.h		\
		h/mmu.h			\
		h/a.out.h		\
		h/seg.h			\
		h/cpu.h			\
		h/intr.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj proc/excep.c

obj/kexit.o:	proc/kexit.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/map.h			\
		h/kfile.h		\
		h/inode.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj proc/kexit.c

obj/nproc.o:	proc/nproc.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/cpu.h			\
		h/map.h			\
		h/mmu.h			\
		h/uarg.h		\
		h/default.h		\
		h/inode.h		\
		h/kfile.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj proc/nproc.c

obj/signal.o:	proc/signal.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/frame.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj proc/signal.c

obj/swap.o:	proc/swap.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/map.h			\
		h/signal.h		\
		h/mon.h			\
		h/inode.h		\
		h/iotab.h		\
		h/devhead.h		\
		h/bhead.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj proc/swap.c

obj/syscall.o:	proc/syscall.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/sysent.h		\
		h/frame.h		\
		h/signal.h		\
		h/uproc.h		\
		h/uerror.h
		$CC -c -O $DEF -o obj proc/syscall.c

#*
#****** Diretório "sync" ****************************************
#*
obj/event.o:	sync/event.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj sync/event.c

obj/ipc.o:	sync/ipc.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/map.h			\
		h/mmu.h			\
		h/inode.h		\
		h/kfile.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h		\
		h/ipc.h			\
		h/default.h
		$CC -c -O $DEF -o obj sync/ipc.c

obj/sema.o:	sync/sema.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj sync/sema.c

obj/sleep.o:	sync/sleep.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj sync/sleep.c

obj/sync.o:	sync/sync.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj sync/sync.c

obj/spinlock.o:	sync/spinlock.s
		$AS -l l/spinlock.l -o obj/spinlock.o sync/spinlock.s

#*
#****** Diretório "sys" *****************************************
#*
obj/sysctl.o:	sys/sysctl.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/bcb.h			\
		h/region.h		\
		h/iotab.h		\
		h/kfile.h		\
		h/inode.h		\
		h/mmu.h			\
		h/frame.h		\
		h/a.out.h		\
		h/kcntl.h		\
		h/pcntl.h		\
		h/tty.h			\
		h/disktb.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj sys/sysctl.c

obj/sysdir.o:	sys/sysdir.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/mntent.h		\
		h/sb.h			\
		h/kfile.h		\
		h/sysdirent.h		\
		h/inode.h		\
		h/stat.h		\
		h/bhead.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj sys/sysdir.c

obj/sysent.o:	sys/sysent.c		\
		h/common.h		\
		h/sysent.h
		$CC -c -O $DEF -o obj sys/sysent.c

obj/sysetc.o:	sys/sysetc.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/map.h			\
		h/mmu.h			\
		h/frame.h		\
		h/kfile.h		\
		h/inode.h		\
		h/iotab.h		\
		h/ioctl.h		\
		h/cpu.h			\
		h/times.h		\
		h/timeout.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj sys/sysetc.c

obj/sysexec.o:	sys/sysexec.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/uarg.h		\
		h/map.h			\
		h/mmu.h			\
		h/frame.h		\
		h/mntent.h		\
		h/sb.h			\
		h/kfile.h		\
		h/inode.h		\
		h/a.out.h		\
		h/sysdirent.h		\
		h/signal.h		\
		h/uproc.h		\
		h/uerror.h
		$CC -c -O $DEF -o obj sys/sysexec.c

obj/sysfio.o:	sys/sysfio.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/mntent.h		\
		h/sb.h			\
		h/kfile.h		\
		h/inode.h		\
		h/iotab.h		\
		h/lockf.h		\
		h/sysdirent.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj sys/sysfio.c

obj/sysfork.o:	sys/sysfork.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/map.h			\
		h/frame.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj sys/sysfork.c

obj/sysipc.o:	sys/sysipc.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/kfile.h		\
		h/inode.h		\
		h/signal.h		\
		h/default.h		\
		h/cpu.h			\
		h/mmu.h			\
		h/ipc.h			\
		h/uerror.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj sys/sysipc.c

obj/syslib.o:	sys/syslib.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/mmu.h			\
		h/stat.h		\
		h/map.h			\
		h/inode.h		\
		h/a.out.h		\
		h/shlib.h		\
		h/sysdirent.h		\
		h/signal.h		\
		h/uproc.h		\
		h/uerror.h
		$CC -c -O $DEF -o obj sys/syslib.c

obj/sysproc.o:	sys/sysproc.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/default.h		\
		h/tty.h			\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj sys/sysproc.c

obj/sysstat.o:	sys/sysstat.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/mntent.h		\
		h/sb.h			\
		h/kfile.h		\
		h/inode.h		\
		h/stat.h		\
		h/bhead.h		\
		h/sysdirent.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj sys/sysstat.c

obj/systime.o:	sys/systime.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/inode.h		\
		h/mntent.h		\
		h/sb.h			\
		h/times.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj sys/systime.c

obj/systree.o:	sys/systree.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/mntent.h		\
		h/sb.h			\
		h/itnet.h		\
		h/nfs.h			\
		h/stat.h		\
		h/ustat.h		\
		h/disktb.h		\
		h/kfile.h		\
		h/inode.h		\
		h/nt.h			\
		h/bhead.h		\
		h/iotab.h		\
		h/sysdirent.h		\
		h/devmajor.h		\
		h/ioctl.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj sys/systree.c

obj/sysuser.o:	sys/sysuser.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/frame.h		\
		h/utsname.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj sys/sysuser.c

#*
#****** Diretório "t1fs" ****************************************
#*
obj/t1blk.o:	t1fs/t1blk.c		\
		h/common.h		\
		h/scb.h			\
		h/sync.h		\
		h/region.h		\
		h/mntent.h		\
		h/sb.h			\
		h/t1.h			\
		h/bhead.h		\
		h/inode.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj t1fs/t1blk.c

obj/t1dir.o:	t1fs/t1dir.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/kfile.h		\
		h/mntent.h		\
		h/sb.h			\
		h/t1.h			\
		h/sysdirent.h		\
		h/inode.h		\
		h/stat.h		\
		h/bhead.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj t1fs/t1dir.c

obj/t1inode.o:	t1fs/t1inode.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/inode.h		\
		h/bhead.h		\
		h/mntent.h		\
		h/sb.h			\
		h/t1.h			\
		h/sysdirent.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj t1fs/t1inode.c

obj/t1mount.o:	t1fs/t1mount.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/disktb.h		\
		h/mntent.h		\
		h/sb.h			\
		h/t1.h			\
		h/ustat.h		\
		h/iotab.h		\
		h/inode.h		\
		h/bhead.h		\
		h/kfile.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj t1fs/t1mount.c

obj/t1read.o:	t1fs/t1read.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/iotab.h		\
		h/kfile.h		\
		h/inode.h		\
		h/bhead.h		\
		h/mntent.h		\
		h/sb.h			\
		h/t1.h			\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj t1fs/t1read.c

#*
#****** Diretório "tty" *****************************************
#*
obj/chartb.o:	tty/chartb.c
		$CC -c -O $DEF -o obj tty/chartb.c

obj/clist.o:	tty/clist.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/tty.h			\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj tty/clist.c

obj/isotoatb.o:	tty/isotoatb.c
		$CC -c -O $DEF -o obj tty/isotoatb.c

obj/isotoisotb.o: tty/isotoisotb.c
		$CC -c -O $DEF -o obj tty/isotoisotb.c

obj/isotou1tb.o: tty/isotou1tb.c
		$CC -c -O $DEF -o obj tty/isotou1tb.c

obj/isotou2tb.o: tty/isotou2tb.c
		$CC -c -O $DEF -o obj tty/isotou2tb.c

obj/ttyctl.o:	tty/ttyctl.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/tty.h			\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj tty/ttyctl.c

obj/ttyin.o:	tty/ttyin.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/signal.h		\
		h/tty.h
		$CC -c -O $DEF -o obj tty/ttyin.c

obj/ttyread.o:	tty/ttyread.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/tty.h			\
		h/video.h		\
		h/kfile.h		\
		h/inode.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj tty/ttyread.c

#*
#****** Diretório "usb" *****************************************
#*
obj/bus.o:	usb/bus.c		\
		h/common.h		\
		h/sync.h		\
		h/mntent.h		\
		h/usb.h
		$CC -c -O $DEF -o obj usb/bus.c

obj/ehci.o:	usb/ehci.c		\
		h/common.h		\
		h/sync.h		\
		h/region.h		\
		h/map.h			\
		h/scb.h			\
		h/pci.h			\
		h/frame.h		\
		h/intr.h		\
		h/mntent.h		\
		h/usb.h			\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h		\
		h/queue.h
		$CC -c -O $DEF -o obj usb/ehci.c

obj/hid.o:	usb/hid.c		\
		h/common.h		\
		h/sync.h		\
		h/usb.h			\
		usb/hid.h		\
		usb/usbhid.h
		$CC -c -O $DEF -o obj usb/hid.c

obj/ohci.o:	usb/ohci.c		\
		h/common.h		\
		h/sync.h		\
		h/region.h		\
		h/map.h			\
		h/scb.h			\
		h/pci.h			\
		h/frame.h		\
		h/intr.h		\
		h/usb.h			\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj usb/ohci.c

obj/quirks.o:	usb/quirks.c		\
		h/common.h		\
		h/sync.h		\
		h/usb.h			\
		usb/usbdevs.h
		$CC -c -O $DEF -o obj usb/quirks.c

obj/ud.o:	usb/ud.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/usb.h			\
		h/disktb.h		\
		h/scsi.h		\
		h/kfile.h		\
		h/devhead.h		\
		h/bhead.h		\
		h/devmajor.h		\
		h/inode.h		\
		h/ioctl.h		\
		h/signal.h		\
		h/uproc.h		\
		h/uerror.h		\
		usb/usbdevs.h
		$CC -c -O $DEF -o obj usb/ud.c

obj/uhci.o:	usb/uhci.c		\
		h/common.h		\
		h/sync.h		\
		h/region.h		\
		h/map.h			\
		h/scb.h			\
		h/pci.h			\
		h/frame.h		\
		h/intr.h		\
		h/usb.h			\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj usb/uhci.c

obj/uhub.o:	usb/uhub.c		\
		h/common.h		\
		h/sync.h		\
		h/usb.h
		$CC -c -O $DEF -o obj usb/uhub.c

obj/ulp.o:	usb/ulp.c		\
		h/common.h		\
		h/sync.h		\
		h/region.h		\
		h/tty.h			\
		h/usb.h			\
		h/inode.h		\
		h/ioctl.h		\
		usb/hid.h		\
		usb/mouse.h		\
		usb/usbhid.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj usb/ulp.c

obj/ums.o:	usb/ums.c		\
		h/common.h		\
		h/sync.h		\
		h/region.h		\
		h/tty.h			\
		h/usb.h			\
		h/inode.h		\
		h/ioctl.h		\
		usb/hid.h		\
		usb/mouse.h		\
		usb/usbhid.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj usb/ums.c

obj/usb.o:	usb/usb.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/signal.h		\
		h/region.h		\
		h/uproc.h		\
		h/usb.h
		$CC -c -O $DEF -o obj usb/usb.c

obj/usb_subr.o:	usb/usb_subr.c		\
		h/common.h		\
		h/sync.h		\
		h/usb.h			\
		usb/usbdevs.h
		$CC -c -O $DEF -o obj usb/usb_subr.c

obj/usbdi.o:	usb/usbdi.c		\
		h/common.h		\
		h/sync.h		\
		h/usb.h			\
		h/signal.h		\
		h/region.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj usb/usbdi.c

obj/usbdi_util.o: usb/usbdi_util.c 	\
		h/common.h		\
		h/sync.h		\
		h/usb.h			\
		usb/usbhid.h
		$CC -c -O $DEF -o obj usb/usbdi_util.c

#*
#****** Diretório "v7fs" ****************************************
#*
obj/v7blk.o:	v7fs/v7blk.c		\
		h/common.h		\
		h/scb.h			\
		h/sync.h		\
		h/region.h		\
		h/mntent.h		\
		h/sb.h			\
		h/v7.h			\
		h/bhead.h		\
		h/inode.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj v7fs/v7blk.c

obj/v7dir.o:	v7fs/v7dir.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/kfile.h		\
		h/mntent.h		\
		h/sb.h			\
		h/v7.h			\
		h/sysdirent.h		\
		h/inode.h		\
		h/stat.h		\
		h/bhead.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj v7fs/v7dir.c

obj/v7inode.o:	v7fs/v7inode.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/inode.h		\
		h/bhead.h		\
		h/mntent.h		\
		h/sb.h			\
		h/v7.h			\
		h/sysdirent.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj v7fs/v7inode.c

obj/v7mount.o:	v7fs/v7mount.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/disktb.h		\
		h/mntent.h		\
		h/sb.h			\
		h/v7.h			\
		h/ustat.h		\
		h/iotab.h		\
		h/inode.h		\
		h/bhead.h		\
		h/kfile.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj v7fs/v7mount.c

obj/v7read.o:	v7fs/v7read.c		\
		h/common.h		\
		h/sync.h		\
		h/scb.h			\
		h/region.h		\
		h/iotab.h		\
		h/kfile.h		\
		h/inode.h		\
		h/bhead.h		\
		h/mntent.h		\
		h/sb.h			\
		h/v7.h			\
		h/sysdirent.h		\
		h/uerror.h		\
		h/signal.h		\
		h/uproc.h
		$CC -c -O $DEF -o obj v7fs/v7read.c

#*
#****** Modif ***************************************************
#*
modif:		modif.r
		sroff modif.r >$mandir/modif/sys/$PGM
		modif $PGM

#*
#****** Outros **************************************************
#*
lint:		$SRC
		$CC -T $DEF $?

lock:		lock.c h/tty.h
		$CC -O -o lock lock.c

sz:		prsz
		prsz


prsz:		prsz.c			\
		h/common.h		\
		h/sync.h		\
		h/syscall.h		\
		h/mntent.h		\
		h/sb.h			\
		h/bcb.h			\
		h/scb.h			\
		h/bhead.h		\
		h/region.h		\
		h/inode.h		\
		h/kfile.h		\
		h/cdfs.h		\
		h/v7.h			\
		h/t1.h			\
		h/nt.h			\
		h/ext2.h		\
		h/fat.h			\
		h/tss.h			\
		h/seg.h			\
		h/mon.h			\
		h/utsname.h		\
		h/disktb.h		\
		h/sysdirent.h		\
		h/a.out.h		\
		h/tty.h			\
		h/signal.h		\
		h/itnet.h		\
		h/nfs.h			\
		h/uproc.h		\
		h/ustat.h		\
		h/devhead.h		\
		h/pci.h			\
		h/scsi.h		\
		h/ata.h			\
		aic/aic7xxx.h	
		$CC -O -o prsz prsz.c

printr:		printr.c
		cc -O -o printr printr.c
		printr

cp:		$PGM
		-cp $PGM /new.$PGM
		edscb /new.$PGM scbfile.v version.v

cmp:
		-cmpobj $PGM /old.$PGM
		-cmpobj $PGM /$PGM
		-cmpobj $PGM /new.$PGM

cmpq:
		-cmpobj $PGM /$PGM

shift:
		-mv /$PGM /old.$PGM
		mv /new.$PGM /$PGM

pento:
		-mount /pen
		-cmptree -sg . /pen$ker
		-forldiff /pen$ker
		-rm garfield

penfrom:
		-mount /pen
		cd /pen$ker &&			\
		cmptree -sg . $ker;		\
		forldiff $ker;			\
		rm garfield

pensave:
		-mount /pen
		-pento $ker
		-umount /pen
