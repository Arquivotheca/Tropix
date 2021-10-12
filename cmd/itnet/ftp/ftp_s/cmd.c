/*
 ****************************************************************
 *								*
 *			cmd.c					*
 *								*
 *	File transfer protocol (servidor)			*
 *								*
 *	Versão	3.0.0, de 12.10.92				*
 *		3.0.5, de 11.11.97				*
 *								*
 *	Módulo: FTP_S						*
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
#include <pwd.h>
#include <errno.h>
#include <xti.h>

#include "ftp_s.h"
#include "../ftp.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
extern const PASSWD	user_pw;	/* Do usuário corrente */

entry const char	*old_file_nm;	/* Para RENAME */

extern char		client_node_nm[]; /* Nome do cliente */

/*
 ****************************************************************
 *	Comando CWD						*
 ****************************************************************
 */
void
do_cwd (int argc, const char *argv[])
{ 
	if (check_ready_state () < 0)
		return;

	if (argc > 1)
	{
		reply (501, "Argumentos demais");
		return;
	}

	if (argc == 0)
		argv[0] = user_pw.pw_dir;

	if (chdir (argv[0]) < 0)
		reply (550, "Erro no comando %s para \"%s\" (%s)", "CWD", argv[0], strerror (errno));
	else
		reply (250, "Comando %s completado com sucesso", "CWD");

}	/* end do_cwd */

/*
 ****************************************************************
 *	Comando CDUP						*
 ****************************************************************
 */
void
do_cdup (int argc, const char *argv[])
{ 
	if (check_ready_state () < 0)
		return;

	if (argc > 0)
	{
		reply (501, "Argumentos demais");
		return;
	}

	if (chdir ("..") < 0)
		reply (550, "Erro no comando %s (%s)", "CDUP", strerror (errno));
	else
		reply (250, "Comando %s completado com sucesso", "CDUP");

}	/* end do_cdup */

/*
 ****************************************************************
 *	Comando QUIT						*
 ****************************************************************
 */
void
do_quit (int argc, const char *argv[])
{
	if (argc > 0)
	{
		reply (501, "Argumentos demais");
		return;
	}

	reply (221, "Tchau");
	
	error
	(	"Logout para \"%s@%s\"",
		user_pw.pw_name, client_node_nm
	);

	if (t_sndrel (cntl_fd) < 0)
		error ("$*Erro no SNDREL"); 

	if (t_rcvrel (cntl_fd) < 0)
		error ("$*Erro no RCVREL"); 

	t_close (cntl_fd);

	exit(0);

}	/* end do_quit */

/*
 ****************************************************************
 *	Comando TYPE						*
 ****************************************************************
 */
void
do_type (int argc, const char *argv[])
{
	int		cmd_type, cmd_form = FORM_N;

	if (argc < 1 || argc > 2)
	{
		reply (501, "Nr. incorreto de argumentos");
		return;
	}

	/*
	 *	Primeiro argumento
	 */
	switch (toupper (argv[0][0]))
	{
	   case 'A':
		cmd_type = TYPE_A; 
		break;

	   case 'E':
		cmd_type = TYPE_E; 
		break;

	   case 'I':
		cmd_type = TYPE_I; 
		break;

	   case 'L':
		cmd_type = TYPE_L; 
		break;

	   default:
		reply
		(	501,
			"Tipo de representação desconhecido: \"%s\"",
			argv[0]
		);
		return;
	}

	/*
	 * 	Segundo argumento
	 */
	if (argc > 1) switch (toupper (argv[1][0]))
	{
	    case 'N':
		cmd_form = FORM_N;
		break;

	    case 'T':
		cmd_form = FORM_T;
		break;

	    case 'C':
		cmd_form = FORM_C;
		break;

	    default:
		reply
		(	501,
			"Tipo de formato desconhecido: \"%s\"",
			argv[1]
		);
		return;
	}

	/*
	 *	Verifica os argumentos dados
	 */
	if (cmd_form != FORM_N)
	{
		reply (504, "Só suportamos o formato NON-PRINT");
		return;
	}

	switch (cmd_type)
	{
	   case TYPE_A:
		reply (200, "Usando o tipo ISO (ASCII) para transferir arquivos");
		break;

	   case TYPE_E:
		reply (504, "Tipo EBCDIC não implementado");
		return;

	   case TYPE_I:
		reply (200, "Usando o tipo IMAGEM para transferir arquivos");
		break;

	   case TYPE_L:
		reply (504, "Tipo LOCAL não implementado");
		return;

	}	/* end switch */

	/*
	 *	Só chega aqui sem erros
	 */
	type = cmd_type;
	form = cmd_form;

}	/* end do_type */

