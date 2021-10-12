.bp
.he 'GETDIRENT (sys)'TROPIX: Manual de Referência'GETDIRENT (sys)'
.fo 'Atualizado em 05.08.01'Versão 4.0.0'Pag. %'

.b NOME
.in 5
.wo "getdirent -"
lê um vetor de entradas de um diretório
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/types.h>
#include <sys/sysdirent.h>

int	getdirent (int fd, DIRENT *area, int count);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "getdirent" tenta ler "count" bytes do diretório
associado ao descritor "fd" na área apontada por "area".
O descritor "fd" é obtido por uma das chamadas "open", "inopen",
"dup" ou "fcntl" (sys).

.sp
A área é preenchida com entradas do diretório em um formato 
independente do sistema de arquivo
(veja o arquivo <sys/sysdirent.h>),
e cada entrada contém os seguintes membros:

.in +3
.ip d_offset: 10
O deslocamento da entrada no diretório original.
Este valor pode ser usado para reler uma entrada
já lida anteriormente (através de "lseek" (sys)).

.ip d_ino: 10
O número do nó-índice.

.ip d_namlen: 10
O tamanho do identificador.

.ip d_name: 10
O identificador, cujo tamanho máximo é de MAXNAMLEN caracteres, além
do NULO terminal.

.ep
.in -3


.sp
A leitura é feita a partir da posição dada pelo ponteiro associado a "fd".
Após a leitura, o ponteiro é incrementado do valor de bytes
realmente lidos.

.sp
Se a operação é realizada com sucesso, "getdirent" retorna o número
de bytes lidos e colocados na área.
Sempre são retornadas entradas completas; se o
tamanho "count" não for um múltiplo do tamanho da estrutura DIRENT,
o valor residual será ignorado.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso, a chamada ao sistema devolve um inteiro
não negativo, indicando o número de bytes efetivamente lidos. 
Um valor NULO indica o final do arquivo.
Em caso contrário, devolve -1 e indica em "errno" a causa do erro.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
dup, fcntl, open, inopen, lseek
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
