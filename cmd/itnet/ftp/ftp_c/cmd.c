/*
 ****************************************************************
 *								*
 *			cmd.c					*
 *								*
 *	File transfer protocol (cliente)			*
 *								*
 *	Versão	3.0.0, de 12.10.92				*
 *		3.0.4, de 23.08.97				*
 *								*
 *	Módulo: FTP_C						*
 *		Utilitários de comunicações			*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <xti.h>

#include "ftp_c.h"
#include "../ftp.h"

/*
 ****************************************************************
 *	Comando "CWD"						*
 ****************************************************************
 */
void
do_cd (int argc, const char *argv[], const CMDTB *tp)
{
	if (argc > 1)
	{
		printf ("Uso: %s %s\n", tp->c_name, tp->c_args);
		return;
	}

	if (argc == 0)
		comando (1, "CWD");
	else
		comando (1, "CWD %s", argv[0]);

}	/* end do_cd */

/*
 ****************************************************************
 *	Comando "cdup"					*
 ****************************************************************
 */
void
do_cdup (int argc, const char *argv[], const CMDTB *tp)
{
	if (argc > 0)
	{
		printf ("Uso: %s %s\n", tp->c_name, tp->c_args);
		return;
	}

	comando (1, "CDUP");

}	/* end do_cdup */

/*
 ****************************************************************
 *	Comando "QUIT"						*
 ****************************************************************
 */
void
do_quit (int argc, const char *argv[], const CMDTB *tp)
{
	if (argc > 0)
	{
		printf ("Uso: %s %s\n", tp->c_name, tp->c_args);
		return;
	}

	if (comando (1, "QUIT") != COMPLETE)
		return;

	if (t_sndrel (cntl_fd) < 0)
		error ("$*Erro no SNDREL");

	if (t_rcvrel (cntl_fd) < 0)
		error ("$*Erro no RCVREL");

	t_close (cntl_fd);

 	exit (0);

}	/* end do_quit */

/*
 ****************************************************************
 *	Comando "QUOTE"						*
 ****************************************************************
 */
void
do_quote (int argc, const char *argv[], const CMDTB *tp)
{
	switch (argc)
	{
	    case 1:
		comando (1, argv[0]);
		break;

	    case 2:
		comando (1, "%s %s", argv[0], argv[1]);
		break;

	    case 3:
		comando (1, "%s %s %s", argv[0], argv[1], argv[2]);
		break;

	    default:
		printf ("Uso: %s %s\n", tp->c_name, tp->c_args);
		break;

	}	/* end switch */

}	/* end do_quote */

/*
 ****************************************************************
 *	Passa o tipo para ISO (ASCII)				*
 ****************************************************************
 */
void
do_ascii (int argc, const char *argv[], const CMDTB *tp)
{
	if (comando (1, "TYPE A") == COMPLETE)
		type = TYPE_A;

}	/* end do_ascii */

/*
 ****************************************************************
 *	Passa o tipo para IMAGE					*
 ****************************************************************
 */
void
do_image (int argc, const char *argv[], const CMDTB *tp)
{
	if (comando (1, "TYPE I") == COMPLETE)
		type = TYPE_I;

}	/* end do_image */

/*
 ****************************************************************
 *	Altera o tipo de transmissão				*
 ****************************************************************
 */
void
do_settype (int argc, const char *argv[], const CMDTB *tp)
{
	if (argc == 0)
	{
		printf
		(	"Usando o tipo %s para transferir arquivos\n",
			type == TYPE_A ? "ISO (ASCII)" : "IMAGEM"
		);
		return;
	}

	if (argc != 1)
	{
		printf ("Uso: %s %s\n", tp->c_name, tp->c_args);
		return;
	}

	switch (toupper (argv[0][0]))
	{
	    case 'A':
		if (comando (1, "TYPE A") == COMPLETE)
			type = TYPE_A;
		break;

	    case 'B':
	    case 'I':
		if (comando (1, "TYPE I") == COMPLETE)
			type = TYPE_I;
		break;

	    case 'E':
		printf ("Tipo EBCDIC não implementado\n");
		break;

	    case 'L':
		printf ("Tipo LOCAL não implementado\n");
		break;

	    default:
		printf ("Tipo inválido: \"%s\"\n", argv[0]);
		break;

	}	/* end switch */

}	/* end do_settype */

