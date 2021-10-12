/*
 ****************************************************************
 *								*
 *			sysent.c				*
 *								*
 *	Tabela de chamadas ao sistema				*
 *								*
 *	Versão	3.0.0, de 11.12.94				*
 *		4.8.0, de 27.10.05				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2005 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"

#include "../h/sysent.h"

#include "../h/extern.h"

/*
 ****************************************************************
 *	Declaração das funções					*
 ****************************************************************
 */
int	access ();
int	alarm ();
int	attention ();
int	boot ();
int	brk ();
int	chdir ();
int	chdirip ();
int	chmod ();
int	chown ();
int	chroot ();
int	close ();
int	settgrp ();
int	creat ();
int	dup ();
int	exec ();
int	exece ();
int	exit ();
int	fcntl ();
int	fork ();
int	fstat ();
int	ftime ();
int	getdirent ();
int	getdirpar ();
int	getgid ();
int	getpgrp ();
int	getpid ();
int	getppid ();
int	getpty ();
int	getscb ();
int	getsn ();
int	gettzmin ();
int	getuid ();
int	gtime ();
int	gtty ();
int	inopen ();
int	instat ();
int	itnet ();
int	itnetrcv ();
int	itnetsnd ();
int	ioctl ();
int	kcntl ();
int	kill ();
int	link ();
int	lockf ();
int	lseek ();
int	llseek ();
int	lstat ();
int	mkdir ();
int	mknod ();
int	mount ();
int	mutime ();
int	mutimes ();
int	nice ();
int	nosys ();
int	open ();
int	pause ();
int	pcntl ();
int	phys ();
int	pipe ();
int	plock ();
int	ptrace ();
int	read ();
int	readlink ();
int	rename ();
int	rmdir ();
int	select ();
int	setgid ();
int	setpgrp ();
int	setppid ();
int	setregid ();
int	setreuid ();
int	stime ();
int	setscb ();
int	setuid ();
int	shlib ();
int	shmem ();
int	sigchild ();
int	symlink ();
int	ksignal ();
int	spy ();
int	stat ();
int	stty ();
int	sync ();
int	thread ();
int	times ();
int	ueventctl ();
int	ueventdone ();
int	ueventtest ();
int	ueventwait ();
int	ulimit ();
int	umask ();
int	umount ();
int	uname ();
int	unlink ();
int	untext ();
int	upstat ();
int	usemactl ();
int	usemafree ();
int	usemalock ();
int	usematestl ();
int	ustat ();
int	utime ();
int	wait ();
int	write ();

/*
 ****************************************************************
 *	Tabela de chamadas ao sistema				*
 ****************************************************************
 */
