.bp 1
.he 'SETBUF (libc)'TROPIX: Manual de Referência'SETBUF (libc)'
.fo 'Atualizado em 10.07.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Atribui uma área de entrada/saída ("buffer") a um arquivo:
.sp
.wo "setbuf  -"
de tamanho implícito
.br
.wo "setvbuf -"
de tamanho dado
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdio.h>

void     setbuf (FILE *fp, char *buf);
int      setvbuf (FILE *fp, char *buf, int mode, size_t size);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "setbuf" pode ser usada depois que o arquivo foi aberto
mas antes que ele seja lido ou escrito.
Se o argumento "buf" não for um ponteiro NULO, a área apontada
por ele será utilizada como área de entrada/saída, ao invés de uma
área alocada automaticamente.
Este vetor deve ter o tamanho "BUFSIZ" (definido em <stdio.h>).
Se  "buf" for um ponteiro NULO, a entrada/saída será realizada
sem nenhuma área de entrada/saída (isto é, de caractere em caractere).

.sp
A função "setvbuf" pode ser usada depois que o arquivo foi aberto
mas antes que ele tenha seja lido ou escrito.
O parametro "mode" determina a natureza da área de entrada/saída
do arquivo. Os valores que "mode"
pode assumir (definidos em <stdio.h>) são os seguintes:

.in +3
.ip _IOFBF 8
o arquivo terá uma área de entrada/saída normal, ou seja,
a área só será descarregada quando ela estiver cheia.

.ip _IOLBF 8
o arquivo terá uma área de entrada/saída de linha, ou seja,
a área será descarregada quando ela estiver cheia,
for escrito um caracter <nl>, e em um caso particular que é o seguinte:
<stdout> e <stderr> são descarregados quando for pedida uma
entrada em <stdin>.

.ip _IONBF 8
o arquivo não terá área de entrada/saída.

.ep
.in -3

.sp
Se o argumento "buf" não for um ponteiro NULO, a área apontada
por ele será utilizada como área de entrada/saída, ao invés de uma
área alocada automaticamente.
O argumento "size"  especifica o tamanho da área de entrada/saída.
Uma boa sugestão é usar a constante "BUFSIZ" (definida em <stdio.h>).
Caso o arquivo seja definido sem área de entrada/saída, os argumentos
"buf" e "size" são ignorados.

.sp
A não ser que outra coisa seja especificada,
todo arquivo de saída para um terminal terá uma área de entrada/saída
de linha e todos os outros arquivos terão áreas normais.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Caso o parametro "mode" ou "size" tenha um valor ilegal, 
ou o pedido não possa ser satisfeito, a função "setvbuf"
devolve um número não-zero. Caso contrário devolve zero.

.in
.sp
.b OBSERVAÇÕES
.in 5
 Atenção para não chamar estas funções com um "buf" alocado em uma
área automática, que irá ser liberada quando a função
que a chamou retornar. 

.sp
A função "setbuf" é mais antiga e limitada do que "setvbuf",
e só é mantida por questões de compatibilidade. 

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
fopen, getc, malloc, putc, stdio
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
