/*
 ****************************************************************
 *								*
 *			login.c					*
 *								*
 *	File transfer protocol (cliente)			*
 *								*
 *	Versão	3.0.0, de 12.10.92				*
 *		4.0.0, de 28.03.01				*
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
#include <sys/syscall.h>
#include <sys/times.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stat.h>
#include <pwd.h>
#include <fcntl.h>
#include <errno.h>
#include <xti.h>

#include "../ftp.h"
#include "ftp_c.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define FILE_MASK	0666

#undef	FTP_DATA_PORT
#define FTP_DATA_PORT	4096

entry INADDR	local_data_addr = { 0, 0, FTP_DATA_PORT };

entry INADDR	server_data_addr;	/* Para o modo passivo */

entry int	passive_mode;		/* Indica o modo passivo */

entry int	last_data_port = FTP_DATA_PORT;

void		print_time_interval (long, MUTM *);

/*
 ******	Protótipos de funções ***********************************
 */
void		scan_pasv (const char *msg);
int		open_active_data_connection (const char *, const char *);
int		open_passive_data_connection (const char *, const char *);

/*
 ****************************************************************
 *	 Envia comandos	para o servidor				*
 ****************************************************************
 */
int
comando (int do_wait, const char *format, ...)
{
	va_list 	args;
	int		r = COMPLETE;
	char		com[BLSZ];

	va_start (args, format);

	vsprintf (com, format, args);

	if (vflag > 2)
		printf ("Enviando o comando \"%s\"\n", com);

	strcat (com, "\r\n");

	if (t_snd (cntl_fd, com, strlen (com), T_PUSH) < 0)
		error (NOSTR);

	if (do_wait)
		r = get_reply ();
	
	va_end (args);

	return (r);

}	/* end comando */

/*
 ****************************************************************
 *	Recebe uma mensagem do servidor				*
 ****************************************************************
 */
int
get_reply (void)
{
	char		first_line[128], next_line[128];
	int		code;

	/*
	 *	Lê a primeira linha
	 */
	if (fngets (first_line, sizeof (first_line), cntl_fp) == NOSTR)
		error ("$*Erro na leitura de uma mensagem do servidor");

	if (first_line[0] >= '0' && first_line[0] <= '5')
		code =  first_line[0] - '0';
	else
		code =  5;

	puts (first_line);

	/*
	 *	Verifica o caso particular da resposta ao PASV
	 */
	if (code == 2 && first_line[1] == '2' && first_line[2] == '7')
		scan_pasv (first_line + 3);

	/*
	 *	Lê as demais linhas
	 */
	if (first_line[3] == '-')
	{
		first_line[3] = '\0';

		for (EVER)
		{
			if (fngets (next_line, sizeof (next_line), cntl_fp) == NOSTR)
				error ("$*Erro na leitura de uma mensagem do servidor");

			puts (next_line);

			if (strlen (next_line) < 4)
				continue;

			if (next_line[3] != ' ' && next_line[3] != '\t')
				continue;

			next_line[3] = '\0';

			if (streq (first_line, next_line))
				break;
		}
	}

	return (code);

}	/* end get_reply */

/*
 ****************************************************************
 *	Procura o número IP e porta na resposta do PASV		*
 ****************************************************************
 */
void
scan_pasv (const char *msg)
{
	const char	*str;
	char		*cp, c = ',';
	int		i, n;

	/*
	 *	Procura o primeiro dígito
	 */
	for (str = msg; /* abaixo */; str++)
	{
		if (*str == '\0')
		{
		    bad:
			printf ("Sintaxe incorreta da resposta ao comando PASV\n");
			return;
		}

		if (*str >= '0' && *str <= '9')
			break;
	}

	/*
	 *	Retira o endereço
	 */
	cp = (char *)&server_data_addr.a_addr;

#ifdef	LITTLE_ENDIAN
	cp += 4;
#endif	LITTLE_ENDIAN

	for (i = 0; i < 4; i++)
	{
		if ((n = strtol (str, &str, 0)) & ~0xFF || *str != c)
			goto bad;

#ifdef	LITTLE_ENDIAN
		*--cp = n; 	str++;
#else
		*cp++ = n; 	str++;
#endif	LITTLE_ENDIAN
	}

	/*
	 *	Retira o "port"
	 */
	cp = (char *)&server_data_addr.a_port;

#ifdef	LITTLE_ENDIAN
	cp += 2;
#endif	LITTLE_ENDIAN

	for (/* i = 4 */; i < 6; i++)
	{
		if (i == 5)
			c = ')';

		if ((n = strtol (str, &str, 0)) & ~0xFF || *str != c)
			goto bad;

#ifdef	LITTLE_ENDIAN
		*--cp = n; 	str++;
#else
		*cp++ = n; 	str++;
#endif	LITTLE_ENDIAN
	}

#if (0)	/*******************************************************/
	passive_mode++;

	printf
	(	"Modo passivo confirmado. Endereço %s, porta %d\n",
		t_addr_to_str (server_data_addr.a_addr),
		server_data_addr.a_port
	);
#endif	/*******************************************************/

}	/* end scan_pasv */

