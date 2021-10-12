.bp
.he 'MKFIFO (cmd)'TROPIX: Manual de Referência'MKFIFO (cmd)'
.fo 'Atualizado em 23.05.97'Versão 3.0.0'Pag. %'

.b NOME
.in 5
.wo "mkfifo -"
cria FIFOs
.br

.in
.sp
.b SINTAXE
.in 5
.(l
mkfifo <fifo> ...
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando  "mkfifo" cria os arquivos <fifo> ... do tipo "FIFO"
("First In, First Out").
Estes arquivos são de uso semelhante aos PIPEs, mas no entanto possuem nomes,
o que possibilita o seu uso por processos que não têm ancendentes em comum. 

.in
.sp
.b OBSERVAÇÃO
.in 5
O comando "mkfifo" cria os arquivos com permissão para escrita e leitura
apenas pelo usuário que os criou.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
mknod, rm
.br
.wo "(sys): "
mknod, pipe
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in
