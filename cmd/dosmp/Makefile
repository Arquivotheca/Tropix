#*
#****************************************************************
#*	Makefile para o programa "dosmp"			*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/bin
PGM	= dosmp
PROT	= a+rx
MAN	= cmd
CC	= cc
DEF	= -D PC -D LITTLE_ENDIAN

.virtual	c lint nm cp cmp cmpq cmpqcp unld man modif prman gar gara garc garcmp
.virtual	zipto zipfrom pento penfrom pensave

SRC	=				\
		src/aux.c		\
		src/bio.c		\
		src/cmd.c		\
		src/dir_walk.c		\
		src/do_arena.c		\
		src/do_cat.c		\
		src/do_cd.c		\
		src/do_chmod.c		\
		src/do_chsize.c		\
		src/do_clri.c		\
		src/do_cmp.c		\
		src/do_compac.c		\
		src/do_df.c		\
		src/do_diff.c		\
		src/do_du.c		\
		src/do_edbad.c		\
		src/do_fsck.c		\
		src/do_get.c		\
		src/do_gettree.c	\
		src/do_lc.c		\
		src/do_ls.c		\
		src/do_mkdir.c		\
		src/do_mkfile.c		\
		src/do_mkfs.c		\
		src/do_mkv.c		\
		src/do_mount.c		\
		src/do_mv.c		\
		src/do_mvtree.c		\
		src/do_param.c		\
		src/do_put.c		\
		src/do_puttree.c	\
		src/do_pwd.c		\
		src/do_quit.c		\
		src/do_rm.c		\
		src/do_rmdir.c		\
		src/do_rmtree.c		\
		src/do_show.c		\
		src/do_status.c		\
		src/do_sys.c		\
		src/do_touch.c		\
		src/do_umount.c		\
		src/do_walk.c		\
		src/do_waste.c		\
		src/do_xd.c		\
		src/dos_ftw.c		\
		src/dos_lfn.c		\
		src/dos_link.c		\
		src/dos_read.c		\
		src/dos_stat.c		\
		src/fat.c		\
		src/getopt.c		\
		src/main.c		\
		src/nm_expand.c

OBJ	=				\
		obj/aux.o		\
		obj/bio.o		\
		obj/cmd.o		\
		obj/dir_walk.o		\
		obj/do_arena.o		\
		obj/do_cat.o		\
		obj/do_cd.o		\
		obj/do_chmod.o		\
		obj/do_chsize.o		\
		obj/do_clri.o		\
		obj/do_cmp.o		\
		obj/do_compac.o		\
		obj/do_df.o		\
		obj/do_diff.o		\
		obj/do_du.o		\
		obj/do_edbad.o		\
		obj/do_fsck.o		\
		obj/do_get.o		\
		obj/do_gettree.o	\
		obj/do_lc.o		\
		obj/do_ls.o		\
		obj/do_mkdir.o		\
		obj/do_mkfile.o		\
		obj/do_mkfs.o		\
		obj/do_mkv.o		\
		obj/do_mount.o		\
		obj/do_mv.o		\
		obj/do_mvtree.o		\
		obj/do_param.o		\
		obj/do_put.o		\
		obj/do_puttree.o	\
		obj/do_pwd.o		\
		obj/do_quit.o		\
		obj/do_rm.o		\
		obj/do_rmdir.o		\
		obj/do_rmtree.o		\
		obj/do_show.o		\
		obj/do_status.o		\
		obj/do_sys.o		\
		obj/do_touch.o		\
		obj/do_umount.o		\
		obj/do_walk.o		\
		obj/do_waste.o		\
		obj/do_xd.o		\
		obj/dos_ftw.o		\
		obj/dos_lfn.o		\
		obj/dos_link.o		\
		obj/dos_read.o		\
		obj/dos_stat.o		\
		obj/fat.o		\
		obj/getopt.o		\
		obj/main.o		\
		obj/nm_expand.o

$PGM:		obj $OBJ
		$CC -u -s -o $PGM $OBJ
		size $PGM
		ls -l $PGM

c:		obj $OBJ

obj:		.exist
		mkdir obj

obj/aux.o:	src/aux.c		\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/aux.c

obj/bio.o:	src/bio.c		\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/bio.c

obj/cmd.o:	src/cmd.c		\
		h/common.h
		$CC $DEF -c -O -o obj src/cmd.c

obj/dir_walk.o:	src/dir_walk.c		\
		h/dos.h			\
		h/common.h
		$CC $DEF -c -O -o obj src/dir_walk.c

obj/do_arena.o:	src/do_arena.c
		$CC $DEF -c -O -o obj src/do_arena.c

obj/do_cat.o:	src/do_cat.c		\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/do_cat.c

obj/do_cd.o:	src/do_cd.c		\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/do_cd.c

obj/do_chmod.o:	src/do_chmod.c		\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/do_chmod.c

obj/do_chsize.o: src/do_chsize.c	\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/do_chsize.c

obj/do_clri.o:	src/do_clri.c		\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/do_clri.c

obj/do_cmp.o:	src/do_cmp.c		\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/do_cmp.c

obj/do_compac.o: src/do_compac.c	\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/do_compac.c

obj/do_df.o:	src/do_df.c		\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/do_df.c

obj/do_diff.o:	src/do_diff.c		\
		h/dos.h			\
		h/common.h
		$CC $DEF -c -O -o obj src/do_diff.c

obj/do_du.o:	src/do_du.c		\
		h/dos.h			\
		h/common.h
		$CC $DEF -c -O -o obj src/do_du.c

