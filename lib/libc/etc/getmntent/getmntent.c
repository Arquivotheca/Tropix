/*
 ****************************************************************
 *								*
 *			getmntent.c				*
 *								*
 *	Leitura do arquivo de dispositivos a montar		*
 *								*
 *	Versão	4.3.0, de 25.09.02				*
 *		4.8.0, de 10.10.05				*
 *								*
 *	Módulo: getmntent					*
 *		libc/etc					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2005 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>

#include <ctype.h>
#include <mntent.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
typedef struct
{
	char		opt_name[12];	/* Nome da opção */
	int		opt_on;		/* Liga este indicador */
	int		opt_off;	/* Desliga este indicador */

}	FSOPT;

#define	EVER	;;
#define	elif	else if

/*
 ******	Tabela das palavras-chaves sem valores ******************
 */
static const FSOPT	fsbittb[] =
{
	/* Nome		Liga		Desliga				*/

	{ "ro",		SB_RONLY,	0				},
	{ "rw",		0,		SB_RONLY			},
	{ "user",	SB_USER,	SB_EXEC|SB_SUID|SB_DEV		},
	{ "nouser",	0,		SB_USER				},
	{ "atime",	SB_ATIME,	0				},
	{ "noatime",	0,		SB_ATIME			},
	{ "dev",	SB_DEV,		0				},
	{ "nodev",	0,		SB_DEV				},
	{ "exec",	SB_EXEC,	0				},
	{ "noexec",	0,		SB_EXEC				},
	{ "suid",	SB_SUID,	0				},
	{ "nosuid",	0,		SB_SUID				},
	{ "eject",	SB_EJECT,	0				},
	{ "noeject",	0,		SB_EJECT			},
	{ "case",	SB_CASE,	0				},
	{ "nocase",	0,		SB_CASE				},
	{ "rock",	SB_ROCK,	0				},
	{ "norock",	0,		SB_ROCK				},
	{ "joliet",	SB_JOLIET,	0				},
	{ "nojoliet",	0,		SB_JOLIET			},
	{ "lfn",	SB_LFN,		0				},
	{ "nolfn",	0,		SB_LFN				},
	{ "hard",	SB_HARD,	0				},
	{ "soft",	0,		SB_HARD				},

	{ "auto",	SB_AUTO,	0				},
	{ "noauto",	0,		SB_AUTO				},
	{ "fsck",	SB_FSCK,	0				},
	{ "nofsck",	0,		SB_FSCK				},
	{ "" }
};

/*
 ******	Tabela das palavras-chaves com valores ******************
 */
#define	opt_default	opt_on
#define	opt_offset	opt_off

#define OFF(member) offsetof (struct mntent, member)

static const FSOPT	fsargtb[] =
{
	{ "uid",	-1,		OFF (me_uid)		},
	{ "gid",	-1,		OFF (me_gid)		},
	{ "rsize",	SB_RSIZE,	OFF (me_rsize)		},
	{ "wsize",	SB_WSIZE,	OFF (me_wsize)		},
	{ "timeout",	SB_TIMEOUT,	OFF (me_timeout)	},
	{ "retrans",	SB_RETRANS,	OFF (me_retrans)	},
	{ "port",	SB_PORT,	OFF (me_port)		},
	{ "nfsvers",	SB_NFSVERS,	OFF (me_nfsvers)	},
	{ "" }
};

/*
 ****************************************************************
 *	Fornece uma linha de "fstab"				*
 ****************************************************************
 */
int
getmntent (FILE *fp, char *dev_nm, char *dir_nm, struct mntent *mntent_ptr)
{
	char		*cp;
	char		area[128];

	/*
	 *	Coloca os valores padronizados na estrutura
	 */
	defmntent (mntent_ptr);

	/*
	 *	Processa a linha
	 */
	for (EVER)
	{
		if (fngets (area, sizeof (area), fp) == NOSTR)
			return (EOF);

		/* Procura o dispositivo */

		if ((cp = strtok (area, " \t")) == NOSTR || cp[0] == '#')
			continue;

		strcpy (dev_nm, cp);

		/* Procura o diretório */

		if ((cp = strtok (NOSTR, " \t")) == NOSTR || cp[0] == '#')
			{ error ("Faltando o diretório de \"%s\"", dev_nm); return (0); }

		strcpy (dir_nm, cp);

		/* Procura as opções */

		if ((cp = strtok (NOSTR, " \t")) == NOSTR || cp[0] == '#')
			return (1);

		return (getmntopt (cp, mntent_ptr));
	}

}	/* end getmntent */

/*
 ****************************************************************
 *	Atribui os valores "default"				*
 ****************************************************************
 */
void
defmntent (struct mntent *mntent_ptr)
{
	/*
	 *	Coloca os valores padronizados na estrutura
	 */
	mntent_ptr->me_flags		= SB_DEFAULT_ON;

	mntent_ptr->me_server_addr	= -1;		/* Endereço IP do servidor */
	mntent_ptr->me_uid		= -1;		/* Usuário */
	mntent_ptr->me_gid		= -1;		/* Grupo */
	mntent_ptr->me_rsize		= SB_RSIZE;	/* Tamanho de leitura */
	mntent_ptr->me_wsize		= SB_WSIZE;	/* Tamanho de escrita */
	mntent_ptr->me_timeout		= SB_TIMEOUT;	/* Tempo para retransmissão */
	mntent_ptr->me_retrans		= SB_RETRANS;	/* No. de retransmissões */
	mntent_ptr->me_port		= SB_PORT;	/* No. da porta */
	mntent_ptr->me_nfsvers		= SB_NFSVERS;	/* Versão do protocolo NFS */

}	/* end defmntent */

