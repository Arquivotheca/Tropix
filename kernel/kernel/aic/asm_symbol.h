/*
 ****************************************************************
 *								*
 *			asm_symbol.h				*
 *								*
 *	Definições acerca dos símbolos do montador 2940		*
 *								*
 *	Versão	4.0.0, de 12.03.01				*
 *		4.0.0, de 12.03.01				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

typedef enum
{
	UNINITIALIZED,
	REGISTER,
	ALIAS,
	SCBLOC,
	SRAMLOC,
	MASK,
	BIT,
	CONST,
	DOWNLOAD_CONST,
	LABEL,
	CONDITIONAL

}	symtype;

typedef enum
{
	RO = 0x01,
	WO = 0x02,
	RW = 0x03

}	amode_t;

struct reg_info
{
	uchar	address;
	int		size;
	amode_t	 mode;
	uchar	valid_bitmask;
	int		typecheck_masks;
};

typedef SLIST_HEAD (symlist, symbol_node) symlist_t;

	struct mask_info
	{
		symlist_t	symrefs;
		uchar	mask;
	};

	struct const_info
	{
		uchar	value;
		int		define;
	};

	struct alias_info
	{
		struct symbol *parent;
	};

	struct label_info
	{
		int		address;
	};

	struct cond_info
	{
		int		func_num;
	};

	typedef struct expression_info
	{
		symlist_t	referenced_syms;
		int		value;
	}		expression_t;

	typedef struct symbol
	{
		char		*name;
		symtype	 type;
		union
		{
			struct reg_info *rinfo;
			struct mask_info *minfo;
			struct const_info *cinfo;
			struct alias_info *ainfo;
			struct label_info *linfo;
			struct cond_info *condinfo;
		}		info;
	}		symbol_t;

	typedef struct symbol_ref
	{
		symbol_t	*symbol;
		int		offset;
	}		symbol_ref_t;

	typedef struct symbol_node
	{
		SLIST_ENTRY (symbol_node) links;
		symbol_t	*symbol;
	}		symbol_node_t;

	typedef struct critical_section
	{
		TAILQ_ENTRY (critical_section) links;
		int		begin_addr;
		int		end_addr;
	}		critical_section_t;

	typedef enum
	{
		SCOPE_ROOT,
		SCOPE_IF,
		SCOPE_ELSE_IF,
		SCOPE_ELSE
	}		scope_type;

	typedef struct patch_info
	{
		int		skip_patch;
		int		skip_instr;
	}		patch_info_t;

	typedef struct scope
	{
		SLIST_ENTRY (scope) scope_stack_links;
		TAILQ_ENTRY (scope) scope_links;
		TAILQ_HEAD (, scope) inner_scope;
		scope_type	type;
		int		inner_scope_patches;
		int		begin_addr;
		int		end_addr;
		patch_info_t	patches[2];
		int		func_num;
	}		scope_t;

TAILQ_HEAD (cs_tailq, critical_section);
SLIST_HEAD (scope_list, scope);
TAILQ_HEAD (scope_tailq, scope);

#define SYMLIST_INSERT_HEAD	0x00
#define SYMLIST_SORT		0x01

#if (0)	/*******************************************************/
	void symbol_delete __P ((symbol_t * symbol));

	void symtable_open __P ((void));

	void symtable_close __P ((void));

	symbol_t	*
			symtable_get __P ((char *name));

	symbol_node_t *
			symlist_search __P ((symlist_t * symlist, char *symname));

	void
	symlist_add	__P ((symlist_t * symlist, symbol_t * symbol, int how));

	void symlist_free __P ((symlist_t * symlist));

	void symlist_merge __P ((symlist_t * symlist_dest, symlist_t * symlist_src1,
						 symlist_t * symlist_src2));
	void symtable_dump __P ((FILE * ofile));
#endif	/*******************************************************/