/*
 ****************************************************************
 *	Processa o "login"					*
 ****************************************************************
 */
void
do_login (int argc, const char *argv[], const CMDTB *tp)
{
	int		r;
	char		line[80];
	const char	*user_nm;

	/*
	 *	Verifica os argumentos
	 */
	if (argc > 1)
	{
		printf ("Uso: %s %s\n", tp->c_name, tp->c_args); 
		return;
	}

	/*
	 *	Obtém o nome do usuário local
	 */
	if ((user_nm = remote_user_nm) == NOSTR)
	{
		if ((user_nm = getlogin ()) == NOSTR)
			user_nm = (getpwuid (getuid ()))->pw_name;

		if (user_nm == NOSTR)
			error ("$Não consegui obter o nome do usuário local");
	}

	if (argc == 0)
	{
		printf ("Nome do usuário (%s: %s): ", remote_node_nm, user_nm);

		gets (line);

		if (line[0] != '\0')
			user_nm = line;
	}
	else
	{
		user_nm = argv[0];
	}

	/*
	 *	Executa o comando "USER"
	 */
	r = comando (1, "USER %s", user_nm);

	/*
	 *	Verifica se é necessária a senha
	 */
	if (r == CONTINUE)
		r = comando (1, "PASS %s", getpass ("Senha: "));

	/*
	 *	Verifica se é necessária a conta
	 */
	if (r == CONTINUE)
		r = comando (1, "ACCT %s", getpass ("Conta: "));

}	/* end do_login */

/*
 ****************************************************************
 *	Recebe o conteúdo de um arquivo remoto			*
 ****************************************************************
 */
void
recebe (const char *cmd, const char *remote_file_nm, const char *local_file_nm)
{
	FILE		*file_fp, *data_fp;
	int		file_fd;
	int		c, c_next, n;
	long		byte_cnt = 0;
	char		buf[BLSZ];
	MUTM		begin_time;

	/*
	 ******	Processa o tipo "ASCII" *************************
	 */
	if (type == TYPE_A)
	{
		if (open_data_connection (cmd, remote_file_nm) < 0)
			return;

		if (streq (local_file_nm, "-"))
		{
			file_fp = stdout;
		}
		else
		{
			if ((file_fp = fopen (local_file_nm, "w")) == NOFILE)
			{
				error ("*Não consegui abrir \"%s\"", local_file_nm);
				close_data_connection ();
				return;
			}
		}

		if ((data_fp = fdopen (data_fd, "r")) == NOFILE)
		{
			error ("*Não consegui associar um FILE à conexão de dados");
			close_data_connection ();
			return;
		}

		if (vflag)
			mutime (&begin_time);

		while ((c = getc (data_fp)) >= 0)	/* not EOF */
		{
			if (c == '\r')
			{
				if ((c_next = getc (data_fp)) == '\n')
					c = '\n';
				else
					ungetc (c_next, data_fp);
			}

			putc (c, file_fp); 	byte_cnt++;
		}

		fflush (file_fp);

		if (vflag)
			print_time_interval (byte_cnt, &begin_time);

		if (ferror (file_fp))
			error ("*Erro na escrita do arquivo \"%s\"", local_file_nm);

		if (ferror (data_fp))
			error ("*Erro na leitura da conexão de dados");

		if (!streq (local_file_nm, "-"))
			fclose (file_fp);

		close_data_connection ();

		fclose (data_fp);

		get_reply ();

		return;

	}	/* end ASCII */

	/*
	 ******	Processa o tipo "IMAGEM" ************************
	 */
	if (type == TYPE_I)
	{
		if (open_data_connection (cmd, remote_file_nm) < 0)
			return;

		if (streq (local_file_nm, "-"))
		{
			file_fd = 1;	/* STDOUT */
		}
		else
		{
			if ((file_fd = open (local_file_nm, O_WRONLY|O_TRUNC|O_CREAT, FILE_MASK)) < 0)
			{
				error ("*Não consegui abrir \"%s\"", local_file_nm);
				close_data_connection ();
				return;
			}
		}

		if (vflag)
			mutime (&begin_time);

		while ((n = read (data_fd, buf, sizeof (buf))) > 0)
		{
			byte_cnt += n;

			if (write (file_fd, buf, n) != n)
				error ("*Erro na escrita do arquivo \"%s\"", local_file_nm);
		}

		if (vflag)
			print_time_interval (byte_cnt, &begin_time);

		if (n < 0)
			error ("*Erro na leitura da conexão de dados");

		if (!streq (local_file_nm, "-"))
			close (file_fd);

		close_data_connection ();

		get_reply ();

		return;

	}	/* end IMAGEM */

	/*
	 *	No momento só suportado ASCII e IMAGEM
	 */
	error ("Tipo não suportado de dados");

}	/* end recebe */

