#*
#****************************************************************
#*	Makefile para o programa "boot2"			*
#****************************************************************
#*
DIR	=
PGM	= boot
APGM	= BOOT
FDISK	= fdisk
PROT	= u+x
DEV	= d5a
CC	= cc

.virtual	c cp cmp cmpq shift man prman modif zipto zipfrom pento penfrom pensave
.virtual	lint lintmain lintkey lintscsi lintfd lintata lintsd lintinflate
.virtual	lintaic lintdma lintbio lintv7bio lintt1bio lintcdbio lintdisktb lintfdisk lintgeo lintstdio
.virtual	lintgets cppfdisk as linted lintvideo
.virtual	cpfd0 cpfd1 cmpfd0 cmpfd1

KOBJ	= ../../kernel/obj
KDIR	= ../../kernel

DEF	= -D LITTLE_ENDIAN -D BOOT

DEFDEV	= NODEV
DEFPGM	= tropix
DEFS	= -D DEFDEV=$DEFDEV -D DEFPGM=$DEFPGM

MAN	= cmd
LDADDR	= 0x100000		# 1 MB

#
#******	Módulos objeto ******************************************
#
#	Os dois primeiros módulos são de 16 bits, e devem
#	ficar no início
#
OBJ	=	obj/seg.16.o		\
		obj/video_iso_tb.o	\
					\
		obj/seg.32.o		\
					\
		obj/ata.o		\
#		obj/ataahci.o		\
		obj/ataall.o		\
		obj/ataatapi.o		\
#		obj/atadma.o		\
		obj/ataintel.o		\
		obj/atajmicron.o	\
		obj/atapci.o		\
		obj/atasis.o		\
		obj/atavia.o		\
					\
		obj/delay.o		\
		obj/main.o		\
		obj/bio.o		\
		obj/v7bio.o		\
		obj/t1bio.o		\
		obj/cdbio.o		\
		obj/fdisk.o		\
		obj/geo.o		\
		obj/inflate.o		\
		obj/disktb.o		\
		obj/video.o		\
		obj/key.o		\
		obj/port.o		\
		obj/fd.o		\
		obj/sd.1542.o		\
		obj/scsi.o		\
		obj/ahc_pci.o		\
		obj/aic_boot.o		\
		obj/aic7xxx.o		\
#		obj/smp.o		\
		obj/pci_probe.o		\
		obj/dma.o		\
		obj/stdio.o		\
		obj/aux.o		\
		obj/str.o		\
		obj/gets.o		\
		obj/mem.o		\
		obj/malloc.o		\
					\
		obj/pci.o

#
#******	Link-edição *********************************************
#
$PGM:		obj l obj/seg.16.o obj/video_iso_tb.o obj/seg.32.o $OBJ
		ld -u -T $LDADDR -o $PGM $OBJ
		size $PGM
		ls -l $PGM

c:		obj l $OBJ

obj:		.exist
		mkdir obj

l:		.exist
		mkdir l

ATASRC =	src/ata.c			\
		$KDIR/ata/ataall.c		\
		$KDIR/ata/ataatapi.c		\
#		$KDIR/ataobj/atadma.c		\
		$KDIR/ata/ataintel.c		\
		$KDIR/ata/atajmicron.c		\
		$KDIR/ata/atapci.c		\
		$KDIR/ata/atasis.c		\
		$KDIR/ata/atavia.c