/*
 ****************************************************************
 *	Comando STRU 						*
 ****************************************************************
 */
void
do_stru (int argc, const char *argv[])
{
	if (argc != 1)
	{
		reply (501, "Nr. incorreto de argumentos");
		return;
	}

	/*
	 *	Primeiro argumento
	 */
	switch (toupper (argv[0][0]))
	{
	    case 'F':
		stru = STRU_F;
		reply (200, "Usando estrutura de arquivo FILE");
		break;

	    case 'R':
	    case 'P':
		reply (504, "Só suportamos estrutura FILE");
		break;

	    default:
		reply
		(	501,
			"Tipo de estrutura de arquivo desconhecido: \"%s\"",
			argv[0]
		);
		break;

	}	/* end switch */

}	/* end do_stru */

/*
 ****************************************************************
 *	Comando MODE 						*
 ****************************************************************
 */
void
do_mode (int argc, const char *argv[])
{
	if (argc != 1)
	{
		reply (501, "Nr. incorreto de argumentos");
		return;
	}

	/*
	 *	Primeiro argumento
	 */
	switch (toupper (argv[0][0]))
	{
	    case 'S':
		mode = MODE_S;
		reply (200, "Usando modo de transmissão STREAM");
		break;

	    case 'B':
	    case 'C':
		reply (504, "Só suportamos modo de transmissão STREAM");
		break;

	    default:
		reply
		(	501,
			"Modo de transmissão desconhecido: \"%s\"",
			argv[0]
		);
		break;

	}	/* end switch */

}	/* end do_mode */

/*
 ****************************************************************
 *	Comando RNFR						*
 ****************************************************************
 */
void
do_rnfr (int argc, const char *argv[])
{ 
	if (check_ready_state () < 0)
		return;

	if   (argc == 0)
	{
		reply (501, "Faltando o nome do arquivo");
		return;
	}
	elif (argc > 1)
	{
		reply (501, "Argumentos demais");
		return;
	}

	if ((old_file_nm = strdup (argv[0])) == NOSTR)
	{
		reply (552, "Não obtive memória para armazenar o nome antigo");
		return;
	}

	reply (350, "Comando %s completado com sucesso", "RNFR");

	server_state = S_RNFR;

}	/* end do_rnfr */

/*
 ****************************************************************
 *	Comando RNTO						*
 ****************************************************************
 */
void
do_rnto (int argc, const char *argv[])
{ 
	if (server_state != S_RNFR)
	{
		reply (503, "Este comando só pode ser executado após um RNFR");
		goto bad;
	}

	if   (argc == 0)
	{
		reply (501, "Faltando o nome do arquivo");
		goto free;
	}
	elif (argc > 1)
	{
		reply (501, "Argumentos demais");
		goto free;
	}

	if (rename (old_file_nm, argv[0]) < 0)
	{
		reply
		(	550,
			"Não consegui trocar o nome do arquivo (%s)",
			strerror (errno)
		);
		goto free;
	}

	reply (250, "Comando %s completado com sucesso", "RNFR");

    free:
	free ((char *)old_file_nm);
    bad:
	server_state = S_READY;

}	/* end do_rnto */

/*
 ****************************************************************
 *	Comando DELE						*
 ****************************************************************
 */
