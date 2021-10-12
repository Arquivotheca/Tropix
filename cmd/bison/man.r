.bp
.he 'BISON (cmd)'TROPIX: Manual de Referência'BISON (cmd)'
.fo 'Escrito em 08.07.93'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "bison -"
GNU Project parser generator (yacc replacement)
.br

.in
.sp
.b SINTAXE
.in 5
.(l
bison [-dltvVy] [-b <file_prefix>] [-o <out_file>]
				[-p <name_prefix>] <grammar>
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
"Bison" is a parser generator in the style of
"yacc" (cmd). It should be upwardly compatible with input files designed
for "yacc".

.sp
Input files should follow the "yacc" convention of ending in ".y".
Unlike "yacc", the generated files do not have fixed names,
but instead use the prefix of the input file.
For instance, a grammar description file named "parse.y"
would produce the generated parser in a file named "parse.tab.c",
instead of "yacc"'s "y.tab.c".

.sp
This description of the options that can be given to "bison"
is adapted from the node "Invocation" in the "bison.texinfo"
manual, which should be taken as authoritative.

.sp
As opções do comando são:

.in +3
.ip -d
Write an extra output file containing macro definitions for the
token type names defined in the grammar and the semantic value type
"YYSTYPE", as well as a few "extern" variable declarations.

.sp
If the parser output file is named "NAME.c" then this file is
named "NAME.h".

.sp
This output file is essential if you wish to put the definition of
"yylex" in a separate source file, because "yylex" needs to be
able to refer to token type codes and the variable "yylval".

.ip -l
Don't put any "#line" preprocessor commands in the parser file.
Ordinarily "bison" puts them in the parser file so that the C
compiler and debuggers will associate errors with your source
file, the grammar file.  This option causes them to associate
errors with the parser file, treating it an independent source
file in its own right.

.ip -t
Output a definition of the macro "YYDEBUG" into the parser file,
so that the debugging facilities are compiled.

.ip -v
Write an extra output file containing verbose descriptions of the
parser states and what is done for each type of look-ahead token in
that state.

.sp
This file also describes all the conflicts, both those resolved by
operator precedence and the unresolved ones.

.sp
The file's name is made by removing ".tab.c" or ".c" from the
parser output file name, and adding ".output" instead.

.sp
Therefore, if the input file is "foo.y", then the parser file is
called "foo.tab.c" by default.  As a consequence, the verbose
output file is called "foo.output".

.ip -V
Print the version number of "bison".

.ip -y
Equivalent to "-o y.tab.c"; the parser output file is called
"y.tab.c", and the other outputs are called "y.output" and
"y.tab.h".  The purpose of this switch is to imitate "yacc"'s output
file name conventions.  Thus, the following shell script can
substitute for "yacc":

.sp
.nf
		     bison -y $*
.fi

.ip -b
Specify <file_prefix> as a prefix to use for all "bison" output file names.
The names are chosen as if the input file were named "<file_prefix>.c".

.ip -o
Specify the name <out_file> for the parser file.

.sp
The other output files names are constructed from <out_file> as
described under the "-v" and "-d" switches.

.ip -p
Rename the external symbols used in the parser so that they start
with <name_prefix> instead of "yy".  The precise list of symbols renamed
is "yyparse", "yylex", "yyerror", "yylval", "yychar" and "yydebug".

.sp
For example, if you use "-p c", the names become "cparse", "clex",
and so on.

.ep
.in -3

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
yacc
.br

.in
.sp
.b ARQUIVOS
.in 5
.nf
     /usr/pub/lib/bison/bison.simple		simple parser
     /usr/pub/lib/bison/bison.hairy		complicated parser
.fi

.in
.sp
.b ESTADO
.in 5
Programa contribuído.

.in
