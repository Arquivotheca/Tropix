.bp 1
.he 'PUTENV (libc)'TROPIX: Manual de Referência'PUTENV (libc)'
.fo 'Atualizado em 30.05.95'Versão 3.0'Pag. %'

.b NOME
.in 5
putenv - modifica ou acrescenta um valor ao ambiente

.in
.sp
.b SINTAXE
.in 5
.(l
int	putenv (const char *str);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "putenv" espera que "str" aponte para uma cadeia da forma
"name=value", e faz com que o valor da variável "name" seja "value",
alterando o valor da variável (caso já exista) ou criando
uma variável nova.

.sp
Em qualquer caso, a cadeia apontada por "str" irá fazer parte
do ambiente ("environment"), de tal modo que se a cadeia for alterada, o
ambiente tambem o será.

.sp
A área utilizada por "str" pode ser liberada quando uma nova cadeia
definindo a mesma variável for dada para "putenv".

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso  a função "putenv" retorna ZERO.
Se a cadeia dada não estiver no formato correto, ou se a área para alocar
ponteiros para as novas variáveis estiver esgotada, a função retorna -1. 

.in
.sp
.b OBSERVAÇÕES
.in 5
A função "putenv" manipula o ambiente apontado por "environ", e pode ser
utilizada conjuntamente com as função "getenv" e "delenv".
Note, no entanto, que o ambiente apontado por "envp"
(o terceiro argumento do programa que utiliza "putenv")
NÃO é modificado.

.sp
 Esta função utiliza "malloc" (ou "realloc") para obter mais espaço para
os apontadores do ambiente.

.sp
 A função "putenv" não põe as variáveis em ordem alfabética.

.sp
 Atenção para não chamar "putenv" com uma cadeia em uma
área automática, que irá ser liberada quando a função
que chamou "putenv" retornar. 

.sp
 Na presente implementação de "putenv", NÃO há limite
no número de cadeias no ambiente.

.in
.sp
.b
VEJA TAMBÉM
.r
.nf
     (sys):  exec
     (libc): getenv, delenv, malloc
     (fmt):  environ
.fi

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