/*
 ****************************************************************
 *	Processa o comando GET					*
 ****************************************************************
 */
void
do_get (int argc, const char *argv[], const CMDTB *tp)
{
	const char	*local_file_nm, *remote_file_nm;
	STAT		s;

	if (argc < 1 || argc > 2)
	{
		printf ("Uso: %s %s\n", tp->c_name, tp->c_args);
		return;
	}

	remote_file_nm = argv[0];

	if (argc == 1)
		local_file_nm = remote_file_nm;
	else
		local_file_nm = argv[1];

	if (stat (local_file_nm, &s) >= 0)
	{
		printf ("Arquivo \"%s\" já existe. Remove/reescreve? (n) ", local_file_nm);

		if (askyesno () <= 0)
			return;

		if (!S_ISREG (s.st_mode) && unlink (local_file_nm) < 0)
		{
			error ("*Não consegui remover \"%s\"", local_file_nm);
			return;
		}
	}

	recebe ("RETR", remote_file_nm, local_file_nm);

}	/* end do_get */

/*
 ****************************************************************
 *	Comando MGET						*
 ****************************************************************
 */
void
do_mget (int argc, const char *argv[], const CMDTB *tp)
{
	FILE		*tmp_fp;
	int		old_type, fflag = 0;
	char		buf[BLSZ];
	const char	*tmp_nm, *buf_addr = buf;

	if (argc > 0 && streq (argv[0], "-f"))
		{ fflag++; argc--; argv++; }

	if (argc > 1)
	{
		printf ("Uso: %s %s\n", tp->c_name, tp->c_args);
		return;
	}

	if ((tmp_nm = mktemp ("/tmp/ftpXXXXXX")) == NOSTR)
		{ error ("Não consegui obter um nome temporário"); return; }

	old_type = type; 	type = TYPE_A;

	if (argc == 0)
		recebe ("NLST", NOSTR, tmp_nm);
	else
		recebe ("NLST", argv[0], tmp_nm);

	type = old_type;

	if ((tmp_fp = fopen (tmp_nm, "r")) == NOFILE)
		{ error ("*Não consegui abrir o arquivo temporário"); return; }

	while (fngets (buf, sizeof (buf), tmp_fp) != NOSTR)
	{
		if (received_intr)
			break;

		if (fflag)
		{
			printf ("%s:\n", buf);
		}
		else
		{
			printf ("Copia \"%s\"? (n): ", buf);

			if (askyesno () <= 0)
				continue;
		}

		do_get (1, &buf_addr, tp);
	}

	fclose (tmp_fp);

	if (unlink (tmp_nm) <  0)
		error ("*Não consegui remover o arquivo temporário");

}	/* end do_mget */

/*
 ****************************************************************
 *	Processa o comando PUT					*
 ****************************************************************
 */
void
do_put (int argc, const char *argv[], const CMDTB *tp)
{
	const char	*local_file_nm, *remote_file_nm;
	STAT		s;

	if (argc < 1 || argc > 2)
	{
		printf ("Uso: %s %s\n", tp->c_name, tp->c_args);
		return;
	}

	local_file_nm = argv[0];

	if (argc == 1)
		remote_file_nm = local_file_nm;
	else
		remote_file_nm = argv[1];

	if (stat (local_file_nm, &s) < 0)
	{
		error ("*O arquivo \"%s\" não foi encontrado", local_file_nm);
		return;
	}

	if (!S_ISREG (s.st_mode))
	{
		error ("O arquivo \"%s\" não é regular", local_file_nm);
		return;
	}

	envia ("STOR", local_file_nm, remote_file_nm);

}	/* end do_put */

/*
 ****************************************************************
 *	Comando MPUT						*
 ****************************************************************
 */
