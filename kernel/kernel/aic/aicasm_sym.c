/*
 ****************************************************************
 *								*
 *			aicasm_sym.c				*
 *								*
 *	Tabela de símbolos para o montador da placa 2940	*
 *								*
 *	Versão	4.0.0, de 15.03.01				*
 *		4.0.0, de 15.03.01				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 *		Baseado no FreeBSD 4.2				*
 *								*
 ****************************************************************
 */

#include "../h/types.h"
#include "../h/queue.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../aic/sysexits.h"
#include "../aic/asm_symbol.h"
#include "../aic/aicasm.h"

#define	elif	else if

/*
 ******	Dados para o "db" de mentirinha *************************
 */
typedef struct
{
	const char	*db_name;
	symbol_t	*db_value;

}	FDB;

#define NFDB	1024

entry FDB	fdb[NFDB];

#define	NOFDB	(FDB *)NULL

/*
 ******	Protótipos de funções ***********************************
 */
int	fake_db_get (const char *, symbol_t **);
int	fake_db_put (const char *, symbol_t *);
void	fake_db_dump (void);

void	symlist_free (symlist_t *symlist);

/*
 ****************************************************************
 *	Cria uma entrada					*
 ****************************************************************
 */
symbol_t *
symbol_create (const char *name)
{
	symbol_t	*new_symbol;

	new_symbol = malloc (sizeof (symbol_t));

	if (new_symbol == NULL)
	{
		perror ("Unable to create new symbol");
		exit (EX_SOFTWARE);
	}

	memset (new_symbol, 0, sizeof (*new_symbol));

	new_symbol->name = strdup (name);
	new_symbol->type = UNINITIALIZED;

	return (new_symbol);

}	/* end symbol_create */

/*
 ****************************************************************
 *	Abre a tabela de símbolos				*
 ****************************************************************
 */
void
symtable_open (void)
{

}	/* end symtable_open */

/*
 ****************************************************************
 *	Fecha a tabela de símbolos				*
 ****************************************************************
 */
void
symtable_close (void)
{

}	/* end symtable_close */

/*
 ****************************************************************
 *	Busca uma entrada					*
 ****************************************************************
 */
symbol_t *
symtable_get (const char *name)
{
	symbol_t *stored_ptr;
	int	  retval;

	if ((retval = fake_db_get (name, &stored_ptr)) != 0)
	{
		if (retval == -1)
		{
			perror ("Symbol table get operation failed");
			exit (EX_SOFTWARE);

		}
		elif (retval == 1)
		{
			/* Symbol wasn't found, so create a new one */

			symbol_t	*new_symbol;

			new_symbol = symbol_create (name);

			if (fake_db_put (name, new_symbol) != 0)
			{
				perror ("Symtable put failed");
				exit (EX_SOFTWARE);
			}

			return (new_symbol);

		}
		else
		{
			perror ("Unexpected return value from db get routine");
			exit (EX_SOFTWARE);
		}
	}

	return (stored_ptr);

}	/* end symtable_get */

/*
 ****************************************************************
 *	Busca em uma lista					*
 ****************************************************************
 */
symbol_node_t *
symlist_search (const symlist_t *symlist, const char *symname)
{
	symbol_node_t *curnode;

	curnode = symlist->slh_first;

	while (curnode != NULL)
	{
		if (strcmp (symname, curnode->symbol->name) == 0)
			break;

		curnode = curnode->links.sle_next;
	}

	return (curnode);

}	/* symlist_search */

/*
 ****************************************************************
 *	Acrescenta à lista					*
 ****************************************************************
 */
void
symlist_add (symlist_t *symlist, symbol_t *symbol, int how)
{
	symbol_node_t	*newnode;

	newnode = malloc (sizeof (symbol_node_t));

	if (newnode == NULL)
		stop ("symlist_add: Unable to malloc symbol_node", EX_SOFTWARE);

	newnode->symbol = symbol;

	if (how == SYMLIST_SORT)
	{
		symbol_node_t	*curnode;
		int		mask;

		mask = FALSE;

		switch (symbol->type)
		{
		    case REGISTER:
		    case SCBLOC:
		    case SRAMLOC:
			break;

		    case BIT:
		    case MASK:
			mask = TRUE;
			break;

		    default:
			stop ("symlist_add: Invalid symbol type for sorting", EX_SOFTWARE);

		}

		curnode = symlist->slh_first;

		if
		(	curnode == NULL ||
			(mask && (curnode->symbol->info.minfo->mask >
		              newnode->symbol->info.minfo->mask)) ||
			(!mask && (curnode->symbol->info.rinfo->address >
		               newnode->symbol->info.rinfo->address)))
		{
			SLIST_INSERT_HEAD (symlist, newnode, links);
			return;
		}

		while (1)
		{
			if (curnode->links.sle_next == NULL)
			{
				SLIST_INSERT_AFTER(curnode, newnode, links);
				break;
			}
			else
			{
				symbol_t	*cursymbol;

				cursymbol = curnode->links.sle_next->symbol;

				if
				(	(mask && (cursymbol->info.minfo->mask >
				              symbol->info.minfo->mask)) ||
					(!mask &&(cursymbol->info.rinfo->address >
				              symbol->info.rinfo->address))
				)
				{
					SLIST_INSERT_AFTER(curnode, newnode, links);
					break;
				}
			}
			curnode = curnode->links.sle_next;
		}
	}
	else
	{
		SLIST_INSERT_HEAD(symlist, newnode, links);
	}

}	/* end symlist_add */

