#*
#****************************************************************
#*	Makefile global para "libcurses/curses"			*
#****************************************************************
#*
PGM	= curses
MAN	= curses

man_exec = DUMMY
.putenv man_exec

.virtual	obj n lint cp cmp man modif

obj:
	cd curses;	make 
	cd addch;	make 
	cd addstr;	make 
	cd beep;	make 
	cd box;		make 
	cd clrtoeol;	make 
	cd data;	make 
	cd delch;	make 
	cd deleteln;	make 
	cd delwin;	make 
	cd erase;	make 
	cd getch;	make 
	cd getctlch;	make 
	cd getstr;	make 
	cd incurses;	make 
	cd insch;	make 
	cd insertln;	make 
	cd insstr;	make 
	cd move;	make 
	cd mvwin;	make 
	cd newwin;	make 
	cd outcurses;	make 
	cd printw;	make 
	cd refresh;	make 
	cd scanw;	make 
	cd scroll;	make 
	cd setctlch;	make 
	cd setmove;	make 
	cd setswitch;	make 
	cd term;	make 
	cd touchwin;	make 

n:
	cd curses;	make -n 
	cd addch;	make -n
	cd addstr;	make -n
	cd beep;	make -n
	cd box;		make -n
	cd clrtoeol;	make -n
	cd data;	make -n
	cd delch;	make -n
	cd deleteln;	make -n
	cd delwin;	make -n
	cd erase;	make -n
	cd getch;	make -n
	cd getctlch;	make -n 
	cd getstr;	make -n
	cd incurses;	make -n
	cd insch;	make -n
	cd insertln;	make -n
	cd insstr;	make -n
	cd move;	make -n
	cd mvwin;	make -n
	cd newwin;	make -n
	cd outcurses;	make -n
	cd printw;	make -n
	cd refresh;	make -n
	cd scanw;	make -n
	cd scroll;	make -n
	cd setctlch;	make -n
	cd setmove;	make -n
	cd setswitch;	make -n
	cd term;	make -n
	cd touchwin;	make -n

lint:
#	cd curses;	make lint
	cd addch;	make lint
	cd addstr;	make lint
	cd beep;	make lint
	cd box;		make lint
	cd clrtoeol;	make lint
	cd data;	make lint
	cd delch;	make lint
	cd deleteln;	make lint
	cd delwin;	make lint
	cd erase;	make lint
	cd getch;	make lint
	cd getctlch;	make lint
	cd getstr;	make lint
	cd incurses;	make lint
	cd insch;	make lint
	cd insertln;	make lint
	cd insstr;	make lint
	cd move;	make lint
	cd mvwin;	make lint
	cd newwin;	make lint
	cd outcurses;	make lint
	cd printw;	make lint
	cd refresh;	make lint
	cd scanw;	make lint
	cd scroll;	make lint
	cd setctlch;	make lint
	cd setmove;	make lint
	cd setswitch;	make lint
	cd term;	make lint
	cd touchwin;	make lint

cp:
	cd curses;	make  cp
	cd addch;	make  cp
	cd addstr;	make  cp
	cd beep;	make  cp
	cd box;		make  cp
	cd clrtoeol;	make  cp
	cd data;	make  cp
	cd delch;	make  cp
	cd deleteln;	make  cp
	cd delwin;	make  cp
	cd erase;	make  cp
	cd getch;	make  cp
	cd getctlch;	make  cp
	cd getstr;	make  cp
	cd incurses;	make  cp
	cd insch;	make  cp
	cd insertln;	make  cp
	cd insstr;	make  cp
	cd move;	make  cp
	cd mvwin;	make  cp
	cd newwin;	make  cp
	cd outcurses;	make  cp
	cd printw;	make  cp
	cd refresh;	make  cp
	cd scanw;	make  cp
	cd scroll;	make  cp
	cd setctlch;	make  cp
	cd setmove;	make  cp
	cd setswitch;	make  cp
	cd term;	make  cp
	cd touchwin;	make  cp

cmp:
	cd curses;	make  cmp
	cd addch;	make  cmp
	cd addstr;	make  cmp
	cd beep;	make  cmp
	cd box;		make  cmp
	cd clrtoeol;	make  cmp
	cd data;	make  cmp
	cd delch;	make  cmp
	cd deleteln;	make  cmp
	cd delwin;	make  cmp
	cd erase;	make  cmp
	cd getch;	make  cmp
	cd getctlch;	make  cmp
	cd getstr;	make  cmp
	cd incurses;	make  cmp
	cd insch;	make  cmp
	cd insertln;	make  cmp
	cd insstr;	make  cmp
	cd move;	make  cmp
	cd mvwin;	make  cmp
	cd newwin;	make  cmp
	cd outcurses;	make  cmp
	cd printw;	make  cmp
	cd refresh;	make  cmp
	cd scanw;	make  cmp
	cd scroll;	make  cmp
	cd setctlch;	make  cmp 
	cd setmove;	make  cmp
	cd setswitch;	make  cmp
	cd term;	make  cmp
	cd touchwin;	make  cmp

man:
	cd curses;	make  man
	cd addch;	make  man
	cd addset;	make  man 
	cd addstr;	make  man
	cd attrset;	make  man
	cd beep;	make  man
	cd box;		make  man
	cd clrtoeol;	make  man
	cd data;	make  man
	cd delch;	make  man
	cd deleteln;	make  man
	cd delwin;	make  man
	cd erase;	make  man
	cd getch;	make  man
	cd getctlch;	make  man 
	cd getstr;	make  man
	cd getset;	make  man
	cd incurses;	make  man
	cd insch;	make  man
	cd insertln;	make  man
	cd insstr;	make  man
	cd move;	make  man
	cd mvwin;	make  man
	cd newwin;	make  man
	cd outcurses;	make  man
	cd printw;	make  man
	cd refresh;	make  man
	cd refset;	make  man
	cd scanw;	make  man
	cd scroll;	make  man 
	cd setctlch;	make  man
	cd setmove;	make  man
	cd setswitch;	make  man
	cd term;	make  man
	cd touchwin;	make  man

modif:	modif.r
	sroff modif.r >$mandir/modif/$MAN/$PGM
	modif $PGM
