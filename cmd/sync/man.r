.bp 1
.he 'SYNC (cmd)'TROPIX: Manual de Referência'SYNC (cmd)'
.fo 'Atualizado em 12.07.97'Versão 3.0.1'Pag. %'

.b NOME
.in 5
sync - atualiza o conteúdo dos discos

.in
.sp
.b SINTAXE
.in 5
sync

.in
.sp
.b DESCRIÇÃO
.in 5
O comando
"sync" atualiza o conteúdo do disco
escrevendo todas as informações contidas no sistema operacional
que ainda estão mais atuais na memória do que no disco.
Isto inclui superblocos modificados, nós-índices modificados
e blocos de E/S ainda não escritos.
.sp
Somente o superusuário pode utilizar este comando, 
e sua utilização é essencial
antes de um "boot".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
 (cmd): update
 (sys): sync

.in
.sp
.b ESTADO
.in 5
Efetivo.
.in
