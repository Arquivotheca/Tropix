.bp
.he 'T_ADDR (xti)'TROPIX: Manual de Referência'T_ADDR (xti)'
.fo 'Escrito em 05.07.99'Versão 3.2.1'Pag. %'

.b NOME
.in 5
Conversões entre os diversos formatos de endereços INTERNET:
.sp
.wo "t_addr_to_name  -"
endereço de 32 bits em nome de nó simbólico
.br
.wo "                  (com consulta a um servidor DNS)"
.br
.wo "t_addr_to_node  -"
endereço de 32 bits em nome de nó simbólico
.br
.wo "                  (sem consulta a um servidor DNS)"
.br
.wo "t_addr_to_str   -"
endereço de 32 bits em 4 números decimais
.br
.wo "t_node_to_addr  -"
nome de nó simbólico em endereço de 32 bits
.br
.wo "t_str_to_addr   -"
4 números decimais em endereço de 32 bits
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <xti.h>

char	*t_addr_to_name (int fd, long addr);

char	*t_addr_to_node (int fd, long addr);

char	*t_addr_to_str (long addr);

long	t_node_to_addr (int fd, const char *node,
						long *my_addr);

long 	t_str_to_addr (const char *str);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
Estas funções auxiliam na conversão entre os diversos formatos
de endereços INTERNET.
Os formatos de endereços são:

.in +3
.ip - 2
O endereço de 32 bits é o formato interno utilizado por todas
as funções que requerem um endereço INTERNET (membro "a_addr"
da estrutura "INADDR", ver <xti.h>).

.ip - 2
O formato de 4 números decimais ("dd.dd.dd.dd") é uma notação
externa para facilitar a leitura dos endereços em documentos técnicos
ou em programas de aplicação.

.ip - 2
O nome de nó simbólico é a notação mais cômoda para um endereço, em
que se utiliza um identificador para representá-lo.
O núcleo do sistema operacional contém uma tabela para associar
os nomes de nós simbólicos aos endereços (e vice-versa).

.ep
.in -3

.sp
Para a utilização das funções "t_addr_to_name", "t_addr_to_node"
e "t_node_to_addr" é necessário já haver algum "endpoint" de transporte
disponível (argumento "fd"), isto é, elas só podem ser utilizadas
após um "t_open" (libxti).

.sp
A função "t_addr_to_name" consulta a tabela mencionada acima e
devolve um ponteiro para uma cadeia contendo o nome do nó simbólico
correspondente ao endereço INTERNET de 32 bits "addr".
Se o endereço dado NÃO for encontrado na tabela, é iniciada uma
consulta a um servidor DNS para obter o nome do nó simbólico.

.sp
A função "t_addr_to_node" consulta a tabela mencionada acima e
devolve um ponteiro para uma cadeia contendo o nome do nó simbólico
correspondente ao endereço INTERNET de 32 bits "addr".
Se o endereço dado NÃO for encontrado na tabela, é devolvido um
ponteiro para uma cadeia contendo um endereço no formato de
4 números decimais (isto é, não é feita a consulta conforme acima).

.sp
A função "t_addr_to_str" devolve um ponteiro para uma cadeia
contendo um endereço no formato de 4 números decimais
correspondente ao endereço INTERNET de 32 bits "addr".

.sp
A função "t_node_to_addr"  consulta a tabela mencionada acima
e devolve um endereço INTERNET no formato de 32 bits correspondente
a um nó remoto "node". Este nó remoto pode ser dado como um
nome de nó simbólico ou um endereço no formato de 4 números decimais.
Se "node" for um ponteiro NULO, será devolvido o endereço do próprio nó local.
A posição de memória apontada pelo ponteiro "my_addr" (se não NULO)
receberá o endereço INTERNET no formato de 32 bits correspondente
ao próprio nó local.

.sp
A função "t_str_to_addr" devolve um endereço INTERNET no formato
de 32 bits correspondente ao endereço no formato de 4 números decimais
"str".

.in
.sp
.b 'DETALHES DE IMPLEMENTAÇÃO'
.in 5
Estas funções não fazem parte da XTI, e portanto seu uso deve ser evitado
se for desejada portabilidade para outras implementações.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso, as funções devolvem os endereços no formatos
descritos acima.

.sp
Em caso de falha:
a função "t_addr_to_node" devolve um ponteiro para uma cadeia
contendo um endereço no formato de 4 números decimais (como
"t_addr_to_str");
"t_addr_to_name" e "t_node_to_addr" devolvem -1 e um código de erro em "t_errno";
"t_str_to_addr" devolve -1.
A função "t_addr_to_str" nunca retorna com falha.

.in
.sp
.b OBSERVAÇÃO
.in 5
A funções "t_addr_to_name", "t_addr_to_node" e "t_addr_to_str"
devolvem um ponteiro para uma área estática, e o
seu conteúdo deve ser copiado caso ele seja necessário após
a função ser chamada novamente.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.br
.wo "(xti): "
t_intro, t_open
.br

.in
.sp
.b
REFERÊNCIAS
.r
.in 5
"X/OPEN Portability Guide: Networking Services",
X/OPEN Company Ltd.,
Prentice Hall, New Jersey, 1st Edition, 1988.

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
