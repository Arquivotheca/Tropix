.bp
.he 'SETVBUF (libc)'TROPIX: Manual de Modificações'SETVBUF (libc)'
.fo 'Atualizado em 04.02.91'Versão 2.3.1'Pag. %'

.b NOME
.in 5
.wo "setvbuf -"
atribui um "buffer" de tamanho variável a um arquivo
.br

.in
.sp 2
.ip "01.04.87  1.0" 20
VERSÃO CORRENTE
.sp

.ip "31.05.88  1.1.0" 20
Foi consertado o erro que ocorria quando não se dava o endereço
do "buffer".
Agora, a função aloca uma área através de "malloc".

.ip "04.02.91  2.3.1" 20
Agora, se "buf" não for dado, aproveita "_sio?buf".

.sp
Corrigido também um êrro da atribuição de "buf" inexistente. 
