.bp 1
.he 'ID (libc)'TROPIX: Manual de Referência'ID (libc)'
.fo 'Atualizado em 29.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
id - manipulação de identificadores:
.sp
.wo "idcan  -"
converte um identificador para sua forma canônica
.br
.wo "idcpy  -"
copia um identificador
.br
.wo "idclr  -"
zera um identificador
.br
.wo "idcmp  -"
compara dois identificadores
.br
.wo "ideq   -"
verifica a igualdade de dois identificadores
.br
.wo "idhash -"
calcula o índice de um id. em uma tabela "hash"
.br

.in
.sp
.b SINTAXE
.(l
#include <id.h>

idp_t	idcan (idp_t id1, const char *id2);
idp_t	idcpy (idp_t id1, const idp_t id2);
idp_t	idclr (idp_t id1);
int	idcmp (const idp_t id1, const idp_t id2);
int	ideq  (const idp_t id1, const idp_t id2);
int	idhash (const idp_t id, int size);
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
Passaremos a descrever as funções da série "id". Para os outros
formatos fixos, consulte "sid" e "lid" (libc).

.sp
O formato padrão (também chamado de forma canônica) da série "id"
consiste de 4 inteiros longos (com o alinhamento adequado), contendo
14 bytes do nome do identificador seguido de 2 bytes NULOS.
Caracteres não presentes no identificador 
(quando seu nome tem menos do que 14 caracteres) são sempre
caracteres NULOS.


.sp
.in 5
A função "idcan" converte a cadeia original "id2"
para um identificador "id1". Se a cadeia original tiver mais
de 14 caracteres, os caracteres excedentes são desprezados.
Se tiver menos de 14 caracteres, os restantes são zerados.
A função retorna "id1".

.sp
.in 5
A função "idcpy" copia o identificador "id2" para "id1".
Retorna "id1".

.sp
.in 5
A função "idclr" zera um identificador.
Retorna "id1".

.sp
.in 5
A função "idcmp" compara identificadores,
retornando 1 se "id1" for lexicograficamente
maior do que "id2", 0 se forem iguais e -1 se "id1" for menor do que "id2".

.sp
.in 5
A função "ideq" compara identificadores,
retornando 1 se "id1" e "id2" forem iguais
e 0 em caso contrário.

.sp
.in 5
A função "idhash" calcula a entrada correspondente 
ao identificador em uma tabela
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
No caso da versão ICARUS da função "idhash",
o seu argumento "size" deve ser menor do que 65536.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
sid, lid, mem, strcpy, strcmp, strtok
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
