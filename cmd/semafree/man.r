.bp
.he 'SEMAFREE (cmd)'TROPIX: Manual de Referência'SEMAFREE (cmd)'
.fo 'Atualizado em 30.06.97'Versão 3.0.0'Pag. %'

.b NOME
.in 5
.wo "semafree -"
libera o semáforo pelo qual o processo está
.wo "            esperando"
.br

.in
.sp
.b SINTAXE
.in 5
.(l
semafree [-v] <pid>
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "semafree" libera o semáforo (recurso) pelo qual o processo
de identificação <pid> está esperando.

.sp
Isto é uma ação que pode acarretar inconsistência das tabelas internas
do núcleo do TROPIX, e só deve ser utilizado em situações de "pânico".

.sp
Este comando está reservado para o superusuário.

.sp
As opções do comando são:

.in +3
.ip -v
Verboso

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
