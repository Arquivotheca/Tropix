#*
#****************************************************************
#*	Makefile global para a "libcurses"			*
#****************************************************************
#*
PGM	= libcurses
LIB	= /lib/$PGM
TI	= /usr/lib
PROT	= a+xr

.virtual	make lint cp cmp man modif dirs clean rmobj

make:		dirs
		cd terminfo;	m
		cd curses;	m

lint:
		cd terminfo;	m lint
		cd curses;	m lint

cp:		dirs
		cd terminfo;	m cp
		cd curses;	m cp

cmp:
		cd terminfo;	m cmp
		cd curses;	m cmp
		mklib -k /lib/libcurses

man:
		cd terminfo;	m man
		cd curses;	m man

modif:		modif.r
		sroff modif.r >$mandir/modif/curses/curses
		modif curses

dirs:		$LIB/curses $LIB/terminfo $TI $TI/terminfo
		cd terminfo/h;		m cp
		cd curses/curses;	m

$LIB/curses:	.exist
		mkdir -f $LIB/curses
		chmod $PROT $LIB $LIB/curses

$LIB/terminfo:	.exist
		mkdir -f $LIB/terminfo
		chmod $PROT $LIB $LIB/terminfo

$TI:		.exist
		mkdir $TI
		chmod $PROT $TI

$TI/terminfo:	.exist
		mkdir $TI/terminfo
		chmod $PROT $TI/terminfo

clean:
	walk -p "*/?.o */? *bobo *garfield" . | rm -iN

rmobj:
	walk -p "*.o" . | rm
