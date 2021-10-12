#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <fcntl.h>
#include <utmp.h>
#include <signal.h>
#include <pwd.h>
#include <termio.h>

/*
 ****************************************************************
 *	Variaves Globais e Definicoes				*
 ****************************************************************
 */
PASSWD	usuarioinv  = {"", "???"};	/* Para Conta errada */
UTMP	utmp;			/* Para atualizar "/etc/utmp" */
TERMIO	termio;			/* Estado do Terminal */

char	name[16];		/* Nome do Usuario */

char	*pgname;		/* Nome do Programa */


/*
 ****************************************************************
 *	Funcoes chamadas					*
 ****************************************************************
 */
PASSWD	*getpwnam ();
char	*strcat ();
char	*crypt ();
char	*getpass ();
char	*rindex (), *index ();
time_t	time ();

/*
 ****************************************************************
 *	Login							*
 ****************************************************************
 */
int
main (argc, argv)
char **argv;
{
	register char	*cp;
	register PASSWD	*pwd;
	short		lowcase;
	int		c;

	pgname = argv[0];

	/*
	 *	Leitura do Nome do Usuario.
	 */
    loop:
	name[0] = '\0';

	if ((name)[0] == '\0')
	{

		ioctl (2, TCGETS, &termio);
		termio.c_iflag &= ~IUCLC;
		termio.c_oflag &= ~OLCUC;
		ioctl (2, TCSETAW, &termio);

		lowcase = 0;

		while ((name)[0] == '\0')
		{
			cp = name;
			printf ("\007CONTA: ");
			while ((c = getchar()) != '\n')
			{
				if ('a' <= c && c <= 'z')
					lowcase++;
				if ('A' <= c && c <= 'Z')
					c += 'a' - 'A';
				if (c == EOF)
					exit (0);
				if (cp < name + sizeof (name) - 1)
					*cp++ = c;
			}
		}

		if (lowcase == 0)
		{
			termio.c_iflag |= IUCLC;
			termio.c_oflag |= OLCUC;
			ioctl (2, TCSETAW, &termio);
		}

	}

	/*
	 *	Prepara para ler a Senha.
	 */
	idcpy (utmp.ut_name, name);

	if ((pwd = getpwnam ((char *)name)) == NOPASSWD)
	{
		printf ("Conta inexistente\n");
		goto loop;
	}

	/*
	 *	Le a Senha.
	 */
	if (pwd->pw_passwd[0] != '\0')
	{
		cp = crypt (getpass ("Senha: "), pwd->pw_passwd);

		if (strcmp (cp, pwd->pw_passwd))
		{
			printf
			(	"*** Senha INCORRETA: \"%s\", \"%s\"\n",
				cp, pwd->pw_passwd
			);
			goto loop;
		}
	}

	printf
	(	"Senha correta: \"%s\", \"%s\"\n",
		cp, pwd->pw_passwd
	);
	goto loop;

}	/* end login */