/*
 ****************************************************************
 *	Analisa as opções de uma linha de "fstab"		*
 ****************************************************************
 */
int
getmntopt (const char *str, struct mntent *mntent_ptr)
{
	char		*cp, *equal;
	const char	*after;
	const FSOPT	*op;
	const PASSWD	*pwd;
	const GROUP	*grp;
	int		value, flags;

	/*
	 *	Repare que esta função faz uma intercalação dos parâmetros
	 *	já presentes em "mntent_ptr" com os novos dados em "str".
	 */
	if (str == NOSTR || str[0] == '\0')
		return (1);

	flags = mntent_ptr->me_flags;

	cp = alloca (strlen (str) + 1); strcpy (cp, str);

	/*
	 *	Procura as opções
	 */
	for (cp = strtok (cp, ","); cp != NOSTR; cp = strtok (NOSTR, ","))
	{
		if ((equal = strchr (cp, '=')) == NOSTR)			/* Opções sem valores */
		{
			if (streq (cp, "default"))
			{
				flags = SB_DEFAULT_ON;
			}
			else for (op = fsbittb; /* abaixo */; op++)
			{
				if (op->opt_name[0] == '\0')
					{ error ("Opção de montagem \"%s\" inválida", cp); return (0); }

				if (streq (op->opt_name, cp))
					{ flags &= ~op->opt_off; flags |= op->opt_on; break; }
			}
		}
		else for (*equal++ = '\0', op = fsargtb; /* abaixo */; op++)	/* Opções com valores */
		{
			if (op->opt_name[0] == '\0')
				{ error ("Opção de montagem \"%s\" inválida", cp); return (0); }

			if (streq (op->opt_name, cp))
			{
				/* Cuidado: "getpwnam" e "getgrnam" usam "strfield" */

				if   (isalpha (equal[0]))
				{
				    if   (op == &fsargtb[0])		/* uid */
				    {
					if ((pwd = getpwnam (equal)) == NOPASSWD)
						{ error ("Usuário \"%s\" desconhecido", equal); return (0); }

					value = pwd->pw_uid;
				    }
				    elif (op == &fsargtb[1])		/* gid */
				    {
					if ((grp = getgrnam (equal)) == NOGROUP)
						{ error ("Grupo \"%s\" desconhecido", equal); return (0); }

					value = grp->gr_gid;
				    }
				    else
				    {
					error ("Valor \"%s\" inválido", equal);
					return (0);
				    }
				}
				elif ((value = strtol (equal, &after, 0)) < 0 || *after)
				{
					error ("Valor \"%s\" inválido", equal);
					return (0);
				}

				*(int *)((void *)mntent_ptr + op->opt_offset) = value;

				break;
			}
		}

	}	/* end percorrendo as opções */

	mntent_ptr->me_flags = flags;

	return (1);

}	/* end getmntopt */

/*
 ****************************************************************
 *	Edita uma lista de opções				*
 ****************************************************************
 */
const char *
editmntopt (char *option_str, const struct mntent *mntent_ptr, int vflag)
{
	int		flags = mntent_ptr->me_flags, value;
	const FSOPT	*op;
	char		first = 1;
	const char	*str;

	/*
	 *	Processa os indicadores
	 */
	if (!vflag && ((flags ^ SB_DEFAULT_ON) & ~SB_NOT_KERNEL) == 0)
	{
		strcpy (option_str, "default");
	}
	else for (option_str[0] = '\0', op = fsbittb; op->opt_name[0] != '\0'; op += 2)
	{
		if (!vflag && ((flags ^ SB_DEFAULT_ON) & op->opt_on) == 0)
			continue;

		if (op->opt_on & flags)
			str = op[0].opt_name;
		else
			str = op[1].opt_name;

		if (first)
			first = 0;
		else
			strcat (option_str, ",");

		strcat (option_str, str);

	}	/* end for (percorrendo os indicadores) */

	/*
	 *	Processa os valores
	 */
	for (op = fsargtb; op->opt_name[0] != '\0'; op += 1)
	{
		char		text[40];

		value = *(int *)((void *)mntent_ptr + op->opt_offset);

		if (value == op->opt_default || value == -1)
			continue;

		if   (op == &fsargtb[0] && (str = pwcache (value)) != NOSTR)	/* uid */
			sprintf (text, ",%s=%s", op->opt_name, str);
		elif (op == &fsargtb[1] && (str = grcache (value)) != NOSTR)	/* gid */
			sprintf (text, ",%s=%s", op->opt_name, str);
		else
			sprintf (text, ",%s=%d", op->opt_name, value);

		strcat (option_str, text);

	}	/* end for (percorrendo os valores) */

	return (option_str);

}	/* end editmntopt */