/*
 ****************************************************************
 *	Libera a lista						*
 ****************************************************************
 */
void
symlist_free (symlist_t *symlist)
{
	symbol_node_t	*node1, *node2;

	node1 = symlist->slh_first;

	while (node1 != NULL)
	{
		node2 = node1->links.sle_next;
		free(node1);
		node1 = node2;
	}

	SLIST_INIT (symlist);

}	/* end symlist_free */

/*
 ****************************************************************
 *	Entrelaça listas					*
 ****************************************************************
 */
void
symlist_merge (symlist_t *symlist_dest, symlist_t *symlist_src1, symlist_t *symlist_src2)
{
	symbol_node_t	*node;

	*symlist_dest = *symlist_src1;

	while ((node = symlist_src2->slh_first) != NULL)
	{
		SLIST_REMOVE_HEAD(symlist_src2, links);
		SLIST_INSERT_HEAD(symlist_dest, node, links);
	}

	/* These are now empty */

	SLIST_INIT (symlist_src1);
	SLIST_INIT (symlist_src2);

}	/* end symlist_merge */

/*
 ****************************************************************
 *	Escreve a tabela de símbolos				*
 ****************************************************************
 */
void
symtable_dump (FILE *ofile)
{
	/*
	 * Sort the registers by address with a simple insertion sort.
	 * Put bitmasks next to the first register that defines them.
	 * Put constants at the end.
	 */
	symlist_t	registers;
	symlist_t	masks;
	symlist_t	constants;
	symlist_t	download_constants;
	symlist_t	aliases;
	FDB		*dbp;

	SLIST_INIT(&registers);
	SLIST_INIT(&masks);
	SLIST_INIT(&constants);
	SLIST_INIT(&download_constants);
	SLIST_INIT(&aliases);

	for (dbp = fdb; dbp->db_name != NOSTR; dbp++)
	{
		symbol_t	*cursym = dbp->db_value;

		switch(cursym->type)
		{
		    case REGISTER:
		    case SCBLOC:
		    case SRAMLOC:
			symlist_add(&registers, cursym, SYMLIST_SORT);
			break;

		    case MASK:
		    case BIT:
			symlist_add(&masks, cursym, SYMLIST_SORT);
			break;

		    case CONST:
			if (cursym->info.cinfo->define == FALSE)
			{
				symlist_add(&constants, cursym, SYMLIST_INSERT_HEAD);
			}
			break;

		    case DOWNLOAD_CONST:
			symlist_add(&download_constants, cursym, SYMLIST_INSERT_HEAD);
			break;

		    case ALIAS:
			symlist_add(&aliases, cursym, SYMLIST_INSERT_HEAD);
			break;

		    default:
			break;
		}
	}

	/* Put in the masks and bits */

	while (masks.slh_first != NULL)
	{
		symbol_node_t	*curnode;
		symbol_node_t	*regnode;
		char		*regname;

		curnode = masks.slh_first;
		SLIST_REMOVE_HEAD(&masks, links);

		regnode = curnode->symbol->info.minfo->symrefs.slh_first;
		regname = regnode->symbol->name;
		regnode = symlist_search(&registers, regname);
		SLIST_INSERT_AFTER(regnode, curnode, links);
	}

	/* Add the aliases */

	while (aliases.slh_first != NULL)
	{
		symbol_node_t	*curnode;
		symbol_node_t	*regnode;
		char		*regname;

		curnode = aliases.slh_first;
		SLIST_REMOVE_HEAD(&aliases, links);

		regname = curnode->symbol->info.ainfo->parent->name;
		regnode = symlist_search(&registers, regname);
		SLIST_INSERT_AFTER(regnode, curnode, links);
	}

	/* Output what we have */

	fprintf
	(	ofile,
		"/*\n"
		" * DO NOT EDIT - This file is automatically generated.\n"
		" */\n"
	);

	while (registers.slh_first != NULL)
	{
		symbol_node_t	*curnode;
		uchar		value;
		char		*tab_str;
		char		*tab_str2;

		curnode = registers.slh_first;
		SLIST_REMOVE_HEAD(&registers, links);

		switch(curnode->symbol->type)
		{
		    case REGISTER:
		    case SCBLOC:
		    case SRAMLOC:
			fprintf(ofile, "\n");
			value = curnode->symbol->info.rinfo->address;
			tab_str = "\t";
			tab_str2 = "\t\t";
			break;

		    case ALIAS:
		    {
			symbol_t *parent;

			parent = curnode->symbol->info.ainfo->parent;
			value = parent->info.rinfo->address;
			tab_str = "\t";
			tab_str2 = "\t\t";
			break;
		    }

		    case MASK:
		    case BIT:
			value = curnode->symbol->info.minfo->mask;
			tab_str = "\t\t";
			tab_str2 = "\t";
			break;

		    default:
			value = 0; /* Quiet compiler */
			tab_str = NULL;
			tab_str2 = NULL;
			stop("symtable_dump: Invalid symbol type "
			     "encountered", EX_SOFTWARE);
			break;

		}	/* end switch */

		fprintf
		(	ofile,
			"#define%s%-16s%s0x%02x\n",
			tab_str, curnode->symbol->name, tab_str2, value
		);

		free (curnode);

	}	/* end while */

	fprintf (ofile, "\n\n");

	while (constants.slh_first != NULL)
	{
		symbol_node_t	*curnode;

		curnode = constants.slh_first;
		SLIST_REMOVE_HEAD(&constants, links);

		fprintf
		(	ofile,
			"#define\t%-8s\t0x%02x\n",
			curnode->symbol->name,
			curnode->symbol->info.cinfo->value
		);

		free(curnode);
	}

	
	fprintf (ofile, "\n\n/* Downloaded Constant Definitions */\n");

	while (download_constants.slh_first != NULL)
	{
		symbol_node_t	*curnode;

		curnode = download_constants.slh_first;
		SLIST_REMOVE_HEAD(&download_constants, links);

		fprintf
		(	ofile,
			"#define\t%-8s\t0x%02x\n",
			curnode->symbol->name,
			curnode->symbol->info.cinfo->value
		);

		free(curnode);

	}	/* end while */

}	/* end symtable_dump */

