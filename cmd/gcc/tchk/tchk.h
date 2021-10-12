/*
 ****************************************************************
 *								*
 *			tchk/tchk-def.h				*
 *								*
 *	Módulo principal					*
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
extern tree	actualparameterlist (tree, tree, tree);
extern void	binary_op_error (enum tree_code);
extern tree	build_array_ref (tree, tree);
extern tree	build_binary_op (enum tree_code, tree, tree);
extern tree	build_binary_op_nodefault (enum tree_code, tree, tree, enum tree_code);
extern tree	build_c_cast (tree, tree);
extern tree	build_component_ref (tree, tree);
extern tree	build_compound_expr (tree);
extern tree	build_conditional_expr (tree, tree, tree);
extern tree	build_function_call (tree, tree);
extern tree	build_indirect_ref (tree, char *);
extern tree	build_modify_expr (tree, enum tree_code, tree);
extern tree	build_unary_op (enum tree_code, tree, int);
extern tree	c_alignof (tree);
extern void	c_expand_asm_operands (tree, tree, tree, tree, int, char *, int);
extern void	c_expand_return (tree);
extern tree	c_expand_start_case (tree);
extern tree	c_sizeof (tree);
extern tree	c_sizeof_nowarn (tree);
extern tree	commontype (tree, tree);
extern int	comp_target_types (tree, tree);
extern int	compparms (tree, tree);
extern int	compparms1 (tree);
extern int	comptypes (tree, tree);
extern tree	convert_for_assignment (tree, tree, char *);
extern tree	convert_sequence (tree, tree);
extern tree	datatype (tree);
extern tree	decl_constant_value (tree);
extern tree	default_conversion (tree);
extern tree	digest_init (tree, tree, tree *);
extern tree	get_floating_type (enum machine_mode);
extern void	incomplete_type_error (tree, tree);
extern int	initializer_constant_valid_p (tree);
extern tree	invert_truthvalue (tree);
extern int	language_lvalue_valid (tree);
extern int	mark_addressable (tree);
extern tree	pointer_diff (tree, tree);
extern tree	pointer_int_sum (enum tree_code, tree, tree);
extern tree	process_init_constructor (tree, tree, tree *);
extern tree	qualify_type (tree, tree);
extern void	readonly_warning (tree arg, char *);
extern tree	require_complete_type (tree);
extern tree	shorten_compare (tree *, tree *, tree *, enum tree_code *);
extern tree	signed_or_unsigned_type (int, tree);
extern tree	signed_type (tree);
extern void	store_init_value (tree, tree);
extern tree	truthvalue_conversion (tree);
extern tree	type_for_size (int, int);
extern tree	unary_complex_lvalue (enum tree_code, tree);
extern tree	unsigned_type (tree);