void
do_dele (int argc, const char *argv[])
{ 
	STAT		s;
	
	if (check_ready_state () < 0)
		return;

	if   (argc == 0)
	{
		reply (501, "Faltando o nome do arquivo");
		return;
	}
	elif (argc > 1)
	{
		reply (501, "Argumentos demais");
		return;
	}

	if (stat (argv[0], &s) < 0)
	{
		reply
		(	550,
			"Não consegui obter o estado do arquivo (%s)",
			strerror (errno)
		);
		return;
	}

	if (S_ISDIR (s.st_mode))
	{
		reply (550, "O arquivo \"%s\" é um diretório", argv[0]);
		return;
	}

	if (unlink (argv[0]) < 0)
	{
		reply
		(	550,
			"Não consegui remover o arquivo (%s)",
			strerror (errno)
		);
		return;
	}

	reply (250, "Comando %s completado com sucesso", "DELE");

}	/* end do_dele */

/*
 ****************************************************************
 *	Comando RMD						*
 ****************************************************************
 */
void
do_rmd (int argc, const char *argv[])
{ 
#define	RMDIR
#ifndef	RMDIR
	STAT		s;
	char		buf[BLSZ];
#endif	RMDIR

	if (check_ready_state () < 0)
		return;

	if   (argc == 0)
	{
		reply (501, "Faltando o nome do diretório");
		return;
	}
	elif (argc > 1)
	{
		reply (501, "Argumentos demais");
		return;
	}

#ifdef	RMDIR
	if (rmdir (argv[0]) < 0)
	{
		reply
		(	550,
			"Não consegui remover o diretório \"%s\" (%s)",
			argv[0], strerror (errno)
		);
		return;
	}
#else
	if (stat (argv[0], &s) < 0)
	{
		reply
		(	550,
			"Não consegui obter o estado do arquivo (%s)",
			strerror (errno)
		);
		return;
	}

	if (!S_ISDIR (s.st_mode))
	{
		reply (550, "O arquivo \"%s\" NÃO é um diretório", argv[0]);
		return;
	}

	sprintf (buf, "/bin/rmdir %s 2>/dev/null", argv[0]);

	if (system (buf) != 0)
	{
		reply
		(	550,
			"Não consegui remover o diretório \"%s\"",
			argv[0]
		);
		return;
	}
#endif	RMDIR

	reply (250, "Comando %s completado com sucesso", "RMD");

}	/* end do_rmd */

/*
 ****************************************************************
 *	Comando MKD						*
 ****************************************************************
 */
void
do_mkd (int argc, const char *argv[])
{ 
#define	MKDIR
#ifndef	MKDIR
	const PASSWD		*pw = &user_pw;
	char			buf[BLSZ];
#endif	MKDIR

	if (check_ready_state () < 0)
		return;

	if   (argc == 0)
	{
		reply (501, "Faltando o nome do diretório");
		return;
	}
	elif (argc > 1)
	{
		reply (501, "Argumentos demais");
		return;
	}

#ifdef	MKDIR
	if (mkdir (argv[0], 0777) < 0)
	{
		reply
		(	550,
			"Não consegui criar o diretório \"%s\" (%s)",
			argv[0], strerror (errno)
		);
		return;
	}
#else
	sprintf (buf, "/bin/mkdir %s 2>/dev/null", argv[0]);

	if (system (buf) != 0)
	{
		reply
		(	550,
			"Não consegui criar o diretório \"%s\"",
			argv[0]
		);
		return;
	}

	seteuid (0);

	if (chown (argv[0], pw->pw_uid, pw->pw_gid) < 0)
		error ("*Erro no CHOWN");

	seteuid (pw->pw_uid);
#endif	MKDIR

	reply (257, "Comando %s completado com sucesso", "MKD");

}	/* end do_mkd */

/*
 ****************************************************************
 *	Comando PWD						*
 ****************************************************************
 */
