/*
 ****************************************************************
 *								*
 *			sym.h					*
 *								*
 *	Informações sobre símbolos				*
 *								*
 *	Versão	3.0.0, de 29.03.96				*
 *		3.0.1, de 01.05.97				*
 *								*
 *	Módulo: Make: Construtor de Programas			*
 *		Interpretador de Comandos			*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 								*
 ****************************************************************
 */

/*
 ****************************************************************
 *	Lista de nomes						*
 ****************************************************************
 */
struct	nlist
{
	SYM	*n_sym;		/* O nome */
	RANGE	n_range;	/* Posição no texto */

	NLIST	*n_next;	/* Próximo nome */
};

/*
 ****************************************************************
 *	Lista de comandos para atualização			*
 ****************************************************************
 */
struct	clist
{
	char	*c_command;	/* Um comando */
	char	*c_line;	/* Linha onde se encontra */
	int	c_nline;	/* Número da linha */
	int	c_metachar;	/* Há metacaracteres no comando */

	CLIST	*c_next;	/* Próximo comando */
};

/*
 ****************************************************************
 *	Informações sobre dependências				*
 ****************************************************************
 */
struct	depen
{
	int	d_flags;	/* Flags */
	time_t	d_time;		/* Ultima modificação (0 = inexistente) */

	NLIST	*d_depen;	/* Lista de Dependências */
	CLIST	*d_command;	/* Lista de comandos para atualização */
	SYM	*d_sibling;	/* Lista circular de alvos irmãos */

	int	d_nline;	/* Número da linha onde aparece como alvo */
};

/*
 ****	Bits de "d_flags"  **************************************
 */
#define	D_READY		0x01	/* O nó já está atualizado */
#define	D_ERROR		0x02	/* Houve erro na atualização */
#define	D_CYCLE		0x04	/* O nó pertence ao caminho sendo explorado */
#define	D_EXIST		0x08	/* Executa os comandos apenas se não existir */
#define	D_VIRTUAL	0x10	/* O alvo é virtual */
#define	D_DEFINED	0x20	/* O alvo foi definido */

/*
 ****************************************************************
 *	Informações sobre macros				*
 ****************************************************************
 */
struct	macro
{
	char	*m_begin,	/* Início do texto da macro */
		*m_line;	/* Início da linha */
	int	m_nline;	/* Número da linha */
};

/*
 ****************************************************************
 *	Informações sobre símbolos				*
 ****************************************************************
 */
/*
 ****	Tipos de símbolos  **************************************
 */
typedef enum
{
	S_PUTENV,		/* Palavra reservada ".putenv" */
	S_EXIST,		/* Palavra reservada ".exist" */
	S_VIRTUAL,		/* Palavra reservada ".virtual" */
	S_NAME 			/* Macro ou nome de arquivo */

}	STYPE;

/*
 ****	Estrutura de informações sobre símbolos   ***************
 */
struct	sym
{
	STYPE	s_type;		/* Tipo (acima) */

	MACRO	s_macro;	/* Informações sobre a macro */
	DEPEN	s_node;		/* Nó do grafo de dependências */

	int	s_isid;		/* É um identificador */
	int	s_namelen;	/* strlen (s_name) */

	char	s_name[1];	/* Início do nome */
};

#define	SYM_FIXED_SZ	((int)(&((NOSYM)->s_name)))

/*
 ****************************************************************
 *	Estrutura da tabela hash				*
 ****************************************************************
 */
#define	HASHSZ	1009		/* Tamanho (deve ser primo) */

struct	hash
{
	SYM	*h_sym;		/* Ponteiro para a estrutura SYM */
};