const SYSENT	sysent[NSYSCALL] =
{
	/*
	 *	Parte Tradicional
	 */
	nosys,		0,	/*   0 = erro!		*/
	exit,		1,	/*   1 = exit		*/
	fork,		0,	/*   2 = fork		*/
	read,		3,	/*   3 = read		*/
	write,		3,	/*   4 = write		*/
	open,		3,	/*   5 = open		*/
	close,		1,	/*   6 = close		*/
	wait,		0,	/*   7 = wait		*/
	creat,		2,	/*   8 = creat		*/
	link,		2,	/*   9 = link		*/
	unlink,		1,	/*  10 = unlink		*/
	nosys,		2,	/*  11 = (exec)		*/
	chdir,		1,	/*  12 = chdir		*/
	gtime,		0,	/*  13 = (g)time	*/
	mknod,		3,	/*  14 = mknod		*/
	chmod,		2,	/*  15 = chmod		*/
	chown,		3,	/*  16 = chown		*/
	brk,		1,	/*  17 = brk		*/
	stat,		2,	/*  18 = stat		*/
	lseek,		3,	/*  19 = lseek		*/
	getpid,		0,	/*  20 = getpid		*/
	mount,		3,	/*  21 = mount		*/
	umount,		2,	/*  22 = umount		*/
	setuid,		1,	/*  23 = setuid		*/
	getuid,		0,	/*  24 = getuid		*/
	stime,		1,	/*  25 = stime		*/
	nosys,		0,	/*  26 = ptrace		*/
	alarm,		1,	/*  27 = alarm		*/
	fstat,		2,	/*  28 = fstat		*/
	pause,		0,	/*  29 = pause		*/
	utime,		2,	/*  30 = utime		*/
	nosys,		0,	/*  31 = stty		*/
	nosys,		0,	/*  32 = gtty		*/
	access,		2,	/*  33 = access		*/
	nice,		1,	/*  34 = nice		*/
	nosys,		0,	/*  35 = ftime		*/
	sync,		0,	/*  36 = sync		*/
	kill,		2,	/*  37 = kill		*/
	nosys,		0,	/*  38 = 		*/
	nosys,		0,	/*  39 = 		*/
	nosys,		0,	/*  40 = 		*/
	dup,		2,	/*  41 = dup		*/
	pipe,		0,	/*  42 = pipe		*/
	times,		1,	/*  43 = times		*/
	nosys,		0,	/*  44 = (prof)		*/
	nosys,		0,	/*  45 = 		*/
	setgid,		1,	/*  46 = setgid		*/
	getgid,		0,	/*  47 = getgid		*/
	ksignal,	3,	/*  48 = signal		*/
	nosys,		0,	/*  49 = 		*/
	nosys,		0,	/*  50 = 		*/
	nosys,		0,	/*  51 = (acct)		*/
	nosys,		0,	/*  52 = 		*/
	nosys,		0,	/*  53 = 		*/
	ioctl,		3,	/*  54 = ioctl		*/
	nosys,		0,	/*  55 = 		*/
	lockf,		3,	/*  56 = lockf		*/
	nosys,		0,	/*  57 = 		*/
	nosys,		0,	/*  58 = 		*/
	exece,		3,	/*  59 = exece		*/
	umask,		1,	/*  60 = umask		*/
	chroot,		1,	/*  61 = chroot		*/
	nosys,		0,	/*  62 = 		*/
	nosys,		0,	/*  63 = 		*/

	/*
	 *	Novos System Calls do SISTEM V ou BSD
	 */
	nosys,		0,	/*  64 =		*/
	fcntl,		3,	/*  65 = fcntl		*/
	getpgrp,	0,	/*  66 = getpgrp	*/
	getppid,	0,	/*  67 = getppid	*/
	nosys,		0,	/*  68 =		*/
	setpgrp,	0,	/*  69 = setpgrp	*/
	nosys,		0,	/*  70 = ulimit		*/
	uname,		1,	/*  71 = uname		*/
	ustat,		2,	/*  72 = ustat		*/
	setreuid,	2,	/*  73 = setreuid (BSD)	*/
	setregid,	2,	/*  74 = setregid (BSD)	*/
	mkdir,		2,	/*  75 = mkdir		*/
	rmdir,		1,	/*  76 = rmdir		*/
	boot,		0,	/*  77 = boot		*/
	gettzmin,	0,	/*  78 = gettzmin	*/
	setscb,		1,	/*  79 = setscb		*/
	getdirent,	4,	/*  80 = getdirent	*/
	select,		5,	/*  81 = select		*/
	symlink,	2,	/*  82 = symlink	*/
	readlink,	3,	/*  83 = readlink	*/
	lstat,		2,	/*  84 = lstat		*/
	rename,		2,	/*  85 = rename		*/
	llseek,		5,	/*  86 = llseek		*/
	getpty,		1,	/*  87 = getpty		*/
	nosys,		0,	/*  88 =		*/
	nosys,		0,	/*  89 =		*/
	nosys,		0,	/*  90 =		*/
	nosys,		0,	/*  91 =		*/
	nosys,		0,	/*  92 =		*/
	nosys,		0,	/*  93 =		*/
	nosys,		0,	/*  94 =		*/
	nosys,		0,	/*  95 =		*/

	/*
	 *	Chamadas ao sistema novas ou não permanentes
	 */
	phys,		3,	/*  96 = phys		*/
	plock,		1,	/*  97 = plock		*/
	chdirip,	1,	/*  98 = chdirip	*/
	getscb,		1,	/*  99 = getscb		*/
	spy,		0,	/* 100 = spy		*/
	shlib,		2,	/* 101 = shlib		*/
	mutime,		1,	/* 102 = mutime 	*/
	mutimes,	1,	/* 103 = mutimes	*/
	settgrp,	1,	/* 104 = settgrp 	*/
	inopen,		2,	/* 105 = inopen		*/
	kcntl,		3,	/* 106 = kcntl 		*/
	untext,		1,	/* 107 = untext		*/
	sigchild,	1,	/* 108 = sigchild	*/
	ueventctl,	3,	/* 109 = (u)eventctl	*/
	ueventwait,	2,	/* 110 = (u)eventwait	*/
	ueventdone,	1,	/* 111 = (u)eventdone	*/
	ueventtest,	1,	/* 112 = (u)eventtest	*/
	usemactl,	4,	/* 113 = (u)semactl	*/
	usemalock,	1,	/* 114 = (u)semalock	*/
	usemafree,	1,	/* 115 = (u)semafree	*/
	usematestl,	1,	/* 116 = (u)sematestl	*/
	shmem,		2,	/* 117 = shmem	*/
	thread,		0,	/* 118 = thread		*/
	attention,	4,	/* 119 = attention	*/
	pcntl,		2,	/* 120 = pcntl		*/
	setppid,	1,	/* 121 = setppid 	*/
	instat,		3,	/* 122 = instat		*/
	upstat,		2,	/* 123 = upstat		*/
	itnet,		4,	/* 124 = itnet 		*/
	itnetrcv,	4,	/* 125 = itnetrcv 	*/
	itnetsnd,	4,	/* 126 = itnetsnd 	*/
	getdirpar,	3	/* 127 = getdirpar 	*/

};	/*  end sysent	*/