/*
 ****************************************************************
 *	Recebe e compara o conteúdo de um arquivo remoto	*
 ****************************************************************
 */
void
compara (const char *cmd, const char *remote_file_nm, const char *local_file_nm)
{
	FILE		*file_fp, *data_fp;
	int		data_c, file_c;
	char		*data_cp, *file_cp;
	long		nc = 1;
	MUTM		begin_time;
	long		nl = 1;
	char		data_line[BLSZ+1], file_line[BLSZ+1];

	if (open_data_connection (cmd, remote_file_nm) < 0)
		return;

	if ((file_fp = fopen (local_file_nm, "r")) == NOFILE)
	{
		error ("*Não consegui abrir \"%s\"", local_file_nm);
		close_data_connection ();
		return;
	}

	if ((data_fp = fdopen (data_fd, "r")) == NOFILE)
	{
		error ("*Não consegui associar um FILE à conexão de dados");
		close_data_connection ();
		return;
	}

	if (vflag)
		mutime (&begin_time);

	/*
	 *	Percorre os dois arquivos
	 */
	for
	(	file_cp = file_line, data_cp = data_line, data_c = '\0';
		(file_c = getc (file_fp)) >= 0 && (data_c = getc (data_fp)) >= 0;
		nc++
	)
	{
		/*
		 *	Verifica se deve pular o '\r'
		 */
		if (data_c == '\r' && type == TYPE_A)
		{
			if ((data_c = getc (data_fp)) < 0)
				break;
		}

		/*
		 *	Guarda os caracteres
		 */
		if (vflag  &&  data_cp < (data_line + BLSZ))
		{
			if (data_c != '\n')
				*data_cp++ = data_c;
			if (file_c != '\n')
				*file_cp++ = file_c;
		}

		/*
		 *	Testa se os caracteres são diferentes
		 */
		if (data_c != file_c)
		{
			/*
			 *	Indica o local da diferença
			 */
			fprintf
			(	stdout,
				"*** \"%s\" (%s) e \"%s\" (%s) diferem na "
				"linha %d, caracter %d\n",
				local_file_nm, local_node_nm,
				remote_file_nm, remote_node_nm, nl, nc
			);
			
			/*
			 *	Lista as linhas diferentes
			 */
			if (vflag)
			{
				if (file_c != '\n')
				{
					while ((file_c = getc (file_fp)) != '\n' && file_c >= 0)
					{
						if (file_cp >= (file_line + BLSZ))
							break;
						*file_cp++ = file_c;
					}
				}

				*file_cp = '\0';

				fprintf
				(	stdout,
					"%s (%s):	%s\n",
					local_file_nm, local_node_nm, file_line
				);

				if (data_c != '\n')
				{
					while (nc++, (data_c = getc (data_fp)) != '\n' && data_c >= 0)
					{
						if (data_c == '\r' && type == TYPE_A)
							{ nc--; continue; }
						if (data_cp >= (data_line + BLSZ))
							break;
						*data_cp++ = data_c;
					}

					if (data_c < 0)
						nc--;
				}

				*data_cp = '\0';

				fprintf
				(	stdout,
					"%s (%s):	%s\n",
					remote_file_nm, remote_node_nm, data_line
				);

			}	/* if (vflag) */

			nc++;

			goto data_flush;

		}	/* end	if (data_c != file_c) */

		if (data_c == '\n')
		{
			nl++;
			data_cp = data_line;
			file_cp = file_line;
		}

	}	/* end lendo as linhas dos arquivos */

	/*
	 *	Verifica se um dos arquivos terminou antes do outro
	 */
	if (data_c == EOF)
	{
		if (nc == 1)
		{
			fprintf
			(	stdout,
				"*** \"%s\" (%s) é um arquivo vazio\n",
				remote_file_nm, remote_node_nm
			);
		}
		else
		{
			fprintf
			(	stdout,
				"*** O arquivo \"%s\" (%s) é um "
				"prefixo de \"%s\" (%s)\n",
				remote_file_nm, remote_node_nm,
				local_file_nm, local_node_nm
			);
		}

		goto bad;
	}

	if (file_c == EOF  &&  getc (data_fp) != EOF)
	{
		if (nc == 1)
		{
			fprintf
			(	stdout,
				"*** \"%s\" (%s) é um arquivo vazio\n",
				local_file_nm, local_node_nm
			);
		}
		else
		{
			fprintf
			(	stdout,
				"*** O arquivo \"%s\" (%s) é um "
				"prefixo de \"%s\" (%s)\n",
				local_file_nm, local_node_nm,
				remote_file_nm, remote_node_nm
			);
		}

		nc++;

	    data_flush:
		while ((data_c = getc (data_fp)) >= 0)	/* not EOF */
		{
			if (data_c != '\r' || type != TYPE_A)
				nc++;
		}
	}
	else
	{
		fprintf
		(	stdout,
			"Os arquivos \"%s\" (%s) e \"%s\" (%s) são iguais\n",
			local_file_nm, local_node_nm,
			remote_file_nm, remote_node_nm
		);
	}

    bad:
	if (vflag)
		print_time_interval (nc - 1, &begin_time);

	if (ferror (file_fp))
		error ("*Erro na leitura do arquivo \"%s\"", local_file_nm);

	if (ferror (data_fp))
		error ("*Erro na leitura da conexão de dados");

	fclose (file_fp);

	close_data_connection ();

	fclose (data_fp);

	get_reply ();

	return;

}	/* end compara */

