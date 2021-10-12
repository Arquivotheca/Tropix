#include <sys/common.h>
#include <sys/sync.h>
#include <sys/scb.h>
#include <sys/tty.h>
#include <sys/syscall.h>
#include <sys/region.h>
#include <sys/signal.h>
#include <sys/uproc.h>
#include <sys/kcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <a.out.h>

void	print_pty_info (const PTY *, int);

int
main (int argc, const char *argv[])
{
	int		pty_no;
	SCB		scb;
	const PTY	*pty_base;

	if (getscb (&scb) == (SCB *)-1)
		error ("$*Não consegui obter o SCB");

	if ((pty_base = phys (scb.y_pty, scb.y_npty * sizeof (PTY), O_RDONLY|O_KERNEL)) == (PTY *)-1)
		error ("$*Não consegui mapear a tabela de pseudo-terminais");

	if (argc == 1)
	{
		for (pty_no = 0; pty_no < scb.y_npty; pty_no++)
			print_pty_info (pty_base, pty_no);
	}
	else if (streq (argv[1], "-H"))
	{
		error ("$Use: pty [<num> ...]");
	}

	while (*++argv != NULL)
	{
		if ((pty_no = atol (*argv)) < 0 || pty_no >= scb.y_npty)
			error ("$Número inválido para pty (%d)", pty_no);

		print_pty_info (pty_base, pty_no);
	}

	return (0);

}	/* end main */

void
print_pty_info (const PTY *pty_base, int pty_no)
{
	const PTY	*pty = pty_base + pty_no;
	const TTY	*tty = &pty->pty;
	const UPROC	*up;

	printf ("/dev/ptyc%02d:\t", pty_no);

	printf ("ptyc_nopen = %d, ptys_nopen = %d, ", pty->ptyc_nopen, pty->ptys_nopen);
	printf ("ptyc_lock = %d, ptys_lock = %d, ", pty->ptyc_lock,  pty->ptys_lock);
	printf ("t_olock = %d, t_obusy = %d\n", tty->t_olock, tty->t_obusy);

	printf
	(	"\t\tt_state:   0x%B\n",
		tty->t_state,
		"\x05" "ONEINT"
		"\x04" "PAGING"
		"\x03" "ESCAPE"
		"\x02" "TTYSTOP"
		"\x01" "ISOPEN"
	);

	if ((tty->t_state & ISOPEN) == 0)
		{ printf ("\n"); return; }

	if (tty->t_oproc != NOVOID)
	{
		SYM	*sp = alloca (SYM_SIZEOF (80)); sp->s_nm_len = 80;

		if (kcntl (K_GETSYMGA, tty->t_oproc, sp) == 0)
			printf ("\t\tt_oproc:   %s\n", sp->s_name[0] == '_' ? sp->s_name + 1 : sp->s_name);
	}

	printf
	(	"\t\tt_inq:     c_count = %d, c_cf = %P, c_cl = %P\n"
		"\t\t           inqnempty = %d\n",
		tty->t_inq.c_count, tty->t_inq.c_cf, tty->t_inq.c_cl, tty->t_inqnempty
	);

	printf
	(	"\t\tt_outq:    c_count = %d, c_cf = %P, c_cl = %P\n"
		"\t\t           outqnfull = %d, outqempty = %d, outqnempty = %d, outqstart = %d\n",
		tty->t_outq.c_count, tty->t_outq.c_cf, tty->t_outq.c_cl,
		tty->t_outqnfull, tty->t_outqempty, tty->t_outqnempty, tty->t_outqstart
	);

	printf ("\t\tattention: ");

	if (pty->uproc != NOUPROC)
	{
		if ((up = phys (pty->uproc, sizeof (UPROC), O_RDONLY|O_KERNEL)) == (UPROC *)-1)
		{
			printf ("<???>");
		}
		else
		{
			printf ("<%s, %d>", up->u_pgname, up->u_pid);
			phys (up, 0, 0);
		}
	}
	else
	{
		printf ("NOUPROC");
	}

	printf (", index = %d, set = %d\n", pty->index, pty->attention_set);

	printf ("\n");

}	/* end print_pty_info */
