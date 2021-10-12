.bp
.he 'LSEEK (sys)'TROPIX: Manual de Referência'LSEEK (sys)'
.fo 'Atualizado em 02.11.02'Versão 4.4.0'Pag. %'

.b NOME
.in 5
Move o ponteiro de posição de um arquivo:
.sp
.wo "lseek  -"
dado um deslocamento de 32 bits
.br
.wo "llseek -"
dado um deslocamento de 64 bits
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>
#include <unistd.h>

off_t lseek (int fd, off_t offset, int whence);

int llseek (int fd, loff_t offset, loff_t *result, int whence);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
As chamada ao sistema "lseek" e "llseek" alteram o valor do ponteiro de posição
de leitura/escrita de um arquivo aberto. O argumento "fd" é um
descritor obtido através de uma chamada "creat", "open", "dup" ou
"fcntl".

.sp
Dependendo do valor de "whence", as chamadas alteram
o ponteiro de posição para:

.in +3
.ip L_SET: 9
o valor de "offset" dado.

.ip L_CUR: 9
o valor antigo do ponteiro acrescido de "offset".

.ip L_END: 9
o tamanho do arquivo acrescido de "offset".

.ep
.in -3

.sp
O valor do ponteiro de posição resultante do arquivo é retornado de modo diferente
em cada uma das chamadas ao sistema:
para o caso de "lseek", é o próprio retorno da função;
para o caso de "llseek", ele é armazenado na variável apontada pelo
ponteiro "result" (se ele for não NULO).

.sp
Repare que o valor de "offset" pode ser negativo (o que faz sentido apenas
para "whence" com os valores "L_CUR" ou L_END").

.sp
Alguns dispositivos são incapazes de alterar o seu ponteiro de posição.
Nestes casos, o valor do ponteiro associado é indefinido.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso, a chamada ao sistema "lseek" devolve o valor do ponteiro
de posição resultante, e "llseek" devolve um valor NULO.
Em caso contrário, as funções devolvem -1 e indicam em "errno" a causa do erro.

.in
.sp
.b OBSERVAÇÕES
.in 5
Movendo-se o ponteiro de posição para além do
final de um arquivo regular,
e em seguida escrevendo-se nele, cria-se um "buraco"
que não ocupa blocos físicos no disco, e é lido como ZEROs. 

.sp
Somente a chamada ao sistema "write" pode aumentar o tamanho
de um arquivo: "lseek" por si só, não aumenta
o tamanho de um arquivo.

.sp
O padrão POSIX definiu as constantes "SEEK_SET", "SEEK_CUR" e "SEEK_END"
como sinônimos de "L_SET", L_CUR" e L_END", respectivamente.


.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
creat, dup, fcntl, open
.br

.in
.sp
.(t
.b ESTADO
.in 5
Efetivo.
.)t