void
do_pwd (int argc, const char *argv[])
{ 
	const char	*cwd;
	char		buf[BLSZ];

	if (check_ready_state () < 0)
		return;

	if (argc > 0)
	{
		reply (501, "Argumentos demais");
		return;
	}

	if ((cwd = getcwd (buf, sizeof (buf))) == NOSTR)
	{
		reply
		(	550,
			"Não consegui obter o diretório corrente (%s)",
			strerror (errno)
		);
		return;
	}

	reply (257, "\"%s\"", cwd);
#if (0)	/*******************************************************/
	reply (257, "O diretório corrente é \"%s\"", cwd);
#endif	/*******************************************************/

}	/* end do_pwd */

/*
 ****************************************************************
 *	Comando LIST						*
 ****************************************************************
 */
void
do_list (int argc, const char *argv[])
{
	if (check_ready_state () < 0)
		return;

	if (argc > 1)
	{
		reply (501, "Argumentos demais");
		return;
	}

	do_function ("list", proc_ls, argv[0]);

}	/* end do_list */

/*
 ****************************************************************
 *	Executa uma função e envia a saída			*
 ****************************************************************
 */
void
do_function (const char *cmd_nm, int (*function) (const char *, FILE *), const char *arg)
{
	FILE		*data_fp;

	if (open_data_connection () < 0)
		return;

	if ((data_fp = fdopen (data_fd, "w")) == NOFILE)
	{
		reply
		(	425,
			"Não consegui associar um FILE à conexão de dados (%s)",
			strerror (errno)
		);

		t_snddis (data_fd, (T_CALL *)NULL);
		t_close (data_fd);

		return;
	}

	reply (150, "Abrindo conexão de dados ISO (ASCII) para \"%s\"", cmd_nm);

	(*function) (arg, data_fp);

	fflush (data_fp); t_push (data_fd);

	if (ferror (data_fp))
		error ("*Erro na escrita da conexão de dados");

	if   (t_sndrel (data_fd) < 0)
	{
		error ("*Não consegui enviar a desconexão ordenada DATA");
		t_snddis (data_fd, (T_CALL*)NULL);
	}
	elif (t_rcvrel (data_fd) < 0)
	{
		error ("*Não consegui receber a desconexão ordenada DATA");
		t_snddis (data_fd, (T_CALL*)NULL);
	}

	fclose (data_fp);

	reply (226, "Transferência completa");

}	/* end do_function */

/*
 ****************************************************************
 *	Comando LC						*
 ****************************************************************
 */
void
do_lc (int argc, const char *argv[])
{
	char		buf[BLSZ];

	if (check_ready_state () < 0)
		return;

	if (argc > 1)
	{
		reply (501, "Argumentos demais");
		return;
	}

	if (argc == 0)
		sprintf (buf, "/bin/lc 2>&1");
	else
		sprintf (buf, "/bin/lc %s 2>&1", argv[0]);

	do_cmd ("lc", buf);

}	/* end do_lc */

/*
 ****************************************************************
 *	Executa um comando e envia a saída			*
 ****************************************************************
 */
void
do_cmd (const char *cmd_nm, const char *cmd_line)
{
	FILE		*file_fp, *data_fp;
	int		c;

	if ((file_fp = popen (cmd_line, "r")) == NOFILE)
	{
		reply
		(	550,
			"Não consegui executar o programa \"%s\" (%s)",
			cmd_nm, strerror (errno)
		);
		return;
	}

	if (open_data_connection () < 0)
	{
		pclose (file_fp);
		return;
	}

	if ((data_fp = fdopen (data_fd, "w")) == NOFILE)
	{
		reply
		(	425,
			"Não consegui associar um FILE à conexão de dados (%s)",
			strerror (errno)
		);

		pclose (file_fp);

		t_snddis (data_fd, (T_CALL *)NULL);
		t_close (data_fd);

		return;
	}

	reply (150, "Abrindo conexão de dados ISO (ASCII) para \"%s\"", cmd_nm);

	while ((c = getc (file_fp)) >= 0)	/* not EOF */
	{
		if (c == '\n')
			putc ('\r', data_fp);

		putc (c, data_fp);
	}

	fflush (data_fp); 	t_push (data_fd);

	if (ferror (file_fp))
		error ("*Erro na leitura do programa \"%s\"", cmd_nm);

	pclose (file_fp);

	if (ferror (data_fp))
		error ("*Erro na escrita da conexão de dados");

	if   (t_sndrel (data_fd) < 0)
	{
		error ("*Não consegui enviar a desconexão ordenada DATA");
		t_snddis (data_fd, (T_CALL*)NULL);
	}
	elif (t_rcvrel (data_fd) < 0)
	{
		error ("*Não consegui receber a desconexão ordenada DATA");
		t_snddis (data_fd, (T_CALL*)NULL);
	}

	fclose (data_fp);

	reply (226, "Transferência completa");

}	/* end do_cmd */

