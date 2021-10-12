.bp
.he 'WC (cmd)'TROPIX: Manual de Referência'WC (cmd)'
.fo 'Atualizado em 30.12.04'Versão 4.7.0'Pag. %'

.b NOME
.in 5
.wo "wc -"
contador de páginas, linhas, palavras e caracteres
.br

.in
.sp
.b SINTAXE
.in 5
.(l
wc [-N] [-l <l>] [<arquivo> ...]
.)l
.bc	/*************************************/
.(l
wc [-plwcN] [-P <l>] [<arquivo> ...]
.)l
.ec	/*************************************/

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "wc" conta o número de páginas, linhas, palavras
e caracteres dos <arquivo>s dados, imprimindo uma linha para
cada <arquivo>.
.bc	/*************************************/
O número de páginas é o necessário pelo comando
"pr" (cmd) para imprimir o arquivo. 
.ec	/*************************************/

.sp
Uma palavra é uma seqüência de caracteres delimitada pelos
caracteres <sp>, <ht>, <nl> ou <np>.

.sp
Se for dado mais de um <arquivo>, será impressa também uma linha com
os totais acumulados.
Se não for dado nenhum nome de <arquivo> nem a opção "-N" (veja abaixo),
será lida a entrada padrão.

.sp
Normalmente, cada página contém 75 linhas (veja a opção "-l").

.sp
As opções do comando são:

.in +3
.bc	/*************************************/
.ip -p 8
Imprime o número de páginas.

.ip -l 8
Imprime o número de linhas.

.ip -w 8
Imprime o número de palavras.

.ip -c 8
Imprime o número de caracteres.
.ec	/*************************************/

.ip -N 8
Lê os nomes dos <arquivo>s de "stdin".

.ip '-l <l>' 8
A página contém <l> linhas (ao invés de 75).
Para obter o número de páginas
que um arquivo necessitará ao ser impresso por "pr" (cmd), use
"-l 56".

.ep
.in -3

.bc	/*************************************/
.sp
As opções "-p", "-l", "-w" e "-c" podem ser dadas em qualquer combinação.
Se não for dada nenhuma delas, são impressos os 4 números.
.ec	/*************************************/

.in
.sp
.b OBSERVAÇÕES
.in 5
Embora o comando "wc" também possa ser usado para módulos-objeto,
o seus números de páginas, linhas e palavras não têm qualquer sentido.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
pr
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