/*
 ****************************************************************
 *	Procura um elemento na tabela				*
 ****************************************************************
 */
int
fake_db_get (const char *nm, symbol_t **ptr)
{
	const FDB	*dbp;

#undef	DEBUG
#ifdef	DEBUG
	printf ("Procurando \"%s\"\n", nm);
	fake_db_dump ();
#endif	DEBUG

	for (dbp = fdb; /* abaixo */; dbp++)
	{
		if (dbp->db_name == NOSTR)
			return (1);

		if (!streq (dbp->db_name, nm))
			continue;

		*ptr = dbp->db_value;
		return (0);
	}

}	/* end fake_db_get */

/*
 ****************************************************************
 *	Insere um elemento na tabela				*
 ****************************************************************
 */
int
fake_db_put (const char *nm, symbol_t *sym)
{
	FDB		*dbp;

	for (dbp = fdb; /* abaixo */; dbp++)
	{
		if (dbp->db_name == NOSTR)
		{
			if (dbp >= &fdb[NFDB-1])
			{
				perror ("Fake DB cheia");
				exit (EX_SOFTWARE);
			}
#ifdef	DEBUG
			printf ("Inserindo \"%s\"\n", nm);
#endif	DEBUG
			if ((dbp->db_name = strdup (nm)) == NOSTR)
			{
				perror ("Memória esgotada");
				exit (EX_SOFTWARE);
			}

			dbp->db_value = sym;
			return (0);
		}

		if (!streq (dbp->db_name, nm))
			continue;

		return (1);
	}

}	/* end fake_db_put */

#ifdef	DEBUG
/*
 ****************************************************************
 *	Imprime toda tabela					*
 ****************************************************************
 */
void
fake_db_dump (void)
{
	FDB	*dbp;

	printf ("\n");

	for (dbp = fdb; dbp->db_name != NOSTR; dbp++)
	{
		printf ("%P\t%s\n", dbp->db_value, dbp->db_name);
	}

	printf ("\n");

}	/* end fake_db_dump */
#endif	DEBUG