/*
 ****************************************************************
 *	Comando NLST						*
 ****************************************************************
 */
void
do_nlst (int argc, const char *argv[])
{
	FILE		*data_fp;
	STAT		s;
	DIR		*dir_fp;
	const DIRENT	*dp;
	char		*area, *cp_point;
	const char	*dir_nm;
	int		dev;

	if (check_ready_state () < 0)
		return;

	if (argc > 1)
	{
		reply (501, "Argumentos demais");
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

	if ((dir_fp = opendir (dir_nm)) == NODIR)
	{
		reply
		(	550,
			"Não consegui abrir o diretório \"%s\" (%s)",
			dir_nm, strerror (errno)
		);
		return;
	}

	if (fstat (dir_fp->d_fd, &s) < 0)
	{
		reply
		(	550,
			"Não consegui obter o estado do diretório (%s)",
			strerror (errno)
		);
		closedir (dir_fp);
		return;
	}

	dev = s.st_dev;

	if (open_data_connection () < 0)
	{
		closedir (dir_fp);
		return;
	}

	if ((data_fp = fdopen (data_fd, "w")) == NOFILE)
	{
		reply
		(	425,
			"Não consegui associar um FILE à conexão de dados (%s)",
			strerror (errno)
		);

		closedir (dir_fp);

		t_snddis (data_fd, (T_CALL *)NULL);
		t_close (data_fd);

		return;
	}

	reply (150, "Abrindo conexão de dados ISO (ASCII) para \"nlst\"");

	while ((dp = readdir (dir_fp)) != NODIRENT)
	{
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

		fprintf (data_fp, "%s%s", area, "\r\n");
	}

	fflush (data_fp); 	t_push (data_fd);

	closedir (dir_fp);

	if (ferror (data_fp))
		error ("*Erro na escrita da conexão de dados");

	if   (t_sndrel (data_fd) < 0)
	{
		error ("*Não consegui enviar a desconexão ordenada DATA");
		t_snddis (data_fd, (T_CALL*)NULL);
	}
	elif (t_rcvrel (data_fd) < 0)
	{
		error ("*Não consegui receber a desconexão ordenada DATA");
		t_snddis (data_fd, (T_CALL*)NULL);
	}

	fclose (data_fp);

	reply (226, "Transferência completa");

}	/* end do_nlst */

/*
 ****************************************************************
 *	Comando SYST						*
 ****************************************************************
 */
void
do_syst (int argc, const char *argv[])
{ 
	UTSNAME		uts;

	if (argc > 0)
	{
		reply (501, "Argumentos demais");
		return;
	}

	if (uname (&uts) < 0)
	{
		reply (215, "UNIX");
	}
	else
	{
		reply
		(	215,
			"UNIX: %s (%s de %s) em um %s (%s)",
			uts.uts_sysname, uts.uts_version,
			uts.uts_date, uts.uts_machine, uts.uts_nodename
		);
	}

}	/* end do_syst */

/*
 ****************************************************************
 *	Comando STAT						*
 ****************************************************************
 */
void
do_stat (int argc, const char *argv[])
{ 
	const PASSWD		*pw = &user_pw;
	UTSNAME			uts;

	if (argc > 1)
		{ reply (501, "Argumentos demais"); return; }

	if (argc == 1)
		{ reply (504, "Estado de arquivo ainda não implementado"); return; }

	if (uname (&uts) >= 0)
	{
		long_reply
		(	211, "%s (%s, %s) Servidor FTP (%s)",
			uts.uts_sysname, uts.uts_machine, uts.uts_nodename, pgversion
		);
	}
	else
	{
		long_reply
		(	211, "Servidor FTP (%s)",
			pgversion
		);
	}

	fprintf
	(	cntl_fp,
		"Conectado a \"%s\"\r\n",
		client_node_nm
	);

	switch (server_state)
	{
	    case S_RESET:
		fprintf (cntl_fp, "Sem usuário em sessão\r\n");
		break;

	    case S_USER:
		fprintf (cntl_fp, "Esperando uma senha\r\n");
		break;

	    case S_PASS:
		fprintf (cntl_fp, "Esperando uma conta\r\n");
		break;

	    case S_READY:
		fprintf
		(	cntl_fp,
			"Em sessão com o usuário \"%s\"%s\r\n",
			pw->pw_name,
			pw->pw_uid == 0 ? " (SUPERUSUÁRIO)" : ""
		);
		break;

	    case S_RNFR:
		fprintf (cntl_fp, "Esperando o nome novo para \"rename\"\r\n");
		break;

	}	/* end switch */

	fprintf (cntl_fp, "Para transferir arquivos:\r\n");

	fprintf
	(	cntl_fp,
		"\tusando o tipo %s (NON PRINT)\r\n",
		type == TYPE_A ? "ISO (ASCII)" : "IMAGE"
	);

	fprintf
	(	cntl_fp,
		"\tUsando a estrutura FILE (isto é, sem registro)\r\n"
	);

	fprintf
	(	cntl_fp,
		"\tUsando o modo STREAM\r\n"
	);

	fprintf (cntl_fp, "PORT de dados = %d\r\n", client_data_addr.a_port);

	fflush (cntl_fp);

	reply (211, "Final do estado");

}	/* end do_stat */

/*
 ****************************************************************
 *	Comando HELP						*
 ****************************************************************
 */
void
do_help (int argc, const char *argv[])
{
	const CMDTB	*tp;
	const char	*cp;
	int		i;

	if (argc == 0)
	{
		long_reply (214, "Este servidor FTP processa os seguintes comandos:");

		for (i = 0, tp = cmdtab; tp->c_nome != NOSTR; tp++)
		{
			if (tp->c_funcao == NOFUNC)
				continue;

			if (i % 5 == 0)
				fprintf (cntl_fp, "\r\n    ");
		
			fprintf (cntl_fp, " %-14.14s", tp->c_nome);

			i++;
		}
	
		if (i > 0)
			fprintf (cntl_fp, "\r\n\r\n");

		fflush (cntl_fp);

		reply (214, "Final da lista de comandos");
		return;
	}

	if (argc > 1)
	{
		reply (501, "Só é permitido o nome de um comando");
		return;
	}

	cp = strtoupper ((char *)argv[0]);

	for (tp = cmdtab; /* teste abaixo */; tp++)
	{
		if (tp->c_nome == NOSTR)
		{
			reply (214, "Comando \"%s\" desconhecido", cp);
			return;
		}

		if (streq (cp, tp->c_nome))
			break;
	}

	if (tp->c_funcao == NOFUNC)
	{
		reply (214, "Comando \"%s\" não implementado", cp);
		return;
	}

	reply (214, "%s:\t%s %s", tp->c_help, cp, tp->c_args);

}	/* end do_help */

/*
 ****************************************************************
 *	Comando NOOP						*
 ****************************************************************
 */
void
do_noop (int argc, const char *argv[])
{ 
	if (argc > 0)
	{
		reply (501, "Argumentos demais");
		return;
	}

	reply
	(	200,
		"Comando NOOP completado com sucesso "
		"(após intenso cálculo)"
	);

}	/* end do_noop */

/*
 ****************************************************************
 *	Tabela de comandos					*
 ****************************************************************
 */
const CMDTB	cmdtab[] =
{
    "USER",	do_user,
		"<nome do usuário>",
		"Especifica o nome do usuário",

    "PASS", 	do_pass,
		"<senha>",
		"Fornece a senha",

    "ACCT",	do_acct,
		"<senha repetida>",
		"Fornece a repetição da senha",

    "CWD",	do_cwd,
		"[<diretório>]",
		"Altera o diretório corrente",

    "CDUP",	do_cdup,
		"",
		"Altera o diretório corrente para o pai do atual",

    "SMNT",	NOFUNC,
		"<nome da árvore>",
		"Montagem de estrutura",

    "QUIT",	do_quit,
		"",
		"Encerra o serviço do servidor",

    "REIN",	NOFUNC,
		"",
		"Reinicializa o servidor",

    "PORT",	do_port,
		"a0,a1,a2,a3,p0,p1",
		"Especifica um endereço para a conexão de dados",

    "PASV",	do_pasv,
		"",
		"O servidor entra no modo passivo",

    "TYPE",	do_type,
		"[ A | E | I | L ]",
		"Especifica o tipo da representacão de dados",

    "STRU",	do_stru,
		"",
		"Especifica a estrutura do arquivo",

    "MODE",	do_mode,
		"",
		"Especifica o modo de transferência de dados",

    "RETR",	do_retr,
		"<nome do arquivo>",
		"Obtém o conteúdo de um arquivo",

    "STOR",	do_stor,
		"<nome do arquivo>",
		"Armazena o conteúdo de um arquivo",

    "STOU",	NOFUNC,
		"",
		"Armazena o conteúdo de um arquivo com um nome unívoco",

    "APPE",	NOFUNC,
		"<nome do arquivo>",
		"Armazena o conteúdo ao final de um arquivo",

    "ALLO",	NOFUNC,
		"",
		"Reserva espaço para um arquivo",

    "REST",	NOFUNC,
		"",
		"Recomeça uma transferência",

    "RNFR",	do_rnfr,
		"<nome do arquivo>",
		"Especifica o nome antigo de um arquivo cujo nome será alterado",

    "RNTO",	do_rnto,
		"<nome do arquivo>",
		"Especifica o nome novo do arquivo",

    "ABOR",	NOFUNC,
		"",
		"Aborta a operação corrente",

    "DELE",	do_dele,
		"<nome do arquivo>",
		"Remove um arquivo",

    "RMD",	do_rmd,
		"<nome do diretório>",
		"Remove um diretório",

    "MKD",	do_mkd,
		"<nome do diretório>",
		"Cria um diretório",

    "XMKD",	do_mkd,
		"<nome do diretório>",
		"Cria um diretório",

    "PWD",	do_pwd,
		"",
		"Retorna o nome do diretório corrente",

    "XPWD",	do_pwd,
		"",
		"Retorna o nome do diretório corrente",

    "LIST",	do_list,
		"[<nome de um arquivo>]",
		"Fornece informações sobre arquivos",

    "LC",	do_lc,
		"[<nome de um diretório>]",
		"Imprime o conteúdo de um diretório",

    "NLST",	do_nlst,
		"[<nome de um diretório>]",
		"Fornece uma lista de nomes de arquivos",

    "SITE",	NOFUNC,
		"[<argumentos>]",
		"Realiza comando particulares deste servidor",

    "SYST",	do_syst,
		"",
		"Fornece informações sobre o sistema operacional",

    "STAT",	do_stat,
		"[<nome de um arquivo>]",
		"Fornece informações sobre o servidor ou um arquivo",

    "HELP",	do_help,
		"[<cadeia>]",
		"Fornece informações sobre um comando",

    "NOOP",	do_noop,
		"",
		"Não realiza nenhuma operação (com cuidado)",

    NOSTR
};