void
do_mput (int argc, const char *argv[], const CMDTB *tp)
{
	int		fflag = 0;
	char		*area;
	const char	*area_ptr;
	const char	*dir_nm;
	char		*cp_point;
	int		dev;
	STAT		s;
	DIR		*dir_fp;
	const DIRENT	*dp;

	if (argc > 0 && streq (argv[0], "-f"))
		{ fflag++; argc--; argv++; }

	if (argc > 1)
	{
		printf ("Uso: %s %s\n", tp->c_name, tp->c_args);
		return;
	}

	if (argc == 0 || streq (argv[0], "."))
	{
		dir_nm = ".";
		area = alloca (sizeof (DIRENT) + 1);

		cp_point = area;
	}
	else
	{
		dir_nm = argv[0];
		area = alloca (strlen (dir_nm) + sizeof (DIRENT) + 2);

		strcpy (area, dir_nm); strcat (area, "/");

		cp_point = strend (area);
	}

	area_ptr = area;

	if ((dir_fp = opendir (dir_nm)) == NODIR)
	{
		error
		(	"*Não consegui abrir o diretório \"%s\"",
			dir_nm
		);
		return;
	}

	if (fstat (dir_fp->d_fd, &s) < 0)
	{
		error
		(	"*Não consegui obter o estado do diretório \"%s\"",
			dir_nm
		);
		closedir (dir_fp);
		return;
	}

	dev = s.st_dev;

	while ((dp = readdir (dir_fp)) != NODIRENT)
	{
		if (received_intr)
			break;

		if (dp->d_name[0] == '.')
		{
			if   (dp->d_name[1] == '\0')
				continue;
			elif (dp->d_name[1] == '.' && dp->d_name[2] == '\0')
				continue;
		}

		strcpy (cp_point, dp->d_name);

		if (instat (dev, dp->d_ino, &s) < 0)
			continue;

		if (!S_ISREG (s.st_mode))
			continue;

		if (fflag)
		{
			printf ("%s:\n", area);
		}
		else
		{
			printf ("Copia \"%s\"? (n): ", area);

			if (askyesno () <= 0)
				continue;
		}

		do_put (1, &area_ptr, tp);
	}

	closedir (dir_fp);

}	/* end do_mput */

/*
 ****************************************************************
 *	Processa o comando CMP					*
 ****************************************************************
 */
void
do_cmp (int argc, const char *argv[], const CMDTB *tp)
{
	const char	*local_file_nm, *remote_file_nm;
	STAT		s;

	if (argc < 1 || argc > 2)
	{
		printf ("Uso: %s %s\n", tp->c_name, tp->c_args);
		return;
	}

	local_file_nm = argv[0];

	if (argc == 1)
		remote_file_nm = local_file_nm;
	else
		remote_file_nm = argv[1];

	if (stat (local_file_nm, &s) < 0)
	{
		error ("*Não encontrei o arquivo \"%s\"", local_file_nm);
		return;
	}

	if (!S_ISREG (s.st_mode))
	{
		error ("O arquivo \"%s\" não é regular", local_file_nm);
		return;
	}

	compara ("RETR", remote_file_nm, local_file_nm);

}	/* end do_cmp */

/*
 ****************************************************************
 *	Processa o comando RENAME				*
 ****************************************************************
 */
void
do_rename (int argc, const char *argv[], const CMDTB *tp)
{
	if (argc != 2)
	{
		printf ("Uso: %s %s\n", tp->c_name, tp->c_args);
		return;
	}

	if (comando (1, "RNFR %s", argv[0]) == CONTINUE)
		comando (1, "RNTO %s", argv[1]);

}	/* end do_rename */

/*
 ****************************************************************
 *	Processa o comando RESET				*
 ****************************************************************
 */
void
do_reset (int argc, const char *argv[], const CMDTB *tp)
{
	if (argc != 0)
	{
		printf ("Uso: %s %s\n", tp->c_name, tp->c_args);
		return;
	}

	if (t_nread (cntl_fd) > 0)
		get_reply ();
	else
		printf ("Nenhuma resposta pendente\n");

}	/* end do_reset */

/*
 ****************************************************************
 *	Processa o comando DELETE				*
 ****************************************************************
 */
void
do_delete (int argc, const char *argv[], const CMDTB *tp)
{
	if (argc != 1)
	{
		printf ("Uso: %s %s\n", tp->c_name, tp->c_args);
		return;
	}

	printf ("Remove \"%s\"? (n): ", argv[0]);

	if (askyesno () <= 0)
		return;

	comando (1, "DELE %s", argv[0]);

}	/* end do_delete */

/*
 ****************************************************************
 *	Processa o comando RMDIR				*
 ****************************************************************
 */