/*
 ****************************************************************
 *	Envia o conteúdo de um arquivo remoto			*
 ****************************************************************
 */
void
envia (const char *cmd, const char *local_file_nm, const char *remote_file_nm)
{
	FILE		*file_fp, *data_fp;
	int		file_fd;
	int		c, n;
	long		byte_cnt = 0;
	char		buf[BLSZ];
	MUTM		begin_time;

	/*
	 ******	Processa o tipo "ASCII" *************************
	 */
	if (type == TYPE_A)
	{
		if (open_data_connection (cmd, remote_file_nm) < 0)
			return;

		if ((file_fp = fopen (local_file_nm, "r")) == NOFILE)
		{
			error ("*Não consegui abrir \"%s\"", local_file_nm);
			close_data_connection ();
			return;
		}

		if ((data_fp = fdopen (data_fd, "w")) == NOFILE)
		{
			error ("*Não consegui associar um FILE à conexão de dados");
			close_data_connection ();
			return;
		}

		if (vflag)
			mutime (&begin_time);

		while ((c = getc (file_fp)) >= 0)	/* not EOF */
		{
			if (c == '\n')
				putc ('\r', data_fp);

			putc (c, data_fp); 	byte_cnt++;
		}

		fflush (data_fp); 	t_push (data_fd);

		if (vflag)
			print_time_interval (byte_cnt, &begin_time);

		if (ferror (file_fp))
			error ("*Erro na leitura do arquivo \"%s\"", local_file_nm);

		fclose (file_fp);

		if (ferror (data_fp))
			error ("*Erro na escrita da conexão de dados");

		if (t_sndrel (data_fd) < 0)
			error ("*Não consegui enviar a desconexão ordenada");

		if (t_rcvrel (data_fd) < 0)
			error ("*Não consegui receber a desconexão ordenada");

		fclose (data_fp);

		get_reply ();

		return;

	}	/* end ASCII */

	/*
	 ******	Processa o tipo "IMAGEM" ************************
	 */
	if (type == TYPE_I)
	{
		if (open_data_connection (cmd, remote_file_nm) < 0)
			return;

		if ((file_fd = open (local_file_nm, O_RDONLY)) < 0)
		{
			error ("*Não consegui abrir \"%s\"", local_file_nm);
			close_data_connection ();
			return;
		}

		if (vflag)
			mutime (&begin_time);

		while ((n = read (file_fd, buf, sizeof (buf))) > 0)
		{
			if (write (data_fd, buf, n) != n)
				error ("*Erro na escrita da conexão de dados");

			byte_cnt += n;
		}

		if (vflag)
			print_time_interval (byte_cnt, &begin_time);

		if (n < 0)
			error ("*Erro na leitura do arquivo \"%s\"", local_file_nm);

		close (file_fd);

		if (t_sndrel (data_fd) < 0)
			error ("*Não consegui enviar a desconexão ordenada");

		if (t_rcvrel (data_fd) < 0)
			error ("*Não consegui receber a desconexão ordenada");

		t_close (data_fd);

		get_reply ();

		return;

	}	/* end IMAGEM */

	/*
	 *	No momento só suportado ASCII e IMAGEM
	 */
	error ("Tipo não suportado de dados");

}	/* end envia */