obj/do_edbad.o:	src/do_edbad.c		\
		h/dos.h			\
		h/common.h
		$CC $DEF -c -O -o obj src/do_edbad.c

obj/do_fsck.o:	src/do_fsck.c		\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/do_fsck.c

obj/do_get.o:	src/do_get.c		\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/do_get.c

obj/do_gettree.o: src/do_gettree.c	\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/do_gettree.c

obj/do_lc.o:	src/do_lc.c		\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/do_lc.c

obj/do_ls.o:	src/do_ls.c		\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/do_ls.c

obj/do_mkdir.o:	src/do_mkdir.c		\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/do_mkdir.c

obj/do_mkfile.o: src/do_mkfile.c	\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/do_mkfile.c

obj/do_mkfs.o:	src/do_mkfs.c		\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/do_mkfs.c

obj/do_mkv.o:	src/do_mkv.c		\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/do_mkv.c

obj/do_mount.o:	src/do_mount.c		\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/do_mount.c

obj/do_mv.o:	src/do_mv.c		\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/do_mv.c

obj/do_mvtree.o: src/do_mvtree.c	\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/do_mvtree.c

obj/do_param.o:	src/do_param.c		\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/do_param.c

obj/do_put.o:	src/do_put.c		\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/do_put.c

obj/do_puttree.o: src/do_puttree.c	\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/do_puttree.c

obj/do_pwd.o:	src/do_pwd.c		\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/do_pwd.c

obj/do_quit.o:	src/do_quit.c		\
		h/common.h
		$CC $DEF -c -O -o obj src/do_quit.c

obj/do_rm.o:	src/do_rm.c		\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/do_rm.c

obj/do_rmdir.o:	src/do_rmdir.c		\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/do_rmdir.c

obj/do_rmtree.o: src/do_rmtree.c	\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/do_rmtree.c

obj/do_show.o:	src/do_show.c		\
		h/dos.h			\
		h/common.h
		$CC $DEF -c -O -o obj src/do_show.c

obj/do_status.o: src/do_status.c	\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/do_status.c

obj/do_sys.o:	src/do_sys.c	\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/do_sys.c

obj/do_touch.o:	src/do_touch.c		\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/do_touch.c

obj/do_umount.o: src/do_umount.c	\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/do_umount.c

obj/do_walk.o:	src/do_walk.c		\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/do_walk.c

obj/do_waste.o: src/do_waste.c		\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/do_waste.c

obj/do_xd.o:	src/do_xd.c		\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/do_xd.c

obj/dos_ftw.o:	src/dos_ftw.c		\
		h/dos.h			\
		h/common.h
		$CC $DEF -c -O -o obj src/dos_ftw.c

obj/dos_lfn.o:	src/dos_lfn.c		\
		h/dos.h			\
		h/common.h
		$CC $DEF -c -O -o obj src/dos_lfn.c

obj/dos_link.o: src/dos_link.c		\
		h/dos.h			\
		h/common.h
		$CC $DEF -c -O -o obj src/dos_link.c

obj/dos_read.o: src/dos_read.c		\
		h/dos.h			\
		h/common.h
		$CC $DEF -c -O -o obj src/dos_read.c

obj/dos_stat.o: src/dos_stat.c		\
		h/dos.h			\
		h/common.h
		$CC $DEF -c -O -o obj src/dos_stat.c

obj/fat.o:	src/fat.c		\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/fat.c

obj/getopt.o:	src/getopt.c		\
		h/common.h
		$CC $DEF -c -O -o obj src/getopt.c

obj/main.o:	src/main.c		\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/main.c

obj/nm_expand.o: src/nm_expand.c	\
		h/common.h		\
		h/dos.h
		$CC $DEF -c -O -o obj src/nm_expand.c


lint:	
		$CC $DEF -T $SRC

sz:		sz.c			\
		h/common.h		\
		h/dos.h
		$CC $DEF -O -s -o sz sz.c
		sz

nm:
		nm -o obj/*.o >nm_list

cp:		$PGM
		cp -f $PGM $DIR
		chmod $PROT $DIR/$PGM
		ls -L $DIR/$PGM

cmp:
		-cmpobj $PGM $DIR

cmpq:
		-cmpobj -q $PGM $DIR

cmpqcp:
		-cmpobj -q $PGM $DIR || make cp

unld:
		-rm -f $PGM

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		man $PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

prman:
		sroff man.r | lpr

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0

gara:
		cd ..; gar -avs0 $PGM
		cd ..; gar -icv0

garc:
		cd ..; gar -ovs0		\
			-p $DIR/$PGM		\
			$PGM			\
			$DIR/$PGM		\

garcmp:
		cd ..; gar -icv0

zipto:
		-cmptree -sg . /zip$cmd/$PGM
		-forldiff /zip$cmd/$PGM
		-rm garfield

zipfrom:
		cd /zip$cmd/$PGM &&		\
		cmptree -sg . $cmd/$PGM;	\
		forldiff $cmd/$PGM;		\
		rm /zip$cmd/$PGM/garfield

pento:
		-mount /pen
		-cmptree -sg . /pen$cmd/$PGM
		-forldiff /pen$cmd/$PGM
		-rm -f garfield
		-umount /pen

penfrom:
		-mount /pen
		cd /pen$cmd/$PGM &&		\
		cmptree -sg . $cmd/$PGM;	\
		forldiff $cmd/$PGM;		\
		rm -f /pen$cmd/$PGM/garfield
		-umount /pen

pensave:
		-mount /pen
		-pento $cmd/$PGM
		-umount /pen

