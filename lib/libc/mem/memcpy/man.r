.bp 1
.he 'MEM (libc)'TROPIX: Manual de Referência' MEM (libc)'
.fo 'Atualizado em 06.07.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Operações com áreas de memória: 
.sp
.wo "memcpy   -"
copia uma área de memória
.br
.wo "memmove  -"
cópia otimizada de uma área de memória
.br
.wo "memccpy  -"
copia uma área para outra até encontrar caractere
.br
.wo "memeq    -"
testa a igualdade de duas áreas de memória
.br
.wo "memcmp   -"
compara duas áreas de memória
.br
.wo "memtcmp  -"
compara duas áreas de memória  usando uma tabela
.br
.wo "memttcmp -"
compara duas áreas de memória usando duas tabelas
.br
.wo "memchr   -"
acha a 1a. ocorrência de um caractere
.br
.wo "memset   -"
preenche uma área de memória com um caractere
.br
.wo "memsetw  -"
preenche um vetor de palavras com um valor
.br
.wo "memsetl  -"
preenche um vetor de longos com um valor
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <string.h>

void	*memcpy (void *a1, const void *a2, size_t n);
void	*memmove (void *a1, const void *a2, size_t n);
void	*memccpy (void *a1, const void *a2, int c, size_t n);
int	memeq   (const void *a1, const void *a2, size_t n);
int	memcmp (const void *a1, const void *a2, size_t n);
int	memtcmp (const void *a1, const void *a2, size_t n,
				const char tb[]);
int	memttcmp (const void *a1, const void *a2, size_t n,
				const char tb[], const char tba[]);
void	*memchr (const void *a, int c, size_t n);
void	*memset (void *a, int c, size_t n);
void	*memsetw (void *a, int c, size_t nelem);
void	*memsetl (void *a, int c, size_t nelem);
.)l
.sp

.in
.sp
.b DESCRIÇÃO
.in 5
Estas funções são indicadas para uso com áreas de memória
cujos tamanhos são conhecidos a priori.
Não é necessário que as áreas sejam terminadas com caracteres
NULOs. 
Os parametros "a", "a1" e "a2" são ponteiros para áreas de memória.

.sp
As comparações realizadas pelas funções "memeq" e "memcmp"
utilizam o próprio código de representação dos caracteres nas áreas,
o que funciona corretamente desde que as cadeias
não contenham letras acentuadas (isto é, se as cadeias contiverem
apenas caracteres ASCII).
Se as cadeias contiverem letras acentuadas (isto é, utilizarem
caracteres da tabela ISO), devem ser utilizadas as funções
"memtcmp" e "memttcmp" com a tabela "cmpisotb".

.sp
A função "memcpy" copia "n" caracteres da área de memória "a2"
para "a1".
A função retorna o valor de "a1".

.sp
A função "memmove" é análoga à função "memcpy", mas, se for possível,
copia a área de memória de palavra em palavra (4 bytes),
o que é bem mais rápido.
Além disto, ela copia de baixo para cima ou vice-versa, para
impedir a destruição de informação no caso de áreas superpostas.
A função retorna o valor de "a1".

.sp
A função "memccpy" copia caracteres da área "a2" para "a1",
até encontrar o caractere "c" ou transferir "n" caracteres.
A função retorna um ponteiro para o caractere seguinte à cópia de "c"
em "a1", ou um ponteiro NULO caso "c" não foi encontrado nos "n"
primeiros caracteres de "a2".

.sp
A função "memeq" compara duas áreas de memória de "n" caracteres
e retorna 1 se forem iguais e 0 se diferentes.

.sp
A função "memcmp" compara as duas áreas de memória de "n" caracteres,
e retorna um inteiro igual a -1, 0 ou 1 de acordo se "a1" é
lexicograficamente menor, igual ou maior do que "a2".

.sp
A função "memtcmp" compara inicialmente as duas áreas de memória
de "n" caracteres através da tabela de conversão "tb" dada;
se a comparação der diferente, retorna -1 ou 1 para os casos de menor ou maior.
Se a comparação der igual, realiza uma segunda comparação, baseada no
código original de representação dos caracteres.
Existem tabelas para caracteres em código ISO, para mapeamento de letras
minúsculas em maiúsculas e outras (veja "cmptb" (libc)).

.sp
A função "memttcmp" é similar à "memtcmp"
utilizando, porém, uma tabela de conversão adicional "tba" para resolver
os casos de igualdade obtidos durante o uso da primeira tabela.
Se a tabela adicional não for dada (isto é, se for dado um endereço NULO),
retorna 0 em caso de igualdade.

.sp
A função "memchr" retorna um apontador para a primeira ocorrência do
caractere "c" nos primeiros "n" caracteres da área "a", ou
um apontador NULO, caso ele não ocorra nesta área.

.sp
A função "memset" preenche os "n" caracteres da área de memória "a",
com o caractere "c".
A função retorna o valor de "a".

.sp
A função "memsetw" preenche o vetor de palavras curtas "a" (de 16 bites),
contendo "nelem" elementos, com o valor "c".
A função retorna o valor de "a".

.sp
A função "memsetl" preenche o vetor de palavras longas "a" (de 32 bites),
contendo "nelem" elementos, com o valor "c".
A função retorna o valor de "a".

.in
.sp
.b
OBSERVAÇÕES
.r
.in 5
As funções "memcpy", "memmove", "memccpy", "memset", "memsetw"
e "memsetl" alteram as áreas "a1"/"a"; elas não verificam se
ultrapassam o seu espaço alocado.

.sp
Se as cópias forem entre objetos superpostos (exceto para "memmove"),
o resultado será imprevisível. 

.sp
Repare que nas funções "memsetw" e "memsetl", o tamanho é dado
pelo número de palavras curtas/longas, e não pelo número de bytes.

.in
.sp
.(t
.b
VEJA TAMBEM
.r
.in 5
.wo "(libc):"
strcpy, strcmp, strtok, cmptb
.br
.wo "(fmt): "
ascii, iso
.)t
.in

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
