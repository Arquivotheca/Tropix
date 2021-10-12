.bp 1
.he 'STRTOK (libc)'TROPIX: Manual de Referência' STRTOK(libc)'
.fo 'Atualizado em 11.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Manipulações diversas em cadeias:
.sp
.wo "strchr   -"
acha a 1a. ocorrência de um caractere em uma cadeia
.br
.wo "strrchr  -"
acha a última ocorrência de um carac. em uma cadeia
.br
.wo "strend   -"
acha a posição do nulo final de uma cadeia
.br
.wo "strpbrk  -"
acha a ocorrência de um caractere de outra cadeia
.br
.wo "strspn   -"
dá o no. de carac. que constam de outra cadeia
.br
.wo "strcspn  -"
dá o no. de carac. que não constam de outra cadeia
.br
.wo "strtok   -"
acha um "token" em uma cadeia
.br
.wo "strfield -"
acha um campo em uma cadeia
.br
.wo "strstr   -"
acha uma subcadeia em uma cadeia
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <string.h>

char	*strchr (const char *s, int c);
char	*strrchr (const char *s, int c);
char	*strend (const char *s);
char	*strpbrk (const char *s1, const char *s2);
int	strspn (const char *s1, const char *s2);
int	strcspn (const char *s1, const char *s2);
char	*strtok (char *s1, const char *s2);
char	*strfield (char *s1, int c);
char	*strstr (const char *s1, const char *s2);
.)l

.in
.sp
.(t
.b DESCRIÇÃO
.in 5
Os parametros "s1", "s2" e "s" são ponteiros para cadeias terminadas por NULOs.

.sp
A função "strchr" retorna um ponteiro para a primeira ocorrência do
caractere "c" na cadeia "s".
Se a cadeia não contém o caractere "c", retorna um ponteiro NULO.
O caractere NULO final é considerado parte da cadeia.
.)t

.sp
A função "strrchr" é semelhante à "strchr", mas retorna um ponteiro
para a última ocorrência do caractere "c".

.sp
A função "strend" retorna o endereço do NULO final da cadeia. 

.sp
A função "strpbrk" retorna um ponteiro na cadeia "s1",
para a primeira ocorrência de qualquer caractere da cadeia "s2".
Se não existir nenhum caractere de "s2" em "s1" retorna um ponteiro NULO.

.sp
A função "strspn" retorna o tamanho do primeiro segmento de "s1"
que consiste inteiramente de caracteres presentes na cadeia "s2".

.sp
A função "strcspn" retorna o tamanho do primeiro segmento de "s1"
que consiste inteiramente de caracteres ausentes na cadeia "s2".

.sp
A função "strtok"
retorna um ponteiro para o primeiro caractere do primeiro "token" de "s1"
cujos delimitadores são os caracteres de "s2".
Os vários "tokens" são separados por um ou mais delimitadores.
Delimitadores antes do primeiro "token" são pulados.
Um caractere NULO é escrito em "s1"
no lugar do primeiro delimitador após o "token".
Para  a busca do próximo token, "s1"
deve ser um ponteiro NULO e "s2"
pode ser modificado. Dessa maneira sucessivas chamadas da função,
fornecerão todos os "tokens" de "s1".
Quando acabarem os "tokens" de "s1", é retornado um ponteiro NULO.

.sp
A função "strfield" é semelhante à "strtok", mas com as
seguintes diferenças:
apenas um delimitador pode ser dado (o caractere "c"); os campos
devem ser separados por apenas um delimitador;
delimitadores antes do primeiro campo não são pulados,
e são interpretados como campos vazios.

.sp
A função "strstr" procura a primeira ocorrência da subcadeia "s2" na
cadeia "s1" (excluindo o caractere NULO que termina "s2").
Ela devolve um ponteiro para a subcadeia encontrada
em "s1", ou um ponteiro NULO se não for encontrada.
Se a subcadeia procurada tem tamanho NULO, é retornado "s1". 

.in
.sp
.b
OBSERVAÇÃO
.br
.in 5
.r
Repare que as funções "strtok" e "strfield" alteram "s1".

.in
.sp
.b
VEJA TAMBEM
.r
.in 5
(libc): strcpy, strcmp, memcpy, memcmp, memchr

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
