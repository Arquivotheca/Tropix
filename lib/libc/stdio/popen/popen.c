/*
 ****************************************************************
 *								*
 *			popen.c					*
 *								*
 *	Abre um arquivo com pipe				*
 *								*
 *	Versão	1.0.0, de 27.11.87				*
 *		3.2.3, de 05.01.00				*
 *								*
 *	Módulo: popen						*
 *		libc/stdio					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

/*
 ****************************************************************
 *	Constantes e váriaveis externas				*
 ****************************************************************
 */
#define	_EOMEM	(char *)EOF
#define elif	else if
#define NOINTP	(int *)NULL

/*
 ****************************************************************
 *	Variáveis e Funções Locais				*
 ****************************************************************
 */
static int	*pids;			/* Identificação dos filhos */
static int	*stats;			/* Resultado de retorno dos filhos */
static int	*status (int);		/* Busca o stat de um processo */

/*
 ****************************************************************
 *	Abre um "pipe"						*
 ****************************************************************
 */
FILE *
popen (register const char *command, register const char *mode)
{
	register FILE	*fp;
	int	 	fd[2];
	register int 	son, par;
	register int	pid;
	register int	d;

	/*
	 *	Prólogo
	 */
	if (pids == NOINTP)
	{
		if ((pids = malloc (_NFILE * sizeof (int))) == NOINTP)
			return (NOFILE);

		if ((stats = malloc ((_NFILE + 1) * sizeof (int))) == NOINTP)
			return (NOFILE);
	}

	/*
	 *	Pega a primeira estrutura disponível
	 */
	for (fp = _ioblk; /* vazio */; fp++)
	{
		if (fp >= &_ioblk[_NFILE])
			return (NOFILE);

		if (fp->_flag == 0)
			break;
	}

	/*
	 *	Verifica o modo de pipe:
	 */
	if   (*mode == 'r')
	{
		par = 0;	/* Pai fará leitura */
		son = 1;	/* Filho fará escrita */
	}
	elif (*mode == 'w')
	{
		par = 1;	/* Pai fará escrita */
		son = 0;	/* Filho fará leitura */
	}
	else
	{
		return (NOFILE);
	}

	/*
	 *	Cria o PIPE
	 */
	if (pipe (fd) < 0)
		return (NOFILE);

	/*
	 *	Cria o Filho
	 */
	switch (pid = fork ())
	{
		/*
		 *	Erro
		 */
	    case -1:
		close (fd[son]);
		close (fd[par]);
		fp = NOFILE;
		break;

		/*
		 *	Filho
		 *
		 *	Troca descritor padrão
		 */
	    case 0:
		close (son);

		if (fcntl (fd[son], F_DUPFD, son) != son)
			return (NOFILE);

		/*
		 *	Fecha o descritor do pai
		 */
		close (fd[par]);

		/*
		 *	Fecha os outros arquivos
		 */
		for (d = 3 ; d < _NFILE ; d++)
			close (d);

		/*
		 *	Executa o comando
		 */
		execl ("/bin/sh", "sh", "-c", command, NULL);
		exit (1);

		/*
		 *	Pai
		 *
		 *	Fecha o descritor do filho
		 */
	    default:
		close (fd[son]);

		/*
		 *	Inicializa a estrutura FILE
		 */
		fp->_ptr = _EOMEM;
		fp->_flag = (par == 0) ? _IOREAD : _IOWRITE;
		fp->_file = fd[par];

		/*
		 *	Guarda a identificação e marca que ainda não terminou
		 */
		pids[fp->_file] = pid;
		stats[fp->_file] = -1;
		break;

	}	/* end switch */

	return (fp);

}	/* end popen */

/*
 ****************************************************************
 *	Fecha um "pipe"						*
 ****************************************************************
 */
int
pclose (FILE *fp)
{
	register int	fpid;
	register int	pid;
	int		stat;

	/*
	 *	Prólogo
	 */
	if (pids == NOINTP)
		return (-1);

	/*
	 *	Verifica a validade do fp
	 */
	if ((fp->_flag & (_IOREAD|_IOWRITE)) == 0)
		return (-1);

	/*
	 *	Pega o processo correspondente ao fp
	 */
	fpid = pids[fp->_file];

	/*
	 *	Fecha o arquivo
	 */
	fclose (fp);

	/*
	 *	Pega o status
	 */
	if ((stat = *status (fpid)) < 0)
	{
		while ((pid = wait (&stat)) >= 0)
		{
			*status (pid) = stat;

			if (pid == fpid)
				break;
		}
	}

	pids[fp->_file] = 0;

	return (stat);

}	/* end pclose */

/*
 ****************************************************************
 *	Procura o status de retorno de um filho			*
 ****************************************************************
 */
static int *
status (register int pid)
{
	register int	p;

	stats[_NFILE] = -1;

	for (p = 0 ; p < _NFILE;  p++)
	{
		if (pids[p] == pid)
			break;
	}

	return (&stats[p]);

}	/* end status */
