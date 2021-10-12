.bp 1
.he 'SID (libc)'TROPIX: Manual de Referência'SID (libc)'
.fo 'Atualizado em 04.07.95'Versão 3.0'Pag. %'

.b NOME
.in 5
sid - manipulação de identificadores curtos:
.sp
.wo "sidcan  -"
converte um identificador para sua forma canônica
.br
.wo "sidcpy  -"
copia um identificador curto
.br
.wo "sidclr  -"
zera um identificador curto
.br
.wo "sidcmp  -"
compara dois identificadores curtos
.br
.wo "sideq   -"
verifica a igualdade de dois identificadores curtos
.br
.wo "sidhash -"
calcula o índice de um id. em uma tabela "hash"
.br

.in
.sp
.b SINTAXE
.(l
#include <id.h>

idp_t	sidcan (idp_t sid1, const char *sid2);
idp_t	sidcpy (idp_t sid1, const idp_t sid2);
idp_t	sidclr (idp_t sid1);
int	sidcmp (const idp_t sid1, const idp_t sid2);
int	sideq  (const idp_t sid1, const idp_t sid2);
int	sidhash (const idp_t sid, int size);
.)l

.sp
.b DESCRIÇÃO
.in 5
No sistema TROPIX, além da série "str" e "mem", temos também as séries
"sid", "id" e "lid". Estas séries de funções destinam-se à manipulação
de identificadores nos formatos fixos de 7, 14 e 31 caracteres.
Estas funções aproveitam estes formatos fixos, e processando em
inteiros longos (32 bits), obtêm uma velocidade superior às funções da série
"str" e "mem".
Passaremos a descrever as funções da série "sid". Para os outros
formatos fixos, consulte "id" e "lid" (libc).

.sp
O formato padrão (também chamado de forma canônica) da série "sid"
consiste de 2 inteiros longos (com o alinhamento adequado), contendo
7 bytes do nome do identificador curto seguido de 1 byte NULO.
Caracteres não presentes no identificador curto
(quando seu nome tem menos do que 7 caracteres) são sempre
caracteres NULOS.

.sp
.in 5
A função "sidcan" converte a cadeia original "sid2"
para um identificador curto "sid1". Se a cadeia original tiver mais
de 7 caracteres, os caracteres excedentes são desprezados.
Se tiver menos de 7 caracteres, os restantes são zerados.
A função retorna "sid1".

.sp
.in 5
A função "sidcpy" copia o identificador curto "sid2" para "sid1".
Retorna "sid1".

.sp
.in 5
A função "sidclr" zera um identificador curto.
Retorna "sid1".

.sp
.in 5
A função "sidcmp" compara identificadores curtos,
retornando 1 se "sid1" for lexicograficamente
maior do que "sid2", 0 se forem iguais e -1 se "sid1" for menor do que "sid2".

.sp
.in 5
A função "sideq" compara identificadores curtos,
retornando 1 se "sid1" e "sid2" forem iguais
e 0 em caso contrário.

.sp
.in 5
A função "sidhash" calcula a entrada correspondente 
ao identificador curto em uma tabela
"hash" de tamanho "size", devolvendo um número inteiro no intervalo
.wo '[0 .. size-1].'

.in
.sp
.b OBSERVAÇÕES
.in 5
Repare que na forma canônica, após o último caracter temos sempre um
caracter NULO.
Isto significa, que também podemos utilizar as funções da série "str"
para identificadores desta forma.

.sp
É utilizada a comparação sem sinal. Isto é útil para utilização
do código de caracteres ISO de 8 bites.

.sp
No caso da versão ICARUS da função "sidhash",
o seu argumento "size" deve ser menor do que 65536.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
id, lid, mem, strcpy, strcmp, strtok
.br
.wo "(fmt): "
iso
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
