.bp
.he 'PRREGION (cmd)'TROPIX: Manual de Referência'PRREGION (cmd)'
.fo Átualizado em 10.08.04'Versão 4.6.0'Pag. %'

.b NOME
.in 5
.wo "prregion -"
imprime as regiões dos processos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
prregion [<pid> ...]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "prregion" imprime informações sobre as regiões dos
diversos processos. Se forem dados <pid>s de processos, somente
serão dadas informações sobre estes processos; em caso contrário
serão dadas informações sobre todos processos.

.sp
São listadas informações das regiões dos textos (programas residentes na memória),
bibliotecas compartilhadas e dos diversos processos. Ao final é dada uma
lista de regiões que não foram atribuídas a nenhum processo
(isto não implica necessariamente em erro, pois a leitura das diversas
tabelas do núcleo do TROPIX se dá em paralelo com a execução do comando
"prregion").

.sp
As informações dadas sobre cada região são as seguintes:

.in +3
.ip Tipo: 10
Tipo da região: (TEXT, DATA ou STACK).

.ip regiong: 10
Endereço da tabela da região global.

.ip vaddr: 10
Endereço virtual da região.

.ip prot: 10
Proteção da região (RW para leitura/escrita e RO para apenas leitura).

.ip size: 10
Tamanho da região em KB.

.ip caddr: 10
Endereço da região.

.ip count: 10
Número de referências à região.

.ip nome: 10
Nome do processo que criou a região (e NÃO o que possui a região).

.ip Tabela 10
Tabela de páginas (endereço da tabela de páginas, número de páginas e de entradas usadas).

.ep
.in -3

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
ps, prsync
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
