.bp 1
.he 'LID (libc)'TROPIX: Manual de Referência'LID (libc)'
.fo 'Atualizado em 03.07.95'Versão 3.0'Pag. %'

.b NOME
.in 5
lid - manipulação de identificadores longos:
.sp
.wo "lidcan  -"
converte um identificador para sua forma canônica
.br
.wo "lidcpy  -"
copia um identificador longo
.br
.wo "lidclr  -"
zera um identificador longo
.br
.wo "lidcmp  -"
compara dois identificadores longos
.br
.wo "lideq   -"
verifica a igualdade de dois identificadores longos
.br
.wo "lidhash -"
calcula o índice de um id. em uma tabela "hash"
.br

.in
.sp
.b SINTAXE
.(l
#include <id.h>

idp_t	lidcan (idp_t lid1, const char *lid2);
idp_t	lidcpy (idp_t lid1, const idp_t lid2);
idp_t	lidclr (idp_t lid1);
int	lidcmp (const idp_t lid1, const idp_t lid2);
int	lideq  (const idp_t lid1, const idp_t lid2);
int	lidhash (const idp_t lid, int size);
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
Passaremos a descrever as funções da série "lid". Para os outros
formatos fixos, consulte "id" e "sid" (libc).

.sp
O formato padrão (também chamado de forma canônica) da série "lid"
consiste de 8 inteiros longos (com o alinhamento adequado), contendo
31 bytes do nome do identificador longo seguido de 1 byte NULO.
Caracteres não presentes no identificador longo
(quando seu nome tem menos do que 31 caracteres) são sempre
caracteres NULOS.

.sp
.in 5
A função "lidcan" converte a cadeia original "lid2"
para um identificador longo "lid1". Se a cadeia original tiver mais
de 31 caracteres, os caracteres excedentes são desprezados.
Se tiver menos de 31 caracteres, os restantes são zerados.
A função retorna "lid1".

.sp
.in 5
A função "lidcpy" copia o identificador longo "lid2" para "lid1".
Retorna "lid1".

.sp
.in 5
A função "lidclr" zera um identificador longo.
Retorna "lid1".

.sp
.in 5
A função "lidcmp" compara identificadores longos,
retornando 1 se "lid1" for lexicograficamente
maior do que "lid2", 0 se forem iguais e -1 se "lid1" for menor do que "lid2".

.sp
.in 5
A função "lideq" compara identificadores longos,
retornando 1 se "lid1" e "lid2" forem iguais
e 0 em caso contrário.

.sp
.in 5
A função "lidhash" calcula a entrada correspondente 
ao identificador longo em uma tabela
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
No caso da versão ICARUS da função "lidhash",
o seu argumento "size" deve ser menor do que 65536.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
id, sid, mem, strcpy, strcmp, strtok
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