void
do_rmdir (int argc, const char *argv[], const CMDTB *tp)
{
	if (argc != 1)
	{
		printf ("Uso: %s %s\n", tp->c_name, tp->c_args);
		return;
	}

	printf ("Remove \"%s\"? (n): ", argv[0]);

	if (askyesno () <= 0)
		return;

	comando (1, "RMD %s", argv[0]);

}	/* end do_rmdir */

/*
 ****************************************************************
 *	Processa o comando MKDIR				*
 ****************************************************************
 */
void
do_mkdir (int argc, const char *argv[], const CMDTB *tp)
{
	if (argc != 1)
	{
		printf ("Uso: %s %s\n", tp->c_name, tp->c_args);
		return;
	}

	comando (1, "MKD %s", argv[0]);

}	/* end do_mkdir */

/*
 ****************************************************************
 *	Processa o comando PWD					*
 ****************************************************************
 */
void
do_pwd (int argc, const char *argv[], const CMDTB *tp)
{
	if (argc != 0)
	{
		printf ("Uso: %s %s\n", tp->c_name, tp->c_args);
		return;
	}

	comando (1, "PWD");

}	/* end do_pwd */

/*
 ****************************************************************
 *	Comando LIST						*
 ****************************************************************
 */
void
do_list (int argc, const char *argv[], const CMDTB *tp)
{
	int		old_type;
	const char	*list_nm;
	STAT		s;

	if (argc > 2)
	{
		printf ("Uso: %s %s\n", tp->c_name, tp->c_args);
		return;
	}

	if (argc > 1)
		list_nm = argv[1];
	else
		list_nm = "-";

	if (!streq (list_nm, "-")  &&  stat (list_nm, &s) >= 0)
	{
		printf ("Arquivo \"%s\" já existe. Remove/reescreve? (n) ", list_nm);

		if (askyesno () <= 0)
			return;

		if (!S_ISREG (s.st_mode) && unlink (list_nm) < 0)
		{
			error ("*Não consegui remover \"%s\"", list_nm);
			return;
		}
	}

	old_type = type; 	type = TYPE_A;

	if (argc == 0)
		recebe ("LIST", NOSTR, list_nm);
	else
		recebe ("LIST", argv[0], list_nm);

	type = old_type;

}	/* end do_list */

/*
 ****************************************************************
 *	Comando LC						*
 ****************************************************************
 */
void
do_lc (int argc, const char *argv[], const CMDTB *tp)
{
	int		old_type;

	if (argc > 1)
	{
		printf ("Uso: %s %s\n", tp->c_name, tp->c_args);
		return;
	}

	old_type = type; 	type = TYPE_A;

	if (argc == 0)
		recebe ("LC", NOSTR, "-");
	else
		recebe ("LC", argv[0], "-");

	type = old_type;

}	/* end do_lc */

/*
 ****************************************************************
 *	Comando NLST						*
 ****************************************************************
 */
void
do_nlst (int argc, const char *argv[], const CMDTB *tp)
{
	int		old_type;

	if (argc > 1)
	{
		printf ("Uso: %s %s\n", tp->c_name, tp->c_args);
		return;
	}

	old_type = type; 	type = TYPE_A;

	if (argc == 0)
		recebe ("NLST", NOSTR, "-");
	else
		recebe ("NLST", argv[0], "-");

	type = old_type;

}	/* end do_nlst */

/*
 ****************************************************************
 *	Comando SYSTEM						*
 ****************************************************************
 */
void
do_system (int argc, const char *argv[], const CMDTB *tp)
{
	if (argc > 0)
	{
		printf ("Uso: %s %s\n", tp->c_name, tp->c_args);
		return;
	}

	comando (1, "SYST");

}	/* end do_system */

/*
 ****************************************************************
 *	Comando STATUS						*
 ****************************************************************
 */
