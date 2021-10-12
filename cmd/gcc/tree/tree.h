/*
 ****************************************************************
 *								*
 *			tree/tree-def.h				*
 *								*
 *	Protótipos do diretório "tree"				*
 *								*
 *	Versão	3.0.0, de 29.12.93				*
 *		3.0.0, de 29.12.93				*
 *								*
 *	Módulo: GCC						*
 *		Compilador "C"					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 1993 UNITEC Informática Ltda	*
 * 		Baseado em software homônimo do GNU		*
 *								*
 ****************************************************************
 */

/*
 ******	Protótipos de funções ***********************************
 */
extern int	allocation_temporary_p (void);
extern tree	array_type_nelts (tree);
extern tree	build (enum tree_code, ...);
extern tree	build_array_type (tree, tree);
extern tree	build_complex (tree, tree);
extern tree	build_decl (enum tree_code, tree, tree);
extern tree	build_function_type (tree, tree);
extern tree	build_index_type (tree);
extern tree	build_int_2 (int, int);
extern tree	build_let (char *, int, tree, tree, tree, tree);
extern tree	build_method_type (tree, tree);
extern tree	build_nt (enum tree_code, ...);
extern tree	build_offset_type (tree, tree);
extern tree	build_op_identifier (tree, tree);
extern tree	build_pointer_type (tree);
extern tree	build_real (tree, REAL_VALUE_TYPE);
extern tree	build_real_from_int_cst (tree, tree);
extern tree	build_reference_type (tree);
extern tree	build_string (int, char *);
extern tree	build_tree_list (tree, tree);
extern tree	build_type_variant (tree, int, int);
extern tree	chainon (tree, tree);
extern void	clear_momentary (void);
extern tree	copy_list (tree);
extern tree	copy_node (tree);
extern void	end_temporary_allocation (void);
extern tree	get_identifier (char *);
extern tree	get_narrower (tree, int *);
extern tree	get_unwidened (tree, tree);
extern void	init_tree (void);
extern int	int_fits_type_p (tree, tree);
extern int	int_size_in_bytes (tree);
extern int	integer_all_onesp (tree);
extern int	integer_onep (tree);
extern int	integer_zerop (tree);
extern int	list_length (tree);
extern int	lvalue_or_else (tree, char *);
extern int	lvalue_p (tree);
extern tree	make_node (enum tree_code);
extern tree	nreverse (tree);
extern void	*oballoc (int);
extern void	obfree (char *);
extern char	*permalloc (long);
extern tree	perm_tree_cons (tree, tree, tree);
extern void	permanent_allocation (void);
extern void	pop_momentary (void);
extern void	preserve_data (void);
extern void	push_momentary (void);
extern REAL_VALUE_TYPE real_value_from_int_cst (tree);
extern void	resume_momentary (int);
extern void	resume_temporary_allocation (void);
extern tree	save_expr (tree);
extern char	*savealloc (int);
extern tree	saveable_tree_cons (tree, tree, tree);
extern void	set_identifier_size (int);
extern int	simple_cst_equal (tree, tree);
extern tree	size_in_bytes (tree);
extern tree	stabilize_reference (tree);
extern void	start_identifier_warnings (void);
extern int	staticp (tree);
extern int	suspend_momentary (void);
extern tree	temp_tree_cons (tree, tree, tree);
extern void	temporary_allocation (void);
extern tree	tree_cons (tree, tree, tree);
extern int	tree_int_cst_equal (tree, tree);
extern int	tree_int_cst_lt (tree, tree);
extern tree	tree_last (tree);
extern void	type_hash_add (int, tree);
extern tree	type_hash_canon (int, tree);
extern int	type_hash_list (tree);
extern tree	type_hash_lookup (int, tree);
extern int	type_list_equal (tree, tree);
extern int	type_precision (tree);
