/*
 ****************************************************************
 *								*
 *			etc.c					*
 *								*
 *	Outras funções auxiliares				*
 *								*
 *	Versão	3.0.0, de 24.09.93				*
 *		3.0.0, de 24.09.93				*
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
 ******	Inclusões de arquivos ***********************************
 */
#pragma	getenv (GEN)

#include ../$GEN.gen/config.h
#include "../h/tree.h"
#include "../h/c-tree.h"

/*
 ****************************************************************
 *	x					*
 ****************************************************************
 */
/* Return something to represent absolute declarators containing a *.
   TARGET is the absolute declarator that the * contains.
   TYPE_QUALS is a list of modifiers such as const or volatile
   to apply to the pointer type, represented as identifiers.

   We return an INDIRECT_REF whose "contents" are TARGET
   and whose type is the modifier list.  */

tree
make_pointer_declarator (type_quals, target)
     tree type_quals, target;
{
  return build (INDIRECT_REF, type_quals, target);
}

/*
 ****************************************************************
 *	x					*
 ****************************************************************
 */
/* Given a chain of STRING_CST nodes,
   concatenate them into one STRING_CST
   and give it a suitable array-of-chars data type.  */

tree
combine_strings (strings)
     tree strings;
{
  register tree value, t;
  register int length = 1;
  int wide_length = 0;
  int wide_flag = 0;
  int nchars;

  if (TREE_CHAIN (strings))
    {
      /* More than one in the chain, so concatenate.  */
      register char *p, *q;

      /* Don't include the \0 at the end of each substring,
	 except for the last one.
	 Count wide strings and ordinary strings separately.  */
      for (t = strings; t; t = TREE_CHAIN (t))
	{
	  if (TREE_TYPE (t) == int_array_type_node)
	    {
	      wide_length += (TREE_STRING_LENGTH (t) - UNITS_PER_WORD);
	      wide_flag = 1;
	    }
	  else
	    length += (TREE_STRING_LENGTH (t) - 1);
	}

      /* If anything is wide, the non-wides will be converted,
	 which makes them take more space.  */
      if (wide_flag)
	length = length * UNITS_PER_WORD + wide_length;

      p = (char *) savealloc (length);

      /* Copy the individual strings into the new combined string.
	 If the combined string is wide, convert the chars to ints
	 for any individual strings that are not wide.  */

      q = p;
      for (t = strings; t; t = TREE_CHAIN (t))
	{
	  int len = (TREE_STRING_LENGTH (t) 
		     - ((TREE_TYPE (t) == int_array_type_node)
			? UNITS_PER_WORD : 1));
	  if ((TREE_TYPE (t) == int_array_type_node) == wide_flag)
	    {
	      bcopy (TREE_STRING_POINTER (t), q, len);
	      q += len;
	    }
	  else
	    {
	      int i;
	      for (i = 0; i < len; i++)
		((int *) q)[i] = TREE_STRING_POINTER (t)[i];
	      q += len * UNITS_PER_WORD;
	    }
	}
      if (wide_flag)
	{
	  int i;
	  for (i = 0; i < UNITS_PER_WORD; i++)
	    *q++ = 0;
	}
      else
	*q = 0;

      value = make_node (STRING_CST);
      TREE_STRING_POINTER (value) = p;
      TREE_STRING_LENGTH (value) = length;
      TREE_LITERAL (value) = 1;
    }
  else
    {
      value = strings;
      length = TREE_STRING_LENGTH (value);
      if (TREE_TYPE (value) == int_array_type_node)
	wide_flag = 1;
    }

  /* Compute the number of elements, for the array type.  */ 
  nchars = wide_flag ? length / UNITS_PER_WORD : length;

  /* Create the array type for the string constant.
     -Wwrite-strings says make the string constant an array of const char
     so that copying it to a non-const pointer will get a warning.  */
  if (warn_write_strings)
    {
      tree elements
	= build_type_variant (wide_flag ? integer_type_node : char_type_node,
			      1, 0);
      TREE_TYPE (value)
	= build_array_type (elements,
			    build_index_type (build_int_2 (nchars - 1, 0)));
    }
  else
    TREE_TYPE (value)
      = build_array_type (wide_flag ? integer_type_node : char_type_node,
			  build_index_type (build_int_2 (nchars - 1, 0)));
  TREE_LITERAL (value) = 1;
  TREE_STATIC (value) = 1;
  return value;
}
