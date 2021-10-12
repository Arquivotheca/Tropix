.bp 1
.he 'STRCMP (libc)'TROPIX: Manual de Referência' STRCMP (libc)'
.fo 'Atualizado em 07.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Comparação de cadeias:
.sp
.wo "streq     -"
testa a igualdade de duas cadeias
.br
.wo "strcmp    -"
compara duas cadeias
.br
.wo "strrcmp   -"
compara duas cadeias retornando o valor invertido
.br
.wo "stricmp   -"
compara duas cadeias ISO
.br
.wo "strircmp  -"
compara duas cadeias ISO ret. o valor invertido
.br
.wo "strncmp   -"
compara no máximo "n" caracteres de duas cadeias
.br
.wo "strtcmp   -"
compara duas cadeias usando uma tabela
.br
.wo "strntcmp  -"
compara no maximo "n" caracteres usando uma tabela
.br
.wo "strttcmp  -"
compara duas cadeias usando duas tabelas
.br
.wo "strnttcmp -"
compara no máx. "n" carac. usando duas tabelas

.in
.sp
.b SINTAXE
.in 5
.(l
#include <string.h>

int	streq (const char *s1, const char *s2);
int	strcmp (const char *s1, const char *s2);
int	strrcmp (const char *s1, const char *s2);
int	stricmp (const char *s1, const char *s2);
int	strircmp (const char *s1, const char *s2);
int	strncmp (const char *s1, const char *s2, size_t n);
int	strtcmp (const char *s1, const char *s2,
			const char tb[]);
int	strntcmp (const char *s1, const char *s2,
			size_t n, const char tb[]);
int	strttcmp (const char *s1, const char *s2,
			const char tb[], const char tba[]);
int	strnttcmp (const char *s1, const char *s2,
		      size_t n, const char tb[], const char tba[]);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
Os parametros  "s1" e "s2" são ponteiros para cadeias de caracteres
terminadas por NULOs.
Para a comparação de áreas sem NULOs no final, veja "memcmp" (libc).

.sp
As comparações realizadas pelas funções "streq", "strcmp",
"strrcmp" e "strncmp" utilizam o próprio código de representação
dos caracteres nas cadeias, o que funciona corretamente desde que as cadeias
não contenham letras acentuadas (isto é, as cadeias contenham
apenas caracteres ASCII).
Se as cadeias contiverem letras acentuadas (isto é, utilizarem
caracteres da tabela ISO), devem ser utilizadas as funções
"stricmp" e "strircmp".

.sp
A função "streq" compara duas cadeias e retorna 1 se forem
iguais e 0 se diferentes.

.sp
A função "strcmp" compara duas cadeias e retorna um inteiro igual
a -1, 0 ou 1 de acordo se "s1" é lexicograficamente menor, igual ou
maior do que "s2".

.sp
A função "strrcmp" é semelhante à "strcmp", mas devolve o resultado
reverso, isto é, retorna 1, 0 e -1 para os casos de menor, igual ou maior.

.sp
A função "stricmp" compara inicialmente as duas cadeias através
de uma tabela que mapeia as vogais acentuadas em não acentuadas, e o
"ç" em "c" (tabela ISO); se a comparação der diferente,
retorna -1 ou 1 para os casos de menor ou maior.
Se a comparação der igual, realiza uma segunda comparação, baseada no
código original de representação dos caracteres (como "strcmp").

.sp
A função "strircmp" é semelhante à "stricmp", mas devolve o resultado
reverso, isto é, retorna 1, 0 e -1 para os casos de menor, igual ou maior.

.sp
A função "strncmp" é similar a "strcmp" mas compara no máximo
"n" caracteres.

.sp
A função "strtcmp" compara inicialmente as duas cadeias através
da tabela de conversão "tb" dada; se a comparação der diferente,
retorna -1 ou 1 para os casos de menor ou maior.
Se a comparação der igual, realiza uma segunda comparação, baseada no
código original de representação dos caracteres.
Existem tabelas para caracteres em código ISO, para mapeamento de letras
minúsculas em maiúsculas e outras (veja "cmptb" (libc)).

.sp
A função "strntcmp" é similar à "strtcmp" comparando, porém, no máximo
"n" caracteres.

.sp
A função "strttcmp" é similar à "strtcmp"
utilizando, porém, uma tabela de conversão adicional "tba" para resolver
os casos de igualdade obtidos pelo uso da primeira tabela.
Se a tabela adicional não for dada (isto é, se for dado um endereço NULO),
retorna 0 em caso de igualdade obtido pelo uso da primeira tabela.

.sp
A função "strnttcmp" é similar à "strttcmp"
comparando, porém, no máximo "n" caracteres.

.in
.sp
.(t
.b
VEJA TAMBEM
.r
.in 5
.wo "(cmd): "
sort
.br
.wo "(libc):"
strcpy, strtok, memcmp
.br
.wo "(fmt): "
ascii, iso
.br
.in
.)t

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
