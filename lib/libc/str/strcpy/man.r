.bp
.he 'STRCPY (libc)'TROPIX: Manual de Referência' STRCPY (libc)'
.fo 'Atualizado em 10.10.04'Versão 4.6.0'Pag. %'

.b NOME
.in 5
Movimentações de cadeias:
.sp
.wo "strcat  -"
concatena duas cadeias
.br
.wo "strncat -"
concatena duas cadeias até "n" caracteres
.br
.wo "strcpy  -"
copia uma cadeia para outra
.br
.wo "strncpy -"
copia uma cadeia para outra até "n" caracteres
.br
.wo "strscpy -"
como acima, mas com um NULO no final da cadeia
.br
.wo "strmove -"
copia uma cadeia para outra superposta
.br
.wo "strlen  -"
obtém o tamanho de uma cadeia (sem o NULO)
.br
.wo "strdup  -"
duplica uma cadeia

.in
.sp
.b SINTAXE
.in 5
.(l
#include <string.h>

char	*strcat (char *s1, const char *s2);
char	*strncat (char *s1, const char *s2, size_t n);
char	*strcpy (char *s1, const char *s2);
char	*strncpy (char *s1, const char *s2, size_t n);
char	*strscpy (char *s1, const char *s2, size_t n);
char	*strmove (char *s1, const char *s2);
size_t	strlen (const char *s);
char	*strdup (const char *s);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
Os parametros "s1", "s2" e "s" são ponteiros para cadeias de caracteres
terminadas por NULOs. Para o tratamento de áreas sem NULOs no final,
veja "memcpy" (libc).

.sp
A função "strcat" adiciona uma cópia de "s2" ao final de "s1".
A função retorna o valor de "s1".

.sp
A função "strncat" adiciona até "n" caracteres da cadeia "s2"
ao final de "s1".
Pode ser gerada uma cadeia não terminada
com um NULO, caso "s2" tenha "n" ou mais caracteres.
A função retorna o valor de "s1".

.sp
A função "strcpy" copia a cadeia "s2" para "s1".
A função retorna o valor de "s1".

.sp
A função "strncpy" copia exatamente "n" caracteres truncando a cópia de
"s2" ou adicionando caracteres NULOs a "s1"
quando necessário. O resultado não será terminado por um caractere
NULO caso o comprimento de "s2" seja "n" ou maior.
A função retorna o valor de "s1".

.sp
A função "strscpy" é similar à "strncpy", mas garante um NULO
no final da cadeia.
A função retorna o valor de "s1".

.sp
A função "strmove" é semelhante à "strcpy",
mas copia de baixo para cima ou vice-versa, de modo a
não perder informações no caso das cadeias serem superpostas. 
Pode ser um pouco mais lenta do que "strcpy".
A função retorna o valor de "s1".

.sp
A função "strlen" retorna o comprimento de "s",
isto é, o número de caracteres da cadeia antes do NULO final.

.sp
A função "strdup" devolve um ponteiro para uma nova cadeia
que é uma duplicata da cadeia dada. O espaço para a
nova cadeia é obtida através de "malloc" (libc). Se a duplicata
não pôde ser criada, devolve um ponteiro NULO.

.in
.sp
.b
OBSERVAÇÕES
.r
.in 5
As funções "strcat", "strncat", "strcpy", "strmove",
"strncpy" e "strscpy" alteram "s1";
elas não verificam se ultrapassam a área alocada para esta cadeia.

.sp
Se as cópias forem entre objetos superpostos (exceto para "strmove"),
o resultado será imprevisível. 

.in
.sp
.b
VEJA TAMBEM
.r
.in 5
(libc): strcmp, strtok, memcpy, malloc

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