void
do_status (int argc, const char *argv[], const CMDTB *tp)
{
	UTSNAME		uts;

	if (argc > 1)
	{
		printf ("Uso: %s %s\n", tp->c_name, tp->c_args);
		return;
	}

	if (uname (&uts) >= 0)
	{
		printf
		(	"%s (%s, %s) Cliente FTP (%s)\n",
			uts.uts_sysname, uts.uts_machine, uts.uts_nodename, pgversion
		);
	}
	else
	{
		printf
		(	"Cliente FTP (%s)\n",
			pgversion
		);
	}

	printf
	(	"Conectado a \"%s\"\n",
		remote_node_nm
	);

	printf ("Para transferir arquivos:\n");

	printf
	(	"\tusando o tipo %s (NON PRINT)\n",
		type == TYPE_A ? "ISO (ASCII)" : "IMAGE"
	);

	printf
	(	"\tUsando a estrutura FILE (isto é, sem registro)\n"
	);

	printf
	(	"\tUsando o modo STREAM\n"
	);

	printf ("PORT de dados = %d\n", last_data_port);

	printf ("Modo passivo %sLIGADO\n\n", passive_mode ? "" : "DES");

	comando (1, "STAT");

}	/* end do_status */

/*
 ****************************************************************
 *	Comando PASV						*
 ****************************************************************
 */
void
do_pasv (int argc, const char *argv[], const CMDTB *tp)
{
	if (argc > 0)
	{
		printf ("Uso: %s %s\n", tp->c_name, tp->c_args);
		return;
	}

	passive_mode ^= 1;

	printf ("Modo passivo %sLIGADO\n", passive_mode ? "" : "DES");

#if (0)	/*******************************************************/
	comando (1, "PASV");
#endif	/*******************************************************/

}	/* end do_pasv */

/*
 ****************************************************************
 *	Comando PORT						*
 ****************************************************************
 */
void
do_port (int argc, const char *argv[], const CMDTB *tp)
{
	int		port_no;
	const char	*str;

	if (argc > 2)
	{
		printf ("Uso: %s %s\n", tp->c_name, tp->c_args);
		return;
	}

	if (argc > 0 && streq (argv[0], "-s"))
		{ sflag++; argc--; argv++; }

	if (argc > 0 && streq (argv[0], "-n"))
		{ sflag = 0; argc--; argv++; }

	if (argc > 0)
	{
		if ((port_no = strtol (argv[0], &str, 0)) < 0 || *str != '\0')
		{
			printf ("No. de \"port\" inválido: \"%s\"\n", argv[0]);
			return;
		}

		local_data_addr.a_port = port_no;
	}

	printf
	(	"%s o número do \"port\" (%d) em toda operação\n", 
		sflag ? "Envia" : "NÃO envia", local_data_addr.a_port
	);

}	/* end do_port */

/*
 ****************************************************************
 *	Comando NOOP						*
 ****************************************************************
 */
void
do_donothing (int argc, const char *argv[], const CMDTB *tp)
{
	if (argc > 0)
	{
		printf ("Uso: %s %s\n", tp->c_name, tp->c_args);
		return;
	}

	comando (1, "NOOP");

}	/* end do_donothing */

/*
 ****************************************************************
 *	Comando HELP						*
 ****************************************************************
 */
void
do_help (int argc, const char *argv[], const CMDTB *tp)
{
	int		i;

	/*
	 *	Informações locais
	 */
	if (argc == 0)
	{
		printf ("Comandos disponíveis:");

		for (i = 0, tp = cmdtab; tp->c_name != NOSTR; tp++)
		{
			if (tp->c_function == NOFUNC)
				continue;

			if (i % 5 == 0)
				printf ("\n    ");

			printf (" %-14.14s", tp->c_name);

			i++;
		}

		printf ("\n");
		return;
	}

	if (!streq (argv[0], "-s"))
	{
		for (i = 0, tp = cmdtab; tp->c_name != NOSTR; tp++)
		{
			if (!streq (tp->c_name, argv[0]))
				continue;

			if (tp->c_function == NOFUNC)
				printf ("O Comando \"%s\" ainda não está implementado\n", argv[0]);
			else
				printf ("%s:\t%s %s\n", tp->c_help, tp->c_name, tp->c_args);

			return;
		}

		printf ("Comando \"%s\" desconhecido\n", argv[0]);
		return;
	}

	/*
	 *	Informações remotas
	 */
	argc--; argv++;

	if (argc == 0)
		comando (1, "HELP");
	else
		comando (1, "HELP %s", argv[0]);

}	/* end do_help */

/*
 ****************************************************************
 *	A tabela de comandos					*
 ****************************************************************
 */