/*
 ****************************************************************
 *	Imprime o tempo de transferência			*
 ****************************************************************
 */
void
print_time_interval (long byte_cnt, MUTM *begin_time)
{
	MUTM		end_time;
	time_t		Time, Utime;
	long		mili_time, i;

	/*
	 *	Faz a diferença de tempos - cuidado com o "carry"
	 */
	mutime (&end_time);

	Time = end_time.mu_time - begin_time->mu_time;

	if ((Utime = end_time.mu_utime - begin_time->mu_utime) < 0)
	{
		Time  -= 1;		/* Tira um segundo daqui */
		Utime += 1000000;	/* E compensa aqui */
	}

	mili_time = (Utime + 500) / 1000;	/* Milésimos de seg. */

	/*
	 *	Agora imprime
	 */
	printf
	(	"Total = %d bytes, tempo = %d:%02d.%03d",
		byte_cnt, Time / 60, Time % 60, mili_time
	);

	if ((i = 1000 * Time + mili_time) > 0)
		printf (" (%d bytes/segundo)\n", 1000 * byte_cnt / i);
	else
		printf ("\n");

}	/* end print_time_interval */

/*
 ****************************************************************
 *	Abre a conexão de dados					*
 ****************************************************************
 */
int
open_data_connection (const char *cmd, const char *remote_file_nm)
{
	if (passive_mode)
		return (open_passive_data_connection (cmd, remote_file_nm));
	else
		return (open_active_data_connection (cmd, remote_file_nm));

} 	/* end open_data_connection */

/*
 ****************************************************************
 *	Abre a conexão de dados ativa				*
 ****************************************************************
 */
