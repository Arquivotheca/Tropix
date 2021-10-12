#*
#****************************************************************
#*	Makefile para o formato "exports"			*
#****************************************************************
#*
DIR	= fmt
PGM	= exports
MAN	= fmt

.virtual	man prman pento penfrom pensave

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		man -f $PGM

prman:		
		sroff man.r | lpr

pento:
		-mount /pen
		-cmptree -sg . /pen$libc/$DIR/$PGM
		-forldiff /pen$libc/$DIR/$PGM
		-rm -f garfield
		-umount /pen

penfrom:
		-mount /pen
		cd /pen$libc/$DIR/$PGM &&		\
		cmptree -sg . $libc/$DIR/$PGM;		\
		forldiff $libc/$DIR/$PGM;		\
		rm -f /pen$libc/$DIR/$PGM/garfield
		-umount /pen

pensave:
		-mount /pen
		-pento $libc/$DIR/$PGM
		-umount /pen