const CMDTB	cmdtab[] =
{
	"?",		do_help,
			"[-s] [<nome_do_comando>]",
			"Fornece informações acêrca dos comandos",

	"!",		do_help,	/* do_help não será chamado */
			"<nome_do_comando> [<argumento> ...]",
			"Executa um comando local (inclusive \"cd\")",

	"ascii",	do_ascii,
			"",
			"Altera o tipo da reresentação de dados para ISO (ASCII)",

	"bin",		do_image,
			"",
			"Altera o tipo da reresentação de dados para IMAGEM",

	"binary",	do_image,
			"",
			"Altera o tipo da reresentação de dados para IMAGEM",

	"bye",		do_quit,
			"",
			"Encerra o programa",

	"cd",		do_cd,
			"[<diretório_remoto>]",
			"Altera o diretório corrente remoto",

	"cdup",		do_cdup,
			"",
			"Altera o diretório corrente remoto para o pai do atual",

	"cmp",		do_cmp,
			"<arquivo_local> [<arquivo_remoto>]",
			"Compara um arquivo local com um remoto",

	"delete",	do_delete,
			"<arquivo_remoto>",
			"Remove um arquivo remoto",

	"dir",		do_list,
			"[<arquivo_remoto> [<arquivo_local>]]",
			"Fornece informações sobre arquivos remotos",

	"exit",		do_quit,
			"",
			"Encerra o programa",

	"get",		do_get,
			"<arquivo_remoto> [<arquivo_local>]",
			"Copia um arquivo do nó remoto para o nó local",

	"help",		do_help,
			"[-s] [<nome_do_comando>]",
			"Fornece informações acêrca dos comandos",

	"image",	do_image,
			"",
			"Altera o tipo da reresentação de dados para IMAGEM",

	"iso",		do_ascii,
			"",
			"Altera o tipo da reresentação de dados para ISO (ASCII)",

	"lc",		do_lc,
			"[<diretório_remoto>]",
			"Imprime o conteúdo de diretório remotos",

	"ls",		do_list,
			"[<arquivo_remoto> [<arquivo_local>]]",
			"Fornece informações sobre arquivos remotos",

	"mget",		do_mget,
			"[-f] [<diretório_remoto>]",
			"Copia diversos arquivos do nó remoto para o nó local",

	"mkdir",	do_mkdir,
			"<diretório_remoto>",
			"Cria um diretório remoto",

	"mput",		do_mput,
			"[-f] [<diretório_local>]",
			"Copia diversos arquivos do nó local para o nó remoto",

	"mv",		do_rename,
			"<nome_antigo> <nome_novo>",
			"Muda o nome de um arquivo remoto",

	"nlst",		do_nlst,
			"[<diretório_remoto>]",
			"Lista os arquivos de um diretório remoto",

	"noop",		do_donothing,
			"",
			"Não realiza nenhuma operação",

	"pasv",		do_pasv,
			"",
			"Troca entre o modo ativo/passivo",

	"passive",	do_pasv,
			"",
			"Troca entre o modo ativo/passivo",

	"port",		do_port,
			"[-sn] [<número_do_port>]",
			"Especifica informações sobre o \"port\" de dados",

	"put",		do_put,
			"<arquivo_local> [<arquivo_remoto>]",
			"Copia um arquivo do nó local para o nó remoto",

	"pwd",		do_pwd,
			"",
			"Imprime o nome do diretório corrente remoto",

	"quit",		do_quit,
			"",
			"Encerra o programa",

	"quote",	do_quote,
			"<nome> [<arg> [<arg>]]",
			"Envia um comando para o servidor remoto",

	"rename",	do_rename,
			"<nome_antigo> <nome_novo>",
			"Muda o nome de um arquivo remoto",

	"reset",	do_reset,
			"",
			"Lê uma resposta do servidor",

	"rm",		do_delete,
			"<arquivo_remoto>",
			"Remove um arquivo remoto",

	"rmdir",	do_rmdir,
			"<diretório_remoto>",
			"Remove um diretório remoto",

	"status",	do_status,
			"[<arquivo>]",
			"Fornece informações sobre o servidor ou um arquivo",

	"system",	do_system,
			"",
			"Obtém o tipo de sistema operacional do servidor",

	"type",		do_settype,
			"[ascii|a|image|i|binary|bin|b]",
			"Altera o tipo da reresentação de dados",

	"user",		do_login,
			"[<nome_do_usuário>]",
			"Inicia a seqüência de \"login\"",

	NOSTR
};