int
open_active_data_connection (const char *cmd, const char *remote_file_nm)
{
	int		i;
	INADDR		remote_data_addr;
	T_BIND		bind;
	T_CALL		call;
	int		fd_vec[2];
	int		received_prelim = 0;

	/*
	 *	Associa um endereço local
	 */
	if ((data_fd = t_open (tcp_dev_nm, O_RDWR, (T_INFO *)NULL)) < 0)
	{
		error ("*Não consegui abrir \"%s\"", tcp_dev_nm);
		return (-1);
	}

	bind.addr.len	 = sizeof (INADDR);
	bind.addr.maxlen = sizeof (INADDR);
	bind.addr.buf	 = &local_data_addr;

	bind.qlen	 = 1;

	for (EVER)
	{
		local_data_addr.a_port++;

		if (t_bind (data_fd, &bind, &bind) >= 0)
			break;

		if (errno == TADDRBUSY && bind.addr.len > 0)
			{ bind.addr.len = 0; continue; }

		error ("*Não consegui associar um endereço local para a conexão de dados");
		t_close (data_fd);
		return (-1);
	}

	if (vflag > 1)
		printf ("Port_no de dados obtido: %d\n", local_data_addr.a_port);

	/*
	 *	Envia o "port" obtido
	 */
	if (sflag || local_data_addr.a_port != last_data_port)
	{
		const char	*ap, *dp;

		ap = (char *)&local_cntl_addr.a_addr;
		dp = (char *)&local_data_addr.a_port;

		if (vflag > 1)
			printf ("Executando PORT\n");

		if (comando
		(	1, "PORT %u,%u,%u,%u,%u,%u",
#ifdef	LITTLE_ENDIAN
			ap[3], ap[2], ap[1], ap[0],
			dp[1], dp[0]
#else
			ap[0], ap[1], ap[2], ap[3],
			dp[0], dp[1]
#endif	LITTLE_ENDIAN

		) != COMPLETE)
			{ t_close (data_fd); return (-1); }

		last_data_port = local_data_addr.a_port;
	}

	/*
	 *	Manda para o servidor o comando
	 */
	if (vflag > 1)
		printf ("Executando \"%s\"\n", cmd);

	if (remote_file_nm == NOSTR)
		comando (0, cmd);
	else
		comando (0, "%s %s", cmd, remote_file_nm);

	/*
	 *	Espera um pedido de conexão ou mensagem de erro
	 */
	fd_vec[0] = data_fd; 
	fd_vec[1] = cntl_fd;

	for (EVER)
	{
		if ((i = attention (2, fd_vec, -1, 0)) < 0)
		{
			error ("*Erro no ATTENTION");
			t_close (data_fd);
			return (-1);
		}

		if (i == 0)		/* data_fd => Listen */
		{
			break;
		}
		else			/* cntl_fd */
		{
			if (get_reply () != PRELIM)
			{
				t_close (data_fd);
				return (-1);
			}

			received_prelim++;
		}
	}

	call.addr.maxlen = sizeof (INADDR);
	call.addr.buf	 = &remote_data_addr;

	if (t_listen (data_fd, &call) < 0)
	{
		error ("*Erro no LISTEN da conexão de dados");
		t_close (data_fd);
		return (-1);
	}

	/*
	 *	Aceita a conexão
	 */
	if (t_accept (data_fd, data_fd, &call) < 0)
	{
		error ("*Erro no ACCEPT da conexão de dados");
		t_close (data_fd);
		return (-1);
	}

	if (received_prelim == 0 && get_reply () != PRELIM)
	{
		t_close (data_fd);
		return (-1);
	}

	if (vflag > 1)
		printf ("ftp: FIM de OPENDATACONN\n");

	return (0);

}	/* end open_active_data_connection */

/*
 ****************************************************************
 *	Abre a conexão de dados passiva				*
 ****************************************************************
 */
int
open_passive_data_connection (const char *cmd, const char *remote_file_nm)
{
	INADDR		serv_addr, bind_addr;
	T_BIND		bind;
	T_CALL		snd_call, rcv_call;

	/*
	 *	Envia o comando PASV e espera a resposta
	 */
	if (comando (1, "PASV") != 2)
		return (-1);

	/*
	 *	Associa um endereço local
	 */
	if ((data_fd = t_open (tcp_dev_nm, O_RDWR, (T_INFO *)NULL)) < 0)
	{
		error ("*Não consegui abrir \"%s\"", tcp_dev_nm);
		return (-1);
	}

	/*
	 *	Associa um endereço local
	 */
	bind_addr.a_addr = 0;

	bind.addr.len	 = 0;
	bind.addr.maxlen = sizeof (INADDR);
	bind.addr.buf	 = &bind_addr;

	bind.qlen	 = 0;

	if (t_bind (data_fd, &bind, &bind) < 0)
	{
		error ("*Não consegui associar um endereço local\n");
		return (-1);
	}

	/*
	 *	Executa o "connect"
	 */
	snd_call.addr.len    = sizeof (INADDR);
	snd_call.addr.buf    = &server_data_addr;

	rcv_call.addr.maxlen = sizeof (INADDR);
	rcv_call.addr.buf    = &serv_addr;

	if (t_connect (data_fd, &snd_call, &rcv_call) < 0)
	{
		if (errno == TLOOK && t_look (data_fd) == T_DISCONNECT)
			error ("A conexão foi recusada por \"%s\"", remote_node_nm);
		else
			error ("*Não consegui estabelecer conexão");

		return (-1);
	}

	/*
	 *	Manda para o servidor o comando
	 */
	if (vflag > 1)
		printf ("Executando \"%s\"\n", cmd);

	if (remote_file_nm == NOSTR)
		comando (1, cmd);
	else
		comando (1, "%s %s", cmd, remote_file_nm);

	return (0);

}	/* end open_passive_data_connection */

/*
 ****************************************************************
 *	Fecha a conexão de dados 				*
 ****************************************************************
 */
void
close_data_connection (void)
{
	if (t_sndrel (data_fd) < 0)
		error ("*Não consegui enviar a desconexão ordenada");

	if (t_rcvrel (data_fd) < 0)
		error ("*Não consegui receber a desconexão ordenada");

	t_close (data_fd);

}	/* end close_data_connection */
