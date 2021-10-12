/*
 ****************************************************************
 *								*
 *		    <sys/syscall.h>				*
 *								*
 *	Declarações das chamadas ao sistema			*
 *								*
 *	Versão	3.0.0, de 01.09.94				*
 *		4.6.0, de 06.07.04				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *		/usr/include/sys				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

/*
 ****************************************************************
 *	Protótipos das funções					*
 ****************************************************************
 *
 *	Não estão incluídos as chamadas ao sistema:
 *
 *	mutime,  mutimes, times,  uname
 */

/*
 ******	Alguns tipos necessários ********************************
 */
typedef unsigned long	off_t;	/* Posição de um arquivo */
typedef long long	loff_t;	/* Posição de um arquivo */
typedef	long   		time_t;	/* Tempo em segundos desde 1.1.70 */

/*
 ******	Protótipos de funções ***********************************
 */
extern void	_exit (int);
extern int	access (const char *, int);
extern unsigned	alarm (unsigned);
extern int	attention (int, const int[], int, int);
extern int	boot (void);
extern int	brk (const void *);
extern int	chdir (const char *);
extern int	chmod (const char *, int);
extern int	chown (const char *, int, int);
extern int	chroot (const char *);
extern int	close (int);
extern int	settgrp (int);
extern int	creat (const char *, int);
extern int	dup (int);
extern int	dup2 (int, int);
extern int	eventctl (int, int, int);
extern int	eventwait (int, int);
extern int	eventdone (int);
extern int	eventtest (int);
extern int	execl (const char *, const char *, ...);
extern int	execle (const char *, const char *, ...);
extern int	execv (const char *, const char *[]);
extern int	execve (const char *, const char *[], const char *[]);
extern void	exit (int);
extern int	fcntl (int, int, int);
extern int	fork (void);
extern int	getegid (void);
extern int	geteuid (void);
extern int	getgid (void);
extern long	getpgrp (void);
extern long	getpid (void);
extern long	getppid (void);
extern long	getsn (void);
extern int	gettzmin (void);
extern int	getuid (void);
extern int	ioctl (int, int, ...);
extern int	itnet (int, int, ...);
extern int	itnetrcv (int, void *, int, int *);
extern int	itnetsnd (int, const void *, int, int);
extern int	kcntl (int, ...);
extern int	kill (int, int);
extern int	link (const char *, const char *);
extern int	lockf (int, int, off_t);
extern off_t	lseek (int, off_t, int);
extern int	llseek (int, loff_t, loff_t *, int);
extern int	mkdir (const char *, int);
extern int	mknod (const char *, int, int);
extern int	nice (int);
extern int	open (const char *, int, ...);
extern int	inopen (int, int);
extern int	pause (void);
extern int	pcntl (int, ...);
extern void	*phys (const void *, int, int);
extern int	pipe (int []);
extern int	plock (int);
extern int	ptrace (int, int, int, int);
extern int	read (int, void *, int);
extern int	readlink (const char *, char *, int);
extern int	rename (const char *, const char *);
extern int	rmdir (const char *);
extern void	*sbrk (int);
extern int	semactl (int, int, ...);
extern int	semalock (int);
extern int	semafree (int);
extern int	sematestl (int);
extern int	setgid (int);
extern int	setrgid (int);
extern int	setegid (int);
extern int	setpgrp (void);
extern int	setuid (int);
extern int	setruid (int);
extern int	seteuid (int);
extern int	setppid (long);
extern void	(*signal (int, void (*) (int, ...))) (int, ...);
extern void	*shmem (int, int);
extern int	sigchild (long);
extern int	spy (void);
extern int	stime (const time_t *);
extern int	symlink (const char *, const char *);
extern int	sync (void);
extern time_t	time (time_t *);
extern int	thread (void);
extern int	umask (int);
extern int	unlink (const char *);
extern int	untext (const char *);
extern int	utime (const char *, const time_t []);
extern int	wait (int *);
extern int	write (int, const void *, int);

#ifdef	INODE_H
extern int	chdirip (const INODE *);
#endif	INODE_H

#ifdef	SCB_H
extern SCB	*getscb (SCB *);
extern int	setscb (SCB *);
#endif	SCB_H

#ifdef	STAT_H
extern int	stat (const char *, STAT *); 
extern int	lstat (const char *, STAT *); 
extern int	fstat (int, STAT *); 
extern int	instat (int, int, STAT *); 
extern int	upstat (int, STAT *); 
#endif	STAT_H

#ifdef	TERMIO_H
extern int	getpty (PTYIO *);
#endif	TERMIO_H

/*
 ******	Algumas variáveis associadas a chamadas ao sistema ******
 */
extern void		*_cbreak;
extern char		etext, edata, end;
extern const char	**environ;
extern void		*_faddr;
extern void		*_fpc;
extern int		_attention_index;
