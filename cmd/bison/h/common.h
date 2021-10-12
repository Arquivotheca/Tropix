/*
 ****************************************************************
 *								*
 *			common.h				*
 *								*
 *	Definições globais para BISON				*
 *								*
 *	Versão	3.0.0, de 06.07.93				*
 *		3.0.0, de 06.07.93				*
 *								*
 *	Módulo: GBISON						*
 *		Gerador de analisadores sintáticos		*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 		Baseado em software homônimo do GNU		*
 * 								*
 ****************************************************************
 */

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define	NOSTR	(char *)NULL
#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

/*
 ******	Alguns tipos ********************************************
 */
typedef unsigned *BSet;
typedef int	bool;

/*
 ******	Protótipos de funções ***********************************
 */
extern FILE	*tryopen (const char *name, char *mode);
extern bool	bits_equal (BSet L, BSet R, int n);
extern bool 	useful_production (int  i, BSet N);
extern char	*copys (char *s);
extern char	*grow_token_buffer (char *p);
extern char	*mallocate (unsigned n);
extern char	*stringappend (char *string1, int end1, char *string2);
extern int	action_row (int state);
extern int	bits_size (BSet S, int n);
extern int	default_goto (int symbol);
extern int	get_state (int symbol);
extern int	get_type (void);
extern int	hash (char *key);
extern int	lex (void);
extern int	map_goto (int, int);
extern int	matching_state (int vector);
extern int	nbits (unsigned i);
extern int	pack_vector (int vector);
extern int	parse_percent_token (void);
extern int	print_closure (int n);
extern int	print_derives (void);
extern int	print_fderives (void);
extern int	print_firsts (void);
extern int	read_signed_integer (FILE *stream);
extern int	skip_white_space (void);
extern short	**transpose (short **, int);
extern void	RTC (unsigned *R, int n);
extern void	TC (unsigned *R, int n);
extern void	add_lookback_edge (int, int, int);
extern void	allocate_itemsets (void);
extern void	allocate_storage (void);
extern void	append_states (void);
extern void	augment_automaton (void);
extern void	berror (char *s);
extern void	build_relations (void);
extern void	compute_FOLLOWS (void);
extern void	compute_lookaheads (void);
extern void	conflict_log (void);
extern void	copy_definition (void);
extern void	count_rr_conflicts (int state);
extern void	count_sr_conflicts (int state);
extern void	digraph (short **);
extern void	done (int k);
extern void	dump_grammar (void);
extern void	fatal (char *s);
extern void	finalize_closure (void);
extern void	finalize_conflicts (void);
extern void	flush_shift (int state, int token);
extern void	free_derives (void);
extern void	free_itemsets (void);
extern void	free_nullable (void);
extern void	free_reductions (void);
extern void	free_shifts (void);
extern void	free_storage (void);
extern void	free_symtab (void);
extern void	generate_states (void);
extern void	getargs (int argc, char *argv[]);
extern void	goto_actions (void);
extern void	inaccessable_symbols (void);
extern void	init_lex (void);
extern void	initialize_F (void);
extern void	initialize_LA (void);
extern void	initialize_closure (int n);
extern void	initialize_conflicts (void);
extern void	initialize_states (void);
extern void	insert_start_shift (void);
extern void	lalr (void);
extern void	log_resolution (int state, int LAno, int token, char *resolution);
extern void	new_itemsets (void);
extern void	open_extra_files (void);
extern void	openfiles (void);
extern void	output (void);
extern void	output_actions (void);
extern void	output_base (void);
extern void	output_check (void);
extern void	output_defines (void);
extern void	output_gram (void);
extern void	output_headers (void);
extern void	output_parser (void);
extern void	output_program (void);
extern void	output_rule_data (void);
extern void	output_stos (void);
extern void	output_table (void);
extern void	output_token_defines (FILE *file);
extern void	output_token_translations (void);
extern void	output_trailers (void);
extern void	pack_table (void);
extern void	packgram (void);
extern void	packsymbols (void);
extern void	parse_assoc_decl (int assoc);
extern void	parse_expect_decl (void);
extern void	parse_start_decl (void);
extern void	parse_token_decl (int what_is, int what_is_not);
extern void	parse_type_decl (void);
extern void	parse_union_decl (void);
extern void	print_actions (int state);
extern void	print_core (int state);
extern void	print_grammar (void);
extern void	print_notices (void);
extern void	print_reductions (int state);
extern void	print_results (void);
extern void	print_state (int state);
extern void	print_token (int extnum, int token);
extern void	read_declarations (void);
extern void	reader (void);
extern void	readgram (void);
extern void	record_rule_line (void);
extern void	reduce_grammar (void);
extern void	reduce_grammar_tables (void);
extern void	resolve_sr_conflict (int state, int lookaheadnum);
extern void	save_column (int symbol, int default_state);
extern void	save_reductions (void);
extern void	save_row (int state);
extern void	save_shifts (void);
extern void	set_accessing_symbol (void);
extern void	set_conflicts (int state);
extern void	set_derives (void);
extern void	set_fderives (void);
extern void	set_firsts (void);
extern void	set_goto_map (void);
extern void	set_maxrhs (void);
extern void	set_nullable (void);
extern void	set_reduction_table (void);
extern void	set_shift_table (void);
extern void	set_state_table (void);
extern void	sort_actions (void);
extern void	tabinit (void);
extern void	terse (void);
extern void	token_actions (void);
extern void	toomany (char *s);
extern void	total_conflicts (void);
extern void	traverse (int);
extern void	unlex (int token);
extern void	useless_nonterminals (void);
extern void	verbose (void);
extern void	verbose_conflict_log (void);