lint:
		$CC -T $DEF $DEFS -I $KDIR/h src/*.c

lintmain:
		$CC -T $DEF $DEFS -I $KDIR/h src/main.c

lintata:
		$CC -T $DEF $DEFS -I $KDIR/h $ATASRC

lintkey:
		$CC -T $DEF $DEFS -I $KDIR/h src/key.c

lintscsi:
		$CC -T $DEF $DEFS -I $KDIR/h src/scsi.c

lintfd:
		$CC -T $DEF $DEFS -I $KDIR/h src/fd.c

lintsd:
		$CC -T $DEF $DEFS -I $KDIR/h src/sd.1542.c

linted:
		$CC -T $DEF $DEFS -I $KDIR/h src/pci_probe.c

lintdma:
		$CC -T $DEF $DEFS -I $KDIR/h src/dma.c

lintbio:
		$CC -T $DEF $DEFS -I $KDIR/h src/bio.c

lintv7bio:
		$CC -T $DEF $DEFS -I $KDIR/h src/v7bio.c

lintt1bio:
		$CC -T $DEF $DEFS -I $KDIR/h src/t1bio.c

lintcdbio:
		$CC -T $DEF $DEFS -I $KDIR/h src/cdbio.c

lintdisktb:
		$CC -T $DEF $DEFS -I $KDIR/h src/disktb.c

lintfdisk:
		$CC -T $DEF $DEFS -I $KDIR/h src/fdisk.c

lintgeo:
		$CC -T $DEF $DEFS -I $KDIR/h src/geo.c

lintgets:
		$CC -T $DEF $DEFS -I $KDIR/h src/gets.c

lintvideo:
		$CC -T $DEF $DEFS -I $KDIR/h src/video.c

lintaic:
		$CC -T $DEF -I $KDIR/h src/aic_boot.c

lintstdio:
		$CC -T $DEF -I $KDIR/h src/stdio.c

lintinflate:
		$CC -T $DEF -I $KDIR/h src/inflate.c

cppfdisk:
		$CC -P $DEF $DEFS -I $KDIR/h src/fdisk.c

as:		src/main.c
		$CC -SO $DEF $DEFS -I $KDIR/h src/main.c

#
#******	Compilações *********************************************
#
obj/ata.o:	src/ata.c
		$CC -c -O $DEF -I $KDIR/h -o obj src/ata.c

obj/ataahci.o:	$KDIR/ata/ataahci.c	\
		$KDIR/h/common.h	\
		$KDIR/h/sync.h		\
		$KDIR/h/pci.h		\
		$KDIR/h/scsi.h		\
		$KDIR/h/devhead.h	\
		$KDIR/h/ata.h
		$CC -c -O $DEF -I $KDIR/h -o obj $KDIR/ata/ataahci.c

obj/ataall.o:	$KDIR/ata/ataall.c	\
		$KDIR/h/common.h	\
		$KDIR/h/sync.h		\
		$KDIR/h/scb.h		\
		$KDIR/h/devhead.h	\
		$KDIR/h/bhead.h		\
		$KDIR/h/pci.h		\
		$KDIR/h/scsi.h		\
		$KDIR/h/ata.h
		$CC -c -O $DEF -I $KDIR/h -o obj $KDIR/ata/ataall.c

obj/ataatapi.o:	$KDIR/ata/ataatapi.c	\
		$KDIR/h/common.h	\
		$KDIR/h/sync.h		\
		$KDIR/h/pci.h		\
		$KDIR/h/scsi.h		\
		$KDIR/h/devhead.h	\
		$KDIR/h/bhead.h		\
		$KDIR/h/ata.h
		$CC -c -O $DEF -I $KDIR/h -o obj $KDIR/ata/ataatapi.c

obj/atadma.o:	$KDIR/ata/atadma.c	\
		$KDIR/h/common.h	\
		$KDIR/h/sync.h		\
		$KDIR/h/map.h		\
		$KDIR/h/pci.h		\
		$KDIR/h/scsi.h		\
		$KDIR/h/devhead.h	\
		$KDIR/h/bhead.h		\
		$KDIR/h/ata.h
		$CC -c -O $DEF -I $KDIR/h -o obj $KDIR/ata/atadma.c

obj/ataintel.o:	$KDIR/ata/ataintel.c	\
		$KDIR/h/common.h	\
		$KDIR/h/sync.h		\
		$KDIR/h/pci.h		\
		$KDIR/h/scsi.h		\
		$KDIR/h/devhead.h	\
		$KDIR/h/ata.h
		$CC -c -O $DEF -I $KDIR/h -o obj $KDIR/ata/ataintel.c

obj/atajmicron.o: $KDIR/ata/atajmicron.c \
		$KDIR/h/common.h	\
		$KDIR/h/sync.h		\
		$KDIR/h/pci.h		\
		$KDIR/h/scsi.h		\
		$KDIR/h/devhead.h	\
		$KDIR/h/ata.h
		$CC -c -O $DEF -I $KDIR/h -o obj $KDIR/ata/atajmicron.c

obj/atapci.o:	$KDIR/ata/atapci.c	\
		$KDIR/h/common.h	\
		$KDIR/h/sync.h		\
		$KDIR/h/scb.h		\
		$KDIR/h/devhead.h	\
		$KDIR/h/pci.h		\
		$KDIR/h/scsi.h		\
		$KDIR/h/ata.h
		$CC -c -O $DEF -I $KDIR/h -o obj $KDIR/ata/atapci.c

obj/atasis.o:	$KDIR/ata/atasis.c	\
		$KDIR/h/common.h	\
		$KDIR/h/sync.h		\
		$KDIR/h/pci.h		\
		$KDIR/h/scsi.h		\
		$KDIR/h/devhead.h	\
		$KDIR/h/ata.h
		$CC -c -O $DEF -I $KDIR/h -o obj $KDIR/ata/atasis.c

obj/atavia.o:	$KDIR/ata/atavia.c	\
		$KDIR/h/common.h	\
		$KDIR/h/sync.h		\
		$KDIR/h/devhead.h	\
		$KDIR/h/bhead.h		\
		$KDIR/h/scsi.h		\
		$KDIR/h/ata.h
		$CC -c -O $DEF -I $KDIR/h -o obj $KDIR/ata/atavia.c

obj/bio.o:	src/bio.c		\
		$KDIR/h/common.h	\
		$KDIR/h/sync.h		\
		$KDIR/h/stat.h		\
		$KDIR/h/disktb.h	\
		$KDIR/h/bhead.h		\
		h/common.h
		$CC -c -O $DEF -I $KDIR/h -o obj src/bio.c

obj/t1bio.o:	src/t1bio.c		\
		$KDIR/h/common.h	\
		$KDIR/h/sync.h		\
		$KDIR/h/t1.h		\
		$KDIR/h/stat.h		\
		$KDIR/h/disktb.h	\
		$KDIR/h/bhead.h		\
		h/common.h
		$CC -c -O $DEF -I $KDIR/h -o obj src/t1bio.c

obj/cdbio.o:	src/cdbio.c		\
		$KDIR/h/common.h	\
		$KDIR/h/sync.h		\
		$KDIR/h/cdfs.h		\
		$KDIR/h/stat.h		\
		$KDIR/h/disktb.h	\
		$KDIR/h/bhead.h		\
		h/common.h
		$CC -c -O $DEF -I $KDIR/h -o obj src/cdbio.c

obj/v7bio.o:	src/v7bio.c		\
		$KDIR/h/common.h	\
		$KDIR/h/sync.h		\
		$KDIR/h/v7.h		\
		$KDIR/h/stat.h		\
		$KDIR/h/disktb.h	\
		$KDIR/h/bhead.h		\
		h/common.h
		$CC -c -O $DEF -I $KDIR/h -o obj src/v7bio.c

obj/disktb.o:	src/disktb.c		\
		$KDIR/h/common.h	\
		$KDIR/h/bcb.h		\
		$KDIR/h/disktb.h	\
		$KDIR/h/devmajor.h	\
		$KDIR/h/scsi.h		\
		$KDIR/h/ata.h		\
		h/common.h
		$CC -c -O $DEF -I $KDIR/h -o obj src/disktb.c

obj/dma.o:	src/dma.c		\
		$KDIR/h/common.h	\
		h/common.h
		$CC -c -O $DEF -I $KDIR/h -o obj src/dma.c

obj/fd.o:	src/fd.c		\
		$KDIR/h/common.h	\
		$KDIR/h/disktb.h	\
		$KDIR/h/devmajor.h	\
		$KDIR/h/bhead.h		\
		h/common.h
		$CC -c -O $DEF -I $KDIR/h -o obj src/fd.c

obj/fdisk.o:	src/fdisk.c		\
		$KDIR/h/common.h	\
		$KDIR/h/bcb.h		\
		$KDIR/h/disktb.h	\
		$KDIR/h/scsi.h		\
		$KDIR/h/ata.h		\
		h/common.h
		$CC -c -O $DEF -I $KDIR/h -o obj src/fdisk.c

obj/geo.o:	src/geo.c		\
		$KDIR/h/common.h	\
		$KDIR/h/bcb.h		\
		$KDIR/h/scsi.h		\
		$KDIR/h/ata.h		\
		h/common.h
		$CC -c -O $DEF -I $KDIR/h -o obj src/geo.c

obj/inflate.o:	src/inflate.c		\
		$KDIR/h/common.h	\
		$KDIR/h/sync.h		\
		$KDIR/h/disktb.h	\
		h/deflate.h		\
		h/common.h
		$CC -c -O $DEF -I $KDIR/h -o obj src/inflate.c

obj/gets.o:	src/gets.c		\
		$KDIR/h/common.h	\
		h/common.h
		$CC -c -O $DEF -I $KDIR/h -o obj src/gets.c

obj/key.o:	src/key.c		\
		$KDIR/h/common.h	\
		h/common.h
		$CC -c -O $DEF -I $KDIR/h -o obj src/key.c

obj/main.o:	src/main.c		\
		$KDIR/h/common.h	\
		$KDIR/h/sync.h		\
		$KDIR/h/v7.h		\
		$KDIR/h/a.out.h		\
		$KDIR/h/stat.h		\
		$KDIR/h/bcb.h		\
		$KDIR/h/disktb.h	\
		$KDIR/h/sb.h		\
		h/common.h
		$CC -c -O $DEF $DEFS -I $KDIR/h -o obj src/main.c

obj/pci_probe.o:	src/pci_probe.c
		$CC -c -O $DEF -I $KDIR/h -o obj src/pci_probe.c

obj/scsi.o:	src/scsi.c		\
		$KDIR/h/common.h	\
		$KDIR/h/bcb.h		\
		$KDIR/h/disktb.h	\
		$KDIR/h/bhead.h		\
		$KDIR/h/scsi.h		\
		$KDIR/h/cdio.h		\
		h/common.h
		$CC -c -O $DEF -I $KDIR/h -o obj src/scsi.c

obj/sd.1542.o:	src/sd.1542.c		\
		$KDIR/h/common.h	\
		$KDIR/h/bcb.h		\
		$KDIR/h/disktb.h	\
		$KDIR/h/devmajor.h	\
		$KDIR/h/bhead.h		\
		$KDIR/h/scsi.h		\
		h/common.h
		$CC -c -O $DEF -I $KDIR/h -o obj src/sd.1542.c

obj/smp.o:	src/smp.c		\
		$KDIR/h/common.h	\
		h/smp.h
		$CC -c -O $DEF -I $KDIR/h -o obj src/smp.c

obj/stdio.o:	src/stdio.c		\
		$KDIR/h/common.h	\
		/usr/include/stdarg.h	\
		h/common.h
		$CC -c -O $DEF -I $KDIR/h -o obj src/stdio.c

obj/aux.o:	src/aux.c		\
		$KDIR/h/common.h	\
		h/common.h
		$CC -c -O $DEF -I $KDIR/h -o obj src/aux.c

obj/video.o:	src/video.c		\
		$KDIR/h/common.h	\
		$KDIR/h/bcb.h		\
		$KDIR/h/video.h		\
		h/common.h
		$CC -c -O $DEF -I $KDIR/h -o obj src/video.c

obj/video_iso_tb.o: src/video_iso_tb.s
		as -l l/video_iso_tb.l -o obj/video_iso_tb.o src/video_iso_tb.s

obj/seg.16.o:	src/seg.16.s
		as -l l/seg.16.l -o obj/seg.16.o src/seg.16.s

obj/seg.32.o:	src/seg.32.s
		as -l l/seg.32.l -o obj/seg.32.o src/seg.32.s

obj/delay.o:	src/delay.s
		as -l l/delay.l -o obj/delay.o src/delay.s

obj/port.o:	src/port.s
		as -l l/port.l -o obj/port.o src/port.s

obj/mem.o:	src/mem.s
		as -l l/mem.l -o obj/mem.o src/mem.s

obj/str.o:	src/str.s
		as -l l/str.l -o obj/str.o src/str.s

obj/pci.o:	$KDIR/dev/pci.c		\
		$KDIR/h/common.h	\
		$KDIR/h/pci.h
		$CC -c -O $DEF -o obj $KDIR/dev/pci.c

obj/ahc_pci.o:	$KDIR/aic/ahc_pci.c	\
		$KDIR/h/common.h	\
		$KDIR/h/sync.h		\
		$KDIR/h/scb.h		\
		$KDIR/h/intr.h		\
		$KDIR/h/pci.h		\
		$KDIR/h/bhead.h		\
		$KDIR/h/scsi.h		\
		$KDIR/h/devhead.h	\
		$KDIR/h/uerror.h	\
		$KDIR/aic/aic7xxx_seq.h	\
		$KDIR/aic/aic7xxx.h	
		$CC -c -O $DEF -o obj $KDIR/aic/ahc_pci.c

obj/aic_boot.o:	src/aic_boot.c		\
		$KDIR/h/common.h	\
		$KDIR/h/sync.h		\
		$KDIR/h/scb.h		\
		$KDIR/h/region.h	\
		$KDIR/h/pci.h		\
		$KDIR/h/frame.h		\
		$KDIR/h/disktb.h	\
		$KDIR/h/devmajor.h	\
		$KDIR/h/scsi.h		\
		$KDIR/h/kfile.h		\
		$KDIR/h/devhead.h	\
		$KDIR/h/bhead.h		\
		$KDIR/h/cdio.h		\
		$KDIR/h/signal.h	\
		$KDIR/h/uproc.h		\
		$KDIR/h/uerror.h	\
		$KDIR/aic/aic7xxx_seq.h	\
		$KDIR/aic/asm_format.h	\
		$KDIR/aic/aic7xxx.h	\
		$KDIR/aic/seq.out	
		$CC -c -O $DEF -I $KDIR/h -o obj src/aic_boot.c

obj/aic7xxx.o:	$KDIR/aic/aic7xxx.c	\
		$KDIR/h/common.h	\
		$KDIR/h/sync.h		\
		$KDIR/h/scb.h		\
		$KDIR/h/region.h	\
		$KDIR/h/pci.h		\
		$KDIR/h/frame.h		\
		$KDIR/h/disktb.h	\
		$KDIR/h/scsi.h		\
		$KDIR/h/kfile.h		\
		$KDIR/h/devhead.h	\
		$KDIR/h/bhead.h		\
		$KDIR/h/cdio.h		\
		$KDIR/h/signal.h	\
		$KDIR/h/uproc.h		\
		$KDIR/h/uerror.h	\
		$KDIR/aic/aic7xxx_seq.h	\
		$KDIR/aic/asm_format.h	\
		$KDIR/aic/aic7xxx.h	\
		$KDIR/aic/seq.out	
		$CC -c -O $DEF -o obj $KDIR/aic/aic7xxx.c

obj/malloc.o:	src/malloc.c
		$CC -c -O $DEF -I $KDIR/h -o obj src/malloc.c

#
#******	Programa de teste da geometria **************************
#
geo:		geo.c
		cc -O -o geo geo.c

#
#******	Cópias, etc ... *****************************************
#
cpfd0:		$PGM
		-mount -w /dev/fd0 /mnt
		cp $PGM /mnt
		umount /dev/fd0

cpfd1:		$PGM
		-mount -w /dev/fd1 /mnt
		-cp $PGM /mnt
		umount /dev/fd1

cmpfd0:		$PGM
		-mount /dev/fd0 /mnt
		-cmpobj $PGM /mnt
		umount /dev/fd0

cmpfd1:		$PGM
		-mount /dev/fd1 /mnt
		-cmpobj $PGM /mnt
		umount /dev/fd1

cp:		$PGM
		cp $PGM $DIR/new.$PGM

cmp:
		-cmpobj $PGM $DIR/new.$PGM
		-cmpobj $PGM $DIR/$PGM
		-cmpobj $PGM $DIR/$APGM
		-cmpobj $PGM $DIR/old.$PGM

cmpq:
		-cmpobj $PGM $DIR/$PGM

shift:
		-mv $DIR/$PGM $DIR/old.$PGM
		-mv $DIR/new.$PGM $DIR/$PGM

man:		man.r
		sroff man.r >$mandir/$MAN/$FDISK
		man $FDISK

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		ln -f $mandir/modif/$MAN/$PGM $mandir/modif/$MAN/boot2
		modif $PGM

prman:
		sroff man.r | lpr

pento:
		-mount /pen
		-cmptree -sg . /pen$boot2
		-forldiff /pen$boot2
		-rm -f garfield
		-umount /pen

penfrom:
		-mount /pen
		cd /pen$boot2 &&		\
		cmptree -sg . $boot2;		\
		forldiff $boot2;		\
		rm -f /pen$boot2/garfield
		-umount /pen

pensave:
		-mount /pen
		-pento $boot2
		-umount /pen

zipto:
		-cmptree -sg . /zip$boot2
		-forldiff /zip$boot2
		-rm garfield

zipfrom:
		cd /zip$boot2 &&	\
		cmptree -sg . $boot2;	\
		forldiff $boot2;	\
		rm garfield
